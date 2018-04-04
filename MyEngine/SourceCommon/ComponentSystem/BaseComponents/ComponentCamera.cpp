//
// Copyright (c) 2014-2018 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "EngineCommonHeader.h"

#if MYFW_USING_WX
bool ComponentCamera::m_PanelWatchBlockVisible = true;
#endif

// 32 layers strings since ComponentBase::ExportVariablesToJSON -> case ComponentVariableType_Flags is looking for 32
const char* g_pVisibilityLayerStrings[32] = //g_NumberOfVisibilityLayers] =
{
    "Main view",
    "HUD",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",
    "22",
    "23",
    "24",
    "25",
    "26",
    "27",
    "28",
    "29",
    "30",
    "31",
    "32",
};

// Component Variable List
MYFW_COMPONENT_IMPLEMENT_VARIABLE_LIST( ComponentCamera ); //_VARIABLE_LIST

ComponentCamera::ComponentCamera()
: ComponentBase()
{
    MYFW_COMPONENT_VARIABLE_LIST_CONSTRUCTOR(); //_VARIABLE_LIST

    ClassnameSanityCheck();

    m_BaseType = BaseComponentType_Camera;

    m_pComponentTransform = 0;
}

ComponentCamera::~ComponentCamera()
{
    SAFE_RELEASE( m_pPostEffectFBOs[0] );
    SAFE_RELEASE( m_pPostEffectFBOs[1] );

    MYFW_COMPONENT_VARIABLE_LIST_DESTRUCTOR(); //_VARIABLE_LIST

    if( m_pDeferredShaderFile )
        g_pFileManager->FreeFile( m_pDeferredShaderFile );
    SAFE_RELEASE( m_pDeferredShader );
    SAFE_RELEASE( m_pDeferredQuadMesh );
    SAFE_RELEASE( m_pDeferredQuadMaterial );
}

