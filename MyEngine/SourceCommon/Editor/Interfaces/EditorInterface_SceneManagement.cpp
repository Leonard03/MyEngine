//
// Copyright (c) 2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "EngineCommonHeader.h"

EditorInterface_SceneManagement::EditorInterface_SceneManagement()
{
}

EditorInterface_SceneManagement::~EditorInterface_SceneManagement()
{
}

void EditorInterface_SceneManagement::OnActivated()
{
    EditorState* pEditorState = g_pEngineCore->m_pEditorState;
    pEditorState->m_pTransformGizmo->m_VisibleIfObjectsSelected = true;
}

void EditorInterface_SceneManagement::OnDeactivated()
{
    EditorState* pEditorState = g_pEngineCore->m_pEditorState;
    pEditorState->m_pTransformGizmo->m_VisibleIfObjectsSelected = false;
}

void EditorInterface_SceneManagement::OnDrawFrame(unsigned int canvasid)
{
    MyAssert( canvasid == 1 );

    EditorState* pEditorState = g_pEngineCore->m_pEditorState;

    float windowwidth = g_pEngineCore->m_WindowWidth;
    float windowheight = g_pEngineCore->m_WindowHeight;

    // EditorInterface class will draw the main editor view
    EditorInterface::OnDrawFrame( canvasid );

    if( g_pEngineCore->m_Debug_DrawSelectedAnimatedMesh && g_GLCanvasIDActive == 1 )
    {
        if( pEditorState->m_pSelectedObjects.size() > 0 )
        {
            // TODO: have the file selecter pick the right game object/mesh
            GameObject* pObject = pEditorState->m_pSelectedObjects[0];

            // if this has an animation player, render the first animation. generally the full timeline.
            ComponentAnimationPlayer* pAnim = pObject->GetAnimationPlayer();
            if( pAnim )
            {
                int backupindex = pAnim->m_AnimationIndex;
                float backuptime = pAnim->m_AnimationTime;

                pAnim->m_AnimationIndex = 0;
                pAnim->m_AnimationTime = (float)MyTime_GetUnpausedTime();
                pAnim->Tick( 0 );

                g_pEngineCore->RenderSingleObject( pObject );

                pAnim->m_AnimationIndex = backupindex;
                pAnim->m_AnimationTime = backuptime;

                if( g_pEngineCore->m_pDebugQuadSprite == 0 )
                    g_pEngineCore->m_pDebugQuadSprite = MyNew MySprite( false );

                g_pEngineCore->m_pDebugQuadSprite->CreateInPlace( "debug", 0.5f, 0.5f, 1.0f, 1.0f, 0, 1, 1, 0, Justify_Center, false );
                g_pEngineCore->m_pMaterial_ClipSpaceTexture->SetTextureColor( pEditorState->m_pDebugViewFBO->m_pColorTexture );
                g_pEngineCore->m_pDebugQuadSprite->SetMaterial( g_pEngineCore->m_pMaterial_ClipSpaceTexture );
                g_pEngineCore->m_pDebugQuadSprite->Draw( 0 );
            }

            // if it's a shadow cam, render the depth texture
            ComponentCameraShadow* pCamera = dynamic_cast<ComponentCameraShadow*>( pObject->GetFirstComponentOfBaseType( BaseComponentType_Camera ) );
            if( pCamera )
            {
                if( g_pEngineCore->m_pDebugQuadSprite == 0 )
                    g_pEngineCore->m_pDebugQuadSprite = MyNew MySprite( false );

                g_pEngineCore->m_pDebugQuadSprite->CreateInPlace( "debug", 0.5f, 0.5f, 1.0f, 1.0f, 0, 1, 1, 0, Justify_Center, false );
                g_pEngineCore->m_pMaterial_ClipSpaceTexture->SetTextureColor( pCamera->m_pDepthFBO->m_pDepthTexture );
                g_pEngineCore->m_pDebugQuadSprite->SetMaterial( g_pEngineCore->m_pMaterial_ClipSpaceTexture );
                g_pEngineCore->m_pDebugQuadSprite->Draw( 0 );
            }
        }
    }

    if( /*m_Debug_DrawSelectedMaterial &&*/ g_GLCanvasIDActive == 1 )
    {
        MaterialDefinition* pMaterial = g_pPanelMemory->GetSelectedMaterial();

        if( pMaterial )
        {
            if( g_pEngineCore->m_pMaterialBallMesh == 0 )
            {
                g_pEngineCore->m_pMaterialBallMesh = MyNew MyMesh();
                g_pEngineCore->m_pSphereMeshFile = RequestFile( "DataEngine/Meshes/sphere.obj.mymesh" );
            }

            if( g_pEngineCore->m_pMaterialBallMesh && g_pEngineCore->m_pMaterialBallMesh->m_MeshReady == false )
            {
                if( g_pEngineCore->m_pSphereMeshFile->m_FileLoadStatus == FileLoadStatus_Success )
                {
                    g_pEngineCore->m_pMaterialBallMesh->CreateFromMyMeshFile( g_pEngineCore->m_pSphereMeshFile );
                }
            }

            if( g_pEngineCore->m_pMaterialBallMesh && g_pEngineCore->m_pMaterialBallMesh->m_MeshReady )
            {
                pEditorState->m_pDebugViewFBO->Bind( true );

                glDisable( GL_SCISSOR_TEST );
                glViewport( 0, 0, pEditorState->m_pMousePickerFBO->m_Width, pEditorState->m_pMousePickerFBO->m_Height );

                glClearColor( 0, 0, 0, 0 );
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                g_pEngineCore->m_pMaterialBallMesh->SetMaterial( pMaterial, 0 );

                MyMatrix matview;
                matview.SetIdentity();
#if MYFW_RIGHTHANDED
                matview.Translate( 0, 0, -4 );
#else
                matview.Translate( 0, 0, 4 );
#endif

                float aspect = windowwidth / windowheight;
                MyMatrix matproj;
                matproj.CreatePerspectiveVFoV( 45, aspect, 0.01f, 100 );

                MyMatrix matviewproj = matproj * matview;
                g_pEngineCore->m_pMaterialBallMesh->Draw( &matviewproj, 0, 0, 0, 0, 0, 0, 0 );

                pEditorState->m_pDebugViewFBO->Unbind( true );
            }

            if( g_pEngineCore->m_pDebugQuadSprite == 0 )
                g_pEngineCore->m_pDebugQuadSprite = MyNew MySprite( false );

            float u = (float)pEditorState->m_pMousePickerFBO->m_Width / pEditorState->m_pMousePickerFBO->m_TextureWidth;
            float v = (float)pEditorState->m_pMousePickerFBO->m_Height / pEditorState->m_pMousePickerFBO->m_TextureHeight;
            g_pEngineCore->m_pDebugQuadSprite->CreateInPlace( "debug", 0.5f, 0.5f, 1.0f, 1.0f, 0, u, v, 0, Justify_Center, false );
            g_pEngineCore->m_pMaterial_ClipSpaceTexture->SetTextureColor( pEditorState->m_pDebugViewFBO->m_pColorTexture );
            g_pEngineCore->m_pDebugQuadSprite->SetMaterial( g_pEngineCore->m_pMaterial_ClipSpaceTexture );
            g_pEngineCore->m_pDebugQuadSprite->Draw( 0 );
        }
    }

    if( g_pEngineCore->m_Debug_DrawPhysicsDebugShapes && g_GLCanvasIDActive == 1 )
    {
        for( int i=0; i<g_pComponentSystemManager->MAX_SCENES_LOADED; i++ )
        {
            if( g_pComponentSystemManager->m_pSceneInfoMap[i].m_InUse && g_pComponentSystemManager->m_pSceneInfoMap[i].m_pBox2DWorld )
                g_pComponentSystemManager->m_pSceneInfoMap[i].m_pBox2DWorld->m_pWorld->DrawDebugData();
        }
    }
}

