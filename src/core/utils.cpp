#include "utils.h"

namespace core {

glm::mat3 normalMatrix(glm::mat4 const & model_matrix) {
    return glm::transpose(glm::inverse(model_matrix));
}

} // namespace core
