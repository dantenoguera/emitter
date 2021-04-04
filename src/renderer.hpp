#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"

struct particle
{
    float pos[4];
    float vel[4]; // avoid using vec3 [https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)]
    float lt;
    float _pad[3]; // padding for GL 430 layout
};

class Renderer
{
public:

    Renderer(int particle_count, unsigned int particle_tex, unsigned int base_program, unsigned int compute_program);

    void render();

private:

    int particle_count; 
    unsigned int particle_tex;
    unsigned int base_program;
    unsigned int compute_program;
    unsigned int SSBO;
    void particles_init(particle vertices[]);
};
