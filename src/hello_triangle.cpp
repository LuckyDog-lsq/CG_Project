#include "hello_triangle.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

HelloTriangle::HelloTriangle(const Options& options)
    : Application(options), _camera(glm::radians(60.0f), static_cast<float>(options.windowWidth) / options.windowHeight, 0.1f, 100.0f) {
    glEnable(GL_DEPTH_TEST);

    _camera.transform.position = glm::vec3(0.0f, 1.5f, 6.0f);
    _camera.transform.lookAt(glm::vec3(0.0f, 0.5f, 0.0f));

    const char* vsCode =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec2 aTexCoord;\n"
        "uniform mat4 uModel;\n"
        "uniform mat4 uView;\n"
        "uniform mat4 uProj;\n"
        "out vec3 vNormal;\n"
        "out vec3 vWorldPos;\n"
        "out vec2 vTexCoord;\n"
        "void main() {\n"
        "    vec4 worldPos = uModel * vec4(aPos, 1.0);\n"
        "    vWorldPos = worldPos.xyz;\n"
        "    vNormal = mat3(transpose(inverse(uModel))) * aNormal;\n"
        "    vTexCoord = aTexCoord;\n"
        "    gl_Position = uProj * uView * worldPos;\n"
        "}\n";

    const char* fsCode =
        "#version 330 core\n"
        "in vec3 vNormal;\n"
        "in vec3 vWorldPos;\n"
        "in vec2 vTexCoord;\n"
        "uniform vec3 uViewPos;\n"
        "uniform vec3 uLightDir;\n"
        "uniform vec3 uColor;\n"
        "uniform bool uHasTexture;\n"
        "uniform sampler2D uDiffuse;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    vec3 baseColor = uColor;\n"
        "    if (uHasTexture) {\n"
        "        baseColor = texture(uDiffuse, vTexCoord).rgb;\n"
        "    }\n"
        "    vec3 N = normalize(vNormal);\n"
        "    vec3 L = normalize(-uLightDir);\n"
        "    float diff = max(dot(N, L), 0.0);\n"
        "    vec3 ambient = 0.2 * baseColor;\n"
        "    vec3 diffuse = 0.8 * diff * baseColor;\n"
        "    FragColor = vec4(ambient + diffuse, 1.0);\n"
        "}\n";

    _shader.reset(new GLSLProgram());
    _shader->attachVertexShader(vsCode);
    _shader->attachFragmentShader(fsCode);
    _shader->link();

    try {
        SceneModel monster;
        monster.model = loadModelFromFile(getAssetFullPath("obj/Monster.obj"), false);
        monster.transform.position = glm::vec3(-3.0f, 0.0f, 0.0f);
        monster.fallbackColor = glm::vec3(0.8f, 0.7f, 0.6f);

        SceneModel judy;
        judy.model = loadModelFromFile(getAssetFullPath("obj/judy_3d.obj"), true);
        judy.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        judy.fallbackColor = glm::vec3(0.7f, 0.7f, 0.9f);

        SceneModel nike;
        nike.model = loadModelFromFile(getAssetFullPath("obj/nike.obj"), true);
        nike.transform.position = glm::vec3(3.0f, 0.0f, 0.0f);
        nike.fallbackColor = glm::vec3(0.9f, 0.9f, 0.9f);

        SceneModel snow;
        snow.model = loadModelFromFile(getAssetFullPath("obj/snow_box.obj"), true);
        snow.transform.position = glm::vec3(0.0f, -2.0f, -2.5f);
        snow.fallbackColor = glm::vec3(0.8f);

        _sceneModels.push_back(std::move(monster));
        _sceneModels.push_back(std::move(judy));
        _sceneModels.push_back(std::move(nike));
        _sceneModels.push_back(std::move(snow));
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

HelloTriangle::~HelloTriangle() {}

void HelloTriangle::handleInput() {
    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (_windowReized) {
        _camera.aspect = static_cast<float>(_windowWidth) / static_cast<float>(_windowHeight);
        _windowReized = false;
    }
}

void HelloTriangle::renderFrame() {
    showFpsInWindowTitle();

    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = _camera.getViewMatrix();
    const glm::mat4 proj = _camera.getProjectionMatrix();
    const glm::vec3 lightDir = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));

    _shader->use();
    _shader->setUniformMat4("uView", view);
    _shader->setUniformMat4("uProj", proj);
    _shader->setUniformVec3("uViewPos", _camera.transform.position);
    _shader->setUniformVec3("uLightDir", lightDir);
    _shader->setUniformInt("uDiffuse", 0);

    for (const auto& sceneModel : _sceneModels) {
        const glm::mat4 modelMat = sceneModel.transform.getLocalMatrix();
        _shader->setUniformMat4("uModel", modelMat);

        for (const auto& mesh : sceneModel.model.getMeshes()) {
            const bool hasTexture = mesh.diffuseTexture != nullptr;
            _shader->setUniformBool("uHasTexture", hasTexture);
            _shader->setUniformVec3("uColor", mesh.baseColor * sceneModel.fallbackColor);

            if (hasTexture) {
                mesh.diffuseTexture->bind(0);
            }

            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indexCount), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            if (hasTexture) {
                mesh.diffuseTexture->unbind();
            }
        }
    }
}
