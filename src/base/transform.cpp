#include "transform.h"

void Transform::setFromTRS(const glm::mat4& trs) {
    // https://blog.csdn.net/hunter_wwq/article/details/21473519
    position = {trs[3][0], trs[3][1], trs[3][2]};//初始位置
    scale = {glm::length(trs[0]), glm::length(trs[1]), glm::length(trs[2])};
    rotation = glm::quat_cast(glm::mat3(
        glm::vec3(trs[0][0], trs[0][1], trs[0][2]) / scale[0],
        glm::vec3(trs[1][0], trs[1][1], trs[1][2]) / scale[1],
        glm::vec3(trs[2][0], trs[2][1], trs[2][2]) / scale[2]));
}//设置矩阵。包括了初始位置、缩放、旋转。如博客，是从变换矩阵中提取了各个矩阵。

void Transform::lookAt(const glm::vec3& target, const glm::vec3& up) {
    rotation = glm::quatLookAt(glm::normalize(target - position), up);
}//根据目标位置和向上向量构造四元数，使得z对准forward同时尽可能保持up方向/
/*
    四元数是存储了旋转轴和旋转角度的[x,y,z,w]，其中：
    x = RotationAxis.x * sin(RotationAngle / 2)
    y = RotationAxis.y * sin(RotationAngle / 2)
    z = RotationAxis.z * sin(RotationAngle / 2)
    w = cos(RotationAngle / 2)
    “一个普遍的共识：程序内部四元数、用户交互欧拉角”。
    - 判断四元数相同
    - 旋转一个点
    - 两个四元数插值
    - 四元数旋转，可能存在的特例
*/

glm::vec3 Transform::getFront() const {
    return rotation * getDefaultFront();
}//根据当前四元数rotation从local到world，第一步变化。*默认的前向

glm::vec3 Transform::getUp() const {
    return rotation * getDefaultUp();
}//*默认的上向

glm::vec3 Transform::getRight() const {
    return rotation * getDefaultRight();
}//*默认的右向

glm::mat4 Transform::getLocalMatrix() const {
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation)
           * glm::scale(glm::mat4(1.0f), scale);
}//按顺序构建局部到世界的 4x4 变换矩阵。T*R*S