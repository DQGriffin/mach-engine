//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef MESH_H
#define MESH_H
#include <vector>

#include "../shader/Shader.h"
#include "Texture.h"
#include "vertex.h"


namespace mach::renderer
{
    class Mesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(Shader& shader);

    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
    };
}



#endif //MESH_H
