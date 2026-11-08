//
// Created by Dquavius Griffin on 5/18/25.
//

#include <sol/sol.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "../engine/archive/Archive.h"
#include "../engine/window/Window.h"
#include "../engine/math/vec.h"
#include "../engine/color/color.h"
#include "../engine/input/input.h"
#include "../engine/assets/AssetManager.h"
#include "../engine/ecs/Scene.h"
#include "../engine/ecs/Entity.h"
#include "../engine/ecs/components/Anchor.h"
#include "../engine/ecs/components/FpsCharacterController.h"
#include "../engine/ecs/components/Interactable.h"
#include "../engine/ecs/components/MeshRender.h"
#include "../engine/ecs/components/PendingDeletion.h"
#include "../engine/ecs/components/PointLight.h"
#include "../engine/ecs/components/Transform.h"
#include "../engine/ecs/systems/MeshRendererSystem.h"
#include "../engine/ecs/systems/PhysicsSystem.h"
#include "../engine/external/entt.h"
#include "../engine/logging/logging.h"
#include "../engine/rendering/FpsCounter.h"
#include "../engine/rendering/FrameLimiter.h"
#include "../engine/rendering/camera/Camera.h"
#include "../engine/rendering/shader/Shader.h"
#include "../engine/external/stb_image.h"
#include "../engine/rendering/primatives/Mesh.h"
#include "../engine/rendering/primatives/Model.h"
#include "../engine/rendering/debug/DebugLineRenderer.h"
#include "../engine/rendering/debug/BulletDebugDrawer.h"
#include "../engine/serialization/SceneSerialization.h"
#include "../engine/serialization/PlainTextSceneSerializer.h"
#include "../engine/ecs/components/Zone.h"
#include "../engine/ecs/components/ZoneTrigger.h"
#include "../engine/ecs/systems/ScriptSystem.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // mach::fs::Path path = mach::fs::Path("/Users/griffin/mach-test-assets");
    // mach::archive::Archive::createArchive("/Users/griffin/test-assets.macha", path, true);

    mach::Window window = mach::Window(1200, 800, "Mach 2 Testbed");
    window.captureMouse();
    window.setVSync(false);
    mach::AssetManager assets;
    mach::Color backgroundColor = mach::Color(0.1f, 0.1f, 0.1f);
    mach::input::InputEngine inputEngine = mach::input::InputEngine(std::make_shared<mach::Window>(window));
    mach::FpsCounter fpsCounter;
    mach::FrameLimiter frameLimiter;

    assets.loadArchive("/Users/griffin/test-assets.macha");
    assets.loadShaderDefinitions("shaders.json");
    assets.preloadShader("lighting");

    Shader debugLineShader("/Users/griffin/dev/mach/engine/rendering/shaders/debug-line/vertex.glsl", "/Users/griffin/dev/mach/engine/rendering/shaders/debug-line/fragment.glsl");
    mach::renderer::DebugLineRenderer lineRenderer(&debugLineShader);

    auto* drawer = new mach::renderer::BulletDebugDrawer(&lineRenderer);

    mach::ecs::Scene scene;
    scene.inputEngine = &inputEngine;
    scene.debugLineRenderer = &lineRenderer;
    // scene.physics.setDebugDrawer(drawer);
    // scene.physics.setDrawWireframe(true);
    // scene.physics.setDrawWireframe(true);
    auto cameraEntity = scene.createCameraEntity("Camera", glm::vec3(0.0f, 1.0f, 7.0f));
    scene.setProcessMouseInput(false);
    scene.activeCamera = &cameraEntity;
    scene.registerInputCallbacks();

    mach::serialization::PlainTextSceneSerializer serializer;
    serializer.deserialize(scene.getRegistry(), assets, scene.physics, "/Users/griffin/scene.machsp");

    mach::Window::registerMousePositionCallback(window, [&inputEngine](int xpos, int ypos) {
        inputEngine.windowMousePositionCallback(xpos, ypos);
    });

    inputEngine.registerKeyPressCallback([&window, &scene](int key, int duration) ->bool {
        if (key == MACH_KEY_PERIOD)
        {
            window.setMouseCaptured(!window.getMouseCaptureState());
            scene.physics.isPaused = !scene.physics.isPaused;
        }
        return false;
    }, mach::input::CallbackPriority::Gameplay, mach::input::KeyEventTrigger::Released);

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 1000.2f, 2.0f),
        glm::vec3( 2.3f, -3000.3f, -4.0f),
        glm::vec3(-4.0f, 2000.0f, -12.0f),
        glm::vec3( 0.0f, 2000.0f, -3.0f)
    };

    for (auto& lightPosition : pointLightPositions)
    {
        auto entity = scene.createEntity("PointLight");
        auto& light = scene.getRegistry().emplace<mach::ecs::components::PointLight>(entity.handle());
        auto& transform = scene.getRegistry().emplace<mach::ecs::components::Transform>(entity.handle());
        transform.position = lightPosition;
    }

    Shader& lightingShader = assets.requireShader("lighting");
    Shader lightCubeShader("/Users/griffin/dev/LearnOpenGL/LearnOpenGL/shaders/light/vertex.glsl", "/Users/griffin/dev/LearnOpenGL/LearnOpenGL/shaders/light/fragment.glsl");

    auto monkeyModel = assets.requireModel("/Users/griffin/dev/mach/test-assets/monkey/monkey.obj");

    auto playerEntity = scene.createEntity("Player 1");
    playerEntity.addComponent<mach::ecs::components::Transform>(glm::vec3(8.0f, 25.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f));
    playerEntity.addComponent<mach::ecs::components::ModelRenderer>(&monkeyModel, &lightingShader);
    scene.getRegistry().emplace<mach::ecs::components::FpsCharacterController>(playerEntity.handle());
    scene.getRegistry().emplace<mach::ecs::components::ZoneTrigger>(playerEntity.handle());
    scene.setPlayerEntity(&playerEntity);

    mach::physics::createPhysicsBody(scene.getRegistry(), playerEntity.handle(), scene.physics.world, monkeyModel, true, 10.0f);
    // mach::physics::createPhysicsBody(scene.getRegistry(), roomEntity.handle(), scene.physics.world, roomModel, false, 0.0f);

    auto& anc = scene.getRegistry().emplace<mach::ecs::components::Anchor>(cameraEntity.handle(), &playerEntity);
    anc.offset = glm::vec3(0.0f, 10.5f, 0.0f);
    anc.y = true;
    anc.x = true;
    anc.z = true;

    auto backpackEntity = scene.createEntity("Backpack");
    auto backpackModel = assets.requireModel("/Users/griffin/dev/mach/test-assets/backpack/backpack.obj");
    backpackEntity.addComponent<mach::ecs::components::Transform>(glm::vec3(20.0f, 1.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    backpackEntity.addComponent<mach::ecs::components::ModelRenderer>(&monkeyModel, &lightingShader);
    scene.attachScript(backpackEntity, "/Users/griffin/dev/mach/test-assets/scripts/hello-world.lua");
    // backpackEntity.addComponent<mach::ecs::components::ScriptComponent>();
    // mach::ecs::systems::attachScriptToEntity(scene.getRegistry(), backpackEntity, "/Users/griffin/dev/mach/test-assets/scripts.hello-world.lua", scene.luaManager.lua);
    std::string prompt = "int";
    backpackEntity.addComponent<mach::ecs::components::Interactable>();


    // scene.attachScript(playerEntity, "/Users/griffin/dev/mach/test-assets/scripts/hello-world.lua");

    // zones
    auto zoneEntity = scene.createEntity("Zone 1");
    auto& zone = scene.getRegistry().emplace<mach::ecs::components::Zone>(zoneEntity.handle());
    zone.bounds.expandToFit(glm::vec3(0.0f, 0.0f, 0.0f));
    zone.bounds.expandToFit(glm::vec3(10.0f, 10.0f, 10.0f));
    zone.isActive = false;

    while(!window.shouldClose()) {
        fpsCounter.tick();
        // LOG_INFO(std::format("FPS: {}", fpsCounter.getFps()));
        inputEngine.processInput();

        // Set clear color and clear the buffer
        window.setClearColor(backgroundColor);
        window.clear();
        lineRenderer.clear();

        scene.physics.step(deltaTime);
        scene.physics.world->debugDrawWorld();

        auto cameraComponent = scene.getRegistry().get<mach::ecs::components::Camera>(cameraEntity.handle());
        auto cameraTransform = scene.getRegistry().get<mach::ecs::components::Transform>(cameraEntity.handle());

        // Activate the program
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.35f);

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("viewPos", cameraTransform.position);

        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);
        lightingShader.setFloat("material.shininess", 3.0f);

        // set directional light values
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        auto entityView = scene.getRegistry().view<mach::ecs::components::PointLight>();
        int pointLightIndex = 0;

        for (auto& entity : entityView)
        {
            auto& light = scene.getRegistry().get<mach::ecs::components::PointLight>(entity);
            auto& trans = scene.getRegistry().get<mach::ecs::components::Transform>(entity);
            std::string accessor = "pointLights[";
            accessor.append(std::to_string(pointLightIndex));
            accessor.append("]");

            lightingShader.setVec3(std::format("{}.position", accessor), trans.position);
            lightingShader.setFloat(std::format("{}.constant", accessor), light.constant);
            lightingShader.setFloat(std::format("{}.linear", accessor), light.linear);
            lightingShader.setFloat(std::format("{}.quadratic", accessor), light.quadratic);
            lightingShader.setVec3(std::format("{}.ambient", accessor), light.ambientColor);
            lightingShader.setVec3(std::format("{}.diffuse", accessor), light.diffuseColor);
            lightingShader.setVec3(std::format("{}.specular", accessor), light.specularColor);
            pointLightIndex++;
        }

        // set spot light values
        lightingShader.setVec3("spotLight.direction", cameraComponent.Front);
        lightingShader.setVec3("spotLight.position", cameraTransform.position);
        lightingShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.0f)));
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);

        // model projection (local space -> world space)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // view projection (world space -> view space)
        glm::mat4 view = cameraComponent.GetViewMatrix(cameraTransform.position);

        // projection matrix
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glm::mat4 viewProj = projection * view;

        // set uniforms
        int modelLoc = glGetUniformLocation(lightingShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // backpackModel.Draw(lightingShader);
        scene.update(deltaTime);
        lineRenderer.render(viewProj);

        // draw light cube
        // lightCubeShader.use();
        // lightCubeShader.setVec3("color", 1.0f, 1.0f, 1.0f);
        // glBindVertexArray(lightVAO);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, lightPos);
        // model = glm::scale(model, glm::vec3(0.2f));
        //
        // modelLoc = glGetUniformLocation(lightCubeShader.ID, "model");
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //
        // viewLoc = glGetUniformLocation(lightCubeShader.ID, "view");
        // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //
        // projectionLoc = glGetUniformLocation(lightCubeShader.ID, "projection");
        // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        //
        // for (unsigned i = 0; i < 4; i++)
        // {
        //     // calculate the model matrix for each object and pass it to shader before drawing
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, pointLightPositions[i]);
        //     model = glm::scale(model, glm::vec3(0.2f));
        //     lightCubeShader.setMat4("model", model);
        //
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        auto eview = scene.getRegistry().view<mach::ecs::components::PendingDeletion>();
        for (auto entity : eview)
        {
            scene.getRegistry().destroy(entity);
            LOG_DEBUG("Entity deleted");
        }

        float currentTime = mach::time::getTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        window.swapBuffers();
        window.pollEvents();
        frameLimiter.tick();
    }

    return 0;
    // mach::fs::Path path = mach::fs::Path("/Users/griffin/dependencies");
    // mach::archive::Archive::createArchive("/Users/griffin/test/archive_compressed.macha", path, true);

    // mach::fs::Path archivePath = mach::fs::Path("/Users/griffin/test/archive_compressed.macha");
    // mach::archive::Archive archive(archivePath);
    // archive.open();
    // archive.unpack("/Users/griffin/test/dependencies/");
}