//
// Copyright (c) 2014-2018 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "MyEnginePCH.h"

#include "Component3DCollisionObject.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "ComponentSystem/BaseComponents/ComponentTransform.h"
#include "ComponentSystem/FrameworkComponents/Physics3D/Component3DJointBase.h"
#include "ComponentSystem/Core/GameObject.h"
#include "Core/EngineComponentTypeManager.h"
#include "Core/EngineCore.h"

// Component Variable List
MYFW_COMPONENT_IMPLEMENT_VARIABLE_LIST( Component3DCollisionObject );

const char* PhysicsPrimitiveTypeStrings[PhysicsPrimitive_NumTypes] = //ADDING_NEW_PhysicsPrimitiveType
{
    "Cube",
    "Sphere",
    "Static Plane",
    "Convex Hull",
};

Component3DCollisionObject::Component3DCollisionObject(ComponentSystemManager* pComponentSystemManager)
: ComponentBase( pComponentSystemManager )
{
    MYFW_COMPONENT_VARIABLE_LIST_CONSTRUCTOR();

    ClassnameSanityCheck();

    m_BaseType = BaseComponentType_Data;
    m_Type = ComponentType_3DCollisionObject;

    m_pBody = 0;

    m_PrimitiveType = PhysicsPrimitiveType_Cube;
    m_pMesh = 0;
}

Component3DCollisionObject::~Component3DCollisionObject()
{
    m_pGameObject->GetTransform()->UnregisterTransformChangedCallbacks( this );

    if( m_pBody )
    {
        g_pBulletWorld->m_pDynamicsWorld->removeRigidBody( m_pBody );
        delete m_pBody;
    }

    SAFE_RELEASE( m_pMesh );

    MYFW_COMPONENT_VARIABLE_LIST_DESTRUCTOR();
}

void Component3DCollisionObject::RegisterVariables(TCPPListHead<ComponentVariable*>* pList, Component3DCollisionObject* pThis) //_VARIABLE_LIST
{
    ComponentVariable* pVar;

    AddVar( pList, "Mass", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_Mass ), true, true, 0, (CVarFunc_ValueChanged)&Component3DCollisionObject::OnValueChanged, 0, 0 );
    
    AddVarEnum( pList, "Primitive", MyOffsetOf( pThis, &pThis->m_PrimitiveType ), true, true, "Primitive Type", PhysicsPrimitive_NumTypes, PhysicsPrimitiveTypeStrings, (CVarFunc_ValueChanged)&Component3DCollisionObject::OnValueChanged, 0, 0 );

    pVar = AddVar( pList, "Scale", ComponentVariableType_Vector3, MyOffsetOf( pThis, &pThis->m_Scale ), true, true, "Scale", (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged, 0, 0 );
#if MYFW_EDITOR
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&Component3DCollisionObject::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "ScaleX", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_Scale ), false, true, "Scale", (CVarFunc_ValueChanged)&ComponentTransform::OnValueChanged, 0, 0 );
#if MYFW_EDITOR
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&Component3DCollisionObject::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVarPointer( pList, "OBJ", true, true, "Collision Mesh",
        (CVarFunc_GetPointerValue)&Component3DCollisionObject::GetPointerValue, (CVarFunc_SetPointerValue)&Component3DCollisionObject::SetPointerValue, (CVarFunc_GetPointerDesc)&Component3DCollisionObject::GetPointerDesc, (CVarFunc_SetPointerDesc)&Component3DCollisionObject::SetPointerDesc,
        (CVarFunc_ValueChanged)&Component3DCollisionObject::OnValueChanged, (CVarFunc_DropTarget)&Component3DCollisionObject::OnDropOBJ, 0 );
#if MYFW_EDITOR
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&Component3DCollisionObject::ShouldVariableBeAddedToWatchPanel) );
#endif
}

