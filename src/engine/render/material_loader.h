#pragma once

#include <unordered_map>
#include <string>
#include "material.h"
#include "../../utils/file_watch_dog.h"



class MaterialLoader
{
public:
    inline static MaterialLoader* getInstance() { return ins; }

    inline static void Initialize() { if (!ins) ins = new MaterialLoader(); }

    std::shared_ptr<Material> getMaterial(const std::string& strFilePath);

    void onFileChangedListener(const std::string& strFilePath, eFileChangeType eType);

private:
    static inline MaterialLoader* ins = nullptr;

    MaterialLoader();
    ~MaterialLoader();

    std::unordered_map<std::string, std::shared_ptr<Material>> m_mapMaterials;

    static std::shared_ptr<Material> loadMaterialFromFile(const std::string& strFilePath);
};
