//
// Copyright (c) 2017-2019 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __EditorInterface_H__
#define __EditorInterface_H__

class GameObject;

class EditorInterface
{
protected:
    bool m_ShowRightClickMenu;
    GameObject* m_pGameObjectRightClicked;

public:
    EditorInterface();
    virtual ~EditorInterface();

    virtual void Initialize() = 0;

    virtual void OnActivated();
    virtual void OnDeactivated();

    virtual void Tick(float deltaTime);
    virtual void OnDrawFrame(unsigned int canvasid);

    virtual bool HandleInput(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure) = 0;

    virtual void RenderObjectIDsToFBO();
    virtual unsigned int GetIDAtPixel(unsigned int x, unsigned int y, bool createNewBitmap, bool includeTransformGizmo);
    virtual GameObject* GetObjectAtPixel(unsigned int x, unsigned int y, bool createNewBitmap, bool includeTransformGizmo);
    virtual void SelectObjectsInRectangle(unsigned int sx, unsigned int sy, unsigned int ex, unsigned int ey);

    void SetModifierKeyStates(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure);
    void ClearModifierKeyStates(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure);
    bool HandleInputForEditorCamera(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure);
};

#endif //__EditorInterface_H__
