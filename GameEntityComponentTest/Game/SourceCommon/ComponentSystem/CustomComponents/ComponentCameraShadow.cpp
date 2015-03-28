//
// Copyright (c) 2015 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "GameCommonHeader.h"

#if MYFW_USING_WX
bool ComponentCameraShadow::m_PanelWatchBlockVisible = true;
#endif

ComponentCameraShadow::ComponentCameraShadow()
: ComponentCamera()
{
    m_BaseType = BaseComponentType_Camera;

    m_pComponentTransform = 0;
    m_pDepthFBO = 0;
}

ComponentCameraShadow::~ComponentCameraShadow()
{
    SAFE_RELEASE( m_pDepthFBO );
}

#if MYFW_USING_WX
void ComponentCameraShadow::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    wxTreeItemId id = g_pPanelObjectList->AddObject( this, ComponentCameraShadow::StaticOnLeftClick, ComponentCamera::StaticOnRightClick, gameobjectid, "Camera" );
}

void ComponentCameraShadow::OnLeftClick(bool clear)
{
    ComponentCamera::OnLeftClick( clear );
}

void ComponentCameraShadow::FillPropertiesWindow(bool clear)
{
    m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "CameraShadow", this, ComponentCamera::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible )
    {
        ComponentCamera::FillPropertiesWindow( clear );
    }
}

void ComponentCameraShadow::OnValueChanged(int id, bool finishedchanging)
{
}
#endif //MYFW_USING_WX

cJSON* ComponentCameraShadow::ExportAsJSONObject()
{
    cJSON* component = ComponentCamera::ExportAsJSONObject();

    //cJSON_AddNumberToObject( component, "Ortho", m_Orthographic );

    //cJSON_AddNumberToObject( component, "DWidth", m_DesiredWidth );
    //cJSON_AddNumberToObject( component, "DHeight", m_DesiredHeight );

    //cJSON_AddNumberToObject( component, "ColorBit", m_ClearColorBuffer );
    //cJSON_AddNumberToObject( component, "DepthBit", m_ClearDepthBuffer );

    //cJSON_AddNumberToObject( component, "Layers", m_LayersToRender );

    return component;
}

void ComponentCameraShadow::ImportFromJSONObject(cJSON* jsonobj, unsigned int sceneid)
{
    ComponentCamera::ImportFromJSONObject( jsonobj, sceneid );

    //cJSONExt_GetBool( jsonobj, "Ortho", &m_Orthographic );

    //cJSONExt_GetFloat( jsonobj, "DWidth", &m_DesiredWidth );
    //cJSONExt_GetFloat( jsonobj, "DHeight", &m_DesiredHeight );

    //cJSONExt_GetBool( jsonobj, "ColorBit", &m_ClearColorBuffer );
    //cJSONExt_GetBool( jsonobj, "DepthBit", &m_ClearDepthBuffer );

    //cJSONExt_GetUnsignedInt( jsonobj, "Layers", &m_LayersToRender );
}

void ComponentCameraShadow::Reset()
{
    ComponentCamera::Reset();

    SAFE_RELEASE( m_pDepthFBO );
#if MYFW_OPENGLES2
    m_pDepthFBO = g_pTextureManager->CreateFBO( (int)512, (int)512, GL_LINEAR, GL_LINEAR, true, false, false );
#else
    m_pDepthFBO = g_pTextureManager->CreateFBO( (int)512, (int)512, GL_LINEAR, GL_LINEAR, true, true, false );
#endif
}

ComponentCameraShadow& ComponentCameraShadow::operator=(const ComponentCameraShadow& other)
{
    assert( &other != this );

    ComponentCamera::operator=( other );

    return *this;
}

void ComponentCameraShadow::Tick(double TimePassed)
{
    ComponentCamera::Tick( TimePassed );
}

void ComponentCameraShadow::OnSurfaceChanged(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height, unsigned int desiredaspectwidth, unsigned int desiredaspectheight)
{
    ComponentCamera::OnSurfaceChanged( startx, starty, width, height, desiredaspectwidth, desiredaspectheight );
}

void ComponentCameraShadow::OnDrawFrame()
{
    //ComponentCamera::OnDrawFrame();

    glDisable( GL_SCISSOR_TEST );
    g_ActiveShaderPass = ShaderPass_ShadowCastRGBA;

    m_pDepthFBO->Bind();
    glViewport( 0, 0, m_pDepthFBO->m_Width, m_pDepthFBO->m_Height );

#if MYFW_OPENGLES2
    glClearColor( 1, 1, 1, 1 );
    glClear( GL_COLOR_BUFFER_BIT ); // | GL_DEPTH_BUFFER_BIT );
#else
    glClearColor( 1, 1, 1, 1 );
    glClearDepth( 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
#endif

    g_pComponentSystemManager->OnDrawFrame( this, &m_Camera3D.m_matViewProj, 0 );

    m_pDepthFBO->Unbind();
    g_ActiveShaderPass = ShaderPass_Main;

    //glViewport( m_WindowStartX, m_WindowStartY, m_WindowWidth, m_WindowHeight );
    //checkGlError( "glViewport" );

    //glClearColor( 0.0f, 0.0f, 0.2f, 1.0f );

    //if( m_ClearColorBuffer && m_ClearDepthBuffer )
    //    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //else if( m_ClearColorBuffer )
    //    glClear( GL_COLOR_BUFFER_BIT );
    //else if( m_ClearDepthBuffer )
    //    glClear( GL_DEPTH_BUFFER_BIT );

    //if( m_Orthographic )
    //{   
    //    g_pComponentSystemManager->OnDrawFrame( this, &m_Camera2D.m_matViewProj, 0 );
    //}
    //else
    //{
    //    g_pComponentSystemManager->OnDrawFrame( this, &m_Camera3D.m_matViewProj, 0 );
    //}
}
