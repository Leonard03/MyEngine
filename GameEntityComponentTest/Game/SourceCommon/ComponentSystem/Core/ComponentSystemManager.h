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

#ifndef __ComponentSystemManager_H__
#define __ComponentSystemManager_H__

class ComponentSystemManager;
class GameObject;
class ComponentBase;

extern ComponentSystemManager* g_pComponentSystemManager;

class ComponentSystemManager
{
public:
    CPPListHead m_GameObjects;

    // a component can only exist in one of these lists ATM
    CPPListHead m_ComponentsInputHandlers;
    CPPListHead m_ComponentsUpdateable;
    CPPListHead m_ComponentsRenderable;
    CPPListHead m_ComponentsData;

public:
    ComponentSystemManager();
    virtual ~ComponentSystemManager();

    GameObject* CreateGameObject();
    ComponentBase* AddComponent(ComponentBase* pComponent);

    void Tick(double TimePassed);
    void OnDrawFrame();

    bool OnTouch(int action, int id, float x, float y, float pressure, float size);
    bool OnButtons(GameCoreButtonActions action, GameCoreButtonIDs id);
};

#endif //__ComponentSystemManager_H__