void ComponentCamera::RegisterVariables(CPPListHead* pList, ComponentCamera* pThis) //_VARIABLE_LIST
{
    ComponentVariable* pVar;

    AddVarFlags( pList, "Layers", MyOffsetOf( pThis, &pThis->m_LayersToRender ), true, true, 0,
                 g_NumberOfVisibilityLayers, g_pVisibilityLayerStrings,
                 (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );

    pVar = AddVar( pList, "Deferred", ComponentVariableType_Bool, MyOffsetOf( pThis, &pThis->m_Deferred ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );

    pVar = AddVar( pList, "Ortho", ComponentVariableType_Bool, MyOffsetOf( pThis, &pThis->m_Orthographic ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );

    pVar = AddVar( pList, "DesiredWidth", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_DesiredWidth ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->SetEditorLimits( 640, 960 );
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "DesiredHeight", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_DesiredHeight ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->SetEditorLimits( 640, 960 );
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "OrthoNearZ", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_OrthoNearZ ), true, true, "Near Z", (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "OrthoFarZ", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_OrthoFarZ ), true, true, "Far Z", (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "FoV", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_FieldOfView ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->SetEditorLimits( 1, 179 );
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif
    
    pVar = AddVar( pList, "PerspectiveNearZ", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_PerspectiveNearZ ), true, true, "Near Z", (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "PerspectiveFarZ", ComponentVariableType_Float, MyOffsetOf( pThis, &pThis->m_PerspectiveFarZ ), true, true, "Far Z", (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
#if MYFW_USING_WX
    pVar->AddCallback_ShouldVariableBeAdded( (CVarFunc_ShouldVariableBeAdded)(&ComponentCamera::ShouldVariableBeAddedToWatchPanel) );
#endif

    pVar = AddVar( pList, "ColorBit", ComponentVariableType_Bool, MyOffsetOf( pThis, &pThis->m_ClearColorBuffer ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
    pVar = AddVar( pList, "DepthBit", ComponentVariableType_Bool, MyOffsetOf( pThis, &pThis->m_ClearDepthBuffer ), true, true, 0, (CVarFunc_ValueChanged)&ComponentCamera::OnValueChanged, 0, 0 );
}

#if MYFW_USING_WX
void ComponentCamera::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    //wxTreeItemId id =
    g_pPanelObjectList->AddObject( this, ComponentCamera::StaticOnLeftClick, ComponentBase::StaticOnRightClick, gameobjectid, "Camera", ObjectListIcon_Component );
}

void ComponentCamera::OnLeftClick(unsigned int count, bool clear)
{
    ComponentBase::OnLeftClick( count, clear );
}

void ComponentCamera::FillPropertiesWindow(bool clear, bool addcomponentvariables, bool ignoreblockvisibleflag)
{
    m_ControlID_ComponentTitleLabel = g_pPanelWatch->AddSpace( "Camera", this, ComponentBase::StaticOnComponentTitleLabelClicked );

    if( m_PanelWatchBlockVisible || ignoreblockvisibleflag == true )
    {
        ComponentBase::FillPropertiesWindow( clear );

        FillPropertiesWindowWithVariables(); //_VARIABLE_LIST
    }
}
#endif //MYFW_USING_WX

#if MYFW_EDITOR
bool ComponentCamera::ShouldVariableBeAddedToWatchPanel(ComponentVariable* pVar)
{
    if( m_Orthographic == true )
    {
        if( strcmp( pVar->m_Label, "DesiredWidth" ) == 0 )       return true;
        if( strcmp( pVar->m_Label, "DesiredHeight" ) == 0 )      return true;
        if( strcmp( pVar->m_Label, "OrthoNearZ" ) == 0 )         return true;
        if( strcmp( pVar->m_Label, "OrthoFarZ" ) == 0 )          return true;
    }
    else //if( m_Orthographic == false )
    {
        if( strcmp( pVar->m_Label, "FoV" ) == 0 )                return true;
        if( strcmp( pVar->m_Label, "PerspectiveNearZ" ) == 0 )   return true;
        if( strcmp( pVar->m_Label, "PerspectiveFarZ" ) == 0 )    return true;
    }

    return false;
}

void* ComponentCamera::OnValueChanged(ComponentVariable* pVar, bool changedbyinterface, bool finishedchanging, double oldvalue, ComponentVariableValue* pNewValue)
{
    void* oldpointer = 0;

    ComputeProjectionMatrices();

    if( finishedchanging )
    {
        m_FullClearsRequired = 1;

#if MYFW_USING_WX
        m_FullClearsRequired = 2;
        g_pPanelWatch->SetNeedsRefresh();
#endif //MYFW_USING_WX
    }

    return oldpointer;
}
#endif //MYFW_EDITOR

cJSON* ComponentCamera::ExportAsJSONObject(bool savesceneid, bool saveid)
{
    cJSON* jComponent = ComponentBase::ExportAsJSONObject( savesceneid, saveid );

    return jComponent;
}

void ComponentCamera::ImportFromJSONObject(cJSON* jComponent, SceneID sceneid)
{
    // if we're loading a camera, start the layers flags at 0, so we set the flags when loading.
    m_LayersToRender = 0;

    ComponentBase::ImportFromJSONObject( jComponent, sceneid );
}

void ComponentCamera::Reset()
{
    ComponentBase::Reset();

    m_pComponentTransform = m_pGameObject->GetTransform();

    m_Orthographic = false;

    m_ClearColorBuffer = true;
    m_ClearDepthBuffer = true;

    m_Deferred = false;
    m_pGBuffer = 0;
    m_pDeferredShaderFile = 0;
    m_pDeferredShader = 0;
    m_pDeferredQuadMesh = 0;
    m_pDeferredQuadMaterial = 0;

    m_DesiredWidth = 640;
    m_DesiredHeight = 960;
    m_OrthoNearZ = 0;
    m_OrthoFarZ = 1000;

    m_FieldOfView = 45;
    m_PerspectiveNearZ = 1;
    m_PerspectiveFarZ = 10000;
    
    m_LayersToRender = 0x00FF;

    m_WindowStartX = 0;
    m_WindowStartY = 0;
    m_WindowWidth = 0;
    m_WindowHeight = 0;

    m_pPostEffectFBOs[0] = 0;
    m_pPostEffectFBOs[1] = 0;

#if MYFW_EDITOR
    m_FullClearsRequired = 1;
#endif //MYFW_EDITOR

#if MYFW_USING_WX
    m_FullClearsRequired = 2;
    m_pPanelWatchBlockVisible = &m_PanelWatchBlockVisible;
#endif
}

ComponentCamera& ComponentCamera::operator=(const ComponentCamera& other)
{
    MyAssert( &other != this );

    ComponentBase::operator=( other );

    this->m_Orthographic = other.m_Orthographic;
    this->m_Deferred = other.m_Deferred;

    this->m_ClearColorBuffer = other.m_ClearColorBuffer;
    this->m_ClearDepthBuffer = other.m_ClearDepthBuffer;
    
    this->m_DesiredWidth = other.m_DesiredWidth;
    this->m_DesiredHeight = other.m_DesiredHeight;
    this->m_OrthoNearZ = other.m_OrthoNearZ;
    this->m_OrthoFarZ = other.m_OrthoFarZ;

    this->m_FieldOfView = other.m_FieldOfView;
    this->m_PerspectiveNearZ = other.m_PerspectiveNearZ;
    this->m_PerspectiveFarZ = other.m_PerspectiveFarZ;

    this->m_LayersToRender = other.m_LayersToRender;

    return *this;
}

void ComponentCamera::RegisterCallbacks()
{
    if( m_Enabled && m_CallbacksRegistered == false )
    {
        m_CallbacksRegistered = true;

        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, Tick );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, OnSurfaceChanged );
#if MYFW_EDITOR
        //MYFW_FILL_COMPONENT_CALLBACK_STRUCT( ComponentCamera, Draw );
        MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, Draw );
#endif //MYFW_EDITOR
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, OnTouch );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, OnButtons );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, OnKeys );
        //MYFW_REGISTER_COMPONENT_CALLBACK( ComponentCamera, OnFileRenamed );
    }
}

void ComponentCamera::UnregisterCallbacks()
{
    if( m_CallbacksRegistered == true )
    {
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( Tick );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnSurfaceChanged );
#if MYFW_EDITOR
        MYFW_UNREGISTER_COMPONENT_CALLBACK( Draw );
#endif //MYFW_EDITOR
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnTouch );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnButtons );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnKeys );
        //MYFW_UNREGISTER_COMPONENT_CALLBACK( OnFileRenamed );

        m_CallbacksRegistered = false;
    }
}

