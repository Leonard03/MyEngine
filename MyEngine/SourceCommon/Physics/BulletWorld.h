//
// Copyright (c) 2014-2017 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __BulletWorld_H__
#define __BulletWorld_H__

class BulletWorld;
class BulletDebugDraw;

extern BulletWorld* g_pBulletWorld;

class BulletWorld
{
public:
    btDiscreteDynamicsWorld* m_pDynamicsWorld;
    btSequentialImpulseConstraintSolver* m_pSolver;
    btDefaultCollisionConfiguration* m_pCollisionConfiguration;
    btCollisionDispatcher* m_pDispatcher;
    btBroadphaseInterface* m_pOverlappingPairCache;

    // keep track of the shapes, we release memory at exit.
    // make sure to re-use collision shapes among rigid bodies whenever possible!
    btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;

    BulletDebugDraw* m_pBulletDebugDraw;

public:
    BulletWorld(MaterialDefinition* debugdrawmaterial, MyMatrix* pMatProj, MyMatrix* pMatView);
    ~BulletWorld();

    void CreateWorld(MaterialDefinition* debugdrawmaterial, MyMatrix* pMatProj, MyMatrix* pMatView);
    void PhysicsUpdate(float deltatime);
    void PhysicsStep();
    void Cleanup();
};

#endif //__BulletWorld_H__
