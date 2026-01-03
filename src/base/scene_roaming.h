#pragma once
#include "camera.h"
#include "input.h"

class SceneRoaming {
public:
    PerspectiveCamera camera; // FPS相机
    float moveSpeed = 5.0f;       // 移动速度
    float mouseSensitivity = 0.2f; // 鼠标灵敏度

public:
    SceneRoaming(float fovy, float aspect, float znear, float zfar);

    // 每帧调用，更新相机
    void update(float deltaTime, const Input& input);
};
