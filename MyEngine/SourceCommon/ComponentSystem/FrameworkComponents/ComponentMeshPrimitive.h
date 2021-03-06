//
// Copyright (c) 2015-2019 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __ComponentMeshPrimitive_H__
#define __ComponentMeshPrimitive_H__

#include "ComponentMesh.h"

class ComponentTransform;

enum ComponentMeshPrimitives // Saved as string, order can change.
{
    ComponentMeshPrimitive_Plane,
    ComponentMeshPrimitive_Icosphere,
    ComponentMeshPrimitive_2DCircle,
    ComponentMeshPrimitive_Grass,
    ComponentMeshPrimitive_NumTypesAccessibleFromInterface,
    ComponentMeshPrimitive_ReferenceToAnotherMeshPrimitive = ComponentMeshPrimitive_NumTypesAccessibleFromInterface,
    ComponentMeshPrimitive_NumTypes,
};

extern const char* ComponentMeshPrimitiveTypeStrings[ComponentMeshPrimitive_NumTypes];

class ComponentMeshPrimitive : public ComponentMesh
{
protected:
    // Component Variable List.
    MYFW_COMPONENT_DECLARE_VARIABLE_LIST( ComponentMeshPrimitive ); //_VARIABLE_LIST

public:
    ComponentMeshPrimitives m_MeshPrimitiveType;

    // Plane.
    Vector2 m_Plane_Size;
    Vector2Int m_Plane_VertCount;
    bool m_Plane_UVsPerQuad;
    Vector2 m_Plane_UVStart;
    Vector2 m_Plane_UVRange;

    // Icosphere and 2D circle.
    float m_Sphere_Radius;

    // Reference to another mesh primitive component.
    ComponentMeshPrimitive* m_pOtherMeshPrimitive;

public:
    ComponentMeshPrimitive(ComponentSystemManager* pComponentSystemManager);
    virtual ~ComponentMeshPrimitive();
    SetClassnameWith2Parents( "MeshPrimitiveComponent", ComponentMesh, ComponentRenderable ); // Only first 8 characters count.

#if MYFW_USING_LUA
    static void LuaRegister(lua_State* luastate);
#endif //MYFW_USING_LUA

    virtual cJSON* ExportAsJSONObject(bool saveSceneID, bool saveID) override;
    virtual void ImportFromJSONObject(cJSON* jComponent, SceneID sceneID) override;
    virtual void FinishImportingFromJSONObject(cJSON* jComponent) override;

    virtual void Reset() override;
    virtual void CopyFromSameType_Dangerous(ComponentBase* pObject) override { *this = (ComponentMeshPrimitive&)*pObject; }
    virtual ComponentMeshPrimitive& operator=(ComponentMeshPrimitive& other);

    virtual void RegisterCallbacks() override;
    virtual void UnregisterCallbacks() override;

    void CreatePrimitive();

protected:
    // Callback functions for various events.
    //MYFW_DECLARE_COMPONENT_CALLBACK_TICK(); // TickCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONSURFACECHANGED(); // OnSurfaceChangedCallback
    MYFW_DECLARE_COMPONENT_CALLBACK_DRAW(); // DrawCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONTOUCH(); // OnTouchCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONBUTTONS(); // OnButtonsCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONKEYS(); // OnKeysCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONFILERENAMED(); // OnFileRenamedCallback

public:
#if MYFW_EDITOR
    bool m_PrimitiveSettingsChangedAtRuntime;

    // Component variable callbacks.
    virtual bool ShouldVariableBeAddedToWatchPanel(ComponentVariable* pVar) override;
    void* OnValueChanged(ComponentVariable* pVar, bool changedByInterface, bool finishedChanging, double oldValue, ComponentVariableValue* pNewValue);
#endif //MYFW_EDITOR
};

#endif //__ComponentMeshPrimitive_H__