void Component3DCollisionObject::Reset()
{
    ComponentBase::Reset();

    m_PrimitiveType = PhysicsPrimitiveType_Cube;

    m_Mass = 0;
    m_Scale.Set( 1,1,1 );
    SAFE_RELEASE( m_pMesh );

#if MYFW_USING_WX
    m_pPanelWatchBlockVisible = &m_PanelWatchBlockVisible;
    m_ControlID_PrimitiveType = -1;

    m_pGameObject->GetTransform()->RegisterTransformChangedCallback( this, StaticOnTransformChanged );
#endif //MYFW_USING_WX
}

void* Component3DCollisionObject::GetPointerValue(ComponentVariable* pVar) //_VARIABLE_LIST
{
    if( strcmp( pVar->m_Label, "OBJ" ) == 0 )
    {
        if( m_pMesh )
            return m_pMesh->GetFile();
    }

    return 0;
}

void Component3DCollisionObject::SetPointerValue(ComponentVariable* pVar, const void* newvalue) //_VARIABLE_LIST
{
    if( strcmp( pVar->m_Label, "OBJ" ) == 0 )
    {
        MeshManager* pMeshManager = m_pComponentSystemManager->GetEngineCore()->GetManagers()->GetMeshManager();
        MyMesh* pMesh = pMeshManager->FindMeshBySourceFile( (MyFileObject*)newvalue );
        SetMesh( pMesh );
    }
}

const char* Component3DCollisionObject::GetPointerDesc(ComponentVariable* pVar) //_VARIABLE_LIST
{
    if( strcmp( pVar->m_Label, "OBJ" ) == 0 )
    {
        //MyAssert( m_pMesh );
        if( m_pMesh == 0 )
            return "none";

        MyFileObject* pFile = m_pMesh->GetFile();
        if( pFile )
            return pFile->GetFullPath();
        else
            return "none";
    }

    return "fix me";
}

void Component3DCollisionObject::SetPointerDesc(ComponentVariable* pVar, const char* newdesc) //_VARIABLE_LIST
{
    if( strcmp( pVar->m_Label, "OBJ" ) == 0 )
    {
        MyAssert( newdesc );
        if( newdesc )
        {
            FileManager* pFileManager = m_pComponentSystemManager->GetEngineCore()->GetManagers()->GetFileManager();
            MyFileObject* pFile = pFileManager->RequestFile( newdesc ); // adds a ref
            if( pFile )
            {
                MeshManager* pMeshManager = m_pComponentSystemManager->GetEngineCore()->GetManagers()->GetMeshManager();

                MyMesh* pMesh = pMeshManager->FindMeshBySourceFile( pFile ); // doesn't add a ref
                if( pMesh == 0 )
                {
                    pMesh = MyNew MyMesh( m_pComponentSystemManager->GetEngineCore() );
                    pMesh->SetSourceFile( pFile );
                    SetMesh( pMesh );
                    pMesh->Release();
                }
                else
                {
                    SetMesh( pMesh );
                }
                pFile->Release(); // free ref from RequestFile
            }
        }
    }
}

#if MYFW_USING_LUA
void Component3DCollisionObject::LuaRegister(lua_State* luastate)
{
    luabridge::getGlobalNamespace( luastate )
        .beginClass<Component3DCollisionObject>( "Component3DCollisionObject" )
            .addData( "mass", &Component3DCollisionObject::m_Mass ) // float
            .addFunction( "ApplyForce", &Component3DCollisionObject::ApplyForce ) // void Component3DCollisionObject::ApplyForce(Vector3 force, Vector3 relpos)
        .endClass();
}
#endif //MYFW_USING_LUA

#if MYFW_EDITOR
#if MYFW_USING_WX
void Component3DCollisionObject::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    //wxTreeItemId id =
    g_pPanelObjectList->AddObject( this, Component3DCollisionObject::StaticOnLeftClick, ComponentBase::StaticOnRightClick, gameobjectid, "3D Collision object", ObjectListIcon_Component );
}

void Component3DCollisionObject::OnLeftClick(unsigned int count, bool clear)
{
    ComponentBase::OnLeftClick( count, clear );
}

void Component3DCollisionObject::FillPropertiesWindow(bool clear, bool addcomponentvariables, bool ignoreblockvisibleflag)
{
    m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "3D Collision Object", this, ComponentBase::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible || ignoreblockvisibleflag == true )
    {
        ComponentBase::FillPropertiesWindow( clear );

        FillPropertiesWindowWithVariables(); //_VARIABLE_LIST
    }
}

