# glm/CMakeLists.txt - Subdirectory mode for add_subdirectory(glm)
# Top-level project provides cmake_minimum_required() and project()

add_library(glm INTERFACE)

# Headers are located under ${CMAKE_CURRENT_SOURCE_DIR}/glm
# So consumers should include with -I<external/glm> and then use <glm/glm.hpp>
target_include_directories(glm INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

# Require a minimal C++ standard (adjust if you need a newer standard)
target_compile_features(glm INTERFACE cxx_std_11)

# Provide a namespaced alias for convenience
if (NOT TARGET glm::glm)
  add_library(glm::glm ALIAS glm)
endif()

# Optional debug message (uncomment if you need to debug configure step)
# message(STATUS "GLM include (build): ${CMAKE_CURRENT_SOURCE_DIR}")