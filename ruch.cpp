#include "ruch.h"

glm::vec3 position = glm::vec3(0, 2, 5);
float horizontalAngle = 3.14f;
float verticalAngle = -1.5f;
float FoV = 60.0f;
float speed = 10.0f;
float mouseSpeed = 0.005f;


double lastTime = glfwGetTime();

glm::vec3 computeMatricesFromInputs(GLFWwindow* window, glm::mat4 &P, glm::mat4 &V,
	float aspect, bool Z_MOVING) {
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	double xpos, ypos;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, width / 2, height / 2);

	horizontalAngle += mouseSpeed * float(width / 2 - xpos);		//obracanie myszka
	if( float(height / 2 - ypos) < 0 && verticalAngle > -1.57 ||
		float(height / 2 - ypos) > 0 && verticalAngle < 1.57)			//ograniczenie zeby nie mozna bylo salta
			verticalAngle += mouseSpeed * float(height / 2 - ypos);

	glm::vec3 direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),										//kierunek w którym bedzieky sie poruszac
		cos(verticalAngle) * cos(horizontalAngle)				//zalezy od kierunku w który patrzymy
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::cross(right, direction);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {						//poruszanie w zaleznosci od przycisku
		position += direction * deltaTime * speed;			
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	if (!Z_MOVING) position.y = 4;

	P = glm::perspective(glm::radians(FoV), aspect, 0.1f, 100.0f);
	V = glm::lookAt(
		position,           // tu jest kamera
		position + direction, // i tu patrzymy :z naszej pozycji w danym kierunku
		up                  // czy patrzymy w góre czy w dół( 0,-1,0 zeby spojrzec w dół)
	);
	printf("%f %f %f\n", position.x, position.y, position.z);
	/*printf_s("%f %f %f %f\n", V[0].x, V[0].y, V[0].z, V[0].w);
	printf_s("%f %f %f %f\n", V[1].x, V[1].y, V[1].z, V[1].w);
	printf_s("%f %f %f %f\n", V[2].x, V[2].y, V[2].z, V[2].w);
	printf_s("%f %f %f %f\n\n\n\n", V[3].x, V[3].y, V[3].z, V[3].w);*/
	
	lastTime = currentTime;
	return position;
}