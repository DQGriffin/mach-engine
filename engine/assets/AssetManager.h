//
// Created by Dquavius Griffin on 5/23/25.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <string>

#include "../archive/Archive.h"
#include "../rendering/debug/BulletDebugDrawer.h"
#include "../rendering/primatives/Model.h"
#include "loaders/shaders.h"


namespace mach
{
    class AssetManager
    {
    private:
        std::vector<std::unique_ptr<mach::archive::Archive>> archives;
        std::unordered_map<std::string, std::unique_ptr<mach::renderer::Model>> models;

        std::unordered_map<std::string, assets::loaders::ShaderDefinition> shaderDefinitions;
        std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

    public:
        AssetManager() = default;
        ~AssetManager() = default;

        void loadArchive(const std::string& archivePath);

        void preloadModel(const std::string& filePath);
        mach::renderer::Model& requireModel(const std::string& filePath);
        mach::renderer::Model* tryGetModel(const std::string& filePath);

        void loadShaderDefinitions(const std::string& filePath);
        void preloadShader(const std::string& shaderName);
        Shader& requireShader(const std::string& shaderName);
        Shader* tryGetShader(const std::string& shaderName);

    private:
        bool isModelLoaded(const std::string& filePath);
        bool isShaderLoaded(const std::string& shaderName);
        void loadModel(const std::string& filePath);
        void loadShader(const std::string& shaderName);
        mach::archive::Archive* getContainingArchive(const std::string& filePath);
    };
}


#endif //ASSETMANAGER_H
