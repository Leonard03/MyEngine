//
// Copyright (c) 2015-2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "MyEnginePCH.h"

#include "ComponentAnimationPlayer.h"
#include "ComponentMeshOBJ.h"
#include "ComponentSystem/Core/GameObject.h"

ComponentAnimationPlayer::ComponentAnimationPlayer(ComponentSystemManager* pComponentSystemManager)
: ComponentUpdateable( pComponentSystemManager )
{
    ClassnameSanityCheck();

    m_BaseType = BaseComponentType_Updateable;
}

ComponentAnimationPlayer::~ComponentAnimationPlayer()
{
}

void ComponentAnimationPlayer::Reset()
{
    ComponentUpdateable::Reset();

#if MYFW_USING_WX
    m_pPanelWatchBlockVisible = &m_PanelWatchBlockVisible;
#endif //MYFW_USING_WX

    m_pMeshComponent = 0;

    m_AnimationIndex = 0;
    m_AnimationTime = 0;

    m_LastAnimationIndex = 0;
    m_LastAnimationTime = 0;

    m_TransitionTimeLeft = 0;
    m_TransitionTimeTotal = 0;
}

#if MYFW_USING_LUA
void ComponentAnimationPlayer::LuaRegister(lua_State* luastate)
{
    luabridge::getGlobalNamespace( luastate )
        .beginClass<ComponentAnimationPlayer>( "ComponentAnimationPlayer" )
            //.addData( "localmatrix", &ComponentAnimationPlayer::m_LocalTransform )
            .addFunction( "SetCurrentAnimation", &ComponentAnimationPlayer::SetCurrentAnimation ) // void ComponentAnimationPlayer::SetCurrentAnimation(unsigned int anim)
        .endClass();
}
#endif //MYFW_USING_LUA

#if MYFW_USING_WX
void ComponentAnimationPlayer::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    MyAssert( gameobjectid.IsOk() );
    //wxTreeItemId id =
    g_pPanelObjectList->AddObject( this, ComponentAnimationPlayer::StaticOnLeftClick, ComponentBase::StaticOnRightClick, gameobjectid, "Animaton Player", ObjectListIcon_Component );
}

void ComponentAnimationPlayer::OnLeftClick(unsigned int count, bool clear)
{
    ComponentBase::OnLeftClick( count, clear );
}

void ComponentAnimationPlayer::FillPropertiesWindow(bool clear, bool addcomponentvariables, bool ignoreblockvisibleflag)
{
    m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "Animation Player", this, ComponentBase::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible || ignoreblockvisibleflag == true )
    {
        ComponentUpdateable::FillPropertiesWindow( clear );

        if( m_pMeshComponent == 0 )
        {
            ComponentBase* pComponent = m_pGameObject->GetFirstComponentOfBaseType( BaseComponentType_Renderable );
            if( pComponent )
                m_pMeshComponent = pComponent->IsA( "MeshOBJComponent" ) ? (ComponentMeshOBJ*)pComponent : 0;
        }
        if( m_pMeshComponent == 0 )
            return;

        MyMesh* pMesh = m_pMeshComponent->m_pMesh;
        if( pMesh == 0 )
            return;

        if( pMesh->GetAnimationCount() > 0 )
        {
            g_pPanelWatch->AddUnsignedInt( "Animation Index", &m_AnimationIndex, 0, (float)pMesh->GetAnimationCount() );
            g_pPanelWatch->AddFloat( "Animation Frame", &m_AnimationTime, 0, 0 );
        }
    }
}

#endif //MYFW_USING_WX

cJSON* ComponentAnimationPlayer::ExportAsJSONObject(bool savesceneid, bool saveid)
{
    cJSON* component = ComponentUpdateable::ExportAsJSONObject( savesceneid, saveid );

    cJSON_AddNumberToObject( component, "AnimIndex", m_AnimationIndex );
    cJSON_AddNumberToObject( component, "AnimFrame", m_AnimationTime );
    
    return component;
}

void ComponentAnimationPlayer::ImportFromJSONObject(cJSON* jsonobj, SceneID sceneid)
{
    ComponentUpdateable::ImportFromJSONObject( jsonobj, sceneid );

    cJSONExt_GetUnsignedInt( jsonobj, "AnimIndex", &m_AnimationIndex );
    cJSONExt_GetFloat( jsonobj, "AnimFrame", &m_AnimationTime );
}

ComponentAnimationPlayer& ComponentAnimationPlayer::operator=(const ComponentAnimationPlayer& other)
{
    MyAssert( &other != this );

    ComponentUpdateable::operator=( other );

    m_AnimationIndex = other.m_AnimationIndex;
    m_AnimationTime = other.m_AnimationTime;

    m_LastAnimationIndex = other.m_LastAnimationIndex;
    m_LastAnimationTime = other.m_LastAnimationTime;

    m_TransitionTimeLeft = other.m_TransitionTimeLeft;
    m_TransitionTimeTotal = other.m_TransitionTimeTotal;

    return *this;
}

void ComponentAnimationPlayer::Tick(float deltaTime)
{
    //ComponentUpdateable::Tick( deltaTime );

    if( m_pMeshComponent == 0 )
    {
        ComponentBase* pComponent = m_pGameObject->GetFirstComponentOfBaseType( BaseComponentType_Renderable );
        if( pComponent )
            m_pMeshComponent = pComponent->IsA( "MeshOBJComponent" ) ? (ComponentMeshOBJ*)pComponent : 0;
    }

    if( m_pMeshComponent == 0 )
        return;

    MyMesh* pMesh = m_pMeshComponent->m_pMesh;

    if( pMesh == 0 )
        return;

    m_AnimationTime += deltaTime;
    m_LastAnimationTime += deltaTime;
    m_TransitionTimeLeft -= deltaTime;

    float perc = 0;
    if( m_TransitionTimeTotal != 0 )
        perc = m_TransitionTimeLeft / m_TransitionTimeTotal;
    pMesh->RebuildAnimationMatrices( m_AnimationIndex, m_AnimationTime, m_LastAnimationIndex, m_LastAnimationTime, perc );

    //if( perc < 1 )
    //    LOGInfo( "Animation", "Blend Perc: %0.2f - %d-%d\n", perc, m_AnimationIndex, m_LastAnimationIndex );
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentAnimationPlayer::SetCurrentAnimation(unsigned int anim)
{
    if( anim == m_AnimationIndex )
        return;

    if( m_pMeshComponent->m_pMesh && anim < m_pMeshComponent->m_pMesh->GetAnimationCount() )
    {
        bool resettransitiontime = true;

        if( anim == m_LastAnimationIndex )
        {
            if( m_TransitionTimeLeft > 0 )
            {
                m_TransitionTimeLeft = m_TransitionTimeTotal - m_TransitionTimeLeft;
                resettransitiontime = false;
            }
        }

        m_LastAnimationIndex = m_AnimationIndex;
        m_LastAnimationTime = m_AnimationTime;

        m_AnimationIndex = anim;
        //m_AnimationTime = 0;

        if( resettransitiontime )
        {
            m_TransitionTimeTotal = 0.25f;
            m_TransitionTimeLeft = m_TransitionTimeTotal;
        }
    }
}
