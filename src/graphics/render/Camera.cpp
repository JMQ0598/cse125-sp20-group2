#include <graphics/render/Camera.h>

/**
 * Camera constructor sets the position, axes, and up values of the camera.
 * 
 * Default values are all defined in graphics/_options/graphics_vars.h .
 * */
Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch, bool freeCam) 
	: front(INIT_FRONT), moveSpeed(Config::getFloat("Camera_Speed")), sensitivity(INIT_SENSITIVITY), fov(Config::getFloat("Camera_FOV"))
{
	this->pos = pos;
	this->staticPos = pos;
	this->worldUp = up;
	this->yaw = Config::getFloat("Camera_Yaw");
	this->pitch = Config::getFloat("Camera_Pitch");
	this->freeCam = freeCam;
	target = NULL;
	updateCameraVectors();
}


Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, bool freeCam)
	: front(INIT_FRONT), moveSpeed(Config::getFloat("Camera_Speed")), sensitivity(INIT_SENSITIVITY), fov(Config::getFloat("Camera_FOV"))
{
	pos = glm::vec3(posX, posY, posZ);
	this->staticPos = pos;
	worldUp = glm::vec3(upX, upY, upZ);
	this->yaw = yaw;
	this->pitch = pitch;
	this->freeCam = freeCam;
	target = NULL;
	updateCameraVectors();
}

void Camera::setTarget(GameObject* target)
{
	this->target = target;
}
GameObject* Camera::getTarget() 
{
	return target;
}

void Camera::toggleFreeCam() {
	freeCam = !freeCam;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(pos, pos + front, up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::processKeyMovement(Camera_Movement direction)
{
	if (direction == FORWARD)
		pos += front * moveSpeed;
	if (direction == BACKWARD)
		pos -= front * moveSpeed;
	if (direction == LEFT)
		pos -= right * moveSpeed;
	if (direction == RIGHT)
		pos += right * moveSpeed;
}

void Camera::warpToTarget()
{
	if (target) pos = target->getPosition() + staticPos;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(float xoffset, float yoffset)
{
	if (!freeCam) return;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update front, right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
	// Calculate the new front vector
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);

	// Also re-calculate the right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}