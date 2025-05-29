//
// Created by Dquavius Griffin on 5/23/25.
//

#include "AssetManager.h"

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

bool mach::AssetManager::isModelLoaded(const std::string& filePath)
{
    return models.contains(filePath);
}

void mach::AssetManager::loadModel(const std::string& filePath)
{
    LOG_INFO(std::format("Loading model '{}'", filePath));
    auto model = std::make_unique<mach::renderer::Model>(filePath);
    models[filePath] = std::move(model);
    LOG_INFO(std::format("Loaded model '{}'", filePath));
}

mach::archive::Archive* mach::AssetManager::getContainingArchive(const std::string& filePath)
{
    for (auto& archive : archives)
    {
        if (archive.contains(filePath))
        {
            return &archive;
        }
    }

    return nullptr;
}
