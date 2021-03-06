//
// Copyright (c) 2014-2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "MyEnginePCH.h"

#include "ComponentUpdateable.h"
#include "ComponentSystem/Core/GameObject.h"

#if MYFW_USING_WX
bool ComponentUpdateable::m_PanelWatchBlockVisible = true;
#endif

ComponentUpdateable::ComponentUpdateable(ComponentSystemManager* pComponentSystemManager)
: ComponentBase( pComponentSystemManager )
{
    ClassnameSanityCheck();

    m_BaseType = BaseComponentType_Updateable;

    m_pComponentTransform = 0;
}

ComponentUpdateable::~ComponentUpdateable()
{
}

void ComponentUpdateable::Reset()
{
    ComponentBase::Reset();

    m_pComponentTransform = m_pGameObject->GetTransform();

#if MYFW_USING_WX
    m_pPanelWatchBlockVisible = &m_PanelWatchBlockVisible;
#endif //MYFW_USING_WX
}

#if MYFW_USING_WX
void ComponentUpdateable::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    //wxTreeItemId id =
    g_pPanelObjectList->AddObject( this, ComponentUpdateable::StaticOnLeftClick, ComponentBase::StaticOnRightClick, gameobjectid, "Updateable", ObjectListIcon_Component );
}

void ComponentUpdateable::OnLeftClick(unsigned int count, bool clear)
{
    ComponentBase::OnLeftClick( count, clear );
}

void ComponentUpdateable::FillPropertiesWindow(bool clear, bool addcomponentvariables, bool ignoreblockvisibleflag)
{
    //m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "Updateable", this, ComponentBase::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible || ignoreblockvisibleflag == true )
    {
        ComponentBase::FillPropertiesWindow( clear );
    }
}
#endif //MYFW_USING_WX

ComponentUpdateable& ComponentUpdateable::operator=(const ComponentUpdateable& other)
{
    MyAssert( &other != this );

    ComponentBase::operator=( other );

    return *this;
}

//void ComponentUpdateable::Tick(float deltaTime)
//{
//}
