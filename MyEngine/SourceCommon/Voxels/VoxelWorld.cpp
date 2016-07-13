//
// Copyright (c) 2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "EngineCommonHeader.h"
#include "VoxelBlock.h"
#include "VoxelChunk.h"
#include "VoxelWorld.h"

VoxelWorld::VoxelWorld()
{
    m_NumChunkPointersAllocated = 0;
    m_pActiveWorldChunkPtrs = 0;

    m_WorldSize.Set( 0, 0, 0 );
    m_ChunkSize.Set( 16, 16, 16 );
    m_BlockSize.Set( 1, 1, 1 );

    m_WorldOffset.Set( 0, 0, 0 );
}

VoxelWorld::~VoxelWorld()
{
    delete[] m_pActiveWorldChunkPtrs;

    for( CPPListNode* pNode = m_pChunksFree.GetHead(); pNode; )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;
        pNode = pNode->GetNext();

        delete pChunk;
    }

    for( CPPListNode* pNode = m_pChunksLoading.GetHead(); pNode; )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;
        pNode = pNode->GetNext();

        delete pChunk;
    }    

    for( CPPListNode* pNode = m_pChunksVisible.GetHead(); pNode; )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;
        pNode = pNode->GetNext();

        delete pChunk;
    }
}

void VoxelWorld::Initialize(Vector3Int visibleworldsize)
{
    // Make sure init is only called once.
    MyAssert( m_NumChunkPointersAllocated == 0 );
    
    SetWorldSize( visibleworldsize );

    Vector3 chunkoffset = m_ChunkSize.MultiplyComponents( m_BlockSize );

    // make 0,0,0 the bottom left corner. TODO: pass in an offset
    m_WorldOffset.Set( 0, 0, 0 );

    for( int z=0; z<m_WorldSize.z; z++ )
    {
        for( int y=0; y<m_WorldSize.y; y++ )
        {
            for( int x=0; x<m_WorldSize.x; x++ )
            {
                VoxelChunk* pChunk = MyNew VoxelChunk;
                m_pChunksFree.AddHead( pChunk );

                PrepareChunk( Vector3Int( x, y, z ) );
            }
        }
    }
}

void VoxelWorld::Tick(double timepassed)
{
    // build the mesh for a single chunk per frame.
    VoxelChunk* pChunk = (VoxelChunk*)m_pChunksLoading.GetHead();

    if( pChunk )
    {
        pChunk->RebuildMesh();

        m_pChunksVisible.MoveTail( pChunk );
    }
}

void VoxelWorld::SetWorldSize(Vector3Int visibleworldsize)
{
    // TODO: remove this once function is properly implemented
    MyAssert( m_NumChunkPointersAllocated == 0 );
    
    unsigned int pointersneeded = visibleworldsize.x * visibleworldsize.y * visibleworldsize.z;

    if( pointersneeded > m_NumChunkPointersAllocated )
    {
        // TODO: allocate an array with enough pointers for the new size if it's bigger than the old,
        //       copy the existing chunk ptrs over
        //       delete the old array

        m_NumChunkPointersAllocated = pointersneeded;
        m_pActiveWorldChunkPtrs = MyNew VoxelChunk*[pointersneeded];
    }

    m_WorldSize = visibleworldsize;
}

void VoxelWorld::SetWorldCenter(Vector3 scenepos)
{
    Vector3Int worldpos;
    worldpos.x = (int)(scenepos.x / m_BlockSize.x) / m_ChunkSize.x;
    worldpos.y = (int)(scenepos.y / m_BlockSize.y) / m_ChunkSize.x;
    worldpos.z = (int)(scenepos.z / m_BlockSize.z) / m_ChunkSize.x;

    SetWorldCenter( worldpos );
}

