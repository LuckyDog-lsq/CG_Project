#include "camera.h"

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(
        transform.position, transform.position + transform.getFront(), transform.getUp());
}

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float znear, float zfar)
    : fovy(fovy), aspect(aspect), znear(znear), zfar(zfar) {}

glm::mat4 PerspectiveCamera::getProjectionMatrix() const {
    return glm::perspective(fovy, aspect, znear, zfar);
}

Frustum PerspectiveCamera::getFrustum() const {
    Frustum frustum;
    // TODO: construct the frustum with the position and orientation of the camera
    // Note: this is for Bonus project 'Frustum Culling'
    // write your code here
    // ----------------------------------------------------------------------
    const float halfVSide = zfar * tanf(fovy * 0.5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = zfar * transform.getFront();//朝向向量按远裁剪距离缩放

    const glm::vec3 camPos = transform.position;
    const glm::vec3 fv = glm::normalize(transform.getFront());
    const glm::vec3 rv = glm::normalize(transform.getRight());
    const glm::vec3 uv = glm::normalize(transform.getUp());

    // extents at far plane
    const float halfV = zfar * tanf(fovy * 0.5f);
    const float halfH = halfV * aspect;

    // center of near and far planes
    const glm::vec3 nc = camPos + fv * znear;
    const glm::vec3 fc = camPos + fv * zfar;

    // four corners of the far plane (world space)
    const glm::vec3 ftl = fc + uv * halfV - rv * halfH; // far top left
    const glm::vec3 ftr = fc + uv * halfV + rv * halfH; // far top right
    const glm::vec3 fbl = fc - uv * halfV - rv * halfH; // far bottom left
    const glm::vec3 fbr = fc - uv * halfV + rv * halfH; // far bottom right

    // Helper to build plane: normal must point inside frustum. signedDistance = -dot(normal, pointOnPlane)
    auto makePlaneFromPoints = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) -> Plane {
        // cross((b - a), (c - a)) gives a normal whose direction depends on winding;
        // pick winding such that normal points inward (we will ensure by testing with camera position).
        glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));
        // ensure the normal points inward: if dot(n, camPos - a) > 0 then n points toward camera (inside), else flip
        if (glm::dot(n, camPos - a) < 0.0f) n = -n;
        float d = -glm::dot(n, a);
        return Plane{ n, d };
        };

    // Near plane: normal points inside (+fv)
    {
        glm::vec3 n = fv;
        n = glm::normalize(n);
        float d = -glm::dot(n, nc);
        frustum.planes[Frustum::NearFace] = { n, d };
    }

    // Far plane: normal points inside (toward camera) -> -fv
    {
        glm::vec3 n = -fv;
        n = glm::normalize(n);
        float d = -glm::dot(n, fc);
        frustum.planes[Frustum::FarFace] = { n, d };
    }

    // Left plane: use camPos, ftl, fbl to form plane; ensure normal points inside
    frustum.planes[Frustum::LeftFace] = makePlaneFromPoints(camPos, fbl, ftl);

    // Right plane: use camPos, ftr, fbr (winding chosen to get inward normal)
    frustum.planes[Frustum::RightFace] = makePlaneFromPoints(camPos, ftr, fbr);

    // Top plane: use camPos, ftl, ftr
    frustum.planes[Frustum::TopFace] = makePlaneFromPoints(camPos, ftl, ftr);

    // Bottom plane: use camPos, fbr, fbl
    frustum.planes[Frustum::BottomFace] = makePlaneFromPoints(camPos, fbr, fbl);


    /*
    frustum.planes[Frustum::NearFace] = { transform.position + znear * fv, fv};
    frustum.planes[Frustum::FarFace] = { transform.position + frontMultFar, -fv };
    frustum.planes[Frustum::LeftFace] = { transform.position,
        glm::cross(transform.getUp(),frontMultFar+rv*halfHSide)};
    frustum.planes[Frustum::RightFace] = { transform.position,
        glm::cross(frontMultFar-rv*halfHSide,uv)};
    frustum.planes[Frustum::BottomFace] = { transform.position,
        glm::cross(frontMultFar+uv*halfVSide,rv)};
    frustum.planes[Frustum::TopFace] = { transform.position,
        glm::cross(rv,frontMultFar-uv*halfVSide)};
        */
    // ----------------------------------------------------------------------

    return frustum;
}

OrthographicCamera::OrthographicCamera(
    float left, float right, float bottom, float top, float znear, float zfar)
    : left(left), right(right), top(top), bottom(bottom), znear(znear), zfar(zfar) {}

glm::mat4 OrthographicCamera::getProjectionMatrix() const {
    return glm::ortho(left, right, bottom, top, znear, zfar);
}

Frustum OrthographicCamera::getFrustum() const {
    Frustum frustum;
    const glm::vec3 fv = transform.getFront();
    const glm::vec3 rv = transform.getRight();
    const glm::vec3 uv = transform.getUp();

    // all of the plane normal points inside the frustum, maybe it's a convention
    frustum.planes[Frustum::NearFace] = {transform.position + znear * fv, fv};
    frustum.planes[Frustum::FarFace] = {transform.position + zfar * fv, -fv};
    frustum.planes[Frustum::LeftFace] = {transform.position - right * rv, rv};
    frustum.planes[Frustum::RightFace] = {transform.position + right * rv, -rv};
    frustum.planes[Frustum::BottomFace] = {transform.position - bottom * uv, uv};
    frustum.planes[Frustum::TopFace] = {transform.position + top * uv, -uv};

    return frustum;
}