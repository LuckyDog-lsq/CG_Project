#include "maze_app.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>

MazeApp::MazeApp(const Options& options)
    : Application(options), _camera(glm::radians(60.0f), static_cast<float>(options.windowWidth) / options.windowHeight, 0.1f, 100.0f) {
    glEnable(GL_DEPTH_TEST);

    _camera.transform.position = glm::vec3(0.0f, 4.0f, 10.5f);
    _camera.transform.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

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
        const auto monsterModel = std::make_shared<Model>(
            loadModelFromFile(getAssetFullPath("obj/Monster.obj"), false));
        const auto judyModel = std::make_shared<Model>(
            loadModelFromFile(getAssetFullPath("obj/judy_3d.obj"), true));
        const auto nikeModel = std::make_shared<Model>(
            loadModelFromFile(getAssetFullPath("obj/nike.obj"), true));
        const auto snowModel = std::make_shared<Model>(
            loadModelFromFile(getAssetFullPath("obj/snow_box.obj"), true));

        auto addInstance = [&](const std::shared_ptr<Model>& model, const glm::vec3& pos, const glm::vec3& color, const glm::vec3& scale = glm::vec3(1.0f)) {
            SceneModel sm;
            sm.model = model;
            sm.transform.position = pos;
            sm.transform.scale = scale;
            sm.fallbackColor = color;
            _sceneModels.push_back(std::move(sm));
            };

        const float cellSize = 1.5f;
        const float wallY = -2.0f;
        const std::vector<std::string> maze = {
            "###############",
            "#S   #     #  #",
            "# ## ### # ## #",
            "#    #   #    #",
            "### #### ## ###",
            "#   #    #   ##",
            "## ### #### # #",
            "#   #     #   #",
            "#   #######   #",
            "#   #     #   #",
            "############E##",
        };

        const int rows = static_cast<int>(maze.size());
        const int cols = static_cast<int>(maze[0].size());
        const float startX = -0.5f * cellSize * static_cast<float>(cols - 1);
        const float startZ = -0.5f * cellSize * static_cast<float>(rows - 1);

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (maze[r][c] == '#') {
                    const glm::vec3 pos = glm::vec3(
                        startX + static_cast<float>(c) * cellSize, wallY,
                        startZ + static_cast<float>(r) * cellSize);
                    SceneModel sm;
                    sm.model = snowModel;
                    sm.transform.position = pos;
                    sm.transform.scale = glm::vec3(1.8f);  // 放大到 1.8 倍，消除缝隙
                    sm.fallbackColor = glm::vec3(0.8f);
                    _sceneModels.push_back(std::move(sm));
                }
            }
        }

        const auto cellToWorld = [&](int c, int r, float y) -> glm::vec3 {
            return glm::vec3(
                startX + static_cast<float>(c) * cellSize,
                y,
                startZ + static_cast<float>(r) * cellSize);
            };

        // Judy at start (near 'S')
        {
            SceneModel judy;
            judy.model = judyModel;
            judy.transform.position = cellToWorld(1, 1, 0.0f);
            judy.transform.scale = glm::vec3(0.5f);  // 缩小到 50%
            judy.transform.lookAt(cellToWorld(3, 3, 0.0f));
            judy.fallbackColor = glm::vec3(0.7f, 0.7f, 0.9f);
            _sceneModels.push_back(std::move(judy));
        }

        // Nike at goal (near 'E')
        {
            SceneModel nike;
            nike.model = nikeModel;
            nike.transform.position = cellToWorld(cols - 3, rows - 2, 0.0f);
            nike.transform.scale = glm::vec3(0.6f);  // 缩小到 60%
            nike.fallbackColor = glm::vec3(0.9f, 0.9f, 0.9f);
            _sceneModels.push_back(std::move(nike));
        }

        // Monster patrol near center
        {
            SceneModel monster;
            monster.model = monsterModel;
            monster.transform.position = cellToWorld(cols / 2, rows / 2, 0.0f);
            monster.transform.scale = glm::vec3(0.4f);  // 缩小到 40%
            monster.fallbackColor = glm::vec3(0.8f, 0.7f, 0.6f);
            _sceneModels.push_back(std::move(monster));
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

MazeApp::~MazeApp() {}

void MazeApp::handleInput() {
    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (_windowReized) {
        _camera.aspect = static_cast<float>(_windowWidth) / static_cast<float>(_windowHeight);
        _windowReized = false;
    }
}

void MazeApp::renderFrame() {
    showFpsInWindowTitle();

    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = _camera.getViewMatrix();
    const glm::mat4 proj = _camera.getProjectionMatrix();
    const glm::vec3 lightDir = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));

    _shader->use();
    _shader->setUniformMat4("uView", view);
    _shader->setUniformMat4("uProj", proj);
    _shader->setUniformVec3("uLightDir", lightDir);
    _shader->setUniformInt("uDiffuse", 0);

    for (const auto& sceneModel : _sceneModels) {
        if (!sceneModel.model) {
            continue;
        }
        const glm::mat4 modelMat = sceneModel.transform.getLocalMatrix();
        _shader->setUniformMat4("uModel", modelMat);

        for (const auto& mesh : sceneModel.model->getMeshes()) {
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