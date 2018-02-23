//
// Copyright (c) 2018 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __EditorPrefs_H__
#define __EditorPrefs_H__

class EditorPrefs;
class EngineCore;

extern EditorPrefs* g_pEditorPrefs;

enum GLViewTypes
{
    GLView_Full,
    GLView_Tall,
    GLView_Square,
    GLView_Wide,
    GLView_NumTypes,
};

enum DefaultPerspectives
{
    Perspective_CenterEditor,
    Perspective_CenterGame,
    Perspective_CenterSideBySide,
    Perspective_FullFrameGame,
    Perspective_NumPerspectives,
};

enum EngineEditorWindowTypes
{
    EngineEditorWindow_Editor,
    EngineEditorWindow_PanelLog,
    EngineEditorWindow_NumTypes,
};

enum LaunchPlatforms
{
#if MYFW_WINDOWS
    LaunchPlatform_Win32,
    LaunchPlatform_Win64,
    LaunchPlatform_NaCl,
    LaunchPlatform_Android,
    LaunchPlatform_Emscripten,
#elif MYFW_OSX
    LaunchPlatform_OSX,
    LaunchPlatform_iOSSimulator,
    LaunchPlatform_iOSDevice,
    LaunchPlatform_iOSDevice_iOS6,
#endif
    // AddNewLaunchPlatform
    LaunchPlatform_NumPlatforms,
};

extern const char* g_DefaultPerspectiveMenuLabels[Perspective_NumPerspectives];
extern const char* g_DefaultEngineEditorWindowTypeMenuLabels[EngineEditorWindow_NumTypes];
extern const char* g_LaunchPlatformsMenuLabels[LaunchPlatform_NumPlatforms];

struct GridSettings
{
    bool visible;
    bool snapenabled;
    Vector3 stepsize;
};

class EditorPrefs
{
private:
    FILE* m_pSaveFile; // Stored temporarily between SaveStart and Save Finish.

protected:
    // Editor preferences JSON object
    cJSON* m_jEditorPrefs;

    // Editor preferences
    int m_WindowX;
    int m_WindowY;
    int m_WindowWidth;
    int m_WindowHeight;
    bool m_IsWindowMaximized;

    bool m_View_ShowEditorIcons;
    bool m_View_SelectedObjects_ShowWireframe;
    bool m_View_SelectedObjects_ShowEffect;
    GLViewTypes m_Aspect_CurrentGameWindowAspectRatio;
    GridSettings m_GridSettings;
    bool m_Mode_SwitchFocusOnPlayStop;
    bool m_Debug_DrawPhysicsDebugShapes;

public:
    EditorPrefs();
    ~EditorPrefs();

    void Init();
    void LoadWindowSizePrefs();
    void LoadPrefs();
    void LoadLastSceneLoaded();
    cJSON* SaveStart();
    void SaveFinish(cJSON* jPrefs);

    cJSON* GetEditorPrefsJSONString() { return m_jEditorPrefs; }

    // Preference Setters
    void SetWindowProperties(int x, int y, int w, int h, bool maximized)
    {
        m_WindowX = x;
        m_WindowY = y;
        m_WindowWidth = w;
        m_WindowHeight = h;
        m_IsWindowMaximized = false;
    }

    GridSettings* GetGridSettings() { return &m_GridSettings; }

    bool Get_View_ShowEditorIcons() { return m_View_ShowEditorIcons; }
    bool Get_View_SelectedObjects_ShowWireframe() { return m_View_SelectedObjects_ShowWireframe; }
    bool Get_View_SelectedObjects_ShowEffect() { return m_View_SelectedObjects_ShowEffect; }
    GLViewTypes Get_Aspect_GameAspectRatio() { return m_Aspect_CurrentGameWindowAspectRatio; }
    bool Get_Grid_Visible() { return m_GridSettings.visible; }
    bool Get_Grid_SnapEnabled() { return m_GridSettings.snapenabled; }
    bool Get_Mode_SwitchFocusOnPlayStop() { return m_Mode_SwitchFocusOnPlayStop; }
    bool Get_Debug_DrawPhysicsDebugShapes() { return m_Debug_DrawPhysicsDebugShapes; }

    void Toggle_View_ShowEditorIcons() { m_View_ShowEditorIcons = !m_View_ShowEditorIcons; }
    void Toggle_View_SelectedObjects_ShowWireframe() { m_View_SelectedObjects_ShowWireframe = !m_View_SelectedObjects_ShowWireframe; }
    void Toggle_View_SelectedObjects_ShowEffect() { m_View_SelectedObjects_ShowEffect = !m_View_SelectedObjects_ShowEffect; }
    void Set_Aspect_GameAspectRatio(GLViewTypes newaspect) { m_Aspect_CurrentGameWindowAspectRatio = newaspect; }
    void Toggle_Grid_Visible();
    void Toggle_Grid_SnapEnabled();
    void Toggle_Mode_SwitchFocusOnPlayStop() { m_Mode_SwitchFocusOnPlayStop = !m_Mode_SwitchFocusOnPlayStop; }
    void Toggle_Debug_DrawPhysicsDebugShapes() { m_Debug_DrawPhysicsDebugShapes = !m_Debug_DrawPhysicsDebugShapes; }
};

#endif //__EditorPrefs_H__
