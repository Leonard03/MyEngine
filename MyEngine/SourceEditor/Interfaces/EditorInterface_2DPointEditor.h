//
// Copyright (c) 2016-2019 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __EditorInterface_2DPointEditor_H__
#define __EditorInterface_2DPointEditor_H__

#include "EditorInterface.h"

class Component2DCollisionObject;

class EditorInterface_2DPointEditor : public EditorInterface
{
public:
    enum MaterialTypes
    {
        Mat_Lines, // Drawn by box2d debug draw code.
        Mat_Points,
        Mat_SelectedPoint,
        Mat_NumMaterials,
    };

protected:
    Component2DCollisionObject* m_pCollisionObject;

    GameObject* m_pPoint;
    int m_IndexOfPointBeingDragged;
    b2Vec2 m_PositionMouseWentDown;
    bool m_NewMousePress;
    bool m_AddedVertexWhenMouseWasDragged;

    MaterialDefinition* m_pMaterials[Mat_NumMaterials];

public:
    EditorInterface_2DPointEditor(EngineCore* pEngineCore);
    virtual ~EditorInterface_2DPointEditor();

    virtual void Initialize();

    virtual void OnActivated();
    virtual void OnDeactivated();
    virtual void OnDrawFrame(unsigned int canvasID);

    virtual bool HandleInput(int keyAction, int keyCode, int mouseAction, int id, float x, float y, float pressure);

    virtual void RenderObjectIDsToFBO();

    void CancelCurrentOperation();

    Component2DCollisionObject* Get2DCollisionObjectBeingEdited() { return m_pCollisionObject; }
    void Set2DCollisionObjectToEdit(Component2DCollisionObject* pCollisionObject);

    MaterialDefinition* GetMaterial(MaterialTypes type);
};

#endif //__EditorInterface_2DPointEditor_H__
