#pragma once

#include "bounding_box.h"
#include "plane.h"
#include <iostream>

struct Frustum {
public:
    Plane planes[6];
    enum {
        LeftFace = 0,
        RightFace = 1,
        BottomFace = 2,
        TopFace = 3,
        NearFace = 4,
        FarFace = 5
    };

    bool intersect(const BoundingBox& aabb, const glm::mat4& modelMatrix) const {
        // TODO: judge whether the frustum intersects the bounding box
        // Note: this is for Bonus project 'Frustum Culling'
        // write your code here
        // ------------------------------------------------------------
        glm::vec3 corners[8] = {
            {aabb.min.x, aabb.min.y, aabb.min.z},
            {aabb.max.x, aabb.min.y, aabb.min.z},
            {aabb.min.x, aabb.max.y, aabb.min.z},
            {aabb.max.x, aabb.max.y, aabb.min.z},
            {aabb.min.x, aabb.min.y, aabb.max.z},
            {aabb.max.x, aabb.min.y, aabb.max.z},
            {aabb.min.x, aabb.max.y, aabb.max.z},
            {aabb.max.x, aabb.max.y, aabb.max.z}
        };

        // 世界空间
        glm::vec3 worldCorners[8];
        for (int i = 0; i < 8; ++i) {
            glm::vec4 wc = modelMatrix * glm::vec4(corners[i], 1.0f);
            worldCorners[i] = glm::vec3(wc);
        }

        // 对每个平面进行测试：若所有点都在平面外侧，则剔除
        for (int p = 0; p < 6; ++p) {
            const Plane& plane = planes[p];
            int outsideCount = 0;
            for (int i = 0; i < 8; ++i) {
                float distance = glm::dot(plane.normal, worldCorners[i]) + plane.signedDistance;
                if (distance < 0.0f) ++outsideCount;
            }
            if (outsideCount == 8) {
                return false;
            }
        }
        return true;
        // ------------------------------------------------------------
    }
};

inline std::ostream& operator<<(std::ostream& os, const Frustum& frustum) {
    os << "frustum: \n";
    os << "planes[Left]:   " << frustum.planes[0] << "\n";
    os << "planes[Right]:  " << frustum.planes[1] << "\n";
    os << "planes[Bottom]: " << frustum.planes[2] << "\n";
    os << "planes[Top]:    " << frustum.planes[3] << "\n";
    os << "planes[Near]:   " << frustum.planes[4] << "\n";
    os << "planes[Far]:    " << frustum.planes[5] << "\n";

    return os;
}