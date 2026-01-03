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


    float _yaw = -90.0f;   // 水平方向角度，初始朝 -Z
    float _pitch = 0.0f;   // 垂直方向角度
    float _moveSpeed = 2.0f;
    float _mouseSensitivity = 0.02f;

    virtual void handleInput();

    virtual void renderFrame();

    void updateCamera(float deltaTime); 

    float _lastFrameTime = 0.0f;

};
