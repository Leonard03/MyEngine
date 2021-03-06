//
// Copyright (c) 2012-2019 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __EngineCore_H__
#define __EngineCore_H__

class BulletWorld;
class ComponentSystemManager;
class ComponentTypeManager;
class GameObject;
class EngineCore;
class EditorInterface;
class EditorMainFrame;
class EditorPrefs;
class EditorState;
class MyMeshText;

#if _DEBUG || MYFW_EDITOR
#define MYFW_PROFILING_ENABLED 1
#endif

extern EngineCore* g_pEngineCore;

enum ModifierKeys
{
    MODIFIERKEY_Control     = 0x0001,
    MODIFIERKEY_Alt         = 0x0002,
    MODIFIERKEY_Shift       = 0x0004,
    MODIFIERKEY_Space       = 0x0008,
    MODIFIERKEY_LeftMouse   = 0x0010,
    MODIFIERKEY_RightMouse  = 0x0020,
    MODIFIERKEY_MiddleMouse = 0x0040,
};

enum LayerValues
{
    Layer_MainScene             = 0x0001,
    Layer_HUD                   = 0x0002,
    Layer_Editor                = 0x2000,
    Layer_EditorUnselectable    = 0x4000,
    Layer_EditorFG              = 0x8000,
};

enum EditorInterfaceTypes
{
    EditorInterfaceType_SceneManagement,
    EditorInterfaceType_2DPointEditor,
    EditorInterfaceType_VoxelMeshEditor,
    EditorInterfaceType_NumInterfaces,
};

struct RequestedSceneInfo
{
    MyFileObject* m_pFile; // Acts as a flag whether or not scene was requested.
    SceneID m_SceneID; // Generally -1, unless scene requested for specific slot.
};

struct FrameTimingInfo
{
    float FrameTime;
    float Tick;
    float Update_Physics;
    float Render_Editor;
    float Render_Game;
};

extern void OnFileUpdated_CallbackFunction(GameCore* pGameCore, MyFileObject* pFile);

class EngineCore : public GameCore
{
    friend class EngineMainFrame;
    friend class EditorMainFrame_ImGui;

public:
    static const int MAX_SCENES_QUEUED_TO_LOAD = 5;
    static const int MAX_FRAMES_TO_STORE = 60*30; // 30 seconds @ 60fps.

protected:
    ComponentSystemManager* m_pComponentSystemManager;
    MyStackAllocator m_SingleFrameMemoryStack;

    ImGuiManager* m_pImGuiManager;
    BulletWorld* m_pBulletWorld;

#if MYFW_USING_LUA
    LuaGameState* m_pLuaGameState;
#endif //MYFW_USING_LUA

    bool m_EditorMode;
    bool m_AllowGameToRunInEditorMode;
    bool m_Paused;
    float m_PauseTimeToAdvance; // Advance clock by this much on next tick.

    Vector2 m_LastMousePos;

    float m_DebugFPS;
    int m_LuaMemoryUsedLastFrame;
    int m_LuaMemoryUsedThisFrame;
    size_t m_TotalMemoryAllocatedLastFrame;
    unsigned int m_SingleFrameStackSizeLastFrame;
    unsigned int m_SingleFrameStackSizeThisFrame;

    char* m_GameObjectFlagStrings[32];

    bool m_Debug_DrawWireframe;

    bool m_FreeAllMaterialsAndTexturesWhenUnloadingScene;

    double m_TimeSinceLastPhysicsStep;

    MyFileObject* m_pShaderFile_TintColor;
    MyFileObject* m_pShaderFile_TintColorWithAlpha;
    MyFileObject* m_pShaderFile_SelectedObjects;
    MyFileObject* m_pShaderFile_ClipSpaceTexture;
    MyFileObject* m_pShaderFile_ClipSpaceColor;
    MyFileObject* m_pShaderFile_FresnelTint;
    ShaderGroup* m_pShader_TintColor;
    ShaderGroup* m_pShader_TintColorWithAlpha;
    ShaderGroup* m_pShader_SelectedObjects;
    ShaderGroup* m_pShader_ClipSpaceTexture;
    ShaderGroup* m_pShader_ClipSpaceColor;
    ShaderGroup* m_pShader_FresnelTint;
    MaterialDefinition* m_pMaterial_Box2DDebugDraw;
    MaterialDefinition* m_pMaterial_3DGrid;
    MaterialDefinition* m_pMaterial_MousePicker;
    MaterialDefinition* m_pMaterial_ClipSpaceTexture;
    MaterialDefinition* m_pMaterial_ClipSpaceColor;
    MaterialDefinition* m_pMaterial_FresnelTint;

    float m_GameWidth;
    float m_GameHeight;

protected:
    bool m_UnloadAllScenesNextTick;
    bool m_SceneReloadRequested;
    
