//
// Copyright (c) 2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __ComponentVoxelMesh_H__
#define __ComponentVoxelMesh_H__

class VoxelChunk;

class ComponentVoxelMesh : public ComponentMesh
{
private:
    // Component Variable List
    MYFW_COMPONENT_DECLARE_VARIABLE_LIST( ComponentVoxelMesh );

protected:
    Vector3Int m_ChunkSize;

public:
    ComponentVoxelMesh();
    virtual ~ComponentVoxelMesh();
    SetClassnameWithParent( "VoxelMeshComponent", ComponentRenderable ); // only first 8 character count.

#if MYFW_USING_LUA
    static void LuaRegister(lua_State* luastate);
#endif //MYFW_USING_LUA

    //virtual cJSON* ExportAsJSONObject(bool savesceneid);
    virtual void ImportFromJSONObject(cJSON* jsonobj, unsigned int sceneid);

    virtual void Reset();
    virtual void CopyFromSameType_Dangerous(ComponentBase* pObject) { *this = (ComponentVoxelMesh&)*pObject; }
    ComponentVoxelMesh& operator=(const ComponentVoxelMesh& other);

    virtual void RegisterCallbacks();
    virtual void UnregisterCallbacks();

    // Rendering
    //virtual MaterialDefinition* GetMaterial() { return m_pMaterial; }
    //virtual void SetMaterial(MaterialDefinition* pMaterial);

    // Queries
    VoxelChunk* GetChunk() { return (VoxelChunk*)m_pMesh; }//m_pVoxelChunk; }
    void AddTileToTileInFocus(Vector2 mousepos);
    void DeleteTileInFocus(Vector2 mousepos);

protected:
    void CreateMesh();

protected:
    // Callback functions for various events.
    //MYFW_DECLARE_COMPONENT_CALLBACK_TICK(); // TickCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONSURFACECHANGED(); // OnSurfaceChangedCallback
#if _DEBUG
    MYFW_DECLARE_COMPONENT_CALLBACK_DRAW(); // DrawCallback
#endif
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONTOUCH(); // OnTouchCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONBUTTONS(); // OnButtonsCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONKEYS(); // OnKeysCallback
    //MYFW_DECLARE_COMPONENT_CALLBACK_ONFILERENAMED(); // OnFileRenamedCallback

public:
    // Runtime component variable callbacks. //_VARIABLE_LIST
    void* GetPointerValue(ComponentVariable* pVar);
    void SetPointerValue(ComponentVariable* pVar, void* newvalue);
    const char* GetPointerDesc(ComponentVariable* pVar);
    void SetPointerDesc(ComponentVariable* pVar, const char* newdesc);

public:
#if MYFW_USING_WX
    static bool m_PanelWatchBlockVisible;

    virtual void AddToObjectsPanel(wxTreeItemId gameobjectid);

    // Object panel callbacks.
    static void StaticOnLeftClick(void* pObjectPtr, wxTreeItemId id, unsigned int count) { ((ComponentVoxelMesh*)pObjectPtr)->OnLeftClick( count, true ); }
    void OnLeftClick(unsigned int count, bool clear);
    virtual void FillPropertiesWindow(bool clear, bool addcomponentvariables = false, bool ignoreblockvisibleflag = false);

    // Component variable callbacks.
    void* OnDrop(ComponentVariable* pVar, wxCoord x, wxCoord y);
    void* OnValueChanged(ComponentVariable* pVar, int controlid, bool finishedchanging, double oldvalue);

    static void StaticOnButtonCreateMesh(void* pObjectPtr) { ((ComponentVoxelMesh*)pObjectPtr)->OnButtonCreateMesh(); }
    void OnButtonCreateMesh();

    static void StaticOnButtonEditMesh(void* pObjectPtr) { ((ComponentVoxelMesh*)pObjectPtr)->OnButtonEditMesh(); }
    void OnButtonEditMesh();
#endif //MYFW_USING_WX
};

#endif //__ComponentVoxelMesh_H__