void Component3DCollisionObject::OnTransformChanged(Vector3& newpos, Vector3& newrot, Vector3& newscale, bool changedbyuserineditor)
{
    if( changedbyuserineditor )
        SyncRigidBodyToTransform();
}
#endif //MYFW_USING_WX

void* Component3DCollisionObject::OnDropOBJ(ComponentVariable* pVar, bool changedByInterface, int x, int y)
{
    void* oldPointer = 0;

    DragAndDropItem* pDropItem = g_DragAndDropStruct.GetItem( 0 );

    if( pDropItem->m_Type == DragAndDropType_FileObjectPointer )
    {
        MyFileObject* pFile = (MyFileObject*)pDropItem->m_Value;
        MyAssert( pFile );

        const char* pExtension = pFile->GetExtensionWithDot();

        if( strcmp( pExtension, ".obj" ) == 0 ||
            strcmp( pExtension, ".mymesh" ) == 0 )
        {
            if( m_pMesh )
                oldPointer = m_pMesh->GetFile();

            MeshManager* pMeshManager = m_pComponentSystemManager->GetEngineCore()->GetManagers()->GetMeshManager();

            MyMesh* pNewMesh = pMeshManager->FindMeshBySourceFile( pFile );
            SetMesh( pNewMesh );
        }
    }

    return oldPointer;
}

bool Component3DCollisionObject::ShouldVariableBeAddedToWatchPanel(ComponentVariable* pVar)
{
    switch( m_PrimitiveType )
    {
    case PhysicsPrimitiveType_Cube:
        {
            if( strcmp( pVar->m_Label, "Scale" ) == 0 )
                return true;
        }
        break;

    case PhysicsPrimitiveType_Sphere:
        {
            if( strcmp( pVar->m_Label, "ScaleX" ) == 0 )
                return true;
        }
        break;

    case PhysicsPrimitiveType_StaticPlane:
        {
            // nothing here, maybe add an pos/rot offset, but ATM uses object transform.
        }
        break;

    case PhysicsPrimitiveType_ConvexHull:
        {
            if( strcmp( pVar->m_Label, "OBJ" ) == 0 )
                return true;
        }
        break;
    }

    return false;
}

void* Component3DCollisionObject::OnValueChanged(ComponentVariable* pVar, bool changedByInterface, bool finishedChanging, double oldValue, ComponentVariableValue* pNewValue)
{
    void* oldpointer = 0;

    if( finishedChanging )
    {
        if( strcmp( pVar->m_Label, "Primitive" ) == 0 )
        {
            // TODO: rethink this, doesn't need refresh if panel isn't visible.
#if MYFW_USING_WX
            g_pPanelWatch->SetNeedsRefresh();
#endif //MYFW_USING_WX
        }
    }

    return oldpointer;
}
#endif //MYFW_EDITOR

cJSON* Component3DCollisionObject::ExportAsJSONObject(bool savesceneid, bool saveid)
{
    cJSON* component = ComponentBase::ExportAsJSONObject( savesceneid, saveid );

    return component;
}

void Component3DCollisionObject::ImportFromJSONObject(cJSON* jsonobj, SceneID sceneid)
{
    ComponentBase::ImportFromJSONObject( jsonobj, sceneid );
}

Component3DCollisionObject& Component3DCollisionObject::operator=(const Component3DCollisionObject& other)
{
    MyAssert( &other != this );

    ComponentBase::operator=( other );

    m_PrimitiveType = other.m_PrimitiveType;

    m_Mass = other.m_Mass;
    m_Scale = other.m_Scale;

    SetMesh( other.m_pMesh );

    return *this;
}

void Component3DCollisionObject::RegisterCallbacks()
{
    MyAssert( m_EnabledState == EnabledState_Enabled );

    if( m_CallbacksRegistered == false )
    {
        m_CallbacksRegistered = true;

        MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, Tick );
        //MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, OnSurfaceChanged );
        //MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, Draw );
        //MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, OnTouch );
        //MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, OnButtons );
        //MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, OnKeys );
        //MYFW_REGISTER_COMPONENT_CALLBACK( Component3DCollisionObject, OnFileRenamed );
    }
}

