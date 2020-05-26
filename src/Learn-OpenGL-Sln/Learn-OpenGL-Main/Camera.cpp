#include "Camera.hpp"
#include <glm/ext/matrix_transform.hpp>

// Global values, placed here for convenience
const glm::vec3 Camera::ZERO = glm::vec3(0);
const glm::vec3 Camera::WORLD_UP = glm::vec3(0, 1, 0);
const glm::vec3 Camera::WORLD_FORWARD = glm::vec3(0, 0, -1);

// Default camera values
const float Camera::DEFAULT_YAW = -90.0f;
const float Camera::DEFAULT_PITCH = 0.0f;
const float Camera::DEFAULT_SPEED = 2.5f;
const float Camera::DEFAULT_SENSITIVITY = 0.1f;
const float Camera::DEFAULT_FOV = 45.0f;

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: forward(WORLD_FORWARD), up(up), right(0), position(position), yaw(yaw), pitch(pitch),
	roll(0), fov(DEFAULT_FOV), movementSpeed(DEFAULT_SPEED), orientationSensitivity(DEFAULT_SENSITIVITY)
{
	updateDirections();
}

glm::mat4 Camera::viewMatrix() const
{
	return glm::lookAt(position, position + forward, up);
}

void Camera::processMovement(const movement directions, const float deltaTime)
{
	glm::vec3 direction(0);
	if (directions & movement::FORWARD)  direction += forward;
	if (directions & movement::BACKWARD) direction -= forward;
	if (directions & movement::LEFT)     direction -= right;
	if (directions & movement::RIGHT)    direction += right;

	position += direction * (movementSpeed * deltaTime);
}

void Camera::processOrientation(glm::vec2 offset, GLboolean constrain)
{
	offset *= orientationSensitivity;

	yaw += offset.x;
	pitch += offset.y;

	if (constrain)
	{
		// Clamping to prevent large numbers and Gimbal lock
		yaw = glm::mod(yaw, 360.0f);
		pitch = glm::clamp(pitch, -89.0f, 89.0f);
	}

	updateDirections();
}

void Camera::processZoom(const float offset)
{
	zoom -= float(offset);
	zoom = glm::clamp(zoom, 1.0f, DEFAULT_FOV);
}

void Camera::updateDirections()
{
	// Manipulate camera direction
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	forward = glm::normalize(direction);

	// > normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	right = glm::normalize(glm::cross(forward, WORLD_UP));
	up = glm::normalize(glm::cross(right, forward));
}