    // TODO: Replace this monstrosity with an ordered list.
    RequestedSceneInfo m_pSceneFilesLoading[MAX_SCENES_QUEUED_TO_LOAD];

#if MYFW_PROFILING_ENABLED
    FrameTimingInfo m_FrameTimingInfo[MAX_FRAMES_TO_STORE];
    unsigned int m_FrameTimingNextEntry;
#endif

#if MYFW_EDITOR
    EditorPrefs* m_pEditorPrefs;
    EditorState* m_pEditorState;
    EditorMainFrame* m_pEditorMainFrame;

    bool m_Debug_DrawMousePickerFBO;
    bool m_Debug_DrawSelectedAnimatedMesh;
    bool m_Debug_DrawSelectedMaterial;
    bool m_Debug_ShowProfilingInfo;
    bool m_Debug_DrawGLStats;

    MyFileObject* m_pSphereMeshFile;
    MySprite* m_pSprite_DebugQuad;
    MyMesh* m_pMesh_MaterialBall;
    FontDefinition* m_pDebugFont;
    MyMeshText* m_pDebugTextMesh; // DEBUG_HACK_SHOWGLSTATS

    EditorInterface* m_pEditorInterfaces[EditorInterfaceType_NumInterfaces];
    EditorInterfaceTypes m_CurrentEditorInterfaceType;
    EditorInterface* m_pCurrentEditorInterface;
#endif //MYFW_EDITOR

public:
    EngineCore(Renderer_Base* pRenderer = nullptr, bool createAndOwnGlobalManagers = true);
    virtual ~EngineCore();
    virtual void Cleanup();

    // EngineCore Getters.
    ComponentSystemManager* GetComponentSystemManager() { return m_pComponentSystemManager; }
    MyStackAllocator* GetSingleFrameMemoryStack() { return &m_SingleFrameMemoryStack; }
    ImGuiManager* GetImGuiManager() { return m_pImGuiManager; }
    BulletWorld* GetBulletWorld() { return m_pBulletWorld; }

    bool IsInEditorMode() { return m_EditorMode; }

    const char** GetGameObjectFlagStringArray() { return (const char**)m_GameObjectFlagStrings; }
    const char* GetGameObjectFlagString(unsigned int num) { MyAssert( num < 32 ); return m_GameObjectFlagStrings[num]; }

    bool GetDebug_DrawWireframe() { return m_Debug_DrawWireframe; }
    void ToggleDebug_DrawWireframe() { m_Debug_DrawWireframe = !m_Debug_DrawWireframe; }

    ShaderGroup* GetShader_TintColor()          { return m_pShader_TintColor; }
    ShaderGroup* GetShader_TintColorWithAlpha() { return m_pShader_TintColorWithAlpha; }
    ShaderGroup* GetShader_SelectedObjects()    { return m_pShader_SelectedObjects; }

    MaterialDefinition* GetMaterial_Box2DDebugDraw()   { return m_pMaterial_Box2DDebugDraw; }
    MaterialDefinition* GetMaterial_MousePicker()      { return m_pMaterial_MousePicker; }
    MaterialDefinition* GetMaterial_ClipSpaceTexture() { return m_pMaterial_ClipSpaceTexture; }
    MaterialDefinition* GetMaterial_ClipSpaceColor()   { return m_pMaterial_ClipSpaceColor; }
    MaterialDefinition* GetMaterial_FresnelTint()      { return m_pMaterial_FresnelTint; }

    // EngineCore Setters.

    // EngineCore Methods.
    void SaveEditorPrefs();

#if MYFW_USING_LUA
    static void LuaRegister(lua_State* luastate);
    virtual LuaGameState* CreateLuaGameState() { return MyNew LuaGameState( this ); }
    LuaGameState* GetLuaGameState() { return m_pLuaGameState; }
#endif //MYFW_USING_LUA

    virtual void InitializeManagers();
    void InitializeGameObjectFlagStrings(cJSON* jStringsArray);

    virtual ComponentTypeManager* CreateComponentTypeManager() = 0;

#if MYFW_EDITOR
    virtual EditorMainFrame* CreateEditorMainFrame();
#endif
    virtual void OneTimeInit();
    virtual bool IsReadyToRender();

    virtual void RequestClose(); // Override GameCore, will popup a confirm dialog in ImGui Editor builds.

    void StartFrame();
    virtual float Tick(float deltaTime);
    virtual void OnFocusGained();
    virtual void OnFocusLost();
    virtual void OnSurfaceChanged(uint32 x, uint32 y, uint32 width, uint32 height);
    virtual void OnDrawFrameStart(unsigned int canvasid);
    virtual void OnDrawFrame(unsigned int canvasid);
    virtual void OnDrawFrameDone();
    virtual void OnFileRenamed(const char* fullpathbefore, const char* fullpathafter);

