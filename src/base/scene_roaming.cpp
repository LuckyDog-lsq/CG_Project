#include "scene_roaming.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

SceneRoaming::SceneRoaming(float fovy, float aspect, float znear, float zfar)
    : camera(fovy, aspect, znear, zfar) { }

void SceneRoaming::update(float deltaTime, const Input& input) {
    // -----------------
    // 1. Êó±êÐý×ª
    // -----------------
    float deltaX = input.mouse.move.xNow - input.mouse.move.xOld;
    float deltaY = input.mouse.move.yNow - input.mouse.move.yOld;

    float yaw   = deltaX * mouseSensitivity;
    float pitch = deltaY * mouseSensitivity;

    camera.rotate(yaw, pitch);

    // -----------------
    // 2. ¼üÅÌÒÆ¶¯
    // -----------------
    glm::vec3 dir(0.0f);

    if (input.keyboard.keyStates[GLFW_KEY_W] == GLFW_PRESS)
        dir += camera.transform.getFront();
    if (input.keyboard.keyStates[GLFW_KEY_S] == GLFW_PRESS)
        dir -= camera.transform.getFront();
    if (input.keyboard.keyStates[GLFW_KEY_A] == GLFW_PRESS)
        dir -= camera.transform.getRight();
    if (input.keyboard.keyStates[GLFW_KEY_D] == GLFW_PRESS)
        dir += camera.transform.getRight();
    if (input.keyboard.keyStates[GLFW_KEY_Q] == GLFW_PRESS)
        dir += Transform::getDefaultUp();
    if (input.keyboard.keyStates[GLFW_KEY_E] == GLFW_PRESS)
        dir -= Transform::getDefaultUp();

    if (glm::length(dir) > 0.0f)
        dir = glm::normalize(dir);

    camera.move(dir * moveSpeed * deltaTime);
}
