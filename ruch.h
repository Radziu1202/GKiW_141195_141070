#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

glm::vec3 computeMatricesFromInputs(GLFWwindow* window, glm::mat4 &P, glm::mat4 &V,
	float aspect, bool z_moving);