//
// Copyright (c) 2014-2015 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "GameCommonHeader.h"

ComponentMesh::ComponentMesh()
: ComponentRenderable()
{
    m_BaseType = BaseComponentType_Renderable;

    m_pMesh = 0;
    m_pShaderGroup = 0;
    m_pTexture = 0;
}

ComponentMesh::~ComponentMesh()
{
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pShaderGroup );
    SAFE_RELEASE( m_pTexture );
}

void ComponentMesh::Reset()
{
    ComponentRenderable::Reset();

    SAFE_RELEASE( m_pMesh );
}

#if MYFW_USING_WX
void ComponentMesh::AddToObjectsPanel(wxTreeItemId gameobjectid)
{
    assert( gameobjectid.IsOk() );
    wxTreeItemId id = g_pPanelObjectList->AddObject( this, ComponentMesh::StaticFillPropertiesWindow, ComponentBase::StaticOnRightClick, gameobjectid, "Mesh" );
}

void ComponentMesh::FillPropertiesWindow(bool clear)
{
    if( clear )
        g_pPanelWatch->ClearAllVariables();

    assert( m_pMesh );

    const char* desc = "no shader";
    if( m_pShaderGroup && m_pShaderGroup->GetShader( ShaderPass_Main )->m_pFile )
        desc = m_pShaderGroup->GetShader( ShaderPass_Main )->m_pFile->m_FilenameWithoutExtension;
    g_pPanelWatch->AddPointerWithDescription( "Shader", 0, desc, this, ComponentMesh::StaticOnDropShader );

    desc = "no texture";
    if( m_pTexture )
        desc = m_pTexture->m_Filename;
    g_pPanelWatch->AddPointerWithDescription( "Texture", 0, desc, this, ComponentMesh::StaticOnDropTexture );    
}

void ComponentMesh::OnDropShader()
{
    if( g_DragAndDropStruct.m_Type == DragAndDropType_ShaderGroupPointer )
    {
        ShaderGroup* pShaderGroup = (ShaderGroup*)g_DragAndDropStruct.m_Value;
        assert( pShaderGroup );
        assert( m_pMesh );

        SetShader( pShaderGroup );

        // update the panel so new Shader name shows up.
        g_pPanelWatch->m_pVariables[g_DragAndDropStruct.m_ID].m_Description = pShaderGroup->GetShader( ShaderPass_Main )->m_pFile->m_FilenameWithoutExtension;
    }
}

void ComponentMesh::OnDropTexture()
{
    if( g_DragAndDropStruct.m_Type == DragAndDropType_FileObjectPointer )
    {
        MyFileObject* pFile = (MyFileObject*)g_DragAndDropStruct.m_Value;
        assert( pFile );
        assert( m_pMesh );

        int len = strlen( pFile->m_FullPath );
        const char* filenameext = &pFile->m_FullPath[len-4];

        if( strcmp( filenameext, ".png" ) == 0 )
        {
            TextureDefinition* pOldTexture = m_pTexture;
            m_pTexture = g_pTextureManager->FindTexture( pFile->m_FullPath );
            SAFE_RELEASE( pOldTexture );
        }

        // update the panel so new Shader name shows up.
        g_pPanelWatch->m_pVariables[g_DragAndDropStruct.m_ID].m_Description = m_pTexture->m_Filename;
    }

    if( g_DragAndDropStruct.m_Type == DragAndDropType_TextureDefinitionPointer )
    {
        TextureDefinition* pOldTexture = m_pTexture;
        m_pTexture = (TextureDefinition*)g_DragAndDropStruct.m_Value;
        SAFE_RELEASE( pOldTexture );

        // update the panel so new Shader name shows up.
        g_pPanelWatch->m_pVariables[g_DragAndDropStruct.m_ID].m_Description = m_pTexture->m_Filename;
    }
}
#endif //MYFW_USING_WX

cJSON* ComponentMesh::ExportAsJSONObject()
{
    cJSON* component = ComponentRenderable::ExportAsJSONObject();

    if( m_pShaderGroup )
        cJSON_AddStringToObject( component, "Shader", m_pShaderGroup->GetName() );
    if( m_pTexture )
        cJSON_AddStringToObject( component, "Texture", m_pTexture->m_Filename );

    return component;
}

void ComponentMesh::ImportFromJSONObject(cJSON* jsonobj, unsigned int sceneid)
{
    ComponentRenderable::ImportFromJSONObject( jsonobj, sceneid );

    cJSON* shaderstringobj = cJSON_GetObjectItem( jsonobj, "Shader" );
    if( shaderstringobj )
    {
        ShaderGroup* pShaderGroup = g_pShaderGroupManager->FindShaderGroupByName( shaderstringobj->valuestring );
        if( pShaderGroup )
        {
            pShaderGroup->AddRef();
            SAFE_RELEASE( m_pShaderGroup );
            m_pShaderGroup = pShaderGroup;
        }
    }

    cJSON* texturestringobj = cJSON_GetObjectItem( jsonobj, "Texture" );
    if( texturestringobj )
    {
        TextureDefinition* pTexture = g_pTextureManager->FindTexture( texturestringobj->valuestring );
        if( pTexture )
        {
            pTexture->AddRef();
            SAFE_RELEASE( m_pTexture );
            m_pTexture = pTexture;
        }
    }
}

ComponentMesh& ComponentMesh::operator=(const ComponentMesh& other)
{
    assert( &other != this );

    ComponentRenderable::operator=( other );

    m_pMesh = other.m_pMesh;
    if( m_pMesh )
        m_pMesh->AddRef();

    m_pShaderGroup = other.m_pShaderGroup;
    if( m_pShaderGroup )
        m_pShaderGroup->AddRef();

    m_pTexture = other.m_pTexture;
    if( m_pTexture )
        m_pTexture->AddRef();

    return *this;
}

void ComponentMesh::SetShader(ShaderGroup* pShader)
{
    ComponentRenderable::SetShader( pShader );

    pShader->AddRef();
    SAFE_RELEASE( m_pShaderGroup );
    m_pShaderGroup = pShader;
}

void ComponentMesh::Draw(MyMatrix* pMatViewProj, ShaderGroup* pShaderOverride, int drawcount)
{
    ComponentRenderable::Draw( pMatViewProj, pShaderOverride, drawcount );

    m_pMesh->SetShaderGroup( m_pShaderGroup );
    m_pMesh->m_pTexture = m_pTexture;

    m_pMesh->m_Position = this->m_pComponentTransform->m_Transform;
    m_pMesh->Draw( pMatViewProj, 0, 0, 0, 0, 0, 0, pShaderOverride );
}
