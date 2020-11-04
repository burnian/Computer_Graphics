/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 01/28/2020, 13:36
*@Last Modify: 11/04/2020, 22:39
*@Desc: An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
*********************************************************/
#pragma once
#include <glad/glad.h>

//////////////////////////////
// Default camera settings
#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 2.5f
#define SENSITIVITY 0.1f
#define ZOOM 2.0f
#define FOV 45.0f

//////////////////////////////
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

	// coordinates are in world space
	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH)
		: _front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoomSensitivity(ZOOM), fov(FOV)
	{
		this->position = position;
		this->worldUp = worldUp;
		_yaw = yaw;
		_pitch = pitch;
		UpdateCameraVectors();
	}

	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw = YAW, GLfloat pitch = PITCH)
		: _front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoomSensitivity(ZOOM), fov(FOV)
	{
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		_yaw = yaw;
		_pitch = pitch;
		UpdateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() {
		//return glm::lookAt(position, position + front, up); // ��������궼��world space��upΪ��������

		// Custom implementation of the LookAt function
		// Create translation and rotation matrix
		// In glm we access elements as mat[col][row] due to column-major layout
		glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
		translation[3][0] = -position.x; // Third column, first row
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;

		glm::mat4 rotation = glm::mat4(1.0f);
		// ��һ��Ϊ�������ϵ��x�ᵥλ����
		rotation[0][0] = _right.x; // First column, first row
		rotation[1][0] = _right.y;
		rotation[2][0] = _right.z;
		// �ڶ���Ϊ�������ϵ��y�ᵥλ����
		rotation[0][1] = _up.x; // First column, second row
		rotation[1][1] = _up.y;
		rotation[2][1] = _up.z;
		// ������Ϊ�������ϵ��z�ᵥλ����
		rotation[0][2] = -_front.x; // First column, third row
		rotation[1][2] = -_front.y;
		rotation[2][2] = -_front.z;

		// Return lookAt matrix as combination of translation and rotation matrix
		return rotation * translation; // Remember to read from right to left (first translation then rotation)
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(MOVEMENT direction, GLfloat deltaTime) {
		GLfloat velocity = movementSpeed * deltaTime;
		//GLfloat temp = position.y; // ��ֹ��ͷ�����ƶ�
		if (direction == FORWARD)
			position += _front * velocity;
		else if (direction == BACKWARD)
			position -= _front * velocity;
		else if (direction == LEFT)
			position -= _right * velocity;
		else if (direction == RIGHT)
			position += _right * velocity;
		//position.y = temp;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;
		_yaw = fmod(_yaw + xoffset, 360.0f); // �����������cam ��ʱ����ת��������˳ʱ����ת���Ը���ȡģ�ȼ��ڶ������ֵȡģ���ٸ�������ϸ���
		_pitch -= yoffset; // �����������yoffset ��СΪ��

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch) {
			if (_pitch > 89.0f)
				_pitch = 89.0f;
			else if (_pitch < -89.0f)
				_pitch = -89.0f;
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
		else if (fov > 45.0f)
			fov = 45.0f;
	}

	void SetYaw(GLfloat yaw) {
		_yaw = fmod(yaw, 360.0f);
		UpdateCameraVectors();
	}

	void SetPitch(GLfloat pitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		else if (pitch < -89.0f)
			pitch = -89.0f;
		_pitch = pitch;
		UpdateCameraVectors();
	}

	void SetYawPitch(GLfloat yaw, GLfloat pitch) {
		_yaw = fmod(yaw, 360.0f);
		if (pitch > 89.0f)
			pitch = 89.0f;
		else if (pitch < -89.0f)
			pitch = -89.0f;
		_pitch = pitch;
		UpdateCameraVectors();
	}

	void LookBack() {
		SetYawPitch(_yaw + 180.0f, -_pitch);
	}

	//Camera& operator = (const Camera& camera) {
	//	std::cout << "operator =" << std::endl;
	//	if (this != &camera) {
	//		//if (name != NULL)
	//		//	delete name;
	//		//this->id = str.id;
	//		//int len = strlen(str.name);
	//		//name = new char[len + 1];
	//		//memcpy(name, str.name, strlen(str.name) + 1);
	//	}
	//	return *this;
	//}

	GLfloat GetYaw() { return _yaw; }
	GLfloat GetPitch() { return _pitch; }
	glm::vec3 GetFront() { return _front; }
	glm::vec3 GetUp() { return _up; }
	glm::vec3 GetRight() { return _right; }

	// Camera Attributes
	glm::vec3 position;
	glm::vec3 worldUp;
	// Camera options
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoomSensitivity;
	GLfloat fov;

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void UpdateCameraVectors() {
		glm::vec3 direction; // ��ʾcamera position ����λ����Ľ���
		direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)); // cos(pitch)��ʾxzƽ��ֱ�������ε�б�߳���pitch��ֵ��Ϊ-90~90
		direction.y = sin(glm::radians(_pitch)); // б�߳�Ϊ1��pitch��ֵ��Ϊ-90~90
		direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_front = glm::normalize(direction);
		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		_right = glm::normalize(glm::cross(_front, worldUp));
		_up = glm::normalize(glm::cross(_right, _front));
	}

	glm::vec3 _front;
	glm::vec3 _up;
	glm::vec3 _right;
	// Euler Angles
	GLfloat _yaw; // ҡͷ�ǣ������pos Ϊ���ĵ�λ����v ��x �����ṹ�ɵļнǡ�v ��x �������غ�Ϊ-180�ȣ�z������Ϊ-90�ȣ�x������Ϊ0�ȣ�z������Ϊ90�ȣ�x������Ϊ180��
	GLfloat _pitch; // �����ǣ������pos Ϊ���ĵ�λ����v ��xz ƽ�湹�ɵļнǡ�vָ��xz ƽ������Ϊ0��90�ȣ�����Ϊ-90��0�ȡ�����Ϊ+-90�ȣ���Ϊ��ᵼ��ҡͷ�ǿ�ȡ����ֵ
};