void Component3DCollisionObject::UnregisterCallbacks()
{
    MyAssert( m_EnabledState != EnabledState_Enabled );

    if( m_CallbacksRegistered == true )
    {
        MYFW_UNREGISTER_COMPONENT_CALLBACK( Tick );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnSurfaceChanged );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( Draw );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnTouch );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnButtons );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnKeys );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnFileRenamed );

        m_CallbacksRegistered = false;
    }
}

void Component3DCollisionObject::SetMesh(MyMesh* pMesh)
{
    if( pMesh )
        pMesh->AddRef();

    SAFE_RELEASE( m_pMesh );
    m_pMesh = pMesh;
}

void Component3DCollisionObject::OnPlay()
{
    ComponentBase::OnPlay();

    //// set the collision object scale on play, guess this should be set whenever the object is scaled.
    ////   TODO: find a better way to handle the object being scaled in editor.
    //Vector3 localscale = m_pGameObject->GetTransform()->GetScale();
    //btVector3 scale( localscale.x, localscale.y, localscale.z );
    //m_pBody->getCollisionShape()->setLocalScaling( scale );
    
    CreateBody();
}

void Component3DCollisionObject::CreateBody()
{
    // create a rigidbody on start
    if( m_pBody == 0 )
    {
        btCollisionShape* colShape = 0;
        
        if( m_PrimitiveType == PhysicsPrimitiveType_ConvexHull )
        {
            if( m_pMesh && m_pMesh->IsReady() )
            {
                //btStridingMeshInterface meshinterface;
                //colShape = new btBvhTriangleMeshShape( meshinterface );

                // TODO: fix this, it assumes position at the start of the vertex format.
                unsigned int stride = m_pMesh->GetStride( 0 );
                colShape = new btConvexHullShape( (btScalar*)m_pMesh->GetVerts( false ), m_pMesh->GetNumVerts(), stride );
                ((btConvexHullShape*)colShape)->setMargin( 0.2f );

                btShapeHull* hull = new btShapeHull( (btConvexShape*)colShape );
                btScalar margin = colShape->getMargin();
                hull->buildHull(margin);
                delete colShape;

                colShape = new btConvexHullShape( (btScalar*)hull->getVertexPointer(), hull->numVertices() );
                delete hull;
            }
            else
            {
                LOGInfo( LOGTag, "Mesh not loaded, ConvexHull not created\n" ); 
                return;
            }
        }
        else
        {
            if( m_PrimitiveType == PhysicsPrimitiveType_Cube )
            {
                colShape = new btBoxShape( btVector3(m_Scale.x/2, m_Scale.y/2, m_Scale.z/2) ); // half-extents
            }
            else if( m_PrimitiveType == PhysicsPrimitiveType_Sphere )
            {
                colShape = new btSphereShape( btScalar(m_Scale.x) );
            }
            else if( m_PrimitiveType == PhysicsPrimitiveType_StaticPlane )
            {
                // plane is flat on x/z and at origin, rigidbody will have transform to place plane in space.
                colShape = new btStaticPlaneShape( btVector3(0, 1, 0), 0 );
            }
        }

        g_pBulletWorld->m_CollisionShapes.push_back( colShape );

        // Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        //btScalar mass( 1.0f );

        // rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (m_Mass != 0.0f);

        btVector3 localInertia( 0,0,0 );
        if( isDynamic )
            colShape->calculateLocalInertia( m_Mass, localInertia );

        //btVector3 pos(m_pGameObject->GetTransform()->m_Position.x, m_pGameObject->GetTransform()->m_Position.y, m_pGameObject->GetTransform()->m_Position.z );
        //startTransform.setOrigin( pos );
        Vector3 localscale = m_pGameObject->GetTransform()->GetLocalScale();
        btVector3 scale( localscale.x, localscale.y, localscale.z );
        colShape->setLocalScaling( scale );

        MyMatrix localmat = m_pGameObject->GetTransform()->GetLocalRotPosMatrix(); //GetLocalTransform();
        startTransform.setFromOpenGLMatrix( &localmat.m11 );

        // using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState( startTransform );
        btRigidBody::btRigidBodyConstructionInfo rbInfo( m_Mass, myMotionState, colShape, localInertia );
        //rbInfo.m_friction = 1; // TODO: expose friction
        m_pBody = new btRigidBody( rbInfo );
        
        g_pBulletWorld->m_pDynamicsWorld->addRigidBody( m_pBody );
    }
}

