//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "MyEnginePCH.h"

#include "VisualScriptNodes.h"

#include "MyNodeGraph.h"
#include "MyNode.h"

#include "../../SourceCommon/ComponentSystem/BaseComponents/ComponentBase.h"
#include "../../SourceCommon/ComponentSystem/BaseComponents/ComponentVariable.h"

#undef AddVar
#define AddVar ComponentBase::AddVariable_Base

VisualScriptNodeTypeManager::VisualScriptNodeTypeManager()
: MyNodeTypeManager()
{
}

MyNodeGraph::MyNode* VisualScriptNodeTypeManager::AddCreateNodeItemsToContextMenu(Vector2 pos, MyNodeGraph* pNodeGraph)
{
    if( ImGui::BeginMenu( "Values" ) )
    {
        if( ImGui::MenuItem( "Float" ) )                { ImGui::EndMenu(); return CreateNode( "Value_Float", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Color" ) )                { ImGui::EndMenu(); return CreateNode( "Value_Color", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "GameObject" ) )           { ImGui::EndMenu(); return CreateNode( "Value_GameObject", pos, pNodeGraph ); }
        if( ImGui::MenuItem( "Component" ) )            { ImGui::EndMenu(); return CreateNode( "Value_Component", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Math Operations" ) )
    {
        if( ImGui::MenuItem( "Add" ) )                  { ImGui::EndMenu(); return CreateNode( "MathOp_Add", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Conditions" ) )
    {
        if( ImGui::MenuItem( "GreaterEqual" ) )         { ImGui::EndMenu(); return CreateNode( "Condition_GreaterEqual", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Events" ) )
    {
        if( ImGui::MenuItem( "KeyPress" ) )             { ImGui::EndMenu(); return CreateNode( "Event_KeyPress", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    if( ImGui::BeginMenu( "Actions" ) )
    {
        if( ImGui::MenuItem( "Disable GameObject" ) )   { ImGui::EndMenu(); return CreateNode( "Disable_GameObject", pos, pNodeGraph ); }
        ImGui::EndMenu();
    }

    return nullptr;
}

MyNodeGraph::MyNode* VisualScriptNodeTypeManager::CreateNode(const char* typeName, Vector2 pos, MyNodeGraph* pNodeGraph)
{
    // TODO: Fix nonsense rand() for NodeID.
    uint32 newNodeID = rand();

#define TypeIs(name) strcmp( typeName, name ) == 0 )

    if( TypeIs( "Value_Float" )             return MyNew VisualScriptNode_Value_Float(              pNodeGraph, newNodeID, "Float",             pos, 0.5f );
    if( TypeIs( "Value_Color" )             return MyNew VisualScriptNode_Value_Color(              pNodeGraph, newNodeID, "Color",             pos, ColorByte(255, 255, 255, 255) );
    if( TypeIs( "Value_GameObject" )        return MyNew VisualScriptNode_Value_GameObject(         pNodeGraph, newNodeID, "GameObject",        pos, nullptr );
    if( TypeIs( "Value_Component" )         return MyNew VisualScriptNode_Value_Component(          pNodeGraph, newNodeID, "Component",         pos, nullptr );
    if( TypeIs( "MathOp_Add" )              return MyNew VisualScriptNode_MathOp_Add(               pNodeGraph, newNodeID, "Add",               pos );
    if( TypeIs( "Condition_GreaterEqual" )  return MyNew VisualScriptNode_Condition_GreaterEqual(   pNodeGraph, newNodeID, "GreaterEqual",      pos );
    if( TypeIs( "Event_KeyPress" )          return MyNew VisualScriptNode_Event_KeyPress(           pNodeGraph, newNodeID, "KeyPress",          pos, 'Z' );
    if( TypeIs( "Disable_GameObject" )      return MyNew VisualScriptNode_Disable_GameObject(       pNodeGraph, newNodeID, "DisableGameObject", pos, nullptr );

#undef TypeIs

    MyAssert( false );

    return nullptr;
}