void VoxelWorld::SetWorldCenter(Vector3Int newworldcenter)
{
    Vector3Int currentworldcenter = m_WorldOffset + m_WorldSize/2;

    int numchunks = m_WorldSize.x * m_WorldSize.y * m_WorldSize.z;

    // shift the current chunks on the x-axis
    {
        int stepx = newworldcenter.x - currentworldcenter.x;

        while( stepx > 0 ) // shift all chunks to the left (x-axis)
        {
            m_WorldOffset.x += 1;

            for( int x=0; x<m_WorldSize.x; x++ )
                for( int y=0; y<m_WorldSize.y; y++ )
                    for( int z=0; z<m_WorldSize.z; z++ )
                        ShiftChunk( Vector3Int( x, y, z ), Vector3Int( x+1, y, z ), x == (m_WorldSize.x - 1) ? true : false );

            stepx--;
        }

        while( stepx < 0 ) // shift all chunks to the right (x-axis)
        {
            m_WorldOffset.x -= 1;

            for( int x=m_WorldSize.x-1; x>=0; x-- ) // loop backwards
                for( int y=0; y<m_WorldSize.y; y++ )
                    for( int z=0; z<m_WorldSize.z; z++ )
                        ShiftChunk( Vector3Int( x, y, z ), Vector3Int( x-1, y, z ), x == 0 ? true : false );

            stepx++;
        }
    }

    // shift the current chunks on the y-axis
    {
        int stepy = newworldcenter.y - currentworldcenter.y;

        while( stepy > 0 ) // shift all chunks up (y-axis)
        {
            m_WorldOffset.y += 1;

            for( int y=0; y<m_WorldSize.y; y++ )
                for( int x=0; x<m_WorldSize.x; x++ )
                    for( int z=0; z<m_WorldSize.z; z++ )
                        ShiftChunk( Vector3Int( x, y, z ), Vector3Int( x, y+1, z ), y == (m_WorldSize.y - 1) ? true : false );

            stepy--;
        }

        while( stepy < 0 ) // shift all chunks down (y-axis)
        {
            m_WorldOffset.y -= 1;

            for( int y=m_WorldSize.y-1; y>=0; y-- ) // loop backwards
                for( int x=0; x<m_WorldSize.x; x++ )
                    for( int z=0; z<m_WorldSize.z; z++ )
                        ShiftChunk( Vector3Int( x, y, z ), Vector3Int( x, y-1, z ), y == 0 ? true : false );

            stepy++;
        }
    }

    // shift the current chunks on the z-axis
    {
        int stepz = newworldcenter.z - currentworldcenter.z;

        while( stepz > 0 ) // shift all chunks in (z-axis)
        {
            m_WorldOffset.z += 1;

            for( int z=0; z<m_WorldSize.z; z++ )
                for( int y=0; y<m_WorldSize.y; y++ )
                    for( int x=0; x<m_WorldSize.x; x++ )
                        ShiftChunk( Vector3Int( x, y, z ), Vector3Int( x, y, z+1 ), z == (m_WorldSize.z - 1) ? true : false );

            stepz--;
        }

        while( stepz < 0 ) // shift all chunks out (y-axis)
        {
            m_WorldOffset.z -= 1;

            for( int z=m_WorldSize.z-1; z>=0; z-- ) // loop backwards
                for( int y=0; y<m_WorldSize.y; y++ )
                    for( int x=0; x<m_WorldSize.x; x++ )
                        ShiftChunk( Vector3Int( x, y, z ), Vector3Int( x, y, z-1 ), z == 0 ? true : false );

            stepz++;
        }
    }
}

void VoxelWorld::UpdateVisibility(void* pUserData)
{
    // TODO: Update list of visible chunks

    // Add all visible chunks to scene graph
    for( CPPListNode* pNode = m_pChunksVisible.GetHead(); pNode; pNode = pNode->GetNext() )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;

        pChunk->AddToSceneGraph( pUserData );
    }
}

// ============================================================================================================================
// Protected/Internal functions
// ============================================================================================================================
unsigned int VoxelWorld::GetActiveChunkArrayIndex(Vector3Int worldpos)
{
    return GetActiveChunkArrayIndex( worldpos.x, worldpos.y, worldpos.z );
}

unsigned int VoxelWorld::GetActiveChunkArrayIndex(int worldx, int worldy, int worldz)
{
    unsigned int index = (worldz - m_WorldOffset.z) * m_WorldSize.y * m_WorldSize.x +
                         (worldy - m_WorldOffset.y) * m_WorldSize.x +
                         (worldx - m_WorldOffset.x);

    MyAssert( index < m_NumChunkPointersAllocated );

    return index;
}

VoxelChunk* VoxelWorld::GetActiveChunk(unsigned int arrayindex)
{
    MyAssert( arrayindex < m_NumChunkPointersAllocated );

    return m_pActiveWorldChunkPtrs[arrayindex];
}

VoxelChunk* VoxelWorld::GetActiveChunk(Vector3Int worldpos)
{
    return GetActiveChunk( GetActiveChunkArrayIndex( worldpos ) );
}

VoxelChunk* VoxelWorld::GetActiveChunk(int x, int y, int z)
{
    return GetActiveChunk( GetActiveChunkArrayIndex( x, y, z ) );
}