    virtual void OnDropFile(const char* filename);

    void SetMousePosition(float x, float y);
    virtual void SetMouseLock(bool lock);
    virtual bool OnTouch(int action, int id, float x, float y, float pressure, float size);
    virtual bool OnTouchGameWindow(int action, int id, float x, float y, float pressure, float size);
    virtual bool OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id);
    virtual bool OnKeys(GameCoreButtonActions action, int keycode, int unicodechar);
    virtual bool OnChar(unsigned int c);

    virtual void OnModeTogglePlayStop();
    virtual void OnModePlay();
    virtual void OnModeStop();
    virtual void OnModePause();
    virtual void OnModeAdvanceTime(float time);

    virtual void RegisterGameplayButtons();
    virtual void UnregisterGameplayButtons();

    void CreateDefaultEditorSceneObjects();
    void CreateDefaultSceneObjects();
    void ReloadScene(SceneID sceneid);
    void ReloadSceneInternal(SceneID sceneid);
    void RequestScene(const char* fullpath);
    RequestedSceneInfo* RequestSceneInternal(const char* fullpath);
    void SwitchScene(const char* fullpath);
    void SaveScene(const char* fullpath, SceneID sceneid);
    void SaveAllScenes();
    void ExportBox2DScene(const char* fullpath, SceneID sceneid);
    void UnloadScene(SceneID sceneid, bool clearEditorObjects);
    void LoadSceneFromJSON(const char* scenename, const char* jsonstr, SceneID sceneid, bool playWhenFinishedLoading);

#if MYFW_EDITOR
    // Editor Getters/Setters.
    EditorPrefs* GetEditorPrefs() { return m_pEditorPrefs; }
    EditorState* GetEditorState() { return m_pEditorState; }

    EditorMainFrame* GetEditorMainFrame() { return m_pEditorMainFrame; }
#if MYFW_USING_IMGUI
    EditorMainFrame_ImGui* GetEditorMainFrame_ImGui() { return (EditorMainFrame_ImGui*)m_pEditorMainFrame; }
#endif

    bool GetDebug_DrawMousePickerFBO()       { return m_Debug_DrawMousePickerFBO; }
    bool GetDebug_DrawSelectedAnimatedMesh() { return m_Debug_DrawSelectedAnimatedMesh; }
    bool GetDebug_ShowProfilingInfo()        { return m_Debug_ShowProfilingInfo; }
    bool GetDebug_DrawGLStats()              { return m_Debug_DrawGLStats; }

    void ToggleDebug_DrawMousePickerFBO()       { m_Debug_DrawMousePickerFBO = !m_Debug_DrawMousePickerFBO; }
    void ToggleDebug_DrawSelectedAnimatedMesh() { m_Debug_DrawSelectedAnimatedMesh = !m_Debug_DrawSelectedAnimatedMesh; }
    void ToggleDebug_ShowProfilingInfo()        { m_Debug_ShowProfilingInfo = !m_Debug_ShowProfilingInfo; }
    void ToggleDebug_DrawGLStats()              { m_Debug_DrawGLStats = !m_Debug_DrawGLStats; }

    MySprite* GetSprite_DebugQuad(); // Will create the sprite if it doesn't exist.
    MyMesh* GetMesh_MaterialBall(); // Will create the mesh if it doesn't exist.

    // Editor Methods.
    bool HandleImGuiInput(int canvasid, int keyaction, int keycode, int mouseaction, int id, float x, float y, float pressure);
    bool HandleEditorInput(int canvasid, int keyaction, int keycode, int mouseaction, int id, float x, float y, float pressure);

    SceneID LoadSceneFromFile(const char* fullpath);
    void Editor_QuickSaveScene(const char* fullpath);
    void Editor_QuickLoadScene(const char* fullpath);
    void Editor_DeleteQuickScene(const char* fullpath);

    void Editor_OnSurfaceChanged(uint32 x, uint32 y, uint32 width, uint32 height);

    void RenderSingleObject(GameObject* pObject, FBODefinition* pFBOToUse = nullptr);
    void GetMouseRay(Vector2 mousepos, Vector3* start, Vector3* end);

    void SetGridVisible(bool visible);

    void SetEditorInterface(EditorInterfaceTypes type);
    EditorInterface* GetEditorInterface(EditorInterfaceTypes type);
    EditorInterface* GetCurrentEditorInterface();
    EditorInterfaceTypes GetCurrentEditorInterfaceType() { return m_CurrentEditorInterfaceType; }
#endif //MYFW_EDITOR
};

#endif //__EngineCore_H__
