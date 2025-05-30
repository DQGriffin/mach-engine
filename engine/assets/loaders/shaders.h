//
// Created by Dquavius Griffin on 5/30/25.
//

#ifndef SHADERS_H
#define SHADERS_H
#include <string>
#include <iostream>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace mach::assets::loaders
{
    struct ShaderDefinition
    {
        std::string name;
        std::string vertexSourcePath;
        std::string fragmentSourcePath;
        std::string geometrySourcePath = "";
        std::vector<std::string> includePaths;
        std::vector<std::string> defines;
        bool preload = false;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("name", name),
                cereal::make_nvp("vertex", vertexSourcePath),
                cereal::make_nvp("fragment", fragmentSourcePath),
                cereal::make_nvp("preload", preload)
            );
        }
    };

    struct ShaderManifest
    {
        std::vector<ShaderDefinition> shaders;

        template<class Archive>
        void serialize(Archive& archive) {
            archive(
                CEREAL_NVP(shaders)
            );
        }
    };

    inline ShaderManifest loadShaderManifest(const std::vector<char>& definitionFileData)
    {
        ShaderManifest manifest;

        std::string jsonContents(definitionFileData.begin(), definitionFileData.end());
        std::istringstream stream(jsonContents);

        // std::cout << stream.rdbuf();

        cereal::JSONInputArchive archive(stream);
        archive(manifest.shaders);

        return manifest;
    }

}

#endif //SHADERS_H
