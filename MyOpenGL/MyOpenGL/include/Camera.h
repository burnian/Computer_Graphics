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
		//return glm::lookAt(position, position + front, up); // 这里的坐标都是world space，up为方向向量

		// Custom implementation of the LookAt function
		// Create translation and rotation matrix
		// In glm we access elements as mat[col][row] due to column-major layout
		glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
		translation[3][0] = -position.x; // Third column, first row
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;

		glm::mat4 rotation = glm::mat4(1.0f);
		// 第一行为相机坐标系的x轴单位向量
		rotation[0][0] = _right.x; // First column, first row
		rotation[1][0] = _right.y;
		rotation[2][0] = _right.z;
		// 第二行为相机坐标系的y轴单位向量
		rotation[0][1] = _up.x; // First column, second row
		rotation[1][1] = _up.y;
		rotation[2][1] = _up.z;
		// 第三行为相机坐标系的z轴单位向量
		rotation[0][2] = -_front.x; // First column, third row
		rotation[1][2] = -_front.y;
		rotation[2][2] = -_front.z;

		// Return lookAt matrix as combination of translation and rotation matrix
		return rotation * translation; // Remember to read from right to left (first translation then rotation)
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(MOVEMENT direction, GLfloat deltaTime) {
		GLfloat velocity = movementSpeed * deltaTime;
		//GLfloat temp = position.y; // 禁止镜头上下移动
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
		_yaw = fmod(_yaw + xoffset, 360.0f); // 鼠标向右移则cam 逆时针旋转，左移则顺时针旋转，对负数取模等价于对其绝对值取模，再给结果添上负号
		_pitch -= yoffset; // 鼠标向上移则yoffset 减小为负

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
		glm::vec3 direction; // 表示camera position 到单位球面的交点
		direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)); // cos(pitch)表示xz平面直角三角形的斜边长，pitch的值域为-90~90
		direction.y = sin(glm::radians(_pitch)); // 斜边长为1，pitch的值域为-90~90
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
	GLfloat _yaw; // 摇头角，以相机pos 为起点的单位向量v 与x 正半轴构成的夹角。v 与x 负半轴重合为-180度，z负半轴为-90度，x正半轴为0度，z正半轴为90度，x负半轴为180度
	GLfloat _pitch; // 俯仰角，以相机pos 为起点的单位向量v 与xz 平面构成的夹角。v指向xz 平面以上为0到90度，以下为-90到0度。不能为+-90度，因为这会导致摇头角可取任意值
};