void VoxelWorld::PrepareChunk(Vector3Int worldpos)
{
    VoxelChunk* pChunk = (VoxelChunk*)m_pChunksFree.GetHead();
    if( pChunk == 0 )
    {
        LOGInfo( "VoxelWorld", "Attempting to prepare chunk, but none available\n" );
        return;
    }

    Vector3Int chunkblockoffset = m_ChunkSize.MultiplyComponents( worldpos );
    Vector3 chunkposition = chunkblockoffset.MultiplyComponents( m_BlockSize );

    unsigned int arrayindex = GetActiveChunkArrayIndex( worldpos );
    m_pActiveWorldChunkPtrs[arrayindex] = pChunk;

    pChunk->Initialize( this, chunkposition, m_ChunkSize, chunkblockoffset );

    m_pChunksLoading.MoveTail( pChunk );
}

void VoxelWorld::ShiftChunk(Vector3Int to, Vector3Int from, bool isedgeblock)
{
    int tooffset = to.z * m_WorldSize.y * m_WorldSize.x + to.y * m_WorldSize.x + to.x;
                    
    VoxelChunk* pChunk = m_pActiveWorldChunkPtrs[tooffset];
    if( pChunk )
    {
        m_pChunksFree.MoveTail( pChunk );
        pChunk->RemoveFromSceneGraph();
    }

    if( isedgeblock )
    {
        PrepareChunk( m_WorldOffset + to );
    }
    else
    {
        int fromoffset = from.z * m_WorldSize.y * m_WorldSize.x + from.y * m_WorldSize.x + from.x;
        m_pActiveWorldChunkPtrs[tooffset] = m_pActiveWorldChunkPtrs[fromoffset];
        m_pActiveWorldChunkPtrs[fromoffset] = 0;
    }
}

// ============================================================================================================================
// Space conversions
// ============================================================================================================================


// ============================================================================================================================
// Collision/Block queries
// ============================================================================================================================
bool VoxelWorld::IsBlockEnabled(Vector3Int pos)
{
    return IsBlockEnabled( pos.x, pos.y, pos.z );
}

bool VoxelWorld::IsBlockEnabled(int x, int y, int z)
{
    if( x < (m_WorldOffset.x * m_ChunkSize.x) || x >= (m_WorldOffset.x + m_WorldSize.x) * m_ChunkSize.x ||
        y < (m_WorldOffset.y * m_ChunkSize.y) || y >= (m_WorldOffset.y + m_WorldSize.y) * m_ChunkSize.y ||
        z < (m_WorldOffset.z * m_ChunkSize.z) || z >= (m_WorldOffset.z + m_WorldSize.z) * m_ChunkSize.z )
    {
        return false;
    }

    Vector3Int chunkpos( x/m_ChunkSize.x, y/m_ChunkSize.y, z/m_ChunkSize.z );
    chunkpos -= m_WorldOffset;

    VoxelChunk* pChunk = m_pActiveWorldChunkPtrs[chunkpos.z * m_WorldSize.y * m_WorldSize.x + chunkpos.y * m_WorldSize.x + chunkpos.x];

    return pChunk->IsBlockEnabled( x, y, z );
}

float VoxelWorld::GetSceneYForNextBlockBelowPosition(Vector3 scenepos, float radius)
{
    //m_WorldSize.Set( 10, 3, 10 );
    //m_ChunkSize.Set( 16, 16, 16 );
    //m_BlockSize.Set( 1, 1, 1 );

    Vector3Int worldpos;
    float highesty = -FLT_MAX;
    for( int i=0; i<4; i++ )
    {
        float xoff;
        float zoff;

        if( i == 0 )      { xoff = radius * -1; zoff = radius * -1; }
        else if( i == 1 ) { xoff = radius * -1; zoff = radius *  1; }
        else if( i == 2 ) { xoff = radius *  1; zoff = radius * -1; }
        else if( i == 3 ) { xoff = radius *  1; zoff = radius *  1; }

        worldpos.x = (int)((scenepos.x + xoff + m_BlockSize.x/2) / m_BlockSize.x);
        worldpos.y = (int)((scenepos.y + m_BlockSize.y * 1.1f) / m_BlockSize.y); // Move player up a bit, then test
        worldpos.z = (int)((scenepos.z + zoff + m_BlockSize.x/2) / m_BlockSize.z);

        bool enabled = IsBlockEnabled( worldpos );

        while( !enabled && worldpos.y > 0 )
        {
            worldpos.y--;
            enabled = IsBlockEnabled( worldpos );
        }

        float sceney = worldpos.y * m_BlockSize.y + m_BlockSize.y;

        if( sceney > highesty )
            highesty = sceney;
    }

    return highesty;
}
