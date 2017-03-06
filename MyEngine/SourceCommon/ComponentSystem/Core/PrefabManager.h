//
// Copyright (c) 2017 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef __PrefabManager_H__
#define __PrefabManager_H__

struct PrefabObject
{
    friend class PrefabFile;
    friend class PrefabManager;

    static const int MAX_PREFAB_NAME_LENGTH = 30;

protected:
    char m_Name[MAX_PREFAB_NAME_LENGTH];
    cJSON* m_jPrefab;
#if MYFW_USING_WX
    GameObject* m_pGameObject; // each prefab is instantiated in editor so inheritance code can compare values.
#endif

public:
    PrefabObject()
    {
        m_Name[0] = 0;
        m_jPrefab = 0;
#if MYFW_USING_WX
        m_pGameObject = 0;
#endif
    }

    void SetName(const char* name)
    {
        strcpy_s( m_Name, MAX_PREFAB_NAME_LENGTH, name );
    }
};

class PrefabFile
{
    friend class PrefabManager;

protected:
    MyFileObject* m_pFile;

#if MYFW_USING_WX
    std::vector<PrefabObject> m_pPrefabs;
#else
    MyList<PrefabObject> m_pPrefabs;
#endif

public:
    PrefabFile(MyFileObject* pFile);
    ~PrefabFile();

    static void StaticOnFileFinishedLoading(void* pObjectPtr, MyFileObject* pFile) { ((PrefabFile*)pObjectPtr)->OnFileFinishedLoading( pFile ); }
    void OnFileFinishedLoading(MyFileObject* pFile);

#if MYFW_USING_WX
    void Save();
#endif
};

class PrefabManager
{
protected:
#if MYFW_USING_WX
    std::vector<PrefabFile*> m_pPrefabFiles;
#else
    MyList<PrefabFile*> m_pPrefabFiles;
#endif

protected:
#if MYFW_USING_WX
    void LoadFileNow(const char* prefabfilename);
#endif

public:
    PrefabManager();
    virtual ~PrefabManager();

    unsigned int GetNumberOfFiles();
    void SetNumberOfFiles(unsigned int numfiles); // for non-editor build, on scene load, allocate enough entries for # of files.

    MyFileObject* GetFile(unsigned int fileindex);
    void RequestFile(const char* prefabfilename);

#if MYFW_USING_WX
    void CreatePrefabInFile(unsigned int fileindex, const char* prefabname, GameObject* pGameObject);

    void CreateFile(const char* relativepath);
    bool CreateOrLoadFile();
#endif
};

#endif //__PrefabManager_H__