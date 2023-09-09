#pragma once

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

namespace core {

glm::mat3 normalMatrix(glm::mat4 const & model_matrix);

} // namespace core
