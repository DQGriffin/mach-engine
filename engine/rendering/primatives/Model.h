//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "../../archive/Archive.h"
#include "../shader/Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace mach::renderer
{
    class Model
    {
    private:
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;

    public:
        Model(char* path);
        void Draw(Shader& shader);

    private:
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}


#endif //MODEL_H
