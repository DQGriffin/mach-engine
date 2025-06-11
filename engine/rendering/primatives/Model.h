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

#include "../../physics/AABB.h"

namespace mach::renderer
{
    class Model
    {
    private:
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;
        std::string path;

    public:
        AABB bounds;

    public:
        Model(char* path);
        Model(std::string path);
        void Draw(Shader& shader);
        void computeBounds();
        std::vector<glm::vec3> getVertices();
        std::vector<unsigned int> getIndices();
        std::string getPath();

    private:
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}


#endif //MODEL_H
