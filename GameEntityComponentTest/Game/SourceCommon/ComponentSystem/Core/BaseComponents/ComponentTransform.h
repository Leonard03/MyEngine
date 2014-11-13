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

#ifndef __ComponentTransform_H__
#define __ComponentTransform_H__

class ComponentTransform : public ComponentBase
{
public:
    MyMatrix m_Transform;

    ComponentTransform* m_pParentTransform;

public:
    Vector3 m_Position;
    Vector3 m_Scale;
    Vector3 m_Rotation;

public:
    ComponentTransform();
    ComponentTransform(GameObject* owner);
    virtual ~ComponentTransform();

    virtual void Reset();
    ComponentTransform& ComponentTransform::operator=(const ComponentTransform& other);

    // recalculate the matrix each time we set any of the 3 properties. // not efficient
    void SetPosition(Vector3 pos);
    void SetScale(Vector3 scale);
    void SetRotation(Vector3 rot);

    void SetParent(ComponentTransform* pNewParent);
    void UpdateMatrix();
    MyMatrix* GetMatrix();

public:
#if MYFW_USING_WX
    virtual void AddToObjectsPanel(wxTreeItemId gameobjectid);
    static void StaticFillPropertiesWindow(void* pObjectPtr) { ((ComponentTransform*)pObjectPtr)->FillPropertiesWindow(); }
    void FillPropertiesWindow();
    static void StaticOnNewParentTransformDrop(void* pObjectPtr) { ((ComponentTransform*)pObjectPtr)->OnNewParentTransformDrop(); }
    void OnNewParentTransformDrop();
#endif //MYFW_USING_WX
};

#endif //__ComponentTransform_H__