void ComponentCamera::SetDesiredAspectRatio(float width, float height)
{
    m_DesiredWidth = width;
    m_DesiredHeight = height;

    ComputeProjectionMatrices();
}

void ComponentCamera::ComputeProjectionMatrices()
{
    if( m_WindowHeight == 0 )
    {
        float ratio = m_DesiredWidth / m_DesiredHeight;

        MyClamp( m_FieldOfView, 1.0f, 179.0f );

        m_Camera3D.SetupProjection( ratio, ratio, m_FieldOfView, m_PerspectiveNearZ, m_PerspectiveFarZ );
        m_Camera2D.SetupDirect( -m_DesiredWidth/2, m_DesiredWidth/2, -m_DesiredHeight/2, m_DesiredHeight/2, m_OrthoNearZ, m_OrthoFarZ );
    }
    else
    {
        float deviceratio = (float)m_WindowWidth / (float)m_WindowHeight;
        float gameratio = m_DesiredWidth / m_DesiredHeight;

        MyClamp( m_FieldOfView, 1.0f, 179.0f );

        m_Camera3D.SetupProjection( deviceratio, gameratio, m_FieldOfView, m_PerspectiveNearZ, m_PerspectiveFarZ );
        m_Camera2D.Setup( (float)m_WindowWidth, (float)m_WindowHeight, m_DesiredWidth, m_DesiredHeight, m_OrthoNearZ, m_OrthoFarZ );
    }
}

void ComponentCamera::Tick(double TimePassed)
{
    m_pComponentTransform->UpdateTransform();
    MyMatrix matView = *m_pComponentTransform->GetWorldTransform();
    matView.Inverse();

    if( m_Orthographic )
    {
        m_Camera2D.m_matView = matView;
        m_Camera2D.UpdateMatrices();
    }
    else
    {
        m_Camera3D.m_matView = matView;
        m_Camera3D.UpdateMatrices();
    }
}

