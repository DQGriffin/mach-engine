//
// Created by Dquavius Griffin on 5/23/25.
//

#include "AssetManager.h"

void mach::AssetManager::loadArchive(const std::string& archivePath)
{
    mach::fs::Path path = mach::fs::Path(archivePath);
    auto archive = std::make_unique<mach::archive::Archive>(path);
    archive->open();
    archives.push_back(std::move(archive));
}

void mach::AssetManager::preloadModel(const std::string& filePath)
{
    LOG_DEBUG(std::format("Preloading model '{}'", filePath));
    loadModel(filePath);
}

mach::renderer::Model& mach::AssetManager::requireModel(const std::string& filePath)
{
    if (filePath.empty())
    {
        LOG_ERROR("AssetManager::loadModel: No model file specified");
        throw std::runtime_error("AssetManager::loadModel: No model file specified");
    }

    if (!isModelLoaded(filePath))
    {
        LOG_INFO(std::format("Model '{}' not in memory", filePath));
        loadModel(filePath);
    }

    auto it = models.find(filePath);
    if (it != models.end())
    {
        mach::renderer::Model& model = *it->second;
        return model;
    }

    throw std::runtime_error("AssetManager::loadModel: The specified model could not be loaded");
}

mach::renderer::Model* mach::AssetManager::tryGetModel(const std::string& filePath)
{
    if (filePath.empty())
    {
        LOG_ERROR("AssetManager::loadModel: No model file specified");
        return nullptr;
    }

    if (!isModelLoaded(filePath))
    {
        loadModel(filePath);
    }

    auto it = models.find(filePath);
    if (it != models.end())
    {
        auto& model = it->second;
        return model.get();
    }

    return nullptr;
}

void mach::AssetManager::preloadShader(const std::string& filePath)
{
    LOG_DEBUG(std::format("Preloading shader '{}'", filePath));
    if (!isShaderLoaded(filePath))
    {
        loadShader(filePath);
    }
}

Shader& mach::AssetManager::requireShader(const std::string& shaderName)
{
    if (shaderName.empty())
    {
        LOG_ERROR("No shader specified");
        throw std::runtime_error("AssetManager::loadShader: No shader specified");
    }

    if (!isShaderLoaded(shaderName))
    {
        loadShader(shaderName);
    }

    auto it = shaders.find(shaderName);
    if (it != shaders.end())
    {
        auto& shader = *it->second;
        return shader;
    }

    throw std::runtime_error("The specified shader could not be loaded");
}

Shader* mach::AssetManager::tryGetShader(const std::string& shaderName)
{
    if (shaderName.empty())
    {
        LOG_ERROR("No shader specified");
        return nullptr;
    }

    if (!isShaderLoaded(shaderName))
    {
        loadShader(shaderName);
    }

    auto it = shaders.find(shaderName);
    if (it != shaders.end())
    {
        return it->second.get();
    }

    return nullptr;
}

bool mach::AssetManager::isModelLoaded(const std::string& filePath)
{
    return models.contains(filePath);
}

bool mach::AssetManager::isShaderLoaded(const std::string& shaderName)
{
    return shaders.contains(shaderName);
}

void mach::AssetManager::loadModel(const std::string& filePath)
{
    LOG_INFO(std::format("Loading model '{}'", filePath));
    auto model = std::make_unique<mach::renderer::Model>(filePath);
    models[filePath] = std::move(model);
    LOG_INFO(std::format("Loaded model '{}'", filePath));
}

void mach::AssetManager::loadShader(const std::string& shaderName)
{
    LOG_INFO(std::format("Loading shader '{}'", shaderName));
    auto& definition = shaderDefinitions[shaderName];

    auto vertexSourceData = archives[0]->get(definition.vertexSourcePath);
    auto fragmentSourceData = archives[0]->get(definition.fragmentSourcePath);

    std::string vertexSource(vertexSourceData.begin(), vertexSourceData.end());
    std::string fraagmentSource(fragmentSourceData.begin(), fragmentSourceData.end());

    auto shader = std::make_unique<Shader>(vertexSource, fraagmentSource);
    shaders[shaderName] = std::move(shader);
}

mach::archive::Archive* mach::AssetManager::getContainingArchive(const std::string& filePath)
{
    for (auto& archive : archives)
    {
        if (archive->contains(filePath))
        {
            return archive.get();
        }
    }

    return nullptr;
}

void mach::AssetManager::loadShaderDefinitions(const std::string& filePath)
{
    LOG_INFO(std::format("Loading shader definitions '{}'", filePath));
    mach::archive::Archive* archive = getContainingArchive(filePath);
    if (archive == nullptr)
    {
        LOG_ERROR("Failed to load shader definitions");
        return;
    }

    auto data = archive->get(filePath);
    auto manifest = mach::assets::loaders::loadShaderManifest(data);

    for (auto& definition : manifest.shaders)
    {
        shaderDefinitions[definition.name] = definition;
        if (definition.preload)
        {
            // load shader into memory and precompile
        }
    }

    LOG_INFO(std::format("Loaded shader definitions '{}'", filePath));
}
