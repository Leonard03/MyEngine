//
// Copyright (c) 2014-2018 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "MyEnginePCH.h"

#include "ComponentTransform.h"
#include "ComponentSystem/Core/ComponentSystemManager.h"
#include "ComponentSystem/Core/GameObject.h"

#if MYFW_USING_WX
bool ComponentTransform::m_PanelWatchBlockVisible = true;
#endif

// Component Variable List
MYFW_COMPONENT_IMPLEMENT_VARIABLE_LIST( ComponentTransform );

MySimplePool<TransformChangedCallbackStruct> ComponentTransform::m_pComponentTransform_TransformChangedCallbackPool;

ComponentTransform::ComponentTransform(ComponentSystemManager* pComponentSystemManager)
: ComponentBase( pComponentSystemManager )
{
    MYFW_COMPONENT_VARIABLE_LIST_CONSTRUCTOR();

    ClassnameSanityCheck();

    m_BaseType = BaseComponentType_Data;

    m_pParentTransform = 0;
}

ComponentTransform::~ComponentTransform()
{
    MYFW_COMPONENT_VARIABLE_LIST_DESTRUCTOR();
}

void ComponentTransform::SystemStartup()
{
    // Create the pool of callback objects.
    if( m_pComponentTransform_TransformChangedCallbackPool.IsInitialized() == false )
        m_pComponentTransform_TransformChangedCallbackPool.AllocateObjects( CALLBACK_POOL_SIZE );
}

void ComponentTransform::SystemShutdown()
{
    m_pComponentTransform_TransformChangedCallbackPool.FreeObjects();
}

