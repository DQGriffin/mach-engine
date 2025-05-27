//
// Created by Dquavius Griffin on 5/18/25.
//

#include "../engine/archive/Archive.h"
#include "../engine/window/Window.h"
#include "../engine/math/vec.h"
#include "../engine/color/color.h"
#include "../engine/input/input.h"
#include "../engine/assets/AssetManager.h"
#include "../engine/ecs/Scene.h"
#include "../engine/ecs/Entity.h"
#include "../engine/ecs/components/MeshRender.h"
#include "../engine/ecs/components/Transform.h"
#include "../engine/ecs/systems/MeshRendererSystem.h"
#include "../engine/external/entt.h"
#include "../engine/logging/logging.h"
#include "../engine/rendering/FpsCounter.h"
#include "../engine/rendering/FrameLimiter.h"
#include "../engine/rendering/camera/Camera.h"
#include "../engine/rendering/shader/Shader.h"
#include "../engine/external/stb_image.h"
#include "../engine/rendering/primatives/Mesh.h"
#include "../engine/rendering/primatives/Model.h"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400;
float lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

float cubeXTranslation = 0.0f;
float cubeYTranslation = 0.0f;
float cubeZTranslation = 0.0f;

void processInput(const mach::Window& window);

int main()
{
    mach::Window window = mach::Window(800, 600, "Mach 2 Testbed");
    window.captureMouse();
    window.setVSync(false);
    mach::Color backgroundColor = mach::Color(0.1f, 0.1f, 0.1f);
    mach::input::InputEngine inputEngine = mach::input::InputEngine(std::make_shared<mach::Window>(window));
    mach::FpsCounter fpsCounter;
    mach::FrameLimiter frameLimiter;

    mach::ecs::Scene scene;

    mach::Window::registerMousePositionCallback(window, [&inputEngine](int xpos, int ypos) {
        inputEngine.windowMousePositionCallback(xpos, ypos);
    });

    inputEngine.registerMousePositionCallback([](int xposIn, int yposIn) -> bool {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
        return false;
    });

    inputEngine.registerKeyPressCallback([&window](int key, int duration) ->bool {
        if (key == MACH_KEY_PERIOD)
        {
            window.setMouseCaptured(!window.getMouseCaptureState());
        }
        return true;
    }, mach::input::CallbackPriority::Gameplay, mach::input::KeyEventTrigger::Released);

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 0.2f, 2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3( 0.0f, 0.0f, -3.0f)
    };

    Shader lightingShader("/Users/griffin/dev/LearnOpenGL/LearnOpenGL/shaders/test/vertex.glsl", "/Users/griffin/dev/LearnOpenGL/LearnOpenGL/shaders/test/fragment.glsl");
    Shader lightCubeShader("/Users/griffin/dev/LearnOpenGL/LearnOpenGL/shaders/light/vertex.glsl", "/Users/griffin/dev/LearnOpenGL/LearnOpenGL/shaders/light/fragment.glsl");

    mach::renderer::Model backpackModel("/Users/griffin/dev/mach/test-assets/table/table.obj");

    auto backpackEntity = scene.createEntity("Backpack 1");
    backpackEntity.addComponent<mach::ecs::components::Transform>(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    backpackEntity.addComponent<mach::ecs::components::ModelRenderer>(&backpackModel, &lightingShader);

    auto backpackEntity2 = scene.createEntity("Backpack 2");
    backpackEntity2.addComponent<mach::ecs::components::Transform>(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    backpackEntity2.addComponent<mach::ecs::components::ModelRenderer>(&backpackModel, &lightingShader);

    while(!window.shouldClose()) {
        fpsCounter.tick();
        // LOG_INFO(std::format("FPS: {}", fpsCounter.getFps()));
        processInput(window);
        inputEngine.processInput();

        // Set clear color and clear the buffer
        window.setClearColor(backgroundColor);
        window.clear();

        // Activate the program
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.35f);

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

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
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.position", camera.Position);
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
        model = glm::translate(model, glm::vec3(cubeXTranslation, cubeYTranslation, cubeZTranslation));

        // view projection (world space -> view space)
        glm::mat4 view = camera.GetViewMatrix();

        // projection matrix
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // set uniforms
        int modelLoc = glGetUniformLocation(lightingShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // backpackModel.Draw(lightingShader);
        // mach::ecs::systems::render(registry);
        scene.update(deltaTime);

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

        float currentTime = mach::time::getTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        window.swapBuffers();
        window.pollEvents();
        frameLimiter.tick();
    }

    // mach::fs::Path path = mach::fs::Path("/Users/griffin/dependencies");
    // mach::archive::Archive::createArchive("/Users/griffin/test/archive_compressed.macha", path, true);

    // mach::fs::Path archivePath = mach::fs::Path("/Users/griffin/test/archive_compressed.macha");
    // mach::archive::Archive archive(archivePath);
    // archive.open();
    // archive.unpack("/Users/griffin/test/dependencies/");
}

void processInput(const mach::Window& window)
{
    if (window.isKeyPressed(MACH_KEY_W))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (window.isKeyPressed(MACH_KEY_S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (window.isKeyPressed(MACH_KEY_A))
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (window.isKeyPressed(MACH_KEY_D))
        camera.ProcessKeyboard(RIGHT, deltaTime);
}