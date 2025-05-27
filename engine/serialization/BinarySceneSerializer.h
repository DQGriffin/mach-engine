//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef BINARYSCENESERIALIZER_H
#define BINARYSCENESERIALIZER_H
#include "SceneSerializer.h"

class BinarySceneSerializer : public mach::serial::SceneSerializer {
public:
    BinarySceneSerializer();
    ~BinarySceneSerializer() override;
    void serialize(const mach::ecs::Scene&, const std::string& outputPath) override;
    bool deserialize(const std::string& inputPath, mach::ecs::Scene& scene) override;
};


#endif //BINARYSCENESERIALIZER_H
