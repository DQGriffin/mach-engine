//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef SCENESERIALIZER_H
#define SCENESERIALIZER_H

#include "../ecs/Scene.h"

namespace mach::serial
{
    class SceneSerializer {
    public:
        SceneSerializer();
        virtual ~SceneSerializer();
        virtual void serialize(const mach::ecs::Scene&, const std::string& outputPath) = 0;
        virtual bool deserialize(const std::string& inputPath, mach::ecs::Scene& scene) = 0;
    };
}



#endif //SCENESERIALIZER_H
