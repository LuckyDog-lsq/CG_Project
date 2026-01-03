#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "base/texture2d.h"
#include "base/vertex.h"

struct Mesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    size_t indexCount = 0;
    glm::vec3 baseColor = glm::vec3(1.0f);
    std::shared_ptr<ImageTexture2D> diffuseTexture;
};

class Model {
public:
    Model() = default;

    explicit Model(std::vector<Mesh>&& meshes);

    Model(const Model&) = delete;

    Model& operator=(const Model&) = delete;

    Model(Model&& rhs) noexcept;

    Model& operator=(Model&& rhs) noexcept;

    ~Model();

    const std::vector<Mesh>& getMeshes() const;

private:
    std::vector<Mesh> _meshes;

    void cleanup();
};

Model loadModelFromFile(const std::string& path, bool loadMtl);
