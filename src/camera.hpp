#pragma once

#include <glm/glm.hpp>

#define CAMERA_SPEED 2.5f
#define CAMERA_SENSITIVITY 0.05f
#define YAW -90.0f
#define PITCH 0.0f
#define FOV 45.0f

#define POSITION glm::vec3(0.0f, 0.0f,  3.0f)
#define UP glm::vec3(0.0f, 1.0f,  0.0f)
#define FRONT glm::vec3(0.0f, 0.0f, -1.0f)

class Camera 
{
public:
    glm::vec3 position = POSITION;
    glm::vec3 front = FRONT;
    glm::vec3 up = UP;
    // euler Angles
    float yaw = YAW;
    float pitch = PITCH;
    // camera options
    float speed = CAMERA_SPEED;
    float sensitivity = CAMERA_SENSITIVITY;
    float fov = FOV; 

    Camera(glm::vec3 position = POSITION) : 
        position(position), front(FRONT), up(UP), yaw(YAW), pitch(PITCH), speed(CAMERA_SPEED), sensitivity(CAMERA_SENSITIVITY)
    {}

    void change_front(float delta_yaw, float delta_pitch)
    {
        yaw += delta_yaw * sensitivity;
        pitch -= delta_pitch * sensitivity;

        if(pitch > 89.0f)
        {
            pitch = 89.0f;
        }
        if(pitch < -89.0f)
        {
            pitch = -89.0f;
        }

        front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        front.y = sinf(glm::radians(pitch));
        front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        front = glm::normalize(front);
    }

    void change_position(float delta_horizontal, float delta_vertical)
    {
        position += glm::normalize(glm::cross(front, up)) * delta_horizontal; 
        position += delta_vertical * front;
    }

    void change_fov(float delta_fov)
    {
        fov -= delta_fov;

        if (fov < 1.0f)
        {
            fov = 1.0f;
        }
        if (fov > 45.0f)
        {
            fov = 45.0f;
        }
    }
};