bool EditorInterface_SceneManagement::HandleInput(int keyaction, int keycode, int mouseaction, int id, float x, float y, float pressure)
{
    EditorState* pEditorState = g_pEngineCore->m_pEditorState;

    EditorInterface::SetModifierKeyStates( keyaction, keycode, mouseaction, id, x, y, pressure );

    if( pEditorState->m_ModifierKeyStates & MODIFIERKEY_LeftMouse )
    {
        if( mouseaction == GCBA_Down && id == 0 )
        {
            pEditorState->m_EditorActionState = EDITORACTIONSTATE_GroupSelectingObjects;
        }

        // select an object when mouse is released and on the same pixel it went down.
        // TODO: make same "pixel test" a "total travel < small number" test.
        //if( mouseaction == GCBA_Up && id == 0 && pEditorState->m_CurrentMousePosition == pEditorState->m_MouseLeftDownLocation )
        //{
        //    pEditorState->m_EditorActionState = EDITORACTIONSTATE_None;

        //    // find the object we clicked on.
        //    GameObject* pObject = GetObjectAtPixel( x, y, true );

        //    // don't select the gizmos. TODO: give object a 'selectable' flag or something.
        //    if( pObject != pEditorState->m_pTransformGizmo->m_pTransformGizmos[0] &&
        //        pObject != pEditorState->m_pTransformGizmo->m_pTransformGizmos[1] &&
        //        pObject != pEditorState->m_pTransformGizmo->m_pTransformGizmos[2] )
        //    {
        //        // if control isn't held, then deselect all objects first.
        //        if( (pEditorState->m_ModifierKeyStates & MODIFIERKEY_Control) == 0 )
        //        {
        //            pEditorState->ClearSelectedObjectsAndComponents();
        //        }

        //        if( pObject && pEditorState->IsObjectSelected( pObject ) == false )
        //        {
        //            pEditorState->m_pSelectedObjects.push_back( pObject );
        //            // select the object in the object tree.
        //            g_pPanelObjectList->SelectObject( pObject ); // passing in 0 will unselect all items.
        //        }
        //    }
        //}

        if( mouseaction == GCBA_Down && id == 0 )
        {
            // find the object we clicked on.
            GameObject* pObject = GetObjectAtPixel( (unsigned int)x, (unsigned int)y, true );

            // reset mouse movement, so we can undo to this state after mouse goes up.
            pEditorState->m_DistanceTranslated.Set( 0, 0, 0 );
            //LOGInfo( LOGTag, "pEditorState->m_DistanceTranslated.Set zero( %f, %f, %f );\n", pEditorState->m_DistanceTranslated.x, pEditorState->m_DistanceTranslated.y, pEditorState->m_DistanceTranslated.z );

            bool selectedgizmo = false;

            // translate to the right.
            if( pObject == pEditorState->m_pTransformGizmo->m_pTransformGizmos[0] )
            {
                pEditorState->m_EditorActionState = EDITORACTIONSTATE_TranslateX;
                selectedgizmo = true;
            }
            if( pObject == pEditorState->m_pTransformGizmo->m_pTransformGizmos[1] )
            {
                pEditorState->m_EditorActionState = EDITORACTIONSTATE_TranslateY;
                selectedgizmo = true;
            }
            if( pObject == pEditorState->m_pTransformGizmo->m_pTransformGizmos[2] )
            {
                pEditorState->m_EditorActionState = EDITORACTIONSTATE_TranslateZ;
                selectedgizmo = true;
            }

            // if shift is held, make a copy of the object and control that one.
            if( selectedgizmo && pEditorState->m_ModifierKeyStates & MODIFIERKEY_Shift )
            {
                std::vector<GameObject*> selectedobjects = pEditorState->m_pSelectedObjects;
                pEditorState->ClearSelectedObjectsAndComponents();
                for( unsigned int i=0; i<selectedobjects.size(); i++ )
                {
                    GameObject* pNewObject = g_pComponentSystemManager->EditorCopyGameObject( selectedobjects[i], false );
                    if( g_pEngineCore->m_EditorMode )
                    {
                        pNewObject->SetSceneID( selectedobjects[i]->GetSceneID() );
                    }

                    pEditorState->m_pSelectedObjects.push_back( pNewObject );
                    // select the object in the object tree.
                    g_pPanelObjectList->SelectObject( pNewObject );
                }
            }

            // if we didn't select the gizmo and gameplay is running.
            if( selectedgizmo == false && g_pEngineCore->m_EditorMode == false )
            {
                // check if we selected a physics object then grab it.
                //if( pEditorState->m_ModifierKeyStates & MODIFIERKEY_Shift )
                {
                    // Get the mouse click ray.
                    Vector3 raystart, rayend;
                    g_pEngineCore->GetMouseRay( pEditorState->m_CurrentMousePosition, &raystart, &rayend );

                    btVector3 RayStart( raystart.x, raystart.y, raystart.z );
                    btVector3 RayEnd( rayend.x, rayend.y, rayend.z );

                    btCollisionWorld::ClosestRayResultCallback rayCallback( RayStart, RayEnd );
                    g_pBulletWorld->m_pDynamicsWorld->rayTest( RayStart, RayEnd, rayCallback );
                    if( rayCallback.hasHit() )
                    {
                        btVector3 pickPos = rayCallback.m_hitPointWorld;

                        pEditorState->m_MousePicker_PickedBody = 0;

                        //pickObject( pickPos, rayCallback.m_collisionObject );
                        btRigidBody* body = (btRigidBody*)btRigidBody::upcast( rayCallback.m_collisionObject );
                        if( body )
                        {
                            // other exclusions?
                            if( !(body->isStaticObject() || body->isKinematicObject()) )
                            {
                                pEditorState->m_MousePicker_PickedBody = body;
                                pEditorState->m_MousePicker_PickedBody->setActivationState( DISABLE_DEACTIVATION );

                                //printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());

                                btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;

                                if( pEditorState->m_ModifierKeyStates & MODIFIERKEY_Shift ) //(m_modifierKeys & BT_ACTIVE_SHIFT) != 0 )
                                {
                                    btTransform tr;
                                    tr.setIdentity();
                                    tr.setOrigin(localPivot);
                                    btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body, tr,false);
                                    dof6->setLinearLowerLimit( btVector3(0,0,0) );
                                    dof6->setLinearUpperLimit( btVector3(0,0,0) );
                                    dof6->setAngularLowerLimit( btVector3(0,0,0) );
                                    dof6->setAngularUpperLimit( btVector3(0,0,0) );

                                    g_pBulletWorld->m_pDynamicsWorld->addConstraint(dof6,true);
                                    pEditorState->m_MousePicker_PickConstraint = dof6;

                                    dof6->setParam( BT_CONSTRAINT_STOP_CFM, 0.8f, 0 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_CFM, 0.8f, 1 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_CFM, 0.8f, 2 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_CFM, 0.8f, 3 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_CFM, 0.8f, 4 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_CFM, 0.8f, 5 );

                                    dof6->setParam( BT_CONSTRAINT_STOP_ERP, 0.1f, 0 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_ERP, 0.1f, 1 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_ERP, 0.1f, 2 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_ERP, 0.1f, 3 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_ERP, 0.1f, 4 );
                                    dof6->setParam( BT_CONSTRAINT_STOP_ERP, 0.1f, 5 );
                                }
                                else
                                {
                                    btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body,localPivot);
                                    g_pBulletWorld->m_pDynamicsWorld->addConstraint(p2p,true);
                                    pEditorState->m_MousePicker_PickConstraint = p2p;
                                    btScalar mousePickClamping = 30.f;
                                    p2p->m_setting.m_impulseClamp = mousePickClamping;
                                    //very weak constraint for picking
                                    p2p->m_setting.m_tau = 0.001f;
                                    /*
                                    p2p->setParam(BT_CONSTRAINT_CFM,0.8,0);
                                    p2p->setParam(BT_CONSTRAINT_CFM,0.8,1);
                                    p2p->setParam(BT_CONSTRAINT_CFM,0.8,2);
                                    p2p->setParam(BT_CONSTRAINT_ERP,0.1,0);
                                    p2p->setParam(BT_CONSTRAINT_ERP,0.1,1);
                                    p2p->setParam(BT_CONSTRAINT_ERP,0.1,2);
                                    */
                                }

                                //save mouse position for dragging
                            }
                        }

                        //gOldPickingPos = RayEnd;
                        //gHitPos = pickPos;

                        pEditorState->m_MousePicker_OldPickingDist = (pickPos-RayStart).length();
                        mouseaction = -1;
                    }
                }
            }
        }

        if( mouseaction == GCBA_Held && id == 0 )
        {
            // gameplay is running and we picked up a physics object in the editor view, so move it around.
            if( pEditorState->m_MousePicker_PickConstraint && g_pBulletWorld->m_pDynamicsWorld )
            {
                // move the constraint pivot
                if( pEditorState->m_MousePicker_PickConstraint->getConstraintType() == D6_CONSTRAINT_TYPE )
                {
                    btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>( pEditorState->m_MousePicker_PickConstraint );
                    if( pickCon )
                    {
                        //keep it at the same picking distance

                        // Get the mouse click ray.
                        Vector3 raystart, rayend;
                        g_pEngineCore->GetMouseRay( pEditorState->m_CurrentMousePosition, &raystart, &rayend );

                        btVector3 newRayTo( rayend.x, rayend.y, rayend.z );
                        btVector3 rayFrom;
                        btVector3 oldPivotInB = pickCon->getFrameOffsetA().getOrigin();

                        btVector3 newPivotB;
                        //if( m_ortho )
                        //{
                        //    newPivotB = oldPivotInB;
                        //    newPivotB.setX(newRayTo.getX());
                        //    newPivotB.setY(newRayTo.getY());
                        //}
                        //else
                        {
                            rayFrom = btVector3( raystart.x, raystart.y, raystart.z );
                            btVector3 dir = newRayTo - rayFrom;
                            dir.normalize();
                            dir *= pEditorState->m_MousePicker_OldPickingDist;

                            newPivotB = rayFrom + dir;
                        }

                        pickCon->getFrameOffsetA().setOrigin(newPivotB);
                    }
                }
                else
                {
                    btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>( pEditorState->m_MousePicker_PickConstraint );
                    if (pickCon)
                    {
                        //keep it at the same picking distance

                        // Get the mouse click ray.
                        Vector3 raystart, rayend;
                        g_pEngineCore->GetMouseRay( pEditorState->m_CurrentMousePosition, &raystart, &rayend );

                        btVector3 newRayTo( rayend.x, rayend.y, rayend.z );
                        btVector3 rayFrom;
                        btVector3 oldPivotInB = pickCon->getPivotInB();
                        btVector3 newPivotB;

                        //if( m_ortho )
                        //{
                        //    newPivotB = oldPivotInB;
                        //    newPivotB.setX(newRayTo.getX());
                        //    newPivotB.setY(newRayTo.getY());
                        //}
                        //else
                        {
                            rayFrom = btVector3( raystart.x, raystart.y, raystart.z );
                            btVector3 dir = newRayTo - rayFrom;
                            dir.normalize();
                            dir *= pEditorState->m_MousePicker_OldPickingDist;

                            newPivotB = rayFrom + dir;
                        }

                        pickCon->setPivotB(newPivotB);
                    }
                }

                //float dx, dy;
                //dx = btScalar(x) - m_mouseOldX;
                //dy = btScalar(y) - m_mouseOldY;


                ///only if ALT key is pressed (Maya style)
                //if (m_modifierKeys& BT_ACTIVE_ALT)
                //{
                //    if(m_mouseButtons & 2)
                //    {
                //        btVector3 hor = getRayTo(0,0)-getRayTo(1,0);
                //        btVector3 vert = getRayTo(0,0)-getRayTo(0,1);
                //        btScalar multiplierX = btScalar(0.001);
                //        btScalar multiplierY = btScalar(0.001);
                //        if (m_ortho)
                //        {
                //            multiplierX = 1;
                //            multiplierY = 1;
                //        }


                //        m_cameraTargetPosition += hor* dx * multiplierX;
                //        m_cameraTargetPosition += vert* dy * multiplierY;
                //    }

                //    if(m_mouseButtons & (2 << 2) && m_mouseButtons & 1)
                //    {
                //    }
                //    else if(m_mouseButtons & 1)
                //    {
                //        m_azi += dx * btScalar(0.2);
                //        m_azi = fmodf(m_azi, btScalar(360.f));
                //        m_ele += dy * btScalar(0.2);
                //        m_ele = fmodf(m_ele, btScalar(180.f));
                //    }
                //    else if(m_mouseButtons & 4)
                //    {
                //        m_cameraDistance -= dy * btScalar(0.02f);
                //        if (m_cameraDistance<btScalar(0.1))
                //            m_cameraDistance = btScalar(0.1);


                //    }
                //}


                //m_mouseOldX = x;
                //m_mouseOldY = y;
                //updateCamera();

                mouseaction = -1;
            }
        }

        if( mouseaction == GCBA_Up && id == 0 )
        {
            pEditorState->ClearConstraint();
        }

        // check if the mouse moved.
        Vector3 mousedragdir = pEditorState->m_CurrentMousePosition - pEditorState->m_LastMousePosition;

        if( mousedragdir.LengthSquared() != 0 )
        {
            // If the mouse moved, move the selected objects along a plane or axis
            pEditorState->m_pTransformGizmo->TranslateSelectedObjects( g_pEngineCore, pEditorState );
        }
    }

    // handle camera movement, with both mouse and keyboard.
    EditorInterface::HandleInputForEditorCamera( keyaction, keycode, mouseaction, id, x, y, pressure );

    if( keyaction == GCBA_Up )
    {
        // Lock to current object, 
        if( keycode == 'L' )
        {
            if( pEditorState->m_CameraState == EditorCameraState_Default && pEditorState->m_pSelectedObjects.size() > 0 )
            {
                GameObject* pSelectedObject = pEditorState->m_pSelectedObjects[0];
                pEditorState->LockCameraToGameObject( pSelectedObject );
            }
            else
            {
                pEditorState->LockCameraToGameObject( 0 );
            }
        }
    }

    // check for mouse ups
    if( mouseaction != -1 )
    {
        if( mouseaction == GCBA_Up )
        {
            if( id == 0 ) // left button up
            {
                // when mouse up, select all object in the box.
                if( pEditorState->m_EditorActionState == EDITORACTIONSTATE_GroupSelectingObjects )
                {
                    SelectObjectsInRectangle(
                        (unsigned int)pEditorState->m_MouseLeftDownLocation.x, (unsigned int)pEditorState->m_MouseLeftDownLocation.y,
                        (unsigned int)pEditorState->m_CurrentMousePosition.x, (unsigned int)pEditorState->m_CurrentMousePosition.y );
                }

                pEditorState->m_EditorActionState = EDITORACTIONSTATE_None;

                // GIZMOTRANSLATE: add translation to undo stack, action itself is done each frame.  We only want to undo to last mouse down.
                if( pEditorState->m_pSelectedObjects.size() > 0 && pEditorState->m_DistanceTranslated.LengthSquared() != 0 )
                {
                    // Create a new list of selected objects, don't include objects that have parents that are selected.
                    std::vector<GameObject*> selectedobjects;
                    for( unsigned int i=0; i<pEditorState->m_pSelectedObjects.size(); i++ )
                    {
                        ComponentTransform* pTransform = pEditorState->m_pSelectedObjects[i]->m_pComponentTransform;

                        // if this object has a selected parent, don't move it, only move the parent.
                        if( pTransform->IsAnyParentInList( pEditorState->m_pSelectedObjects ) == false )
                        {
                            selectedobjects.push_back( pEditorState->m_pSelectedObjects[i] );
                        }
                    }

                    if( selectedobjects.size() > 0 )
                    {
                        g_pEngineMainFrame->m_pCommandStack->Add( MyNew EditorCommand_MoveObjects( pEditorState->m_DistanceTranslated, selectedobjects ) );
                    }
                }
            }
        }
    }

    // clear mouse button states.
    EditorInterface::ClearModifierKeyStates( keyaction, keycode, mouseaction, id, x, y, pressure );

    return false;
}