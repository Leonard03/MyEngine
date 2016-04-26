//
// Copyright (c) 2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "EngineCommonHeader.h"

#if MYFW_USING_WX
bool ComponentGameObjectProperties::m_PanelWatchBlockVisible = true;
#endif

// Component Variable List
MYFW_COMPONENT_IMPLEMENT_VARIABLE_LIST( ComponentGameObjectProperties ); //_VARIABLE_LIST

ComponentGameObjectProperties::ComponentGameObjectProperties()
: ComponentBase()
{
    MYFW_COMPONENT_VARIABLE_LIST_CONSTRUCTOR(); //_VARIABLE_LIST

    ClassnameSanityCheck();

    m_BaseType = BaseComponentType_Data;

    m_Flags = 0;
}

ComponentGameObjectProperties::~ComponentGameObjectProperties()
{
    MYFW_COMPONENT_VARIABLE_LIST_DESTRUCTOR(); //_VARIABLE_LIST
}

void ComponentGameObjectProperties::RegisterVariables(CPPListHead* pList, ComponentGameObjectProperties* pThis) //_VARIABLE_LIST
{
    // just want to make sure these are the same on all compilers.  They should be since this is a simple class.
#if MYFW_IOS || MYFW_OSX || MYFW_NACL
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
    //MyAssert( offsetof( ComponentGameObjectProperties, m_SampleVector3 ) == MyOffsetOf( pThis, &pThis->m_SampleVector3 ) );
#if MYFW_IOS || MYFW_OSX
#pragma GCC diagnostic default "-Winvalid-offsetof"
#endif

    const char** strings = (const char**)g_pEngineCore->m_GameObjectFlagStrings;
    AddVarFlags( pList, "Flags", MyOffsetOf( pThis, &pThis->m_Flags ), true, true, 0, 32, strings, (CVarFunc_ValueChanged)&ComponentGameObjectProperties::OnValueChanged, (CVarFunc_DropTarget)&ComponentGameObjectProperties::OnDrop, 0 );
}

void ComponentGameObjectProperties::Reset()
{
    ComponentBase::Reset();

    m_Flags = 0;

#if MYFW_USING_WX
    m_pPanelWatchBlockVisible = &m_PanelWatchBlockVisible;
#endif //MYFW_USING_WX
}

#if MYFW_USING_LUA
void ComponentGameObjectProperties::LuaRegister(lua_State* luastate)
{
    luabridge::getGlobalNamespace( luastate )
        .beginClass<ComponentSprite>( "ComponentGameObjectProperties" )
            //.addData( "m_SampleVector3", &ComponentSprite::m_SampleVector3 )
            //.addFunction( "GetVector3", &ComponentSprite::GetVector3 )
        .endClass();
}
#endif //MYFW_USING_LUA

#if MYFW_USING_WX
void ComponentGameObjectProperties::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    //wxTreeItemId id =
    g_pPanelObjectList->AddObject( this, ComponentGameObjectProperties::StaticOnLeftClick, ComponentBase::StaticOnRightClick, gameobjectid, "GameObjectProperties", ObjectListIcon_Component );
}

void ComponentGameObjectProperties::OnLeftClick(unsigned int count, bool clear)
{
    ComponentBase::OnLeftClick( count, clear );
}

void ComponentGameObjectProperties::FillPropertiesWindow(bool clear, bool addcomponentvariables, bool ignoreblockvisibleflag)
{
    //m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "GameObjectProperties", this, ComponentBase::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible || ignoreblockvisibleflag == true )
    {
        ComponentBase::FillPropertiesWindow( clear );

        FillPropertiesWindowWithVariables(); //_VARIABLE_LIST
    }
}

void* ComponentGameObjectProperties::OnDrop(ComponentVariable* pVar, wxCoord x, wxCoord y)
{
    void* oldvalue = 0;

    if( g_DragAndDropStruct.m_Type == DragAndDropType_ComponentPointer )
    {
        (ComponentBase*)g_DragAndDropStruct.m_Value;
    }

    if( g_DragAndDropStruct.m_Type == DragAndDropType_GameObjectPointer )
    {
        (GameObject*)g_DragAndDropStruct.m_Value;
    }

    return oldvalue;
}

void* ComponentGameObjectProperties::OnValueChanged(ComponentVariable* pVar, int controlid, bool finishedchanging, double oldvalue)
{
    void* oldpointer = 0;

    if( pVar->m_Offset == MyOffsetOf( this, &m_Flags ) )
    {
        MyAssert( pVar->m_ControlID != -1 );
    }

    return oldpointer;
}
#endif //MYFW_USING_WX

cJSON* ComponentGameObjectProperties::ExportAsJSONObject(bool savesceneid)
{
    cJSON* jComponent = 0;

    // not calling ComponentBase, manually adding the divorced variables, don't want to save ID, Type, etc.
    {
        //cJSON* jComponent = ComponentBase::ExportAsJSONObject( savesceneid );
        jComponent = cJSON_CreateObject();

        // TODO: this will break if more variables are added to a component or it's parents.
        if( m_pGameObject && m_pGameObject->GetGameObjectThisInheritsFrom() != 0 && m_DivorcedVariables != 0 )
            cJSON_AddNumberToObject( jComponent, "Divorced", m_DivorcedVariables );
    }

    ExportVariablesToJSON( jComponent ); //_VARIABLE_LIST

    return jComponent;
}

void ComponentGameObjectProperties::ImportFromJSONObject(cJSON* jComponent, unsigned int sceneid)
{
    ComponentBase::ImportFromJSONObject( jComponent, sceneid );

    ImportVariablesFromJSON( jComponent ); //_VARIABLE_LIST
}

ComponentGameObjectProperties& ComponentGameObjectProperties::operator=(const ComponentGameObjectProperties& other)
{
    MyAssert( &other != this );

    ComponentBase::operator=( other );

    // TODO: replace this with a CopyComponentVariablesFromOtherObject... or something similar.
    m_Flags = other.m_Flags;

    return *this;
}

void ComponentGameObjectProperties::RegisterCallbacks()
{
    if( m_Enabled && m_CallbacksRegistered == false )
    {
        m_CallbacksRegistered = true;

        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, Tick );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, OnSurfaceChanged );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, Draw );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, OnTouch );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, OnButtons );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, OnKeys );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentGameObjectProperties, OnFileRenamed );
    }
}

void ComponentGameObjectProperties::UnregisterCallbacks()
{
    if( m_CallbacksRegistered == true )
    {
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( Tick );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnSurfaceChanged );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( Draw );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnTouch );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnButtons );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnKeys );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnFileRenamed );

        m_CallbacksRegistered = false;
    }
}