void Component3DCollisionObject::OnStop()
{
    ComponentBase::OnStop();

    // Loop through joints/constraints on this gameobject and remove them before removing body.
    for( unsigned int i=0; i<m_pGameObject->GetComponentCount(); i++ )
    {
        ComponentBase* pComponent = m_pGameObject->GetComponentByIndex( i );
        if( pComponent->IsA( "3DJoint-" ) )
        {
            ((Component3DJointBase*)pComponent)->RemoveJointFromWorld();
        }
    }

    if( m_pBody )
    {
        g_pBulletWorld->m_pDynamicsWorld->removeRigidBody( m_pBody );
        SAFE_DELETE( m_pBody );
    }
}

void Component3DCollisionObject::TickCallback(float deltaTime)
{
    if( deltaTime == 0 )
    {
        return;
    }

    // if gameplay started before the mesh was loaded, the body won't be made yet.
    // make it now if possible.
    if( m_pBody == 0 )
    {
        CreateBody();

        // if we still don't have a body, return.
        if( m_pBody == 0 )
            return;
    }

    // TODO: MotionStates are coming up 0 on the mac, test on pc.
    btMotionState* pMotionState = m_pBody->getMotionState();
    if( pMotionState == 0 )
        return;

    btTransform transform;
    pMotionState->getWorldTransform( transform );
    MyMatrix matRotPos;
    MyMatrix matBulletGL;
    transform.getOpenGLMatrix( &matBulletGL.m11 );

    // if the collisionshape is scaled, scale our object to match.
    btVector3 scale = m_pBody->getCollisionShape()->getLocalScaling();
    if( scale.x() != 1 || scale.y() != 1 || scale.z() != 1 )
    {
        MyMatrix matScale;
        matScale.CreateScale( scale.x(), scale.y(), scale.z() );
        matBulletGL = matBulletGL * matScale;
    }

    m_pGameObject->GetTransform()->SetWorldTransform( &matBulletGL );

    //btVector3 pos = transform.getOrigin();
    //btQuaternion rot = transform.getRotation();
    //m_pGameObject->GetTransform()->SetPosition( Vector3( pos.getX(), pos.getY(), pos.getZ() ) );
    //m_pGameObject->GetTransform()->SetRotation( Vector3( rot.g, pos.getY(), pos.getZ() ) );
}

void Component3DCollisionObject::SyncRigidBodyToTransform()
{
    if( m_pBody == 0 )
        return;

    btTransform transform;
    //btVector3 pos(m_pGameObject->GetTransform()->m_Position.x, m_pGameObject->GetTransform()->m_Position.y, m_pGameObject->GetTransform()->m_Position.z );
    //transform.setIdentity();
    //transform.setOrigin( pos );
    MyMatrix localmat = m_pGameObject->GetTransform()->GetLocalRotPosMatrix();
    transform.setFromOpenGLMatrix( &localmat.m11 );

    m_pBody->getMotionState()->setWorldTransform( transform );
    m_pBody->setWorldTransform( transform );

    m_pBody->activate( true );

    g_pBulletWorld->m_pDynamicsWorld->removeRigidBody( m_pBody );
    g_pBulletWorld->m_pDynamicsWorld->addRigidBody( m_pBody );
}

// Exposed to Lua, change elsewhere if function signature changes.
void Component3DCollisionObject::ApplyForce(Vector3 force, Vector3 relpos)
{
    btVector3 btforce( force.x, force.y, force.z );
    btVector3 btrelpos( relpos.x, relpos.y, relpos.z );

    m_pBody->activate();
    m_pBody->applyForce( btforce, btrelpos );
}
