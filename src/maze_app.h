#pragma once

#include "base/application.h"
#include "base/camera.h"
#include "base/glsl_program.h"
#include "base/transform.h"
#include "model.h"
#include <memory>
#include <vector>

// High-level app that builds a snow-box maze and places Judy/Nike/Monster models.
class MazeApp : public Application {
public:
    MazeApp(const Options& options);

    ~MazeApp();

private:
    struct SceneModel {
        std::shared_ptr<Model> model;
        Transform transform;
        glm::vec3 fallbackColor = glm::vec3(0.8f);
    };

    PerspectiveCamera _camera;
    std::unique_ptr<GLSLProgram> _shader;
    std::vector<SceneModel> _sceneModels;

    float _moveSpeed = 5.0f;
    float _mouseSensitivity = 0.02f;

    virtual void handleInput();

    virtual void renderFrame();

    void updateCamera(float deltaTime); 

    float _lastFrameTime = 0.0f;

};
