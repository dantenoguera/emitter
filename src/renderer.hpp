#ifndef RENDERER_HPP
#define RENDERER_HPP

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
    float vel[4];
};

class Renderer
{
public:

    Renderer();
    ~Renderer();

    GLFWwindow *window;
    void load_base_shader(const char* vertex_path, const char* fragment_path);
    void load_compute_shader(const char* compute_path);
    void load_texture(char const * path);
    void gen_buffers();
    void render();

private:

    GLuint base_program;
    GLuint compute_program;
    GLuint SSBO;
    GLuint particle_tex;
    int particle_count = 128;
    float dt = 0.0f;
    void particles_init(particle vertices[]);
};
#endif
