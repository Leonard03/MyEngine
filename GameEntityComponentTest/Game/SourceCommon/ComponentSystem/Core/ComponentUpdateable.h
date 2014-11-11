//
// Copyright (c) 2014 Jimmy Lord http://www.flatheadgames.com
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

#ifndef __ComponentUpdateable_H__
#define __ComponentUpdateable_H__

class ComponentTransform;

class ComponentUpdateable : public ComponentBase
{
public:
    ComponentTransform* m_pComponentTransform;

public:
    ComponentUpdateable();
    ComponentUpdateable(GameObject* owner);
    virtual ~ComponentUpdateable();

    virtual void Reset();

    // will return true if input is used.
    virtual bool OnTouch(int action, int id, float x, float y, float pressure, float size) = 0;
    virtual bool OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id) = 0;

public:
#if MYFW_USING_WX
    virtual void AddToObjectsPanel(wxTreeItemId gameobjectid);
    static void StaticFillPropertiesWindow(void* pObjectPtr) { ((ComponentUpdateable*)pObjectPtr)->FillPropertiesWindow(); }
    void FillPropertiesWindow();
#endif //MYFW_USING_WX
};

#endif //__ComponentUpdateable_H__