void ComponentCamera::OnSurfaceChanged(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height, unsigned int desiredaspectwidth, unsigned int desiredaspectheight)
{
    if( m_WindowStartX == startx && m_WindowStartY == starty && m_WindowWidth == width && m_WindowHeight == height )
        return;

    //m_DesiredWidth = (float)desiredaspectwidth;
    //m_DesiredHeight = (float)desiredaspectheight;

    m_WindowStartX = startx;
    m_WindowStartY = starty;
    m_WindowWidth = width;
    m_WindowHeight = height;

    ComputeProjectionMatrices();

#if MYFW_EDITOR
    m_FullClearsRequired = 1;
#endif //MYFW_EDITOR

#if MYFW_USING_WX
    m_FullClearsRequired = 2;
#endif
}

ComponentPostEffect* ComponentCamera::GetNextPostEffect(ComponentPostEffect* pLastEffect) // pass in 0 to get first effect.
{
    ComponentBase* pComponent;

    if( pLastEffect == 0 )
        pComponent = m_pGameObject->GetFirstComponentOfBaseType( BaseComponentType_Data );
    else
        pComponent = m_pGameObject->GetNextComponentOfBaseType( pLastEffect );

    ComponentPostEffect* pPostEffect = 0;

    while( pComponent != 0 )
    {
        pPostEffect = pComponent->IsA( "PostEffectComponent" ) ? (ComponentPostEffect*)pComponent : 0;
        
        if( pPostEffect && pPostEffect->m_pMaterial != 0 && pPostEffect->IsEnabled() )
            return pPostEffect; // if we found a valid initialized post effect, return it.
        else
            pComponent = m_pGameObject->GetNextComponentOfBaseType( pComponent );
    }

    return 0;
}

