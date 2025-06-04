//
// Created by Dquavius Griffin on 5/30/25.
//

#include "glm/detail/func_packing_simd.inl"
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
#include "../engine/ecs/components/MeshRender.h"
#include "../engine/ecs/components/PendingDeletion.h"
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

#include <imgui.h>
#include <imconfig.h>
#include <backends/imgui_impl_opengl3_loader.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include "imgui_internal.h"
#include "BulletInverseDynamics/details/MultiBodyTreeImpl.hpp"
#include "ecs/components/Name.h"
#include "ecs/components/ScriptComponent.h"
#include "serialization/PlainTextSceneSerializer.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstFrame = true;
std::string dockSpaceId = "MyDockSpace";

entt::entity selectedEntity;
char selectedEntityModelPath[512] = "";

int main(int argc, const char * argv[])
{
    mach::Window window = mach::Window(1200, 800, "Mach Studio");
    window.setVSync(false);
    mach::AssetManager assets;
    mach::Color backgroundColor = mach::Color(0.1f, 0.1f, 0.1f);
    mach::input::InputEngine inputEngine = mach::input::InputEngine(std::make_shared<mach::Window>(window));
    mach::FpsCounter fpsCounter;
    mach::FrameLimiter frameLimiter;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    Shader debugLineShader("/Users/griffin/dev/mach/engine/rendering/shaders/debug-line/vertex.glsl", "/Users/griffin/dev/mach/engine/rendering/shaders/debug-line/fragment.glsl");
    mach::renderer::DebugLineRenderer lineRenderer(&debugLineShader);

    assets.loadArchive("/Users/griffin/test-assets.macha");
    assets.loadShaderDefinitions("shaders.json");
    assets.preloadShader("lighting");

    mach::ecs::Scene scene;
    scene.inputEngine = &inputEngine;
    scene.registerInputCallbacks();
    auto* drawer = new mach::renderer::BulletDebugDrawer(&lineRenderer);
    scene.physics.setDebugDrawer(drawer);
    scene.physics.setDrawWireframe(true);
    scene.physics.setDrawWireframe(true);
    scene.setProcessMouseInput(false);

    mach::serialization::PlainTextSceneSerializer serializer;
    serializer.deserialize(scene.getRegistry(), assets, scene.physics, "/Users/griffin/scene.machsp");

    auto cameraEntity = scene.createCameraEntity("Editor Camera", glm::vec3(0.0f, 25.0f, 7.0f));
    scene.activeCamera = &cameraEntity;
    scene.registerInputCallbacks();

    mach::Window::registerMousePositionCallback(window, [&inputEngine](int xpos, int ypos) {
        inputEngine.windowMousePositionCallback(xpos, ypos);
    });
    // scene.toggleFlyCamera(true);

    inputEngine.registerKeyPressCallback([&window, &scene](int key, int duration) ->bool {
        if (key == MACH_KEY_PERIOD)
        {
            scene.setProcessMouseInput(!scene.getProcessMouseInput());
            // window.setMouseCaptured(!window.getMouseCaptureState());
        }
        if (key == MACH_KEY_P)
        {
            LOG_DEBUG("Toggling flycam")
            scene.isFlyCam = !scene.isFlyCam;
        }

        if (key == MACH_KEY_SLASH)
        {
            window.setMouseCaptured(!window.getMouseCaptureState());
            scene.isFlyCam = !scene.isFlyCam;
            scene.setProcessMouseInput(!scene.getProcessMouseInput());
        }

        return false;
    }, mach::input::CallbackPriority::EditorUI, mach::input::KeyEventTrigger::Released);

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 0.2f, 2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3( 0.0f, 0.0f, -3.0f)
    };

    Shader& lightingShader = assets.requireShader("lighting");

    auto floorModel = assets.requireModel("/Users/griffin/dev/mach/test-assets/floor-thing/floor-thing.obj");

    // auto floorEntity = scene.createEntity("Floor 1");
    // floorEntity.addComponent<mach::ecs::components::Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    // floorEntity.addComponent<mach::ecs::components::ModelRenderer>(&floorModel, &lightingShader);
    // mach::physics::createPhysicsBody(scene.getRegistry(), floorEntity.handle(), scene.physics.world, floorModel, false, 0.0f);

    bool active = true;

    GLuint framebuffer, textureColorbuffer, rbo;
    int width = 1200;
    int height = 800;

    // Generate framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create color attachment
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Create renderbuffer object for depth and stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    while(!window.shouldClose())
    {
        fpsCounter.tick();
        // LOG_INFO(std::format("FPS: {}", fpsCounter.getFps()));
        inputEngine.processInput();

        // Set clear color and clear the buffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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

        // set point light values
        for (int i = 0; i < 4; i++)
        {
            std::string accessor = "pointLights[";
            accessor.append(std::to_string(i));
            accessor.append("]");

            lightingShader.setVec3(std::format("{}.position", accessor), pointLightPositions[i]);
            lightingShader.setFloat(std::format("{}.constant", accessor), 1.0f);
            lightingShader.setFloat(std::format("{}.linear", accessor), 0.09f);
            lightingShader.setFloat(std::format("{}.quadratic", accessor), 0.032f);
            lightingShader.setVec3(std::format("{}.ambient", accessor), 0.1f, 0.1f, 0.1f);
            lightingShader.setVec3(std::format("{}.diffuse", accessor), 0.8f, 0.8f, 0.8f);
            lightingShader.setVec3(std::format("{}.specular", accessor), 1.0f, 1.0f, 1.0f);
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

        auto eview = scene.getRegistry().view<mach::ecs::components::PendingDeletion>();
        for (auto entity : eview)
        {
            scene.getRegistry().destroy(entity);
            LOG_DEBUG("Entity deleted");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // default framebuffer
        glViewport(0, 0, window.getSize().x, window.getSize().y); // set to screen size
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiID dockspace_id = ImGui::GetID(dockSpaceId.c_str());
        // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        // Fullscreen dockspace setup
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // Optional: make background transparent
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("MainDockSpaceWindow", nullptr, window_flags);
        ImGui::PopStyleVar(2);

        // DockSpace ID must match your layout setup code
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        // ImGui::End();

        ImGui::Begin("Viewport");

        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        int fbWidth = static_cast<int>(availableSize.x);
        int fbHeight = static_cast<int>(availableSize.y);

        // Only recreate framebuffer if size actually changed
        static int lastFbWidth = 0;
        static int lastFbHeight = 0;

        if (fbWidth != lastFbWidth || fbHeight != lastFbHeight) {
            lastFbWidth = fbWidth;
            lastFbHeight = fbHeight;

            width = fbWidth;
            height = fbHeight;

            // Resize color attachment
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

            // Resize depth/stencil
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

            // Reattach if needed (shouldn’t be necessary if only resizing)
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        }

        ImGui::Image((ImTextureID)(intptr_t)textureColorbuffer, availableSize, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();

        ImGui::Begin("Metrics", &active, ImGuiWindowFlags_MenuBar);
        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1,1,0,1), "Metrics");
        ImGui::BeginChild("Scrolling");
        ImGui::Text("Average FPS: %f", fpsCounter.average());
        ImGui::Text("Current FPS: %f", fpsCounter.getFps());
        ImGui::Text("Frame Time: %.3f ms", 1000.0 / fpsCounter.getFps());
        ImGui::Text("Physics: %s", scene.physics.isPaused ? "Paused" : "Running");
        ImGui::Text("Mouse Input: %s", scene.getProcessMouseInput() ? "Tracking" : "Ignored");
        ImGui::EndChild();
        ImGui::End();

        ImGui::Begin("Entities", &active, ImGuiWindowFlags_MenuBar);
        if (ImGui::Button("Save"))
        {
            scene.serialize("/Users/griffin/scene.machsp", mach::ecs::SerializationType::PLAIN_TEXT);
        }
        if (ImGui::Button("New Entity"))
        {
            scene.createEntity("Entity");
        }
        for (auto entity : scene.getRegistry().storage<entt::entity>()) {
            auto nameComponent = scene.getRegistry().try_get<mach::ecs::components::Name>(entity);
            if (nameComponent == nullptr) continue;

            if (ImGui::Button(nameComponent->name.c_str(), ImVec2(200, 20)))
            {
                selectedEntity = entity;
            }
        }
        ImGui::End();

        ImGui::ShowDemoWindow();

        ImGui::Begin("Components", &active, ImGuiWindowFlags_MenuBar);
        auto transformComponent = scene.getRegistry().try_get<mach::ecs::components::Transform>(selectedEntity);
        auto& nameComponent = scene.getRegistry().get<mach::ecs::components::Name>(selectedEntity);

        ImGui::Text(nameComponent.name.c_str());

        if (ImGui::Button("Add Transform"))
        {
            mach::ecs::components::Transform transform;
            transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
            transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
            scene.getRegistry().emplace<mach::ecs::components::Transform>(selectedEntity, transform);
        }

        if (ImGui::Button("Add Model"))
        {
            auto model = assets.requireModel("/Users/griffin/Game Assets/Models/Test Cube/cube.obj");
            mach::ecs::components::ModelRenderer modelComponent;
            modelComponent.model = &model;
            modelComponent.shader = &lightingShader;
            scene.getRegistry().emplace<mach::ecs::components::ModelRenderer>(selectedEntity, modelComponent);
        }

        if (ImGui::Button("Add RigidBody"))
        {
            auto model = scene.getRegistry().get<mach::ecs::components::ModelRenderer>(selectedEntity);
            mach::physics::createPhysicsBody(scene.getRegistry(), selectedEntity, scene.physics.world, *model.model, false, 0.0f);
        }

        if (ImGui::Button("Add MeshCollider"))
        {
            auto model = scene.getRegistry().try_get<mach::ecs::components::ModelRenderer>(selectedEntity);
            if (model != nullptr)
            {
                mach::physics::createMeshCollider(scene.getRegistry(), selectedEntity, scene.physics.world, model->model->getVertices(), model->model->getIndices(), 0.0f);
            }

        }

        if (ImGui::CollapsingHeader("Name"))
        {
            ImGui::InputText("Edit", &nameComponent.name);
        }

        if (transformComponent != nullptr)
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "Position");
                ImGui::InputFloat("X", &transformComponent->position.x, 0.5f, 1.0f);
                ImGui::InputFloat("Y", &transformComponent->position.y, 0.5f, 1.0f);
                ImGui::InputFloat("Z", &transformComponent->position.z, 0.5f, 1.0f);

                ImGui::TextColored(ImVec4(1,1,0,1), "Rotation");
                ImGui::InputFloat("X#", &transformComponent->rotation.x, 0.261799f, 30.0f);
                ImGui::InputFloat("Y#", &transformComponent->rotation.y, 0.261799f, 30.0f);
                ImGui::InputFloat("Z#", &transformComponent->rotation.z, 0.261799f, 30.0f);

                ImGui::TextColored(ImVec4(1,1,0,1), "Scale");
                ImGui::InputFloat("X##", &transformComponent->scale.x, 0.5f, 1.0f);
                ImGui::InputFloat("Y##", &transformComponent->scale.y, 0.5f, 1.0f);
                ImGui::InputFloat("Z##", &transformComponent->scale.z, 0.5f, 1.0f);
            }
        }

        auto modelComponent = scene.getRegistry().try_get<mach::ecs::components::ModelRenderer>(selectedEntity);
        if (modelComponent != nullptr)
        {
            if (ImGui::CollapsingHeader("Model"))
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "Model");
                ImGui::InputText("File", selectedEntityModelPath, sizeof(selectedEntityModelPath));
                if (ImGui::Button("Update"))
                {
                    auto newModel = assets.tryGetModel(selectedEntityModelPath);
                    if (newModel != nullptr)
                        modelComponent->model = newModel;
                }
            }
        }

        auto rigidBody = scene.getRegistry().try_get<mach::ecs::components::RigidBody>(selectedEntity);
        if (rigidBody != nullptr)
        {
            if (ImGui::CollapsingHeader("RigidBody"))
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "RigidBody");
                if (ImGui::Button("Recalculate"))
                {
                    scene.getRegistry().remove<mach::ecs::components::RigidBody>(selectedEntity);
                    const auto model = scene.getRegistry().try_get<mach::ecs::components::ModelRenderer>(selectedEntity);
                    mach::physics::createPhysicsBody(scene.getRegistry(), selectedEntity, scene.physics.world, *model->model, false, 0.0f);

                }
                ImGui::Text("Static: %s", rigidBody->body->getMass() == 0.0f ? "Yes" : "No");
                ImGui::Text("Mass: %f", rigidBody->body->getMass());
                if (ImGui::Button("Remove"))
                {
                    scene.getRegistry().remove<mach::ecs::components::RigidBody>(selectedEntity);
                }
            }
        }

        auto meshCollider = scene.getRegistry().try_get<mach::ecs::components::MeshCollider>(selectedEntity);
        if (meshCollider != nullptr)
        {
            if (ImGui::CollapsingHeader("MeshCollider"))
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "MeshCollider");
                if (ImGui::Button("Recalculate Mesh"))
                {
                    scene.getRegistry().remove<mach::ecs::components::RigidBody>(selectedEntity);
                    const auto model = scene.getRegistry().try_get<mach::ecs::components::ModelRenderer>(selectedEntity);
                    if (model != nullptr)
                    {
                        scene.getRegistry().remove<mach::ecs::components::MeshCollider>(selectedEntity);
                        mach::physics::createMeshCollider(scene.getRegistry(), selectedEntity, scene.physics.world, model->model->getVertices(), model->model->getIndices(), 0.0f);
                    }

                }
                ImGui::Text("Static: %s", meshCollider->body->getMass() == 0.0f ? "Yes" : "No");
                ImGui::Text("Mass: %f", meshCollider->body->getMass());
                if (ImGui::Button("Remove"))
                {
                    auto collider = scene.getRegistry().try_get<mach::ecs::components::MeshCollider>(selectedEntity);
                    if (collider != nullptr)
                    {
                        collider->world->removeRigidBody(collider->body);
                        delete collider->body;
                        delete collider->motionState;
                        delete collider->shape;
                        scene.getRegistry().remove<mach::ecs::components::MeshCollider>(selectedEntity);
                    }
                }
            }
        }

        auto controller = scene.getRegistry().try_get<mach::ecs::components::FpsCharacterController>(selectedEntity);
        if (controller != nullptr)
        {
            if (ImGui::CollapsingHeader("FPS Character Controller"))
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "Controller");
                ImGui::SliderFloat("Walk Speed", &controller->movementSpeed, -50.0f, 50.0f);
                ImGui::SliderFloat("Crouch Speed", &controller->crouchMovementSpeed, -50.0f, 50.0f);
                ImGui::SliderFloat("Sprint Speed", &controller->sprintMovementSpeed, -50.0f, 50.0f);
                ImGui::Selectable("True", controller->canMove);
                ImGui::Selectable("False", !controller->canMove);
            }
        }

        auto scriptComponent = scene.getRegistry().try_get<mach::ecs::components::ScriptComponent>(selectedEntity);
        if (scriptComponent != nullptr)
        {
            if (ImGui::CollapsingHeader("Script"))
            {
                ImGui::TextColored(ImVec4(1,1,0,1), "Script");
            }
        }

        ImGui::End();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        float currentTime = mach::time::getTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        window.swapBuffers();
        window.pollEvents();
        frameLimiter.tick();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
