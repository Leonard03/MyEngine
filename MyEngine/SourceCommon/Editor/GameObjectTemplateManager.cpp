//
// Copyright (c) 2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "EngineCommonHeader.h"

GameObjectTemplateManager::GameObjectTemplateManager()
{
    char* filestring = PlatformSpecific_LoadFile( "DataEngine/EngineGameObjects.mytemplate", 0 );

    m_jTemplatesRoot = cJSON_Parse( filestring );

    m_jTemplatesArray = cJSON_GetObjectItem( m_jTemplatesRoot, "Templates" );
}

GameObjectTemplateManager::~GameObjectTemplateManager()
{
    cJSON_Delete( m_jTemplatesRoot );
}

cJSON* GameObjectTemplateManager::GetTemplateJSONObject(unsigned int templateid)
{
    cJSON* jTemplate = cJSON_GetArrayItem( m_jTemplatesArray, templateid );
    
    return jTemplate;
}

unsigned int GameObjectTemplateManager::GetNumberOfTemplates()
{
    return (unsigned int)cJSON_GetArraySize( m_jTemplatesArray );
}

const char* GameObjectTemplateManager::GetTemplateName(unsigned int templateid)
{
    cJSON* jTemplate = cJSON_GetArrayItem( m_jTemplatesArray, templateid );

    cJSON* jName = cJSON_GetObjectItem( jTemplate, "Name" );
    return jName->valuestring;
}