void ComponentCamera::OnDrawFrame()
{
    // Store the current FBO, we will use it as the final render target.
    unsigned int startingFBO = g_GLStats.m_CurrentFramebuffer;

    // TODO: Clean up, make func other than tick to this.
    // Update camera view/proj before drawing.
    Tick( 0 );

    g_GLStats.m_CurrentFramebufferWidth = m_WindowWidth;
    g_GLStats.m_CurrentFramebufferHeight = m_WindowHeight;

#if MYFW_EDITOR
    // If we resize the window and we're in a wx build, clear the entire backbuffer for 2 frames.
    // This is required since we're potentially GL_SCISSOR_TEST'ing an uncleared area.
    if( m_ClearColorBuffer && m_FullClearsRequired > 0 )
    {
        glDisable( GL_SCISSOR_TEST );
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        m_FullClearsRequired--;
    }
#endif //MYFW_EDITOR

    // If there are any post effect components, render to a texture and pass that into the effect component.

    // Find the first actual ComponentPostEffect.
    ComponentPostEffect* pPostEffect = GetNextPostEffect( 0 );

    // First pass: Render the main scene... into a post effect FBO if a ComponentPostEffect was found.
    {
        if( pPostEffect )
        {
            // If a post effect was found, render to an FBO.
            if( m_pPostEffectFBOs[0] == 0 )
            {
                m_pPostEffectFBOs[0] = g_pTextureManager->CreateFBO( m_WindowWidth, m_WindowHeight, GL_NEAREST, GL_NEAREST, FBODefinition::FBOColorFormat_RGBA, 32, false );
            }
            else
            {
                g_pTextureManager->ReSetupFBO( m_pPostEffectFBOs[0], m_WindowWidth, m_WindowHeight, GL_NEAREST, GL_NEAREST, FBODefinition::FBOColorFormat_RGBA, 32, false );
            }

            m_pPostEffectFBOs[0]->Bind( false );
            glDisable( GL_SCISSOR_TEST );
            glViewport( 0, 0, m_WindowWidth, m_WindowHeight );
        }
        else
        {
            // Set up scissor test and glViewport if not drawing to the whole window.
            if( m_WindowStartX != 0 || m_WindowStartY != 0 )
            {
                // Scissor test is really only needed for the glClear call.
                glEnable( GL_SCISSOR_TEST );
                glScissor( m_WindowStartX, m_WindowStartY, m_WindowWidth, m_WindowHeight );
            }
            else
            {
                glDisable( GL_SCISSOR_TEST );
            }

            glViewport( m_WindowStartX, m_WindowStartY, m_WindowWidth, m_WindowHeight );
        }

        DrawScene();
    }

    // Other passes: Ping pong between 2 FBOs for the remaining post effects, render the final one to screen.
    int fboindex = 0;
    while( pPostEffect )
    {
        ComponentPostEffect* pNextPostEffect = GetNextPostEffect( pPostEffect );

        if( pNextPostEffect )
        {
            // If there is a next effect, render into the next unused FBO.
            if( m_pPostEffectFBOs[!fboindex] == 0 )
            {
                m_pPostEffectFBOs[!fboindex] = g_pTextureManager->CreateFBO( m_WindowWidth, m_WindowHeight, GL_NEAREST, GL_NEAREST, FBODefinition::FBOColorFormat_RGBA, 32, false );
            }
            else
            {
                g_pTextureManager->ReSetupFBO( m_pPostEffectFBOs[!fboindex], m_WindowWidth, m_WindowHeight, GL_NEAREST, GL_NEAREST, FBODefinition::FBOColorFormat_RGBA, 32, false );
            }
            m_pPostEffectFBOs[!fboindex]->Bind( false );

            glDisable( GL_SCISSOR_TEST );
            glViewport( 0, 0, m_WindowWidth, m_WindowHeight );
        }
        else
        {
            // If there isn't another post effect, render back to the FBO that was set before this function.
            MyBindFramebuffer( GL_FRAMEBUFFER, startingFBO, m_WindowWidth, m_WindowHeight );

            // Set up scissor test and glViewport if not drawing to the whole window.
            if( m_WindowStartX != 0 || m_WindowStartY != 0 )
            {
                // Scissor test is really only needed for the glClear call.
                glEnable( GL_SCISSOR_TEST );
                glScissor( m_WindowStartX, m_WindowStartY, m_WindowWidth, m_WindowHeight );
            }
            else
            {
                glDisable( GL_SCISSOR_TEST );
            }

            glViewport( m_WindowStartX, m_WindowStartY, m_WindowWidth, m_WindowHeight );
        }

        glClearColor( 0.0f, 0.0f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        pPostEffect->Render( m_pPostEffectFBOs[fboindex] );

        fboindex = !fboindex;
        pPostEffect = pNextPostEffect;
    }

    // Restore the FBO to what was set when we entered this method.
    if( startingFBO != g_GLStats.m_CurrentFramebuffer )
    {
        // The FBO should already be set, either we didn't change it, or the final pass was sent to this FBO.
        MyAssert( false );
        MyBindFramebuffer( GL_FRAMEBUFFER, startingFBO, m_WindowWidth, m_WindowHeight );
    }
}

// DrawScene() is an internal method.
void ComponentCamera::DrawScene()
{
    // Store the current FBO, we will use it as the final render target.
    unsigned int startingFBO = g_GLStats.m_CurrentFramebuffer;

    bool renderedADeferredPass = false;

    // Start a deferred render pass, creating and binding the G-Buffer.
    if( m_Deferred && g_ActiveShaderPass == ShaderPass_Main )
    {
        // Create gbuffer and deferred shader if they don't exist.
        if( m_pGBuffer == 0 )
        {
            const int numcolorformats = 3;
            FBODefinition::FBOColorFormat colorformats[numcolorformats];
            colorformats[0] = FBODefinition::FBOColorFormat_RGBA; // Albedo (RGB) / Specular Power (A)
            colorformats[1] = FBODefinition::FBOColorFormat_RGBA; // Positions (RGB) / Nothing in (A)
            colorformats[2] = FBODefinition::FBOColorFormat_RGBA; // Normals (RGB) / Nothing in (A)

            m_pGBuffer = g_pTextureManager->CreateFBO( m_WindowWidth, m_WindowHeight, GL_NEAREST, GL_NEAREST, colorformats, numcolorformats, 32, false );

            MyAssert( m_pDeferredShaderFile == 0 );
            MyAssert( m_pDeferredShader == 0 );

            m_pDeferredShaderFile = g_pEngineFileManager->RequestFile_UntrackedByScene( "Data/DataEngine/Shaders/Shader_Deferred.glsl" );
#if MYFW_EDITOR
            m_pDeferredShaderFile->MemoryPanel_Hide();
#endif
            m_pDeferredShader = MyNew ShaderGroup( m_pDeferredShaderFile );

            m_pDeferredQuadMesh = new MyMesh();
            m_pDeferredQuadMesh->CreateClipSpaceQuad();
            m_pDeferredQuadMaterial = new MaterialDefinition( m_pDeferredShader );
            m_pDeferredQuadMesh->SetMaterial( m_pDeferredQuadMaterial, 0 );
        }

        // Set the global render pass to deferred, so each object will render with the correct shader.
        g_ActiveShaderPass = ShaderPass_MainDeferred;
        renderedADeferredPass = true;
        m_pGBuffer->Bind( false );

        // Set up the 3 textures for GL to write to.
        // TODO: Do this once when FBO is done setting up.
        GLenum buffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers( 3, buffers );
    }

    // Clear the buffer and render the scene.
    {
        glClearColor( 0.0f, 0.0f, 0.2f, 1.0f );

        if( m_ClearColorBuffer && m_ClearDepthBuffer )
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        else if( m_ClearColorBuffer )
            glClear( GL_COLOR_BUFFER_BIT );
        else if( m_ClearDepthBuffer )
            glClear( GL_DEPTH_BUFFER_BIT );

        if( m_Orthographic )
        {
            g_pComponentSystemManager->OnDrawFrame( this, &m_Camera2D.m_matViewProj, 0 );
        }
        else
        {
            g_pComponentSystemManager->OnDrawFrame( this, &m_Camera3D.m_matViewProj, 0 );
        }
    }

    // Restore the FBO to what was set when we entered this method.
    if( startingFBO != g_GLStats.m_CurrentFramebuffer )
    {
        // The FBO should already be set, either we didn't change it, or the final pass was sent to this FBO.
        //MyAssert( false );
        MyBindFramebuffer( GL_FRAMEBUFFER, startingFBO, m_WindowWidth, m_WindowHeight );
    }

    // Finish our deferred render if we started it.
    if( renderedADeferredPass )
    {
        // TODO: Render a full screen quad to combine the 3 textures from the G-Buffer.
        m_pDeferredQuadMaterial->SetTextureColor( m_pGBuffer->GetColorTexture( 0 ) );
        m_pDeferredQuadMesh->Draw( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );

        g_ActiveShaderPass = ShaderPass_Main;
    }
}

bool ComponentCamera::IsVisible()
{
    return true;
}

bool ComponentCamera::ExistsOnLayer(unsigned int layerflags)
{
    if( layerflags & Layer_EditorFG )
        return true;
    
    return false;
}

#if MYFW_EDITOR
void ComponentCamera::DrawCallback(ComponentCamera* pCamera, MyMatrix* pMatViewProj, ShaderGroup* pShaderOverride)
{
    if( g_pEngineCore->GetEditorPrefs()->Get_View_ShowEditorIcons() == false )
        return;

    MySprite* pSprite = g_pEngineCore->GetEditorState()->m_pEditorIcons[EditorIcon_Camera];
    if( pSprite == 0 )
        return;

    // Set the sprite color
    pSprite->GetMaterial()->m_ColorDiffuse = ColorByte( 255, 255, 255, 255 );

    // make the sprite face the same direction as the camera.
    MyMatrix rot90;
    rot90.SetIdentity();
    rot90.Rotate( -90, 0, 1, 0 );

    MyMatrix transform = *m_pComponentTransform->GetWorldTransform() * rot90;
    //pSprite->SetPosition( &transform );
    
    // disable culling, so we see the camera from both sides.
    glPolygonMode( GL_FRONT, GL_FILL );
    glDisable( GL_CULL_FACE );
    pSprite->Draw( &transform, pMatViewProj, pShaderOverride );
    glEnable( GL_CULL_FACE );
    if( g_pEngineCore->GetDebug_DrawWireframe() )
        glPolygonMode( GL_FRONT, GL_LINE );
}
#endif //MYFW_EDITOR
