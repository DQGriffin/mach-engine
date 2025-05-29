//
// Created by Dquavius Griffin on 5/23/25.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <string>

#include "../archive/Archive.h"
#include "../rendering/debug/BulletDebugDrawer.h"
#include "../rendering/primatives/Model.h"


namespace mach
{
    class AssetManager
    {
    private:
        std::vector<mach::archive::Archive> archives;
        std::unordered_map<std::string, std::unique_ptr<mach::renderer::Model>> models;

    public:
        AssetManager() = default;
        ~AssetManager() = default;

        void preloadModel(const std::string& filePath);
        mach::renderer::Model& requireModel(const std::string& filePath);
        mach::renderer::Model* tryGetModel(const std::string& filePath);

    private:
        bool isModelLoaded(const std::string& filePath);
        void loadModel(const std::string& filePath);
        mach::archive::Archive* getContainingArchive(const std::string& filePath);
    };
}


#endif //ASSETMANAGER_H