void ComponentTransform::RegisterVariables(TCPPListHead<ComponentVariable*>* pList, ComponentTransform* pThis) //_VARIABLE_LIST
{
    // just want to make sure these are the same on all compilers.  They should be since this is a simple class.
#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
    //MyAssert( offsetof( ComponentTransform, m_pParentGameObject ) == MyOffsetOf( pThis, &pThis->m_pParentGameObject ) );
    MyAssert( offsetof( ComponentTransform, m_pParentTransform )  == MyOffsetOf( pThis, &pThis->m_pParentTransform )  );
    MyAssert( offsetof( ComponentTransform, m_LocalPosition )     == MyOffsetOf( pThis, &pThis->m_LocalPosition )     );
    MyAssert( offsetof( ComponentTransform, m_LocalRotation )     == MyOffsetOf( pThis, &pThis->m_LocalRotation )     );
    MyAssert( offsetof( ComponentTransform, m_LocalScale )        == MyOffsetOf( pThis, &pThis->m_LocalScale )        );
#if __GNUC__
#pragma GCC diagnostic pop
#endif

    //AddVar( pList, "ParentGOID",      ComponentVariableType_GameObjectPtr,    MyOffsetOf( pThis, &pThis->m_pParentGameObject ),  true, false, 0,                  (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
#if MYFW_USING_WX
    AddVar( pList, "ParentTransform", ComponentVariableType_ComponentPtr,     MyOffsetOf( pThis, &pThis->m_pParentTransform ),  false, false, "Parent Transform", (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged, (CVarFunc_DropTarget)&ComponentTransform::OnDropTransform, 0 );
#else
    AddVar( pList, "ParentTransform", ComponentVariableType_ComponentPtr,     MyOffsetOf( pThis, &pThis->m_pParentTransform ),  false, false, "Parent Transform", (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged, 0, 0 );
#endif
    AddVar( pList, "WorldPos",        ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_WorldPosition ),     false,  true, "World Pos",        (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    AddVar( pList, "WorldRot",        ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_WorldRotation ),     false,  true, "World Rot",        (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    AddVar( pList, "WorldScale",      ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_WorldScale ),        false,  true, "World Scale",      (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );

    // FillPropertiesWindow() changes m_DisplayInWatch for these 3, will show if parented, won't otherwise.
    AddVar( pList, "Pos",             ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_LocalPosition ),      true, false, "Local Pos",        (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    AddVar( pList, "Rot",             ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_LocalRotation ),      true, false, "Local Rot",        (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    AddVar( pList, "Scale",           ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_LocalScale ),         true, false, "Local Scale",      (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );

//#if MYFW_USING_WX
//    //ComponentVariable* pVarWorldScale = FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "WorldScale" );
//    //pVarWorldScale->m_FloatLowerLimit = 0;
//    //pVarWorldScale->m_FloatUpperLimit = 1000000.0f;
//
//    //ComponentVariable* pVarScale = FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Scale" );
//    //pVarScale->m_FloatLowerLimit = 0;
//    //pVarScale->m_FloatUpperLimit = 1000000.0f;
//#endif
}

void ComponentTransform::Reset()
{
    ComponentBase::Reset();

    m_pParentTransform = 0;

    m_WorldTransform.SetIdentity();
    m_WorldTransformIsDirty = true;
    m_WorldPosition.Set( 0,0,0 );
    m_WorldRotation.Set( 0,0,0 );
    m_WorldScale.Set( 1,1,1 );

    m_LocalTransform.SetIdentity();
    m_LocalTransformIsDirty = true;
    m_LocalPosition.Set( 0,0,0 );
    m_LocalRotation.Set( 0,0,0 );
    m_LocalScale.Set( 1,1,1 );

#if MYFW_USING_WX
    //m_ControlID_ParentTransform = -1;
    m_pPanelWatchBlockVisible = &m_PanelWatchBlockVisible;
#endif //MYFW_USING_WX
}

#if MYFW_USING_LUA
void ComponentTransform::LuaRegister(lua_State* luastate)
{
    luabridge::getGlobalNamespace( luastate )
        .beginClass<ComponentTransform>( "ComponentTransform" )
            .addData( "localtransform", &ComponentTransform::m_LocalTransform ) // MyMatrix

            .addFunction( "GetWorldTransform", &ComponentTransform::GetWorldTransform ) // MyMatrix* ComponentTransform::GetWorldTransform(bool markdirty)
            .addFunction( "SetWorldTransform", &ComponentTransform::SetWorldTransform ) // void ComponentTransform::SetWorldTransform(MyMatrix* mat)
            .addFunction( "UpdateWorldSRT",    &ComponentTransform::UpdateWorldSRT )    // void ComponentTransform::UpdateWorldSRT()

            .addFunction( "GetLocalTransform", &ComponentTransform::GetLocalTransform ) // MyMatrix* ComponentTransform::GetLocalTransform(bool markdirty)
            .addFunction( "SetLocalTransform", &ComponentTransform::SetLocalTransform ) // void ComponentTransform::SetLocalTransform(MyMatrix* mat)
            .addFunction( "UpdateLocalSRT",    &ComponentTransform::UpdateLocalSRT )    // void ComponentTransform::UpdateLocalSRT()

            .addFunction( "SetLocalPosition", &ComponentTransform::SetLocalPosition ) // void ComponentTransform::SetLocalPosition(Vector3 pos)
            .addFunction( "SetLocalRotation", &ComponentTransform::SetLocalRotation ) // void ComponentTransform::SetLocalRotation(Vector3 rot)
            .addFunction( "SetLocalScale",    &ComponentTransform::SetLocalScale )    // void ComponentTransform::SetLocalScale(Vector3 scale)
            .addFunction( "GetLocalPosition", &ComponentTransform::GetLocalPosition ) // Vector3 ComponentTransform::GetLocalPosition()
            .addFunction( "GetLocalRotation", &ComponentTransform::GetLocalRotation ) // Vector3 ComponentTransform::GetLocalRotation()
            .addFunction( "GetLocalScale",    &ComponentTransform::GetLocalScale )    // Vector3 ComponentTransform::GetLocalScale()

            .addFunction( "SetWorldPosition", &ComponentTransform::SetWorldPosition ) // void ComponentTransform::SetWorldPosition(Vector3 pos)
            .addFunction( "SetWorldRotation", &ComponentTransform::SetWorldRotation ) // void ComponentTransform::SetWorldRotation(Vector3 rot)
            .addFunction( "SetWorldScale",    &ComponentTransform::SetWorldScale )    // void ComponentTransform::SetWorldScale(Vector3 scale)
            .addFunction( "GetWorldPosition", &ComponentTransform::GetWorldPosition ) // Vector3 ComponentTransform::GetWorldPosition()
            .addFunction( "GetWorldRotation", &ComponentTransform::GetWorldRotation ) // Vector3 ComponentTransform::GetWorldRotation()
            .addFunction( "GetWorldScale",    &ComponentTransform::GetWorldScale )    // Vector3 ComponentTransform::GetWorldScale()

            .addFunction( "LookAt", &ComponentTransform::LookAt ) // void ComponentTransform::LookAt(Vector3 pos)
        .endClass();
}
#endif //MYFW_USING_LUA

#if MYFW_EDITOR
bool ComponentTransform::IsAnyParentInList(std::vector<GameObject*>& gameobjects)
{
    ComponentTransform* pTransform = m_pParentTransform;

    while( pTransform )
    {
        for( unsigned int i=0; i<gameobjects.size(); i++ )
        {
            GameObject* pGameObject = gameobjects[i];
            MyAssert( pGameObject->IsA( "GameObject" ) );

            if( pGameObject == pTransform->m_pGameObject )
                return true;
        }

        pTransform = pTransform->m_pParentTransform;
    }

    return false;
}
#endif //MYFW_EDITOR

#if MYFW_USING_WX
void ComponentTransform::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    wxTreeItemId id = g_pPanelObjectList->AddObject( this, ComponentTransform::StaticOnLeftClick, ComponentBase::StaticOnRightClick, gameobjectid, "Transform", ObjectListIcon_Component );
    g_pPanelObjectList->SetDragAndDropFunctions( id, ComponentBase::StaticOnDrag, ComponentBase::StaticOnDrop );
}

void ComponentTransform::OnLeftClick(unsigned int count, bool clear)
{
    ComponentBase::OnLeftClick( count, clear );
}

void ComponentTransform::FillPropertiesWindow(bool clear, bool addcomponentvariables, bool ignoreblockvisibleflag)
{
    m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "Transform", this, ComponentBase::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible || ignoreblockvisibleflag == true )
    {
        ComponentBase::FillPropertiesWindow( clear );

        //const char* desc = "none";
        //if( m_pParentTransform )
        //{
        //    MyAssert( m_pParentGameObject == m_pParentTransform->m_pGameObject );
        //    desc = m_pParentTransform->m_pGameObject->GetName();
        //}
        //m_ControlID_ParentTransform = g_pPanelWatch->AddPointerWithDescription( "Parent transform", m_pParentTransform, desc, this, ComponentTransform::StaticOnDropTransform, ComponentTransform::StaticOnValueChanged );

        //g_pPanelWatch->AddVector3( "Pos", &m_Position, 0.0f, 0.0f, this, ComponentTransform::StaticOnValueChanged );
        //g_pPanelWatch->AddVector3( "Rot", &m_Rotation, 0, 0, this, ComponentTransform::StaticOnValueChanged );
        //g_pPanelWatch->AddVector3( "Scale", &m_Scale, 0.0f, 0.0f, this, ComponentTransform::StaticOnValueChanged );

        if( m_pParentTransform )
        {
            FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Pos" )->m_DisplayInWatch = true;
            FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Rot" )->m_DisplayInWatch = true;
            FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Scale" )->m_DisplayInWatch = true;
        }
        else
        {
            FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Pos" )->m_DisplayInWatch = false;
            FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Rot" )->m_DisplayInWatch = false;
            FindComponentVariableByLabel( &m_ComponentVariableList_ComponentTransform, "Scale" )->m_DisplayInWatch = false;
        }

        if( addcomponentvariables )
            FillPropertiesWindowWithVariables(); //_VARIABLE_LIST
    }
}
#endif //MYFW_USING_WX

#if MYFW_EDITOR
void* ComponentTransform::OnDropTransform(ComponentVariable* pVar, bool changedByInterface, int x, int y)
{
    void* oldPointer = 0;

    ComponentTransform* pComponent = 0;

    DragAndDropItem* pDropItem = g_DragAndDropStruct.GetItem( 0 );

    if( pDropItem->m_Type == DragAndDropType_ComponentPointer )
    {
        pComponent = (ComponentTransform*)pDropItem->m_Value;
    }

    if( pDropItem->m_Type == DragAndDropType_GameObjectPointer )
    {
        pComponent = ((GameObject*)pDropItem->m_Value)->GetTransform();
    }

    if( pComponent )
    {
        if( pComponent == this )
            return 0;

        if( pComponent->IsA( "TransformComponent" ) )
        {
            if( pDropItem->m_Type == DragAndDropType_ComponentPointer )
            {
                oldPointer = this->m_pParentTransform;
            }

            if( pDropItem->m_Type == DragAndDropType_GameObjectPointer )
            {
                if( this->m_pParentTransform )
                    oldPointer = this->m_pParentTransform->m_pGameObject;
            }

            this->m_pGameObject->SetParentGameObject( pComponent->m_pGameObject );
        }

#if MYFW_USING_WX
        // update the panel so new OBJ name shows up.
        g_pPanelWatch->GetVariableProperties( g_DragAndDropStruct.GetControlID() )->m_Description = m_pParentTransform->m_pGameObject->GetName();
#endif //MYFW_USING_WX
    }

    return oldPointer;
}

void* ComponentTransform::OnValueChanged(ComponentVariable* pVar, bool changedByInterface, bool finishedChanging, double oldValue, ComponentVariableValue* pNewValue)
{
    void* oldpointer = 0;

    if( pVar->m_Offset == MyOffsetOf( this, &m_pParentTransform ) )
    {
        if( changedByInterface )
        {
#if MYFW_USING_WX
            wxString text = g_pPanelWatch->GetVariableProperties( pVar->m_ControlID )->GetTextCtrl()->GetValue();
            if( text == "" || text == "none" )
            {
                g_pPanelWatch->ChangeDescriptionForPointerWithDescription( pVar->m_ControlID, "none" );
                oldpointer = this->m_pParentTransform;
                this->SetParentTransform( 0 );
            }
#endif //MYFW_USING_WX
        }
        else //if( pNewValue->GetComponentPtr() != 0 )
        {
            MyAssert( false ); // this block is untested
            oldpointer = this->GetParentTransform();
            this->SetParentTransform( pNewValue ? (ComponentTransform*)pNewValue->GetComponentPtr() : 0 );
        }
    }
    else
    {
        if( pVar->m_Offset == MyOffsetOf( this, &m_WorldPosition ) ||
            pVar->m_Offset == MyOffsetOf( this, &m_WorldRotation ) ||
            pVar->m_Offset == MyOffsetOf( this, &m_WorldScale ) )
        {
            if( m_pParentTransform == 0 )
            {
                m_LocalPosition = m_WorldPosition;
                m_LocalRotation = m_WorldRotation;
                m_LocalScale = m_WorldScale;
                m_LocalTransformIsDirty = true;
            }
            else
            {
                // calculate new local transform matrix, decompose it into local SRT.
                MyMatrix matworld;
                matworld.CreateSRT( m_WorldScale, m_WorldRotation, m_WorldPosition );

                MyMatrix matparentworld = *m_pParentTransform->GetWorldTransform();
                matparentworld.Inverse();
                MyMatrix matlocal = matparentworld * matworld;
                
                SetLocalTransform( &matlocal );
                UpdateLocalSRT();
            }

            m_WorldTransformIsDirty = true;
            UpdateTransform();

            for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
            {
                TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

                pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
            }
        }
        else if( pVar->m_Offset == MyOffsetOf( this, &m_LocalPosition ) ||
                 pVar->m_Offset == MyOffsetOf( this, &m_LocalRotation ) ||
                 pVar->m_Offset == MyOffsetOf( this, &m_LocalScale ) )
        {
            m_LocalTransformIsDirty = true;

            for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
            {
                TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

                pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
            }
        }
    }

    return oldpointer;
}
#endif //MYFW_EDITOR

cJSON* ComponentTransform::ExportAsJSONObject(bool savesceneid, bool saveid)
{
    cJSON* jComponent = ComponentBase::ExportAsJSONObject( savesceneid, saveid );

    return jComponent;
}

void ComponentTransform::ImportFromJSONObject(cJSON* jsonobj, SceneID sceneid)
{
    // moved into GameObject, here for old scene files.
    {
        // import the parent goid, set the parent, then import the rest.        
        //ImportVariablesFromJSON( jsonobj, "ParentGOID" );
        unsigned int parentgoid = 0;
        cJSONExt_GetUnsignedInt( jsonobj, "ParentGOID", &parentgoid );

        if( parentgoid > 0 )
        {
            GameObject* pParentGameObject = g_pComponentSystemManager->FindGameObjectByID( sceneid, parentgoid );
            MyAssert( pParentGameObject );

            if( pParentGameObject )
            {
                m_pGameObject->SetParentGameObject( pParentGameObject );
                m_WorldTransformIsDirty = true;
            }
            m_LocalTransformIsDirty = true;
        }
    }

    // load all the registered variables.
    ComponentBase::ImportFromJSONObject( jsonobj, sceneid );

    // local scale/rotation/position should be loaded, update the transform.
    m_LocalTransformIsDirty = true;
    UpdateTransform();

    // inform all children/other objects that our transform changed.
    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

cJSON* ComponentTransform::ExportLocalTransformAsJSONObject()
{
    cJSON* jComponent = cJSON_CreateObject();

    //AddVar( pList, "Pos",             ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_LocalPosition ),      true, false, "Local Pos",        (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    //AddVar( pList, "Rot",             ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_LocalRotation ),      true, false, "Local Rot",        (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    //AddVar( pList, "Scale",           ComponentVariableType_Vector3,          MyOffsetOf( pThis, &pThis->m_LocalScale ),         true, false, "Local Scale",      (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged,                                                         0, 0 );
    cJSONExt_AddFloatArrayToObject( jComponent, "Pos",   &this->m_LocalPosition.x, 3 );
    cJSONExt_AddFloatArrayToObject( jComponent, "Rot",   &this->m_LocalRotation.x, 3 );
    cJSONExt_AddFloatArrayToObject( jComponent, "Scale", &this->m_LocalScale.x, 3 );

    return jComponent;
}

void ComponentTransform::ImportLocalTransformFromJSONObject(cJSON* jsonobj)
{
    cJSONExt_GetFloatArray( jsonobj, "Pos", &this->m_LocalPosition.x, 3 );
    cJSONExt_GetFloatArray( jsonobj, "Rot", &this->m_LocalRotation.x, 3 );
    cJSONExt_GetFloatArray( jsonobj, "Scale", &this->m_LocalScale.x, 3 );

    m_LocalTransformIsDirty = true;
    UpdateTransform();

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

ComponentTransform& ComponentTransform::operator=(const ComponentTransform& other)
{
    MyAssert( &other != this );

    ComponentBase::operator=( other );

    this->m_WorldTransform = other.m_WorldTransform;
    this->m_WorldTransformIsDirty = other.m_WorldTransformIsDirty;
    this->m_WorldPosition = other.m_WorldPosition;
    this->m_WorldRotation = other.m_WorldRotation;
    this->m_WorldScale = other.m_WorldScale;

    this->SetParentTransform( other.m_pParentTransform );

    this->m_LocalTransform = other.m_LocalTransform;
    this->m_LocalTransformIsDirty = other.m_LocalTransformIsDirty;
    this->m_LocalPosition = other.m_LocalPosition;
    this->m_LocalRotation = other.m_LocalRotation;
    this->m_LocalScale = other.m_LocalScale;

    return *this;
}

#if MYFW_EDITOR
void ComponentTransform::SetPositionByEditor(Vector3 pos)
{
    m_LocalPosition = pos;
    m_LocalTransform.CreateSRT( m_LocalScale, m_LocalRotation, m_LocalPosition );

    if( m_pParentTransform == 0 )
    {
        m_WorldPosition = pos;
        m_WorldTransform.CreateSRT( m_WorldScale, m_WorldRotation, m_WorldPosition );
    }
    else
    {
        m_LocalTransformIsDirty = true;
    }

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

void ComponentTransform::SetScaleByEditor(Vector3 scale)
{
    m_LocalScale = scale;
    m_LocalTransform.CreateSRT( m_LocalScale, m_LocalRotation, m_LocalPosition );

    if( m_pParentTransform == 0 )
    {
        m_WorldScale = scale;
        m_WorldTransform.CreateSRT( m_WorldScale, m_WorldRotation, m_WorldPosition );
    }
    else
    {
        m_LocalTransformIsDirty = true;
    }

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

void ComponentTransform::SetRotationByEditor(Vector3 eulerangles)
{
    m_LocalRotation = eulerangles;
    m_LocalTransform.CreateSRT( m_LocalScale, m_LocalRotation, m_LocalPosition );

    if( m_pParentTransform == 0 )
    {
        m_WorldRotation = eulerangles;
        m_WorldTransform.CreateSRT( m_WorldScale, m_WorldRotation, m_WorldPosition );
    }
    else
    {
        m_LocalTransformIsDirty = true;
    }

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}
#endif //MYFW_EDITOR

void ComponentTransform::SetWorldPosition(Vector3 pos)
{
    m_WorldPosition = pos;
    m_WorldTransformIsDirty = true;
    if( m_pParentTransform == 0 )
    {
        m_LocalPosition = m_WorldPosition;
        m_LocalTransformIsDirty = true;
    }

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

void ComponentTransform::SetWorldRotation(Vector3 rot)
{
    m_WorldRotation = rot;
    m_WorldTransformIsDirty = true;
    if( m_pParentTransform == 0 )
    {
        m_LocalRotation = m_WorldRotation;
        m_LocalTransformIsDirty = true;
    }

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

void ComponentTransform::SetWorldScale(Vector3 scale)
{
    m_WorldScale = scale;
    m_WorldTransformIsDirty = true;
    if( m_pParentTransform == 0 )
    {
        m_LocalScale = m_WorldScale;
        m_LocalTransformIsDirty = true;
    }
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::SetLocalTransform(MyMatrix* mat)
{
    m_LocalTransform = *mat;
    UpdateLocalSRT();

    if( m_pParentTransform )
    {
        m_WorldTransform = *m_pParentTransform->GetWorldTransform() * m_LocalTransform;
        UpdateWorldSRT();
    }
    else
    {
        m_WorldTransformIsDirty = false;
        m_WorldTransform = m_LocalTransform;
        m_WorldPosition = m_LocalPosition;
        m_WorldRotation = m_LocalRotation;
        m_WorldScale = m_LocalScale;
    }
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::SetLocalPosition(Vector3 pos)
{
    if( m_LocalPosition == pos )
        return;

    m_LocalPosition = pos;
    m_LocalTransformIsDirty = true;
    if( m_pParentTransform == 0 )
    {
        m_WorldPosition = m_LocalPosition;
        m_WorldTransformIsDirty = true;
    }

    UpdateTransform();

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, true );
    }
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::SetLocalRotation(Vector3 rot)
{
    m_LocalRotation = rot;
    m_LocalTransformIsDirty = true;
    if( m_pParentTransform == 0 )
    {
        m_WorldRotation = m_LocalRotation;
        m_WorldTransformIsDirty = true;
    }

    UpdateTransform();
}

void ComponentTransform::SetLocalScale(Vector3 scale)
{
    m_LocalScale = scale;
    m_LocalTransformIsDirty = true;
    if( m_pParentTransform == 0 )
    {
        m_WorldScale = m_LocalScale;
        m_WorldTransformIsDirty = true;
    }

    UpdateTransform();
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::SetWorldTransform(MyMatrix* mat)
{
    //MyAssert( false );
    m_WorldTransform = *mat;
    UpdateWorldSRT();

    if( m_pParentTransform )
    {
        m_pParentTransform->UpdateTransform();
        m_LocalTransform = m_pParentTransform->m_WorldTransform.GetInverse() * m_WorldTransform;
        UpdateLocalSRT();
    }
    else
    {
        m_LocalTransform = m_WorldTransform;
        UpdateLocalSRT();
    }

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, false );
    }
}

// Exposed to Lua, change elsewhere if function signature changes.
MyMatrix* ComponentTransform::GetWorldTransform(bool markdirty)
{
    UpdateTransform();

    if( markdirty )
        m_WorldTransformIsDirty = true;

    //if( m_pParentTransform == 0 )
    //    return &m_LocalTransform;

    return &m_WorldTransform;
}

Vector3 ComponentTransform::GetWorldPosition()
{
    //return m_WorldTransform.GetTranslation();
    return m_WorldPosition;
}
Vector3 ComponentTransform::GetWorldScale()
{
    //return m_WorldTransform.GetScale();
    return m_WorldScale;
}
Vector3 ComponentTransform::GetWorldRotation()
{
    //return m_WorldTransform.GetEulerAngles();
    return m_WorldRotation;
}

MyMatrix ComponentTransform::GetWorldRotPosMatrix()
{
    MyMatrix world;
    world.CreateSRT( Vector3(1,1,1), m_WorldRotation, m_WorldPosition );

    return world;
}

// Exposed to Lua, change elsewhere if function signature changes.
MyMatrix* ComponentTransform::GetLocalTransform(bool markdirty)
{
    UpdateTransform();

    if( markdirty )
        m_LocalTransformIsDirty = true;

    return &m_LocalTransform;
}

// Exposed to Lua, change elsewhere if function signature changes.
Vector3 ComponentTransform::GetLocalPosition()
{
    //return m_LocalTransform.GetTranslation();
    return m_LocalPosition;
}
Vector3 ComponentTransform::GetLocalScale()
{
    //return m_LocalTransform.GetScale();
    return m_LocalScale;
}
// Exposed to Lua, change elsewhere if function signature changes.
Vector3 ComponentTransform::GetLocalRotation()
{
    //return m_LocalTransform.GetEulerAngles();
    return m_LocalRotation;
}

MyMatrix ComponentTransform::GetLocalRotPosMatrix()
{
    MyMatrix local;
    local.CreateSRT( Vector3(1,1,1), m_LocalRotation, m_LocalPosition );

    return local;
}

void ComponentTransform::Scale(MyMatrix* pScaleMatrix, Vector3 pivot)
{
    MyMatrix worldTransform = *GetWorldTransform();
    worldTransform.Translate( -pivot );
    worldTransform = *pScaleMatrix * worldTransform;
    worldTransform.Translate( pivot );

    SetWorldTransform( &worldTransform );
}

void ComponentTransform::Rotate(MyMatrix* pRotMatrix, Vector3 pivot)
{
    MyMatrix worldTransform = *GetWorldTransform();
    worldTransform.Translate( -pivot );
    worldTransform = *pRotMatrix * worldTransform;
    worldTransform.Translate( pivot );

    SetWorldTransform( &worldTransform );
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::LookAt(Vector3 pos)
{
    MyMatrix temp;
    temp.CreateLookAtWorld( m_WorldPosition, Vector3(0,1,0), pos );

    Vector3 rot = temp.GetEulerAngles() * 180.0f/PI;

    //LOGInfo( "LookAt", "(%0.2f, %0.2f, %0.2f)\n", rot.x, rot.y, rot.z );

    SetWorldRotation( rot );
}

void ComponentTransform::SetParentTransform(ComponentTransform* pNewParentTransform)
{
    if( m_pParentTransform == pNewParentTransform )
        return;

    MyMatrix wantedWorldSpaceTransform;

    // if we had an old parent:
    if( m_pParentTransform != 0 )
    {
        // stop sending old parent position changed messages
        m_pParentTransform->m_pGameObject->GetTransform()->UnregisterTransformChangedCallbacks( this );

        // Maintain our world space position by setting local transform to match world
        wantedWorldSpaceTransform = m_WorldTransform;
        m_LocalPosition = m_WorldPosition;
        m_LocalRotation = m_WorldRotation;
        m_LocalScale = m_WorldScale;

        m_LocalTransformIsDirty = true;
    }
    else
    {
        wantedWorldSpaceTransform = m_WorldTransform;
    }

    if( pNewParentTransform == 0 )
    {
        // If no new parent, set world transform to match local
        m_WorldTransform = wantedWorldSpaceTransform;
        m_WorldPosition = m_LocalPosition;
        m_WorldRotation = m_LocalRotation;
        m_WorldScale = m_LocalScale;

        m_pParentTransform = 0;
    }
    else
    {
        // If there's a new parent, set it as the parent, then recalculate it's world/local transform.
        m_pParentTransform = pNewParentTransform;
        SetWorldTransform( &wantedWorldSpaceTransform );

        // Register this transform with it's parent to notify us if it changes.
        GameObject* pParentGameObject = m_pGameObject->GetParentGameObject();
        pParentGameObject->GetTransform()->RegisterTransformChangedCallback( this, StaticOnParentTransformChanged );
    }

    UpdateTransform();
    UpdateWorldSRT();
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::UpdateWorldSRT()
{
    m_WorldPosition = m_WorldTransform.GetTranslation();
    m_WorldRotation = m_WorldTransform.GetEulerAngles() * 180.0f/PI;
    m_WorldScale = m_WorldTransform.GetScale();
}

// Exposed to Lua, change elsewhere if function signature changes.
void ComponentTransform::UpdateLocalSRT()
{
    m_LocalPosition = m_LocalTransform.GetTranslation();
    m_LocalRotation = m_LocalTransform.GetEulerAngles() * 180.0f/PI;
    m_LocalScale = m_LocalTransform.GetScale();
}

void ComponentTransform::UpdateTransform()
{
    if( m_pParentTransform )
    {
        if( m_WorldTransformIsDirty )
        {
            //MyAssert( m_LocalTransformIsDirty == false );

            m_WorldTransformIsDirty = false;
            m_WorldTransform.CreateSRT( m_WorldScale, m_WorldRotation, m_WorldPosition );
        }

        if( m_LocalTransformIsDirty )
        {
            //MyAssert( m_WorldTransformIsDirty == false );

            m_LocalTransformIsDirty = false;
            m_LocalTransform.CreateSRT( m_LocalScale, m_LocalRotation, m_LocalPosition );

            m_WorldTransform = *m_pParentTransform->GetWorldTransform() * m_LocalTransform;
            UpdateWorldSRT();
        }
    }
    else
    {
        if( m_LocalTransformIsDirty )
        {
            m_LocalTransformIsDirty = false;
            m_LocalTransform.CreateSRT( m_LocalScale, m_LocalRotation, m_LocalPosition );

            m_WorldTransformIsDirty = false;
            m_WorldTransform = m_LocalTransform;
            m_WorldPosition = m_LocalPosition;
            m_WorldRotation = m_LocalRotation;
            m_WorldScale = m_LocalScale;
        }
    }
}

//MyMatrix* ComponentTransform::GetMatrix()
//{
//    return &m_Transform;
//}

void ComponentTransform::RegisterTransformChangedCallback(void* pObj, TransformChangedCallbackFunc* pCallback)
{
    MyAssert( pCallback != 0 );

    TransformChangedCallbackStruct* pCallbackStruct = m_pComponentTransform_TransformChangedCallbackPool.GetObjectFromPool();

    //LOGInfo( "TransformPool", "Grabbed an object (%d) - %s\n", m_pComponentTransform_TransformChangedCallbackPool.GetNumUsed(), ((ComponentBase*)pObj)->m_pGameObject->GetName() );

    if( pCallbackStruct != 0 )
    {
        pCallbackStruct->pObj = pObj;
        pCallbackStruct->pFunc = pCallback;

        m_TransformChangedCallbackList.AddTail( pCallbackStruct );
    }
}

void ComponentTransform::UnregisterTransformChangedCallbacks(void* pObj)
{
    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; )
    {
        CPPListNode* pNextNode = pNode->GetNext();

        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        if( pCallbackStruct->pObj == pObj )
        {
            pCallbackStruct->Remove();
            m_pComponentTransform_TransformChangedCallbackPool.ReturnObjectToPool( pCallbackStruct );

            //LOGInfo( "TransformPool", "Returned an object (%d) %s\n", m_pComponentTransform_TransformChangedCallbackPool.GetNumUsed(), ((ComponentBase*)pObj)->m_pGameObject->GetName() );
        }

        pNode = pNextNode;
    }
}

void ComponentTransform::OnParentTransformChanged(Vector3& newpos, Vector3& newrot, Vector3& newscale, bool changedbyuserineditor)
{
    m_LocalTransformIsDirty = true;
    UpdateTransform();

    for( CPPListNode* pNode = m_TransformChangedCallbackList.GetHead(); pNode != 0; pNode = pNode->GetNext() )
    {
        TransformChangedCallbackStruct* pCallbackStruct = (TransformChangedCallbackStruct*)pNode;

        pCallbackStruct->pFunc( pCallbackStruct->pObj, m_WorldPosition, m_WorldRotation, m_WorldScale, changedbyuserineditor );
    }
}
