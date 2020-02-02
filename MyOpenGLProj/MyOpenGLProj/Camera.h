/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 01/28/2020, 13:36
*@Last Modify: 01/28/2020, 13:36
*@Desc: ���
*********************************************************/
#pragma once
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Default camera settings
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 2.5f;
const GLfloat SENSITIVITY = 0.1f;
const GLfloat ZOOM = 2.0f;
const GLfloat FOV = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	enum MOVEMENT {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoomSensitivity(ZOOM), fov(FOV)
	{
		this->position = position;
		this->worldUp = worldUp;
		this->yaw = yaw;
		this->pitch = pitch;
		UpdateCameraVectors();
	}

	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw = YAW, GLfloat pitch = PITCH)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoomSensitivity(ZOOM), fov(FOV)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		UpdateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() {
		//return glm::lookAt(position, position + front, up);

		// Custom implementation of the LookAt function
		// Create translation and rotation matrix
		// In glm we access elements as mat[col][row] due to column-major layout
		glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
		translation[3][0] = -position.x; // Third column, first row
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;

		glm::mat4 rotation = glm::mat4(1.0f);
		// ��һ��Ϊ�������ϵ��x�ᵥλ����
		rotation[0][0] = right.x; // First column, first row
		rotation[1][0] = right.y;
		rotation[2][0] = right.z;
		// �ڶ���Ϊ�������ϵ��y�ᵥλ����
		rotation[0][1] = up.x; // First column, second row
		rotation[1][1] = up.y;
		rotation[2][1] = up.z;
		// ������Ϊ�������ϵ��z�ᵥλ����
		rotation[0][2] = -front.x; // First column, third row
		rotation[1][2] = -front.y;
		rotation[2][2] = -front.z;

		// Return lookAt matrix as combination of translation and rotation matrix
		return rotation * translation; // Remember to read from right to left (first translation then rotation)
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(MOVEMENT direction, GLfloat deltaTime) {
		GLfloat velocity = movementSpeed * deltaTime;
		//GLfloat temp = position.y;
		if (direction == FORWARD)
			position += front * velocity;
		else if (direction == BACKWARD)
			position -= front * velocity;
		else if (direction == LEFT)
			position -= right * velocity;
		else if (direction == RIGHT)
			position += right * velocity;
		//position.y = temp;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;
		yaw += xoffset; // �����������cam ��ʱ����ת��������˳ʱ����ת
		pitch -= yoffset; // �����������yoffset ��СΪ��

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
		// Update Front, Right and Up Vectors using the updated Euler angles
		UpdateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset) {
		if (fov >= 1.0f && fov <= 45.0f)
			fov -= yoffset * zoomSensitivity;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 45.0f)
			fov = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void UpdateCameraVectors() {
		glm::vec3 direction; // ��ʾcamera position ����λ����Ľ���
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); // cos(pitch)��ʾxzƽ��ֱ�������ε�б�߳�
		direction.y = sin(glm::radians(pitch)); // б�߳�Ϊ1
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);
		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

public:
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	// Euler Angles
	GLfloat yaw; // ҡͷ�ǣ������pos Ϊ���ĵ�λ����v ��x �����ṹ�ɵļнǡ�v ��x �������غ�Ϊ-180�ȣ�z������Ϊ-90�ȣ�x������Ϊ0�ȣ�z������Ϊ90�ȣ�x������Ϊ180��
	GLfloat pitch; // �����ǣ������pos Ϊ���ĵ�λ����v ��xz ƽ�湹�ɵļнǡ�vָ��xz ƽ������Ϊ0��90�ȣ�����Ϊ-90��0��
	// Camera options
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoomSensitivity;
	GLfloat fov;
};
