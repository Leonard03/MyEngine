//
// Copyright (c) 2012-2014 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "GameCommonHeader.h"

// sort by category, otherwise right-click menu will have duplicates.
ComponentTypeInfo g_ComponentTypeInfo[Component_NumComponents] =
{
    { "Renderables",    "Sprite",          },
    { "Input handlers", "Track mouse pos", },
    { "AI",             "AI Chase Player", },
};

ComponentBase* GameComponentTypeManager::CreateComponent(int type)
{
    switch( type )
    {
    case Component_Sprite:              return MyNew ComponentSprite;
    case Component_InputTrackMousePos:  return MyNew ComponentInputTrackMousePos;
    case Component_AIChasePlayer:       return MyNew ComponentAIChasePlayer;
    }

    assert( false );
    return 0;
}

unsigned int GameComponentTypeManager::GetNumberOfComponentTypes()
{
    return Component_NumComponents;
}

char* GameComponentTypeManager::GetTypeCategory(int type)
{
    return g_ComponentTypeInfo[type].category;
}

char* GameComponentTypeManager::GetTypeName(int type)
{
    return g_ComponentTypeInfo[type].name;
}
