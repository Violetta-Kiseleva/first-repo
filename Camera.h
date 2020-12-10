#pragma once

#include <vector>

// библиотеки
#include <GLAD/include/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// опции для движения камеры
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// const для камеры
const GLfloat YAW        = -90.0f;	// поворот влево-вправо
const GLfloat PITCH      =  0.0f;	// наклон верх-вниз
const GLfloat SPEED      =  3.0f;	// скорость перемещения
const GLfloat SENSITIVTY =  0.05f;	// чувст-ть мышки
const GLfloat ZOOM       =  45.0f;	// max значение отдаления (колесико)


///////////////////////// class Camera (вычисление углов Эйлера), векторов и матриц	/////
class Camera
{
public:
    // позиция камеры
    glm::vec3 Position;		
    glm::vec3 Front;		// направление
    glm::vec3 Up;		// верхняя ось
    glm::vec3 Right;		// правая ось
    glm::vec3 WorldUp;
    
    // углы Эйлера (рыскание, тангаж)
    GLfloat Yaw;
    GLfloat Pitch;

    // Опции
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    ///////////////////////	конструктор (вектора)	/////////////////
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }
    //////////////////////	конструктор (скаляры)	/////////////////
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    // возвращает матрицу вида (через углы Эйлера и матрицу LookAt)
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    // движение камеры (WASD)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;	// скорость
        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * velocity;
        if (direction == RIGHT)
            this->Position += this->Right * velocity;
    }

    // движение камеры (мышка)
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw   += xoffset;
        this->Pitch += yoffset;

        // верх неба до ног, не дальше (cos > 90 ---> отрицательный)
        if (constrainPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }

        // обновляем вектора камеры
        this->updateCameraVectors();
    }

    // движение камеры (колесико мышки)
    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
            this->Zoom -= yoffset;
        if (this->Zoom <= 1.0f)
            this->Zoom = 1.0f;
        if (this->Zoom >= 45.0f)
            this->Zoom = 45.0f;
    }

private:
    // вычисление новых векторов камеры
    void updateCameraVectors()
    {
        // новый вектор направления 
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // новые правая и верхняя ось
	// нормализуем ибо их длина~0 и из-за этого движении тормозит
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
    }
};