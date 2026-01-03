#pragma once

#include "base/application.h"
#include "base/camera.h"
#include "base/glsl_program.h"
#include "base/transform.h"
#include "model.h"
#include <memory>
#include <vector>

class HelloTriangle : public Application {
public:
    HelloTriangle(const Options& options);

    ~HelloTriangle();

private:
    struct SceneModel {
        Model model;
        Transform transform;
        glm::vec3 fallbackColor = glm::vec3(0.8f);
    };

    PerspectiveCamera _camera;
    std::unique_ptr<GLSLProgram> _shader;
    std::vector<SceneModel> _sceneModels;

    virtual void handleInput();

    virtual void renderFrame();
};
