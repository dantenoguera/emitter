#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "renderer.hpp"

extern Camera camera;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

std::string load_code(const char* path);
void checkCompileErrors(GLuint shader, std::string type);

Renderer::Renderer()
{}

Renderer::~Renderer()
{}

void Renderer::load_base_shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertex_code = load_code(vertexPath);
    std::string fragment_code = load_code(fragmentPath);
    const char* vShaderCode = vertex_code.c_str();
    const char * fShaderCode = fragment_code.c_str();
    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    base_program = glCreateProgram();
    glAttachShader(base_program, vertex);
    glAttachShader(base_program, fragment);
    glLinkProgram(base_program);

    checkCompileErrors(base_program, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Renderer::load_compute_shader(const char* compute_path)
{
    std::string compute_code = load_code(compute_path);
    const char *compute_src = compute_code.c_str();

    GLuint compute = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(compute, 1, &compute_src, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    compute_program = glCreateProgram();
    glAttachShader(compute_program, compute);
    glLinkProgram(compute_program);

    checkCompileErrors(compute_program, "PROGRAM");

    glDeleteShader(compute);
}

void Renderer::load_texture(char const * path)
{
    GLuint texture;
    glGenTextures(1, &texture);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    particle_tex = texture;
}

void Renderer::render()
{
    float t = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(compute_program);
	glUniform1f(glGetUniformLocation(compute_program, "dt"), dt); 
	glUniform3f(glGetUniformLocation(compute_program, "attr"), 0, 0, 0);

    int workgroup_size = 2;
    int workgroups = particle_count / workgroup_size;
    glDispatchCompute(workgroups, 1, 1);

    /*
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << std::hex << err << '\n';
    }
    */

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    // vs setup
	glUseProgram(base_program);
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(base_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    glUniformMatrix4fv(glGetUniformLocation(base_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(base_program, "model"), 1, GL_FALSE, glm::value_ptr(model));


	glBindTexture(GL_TEXTURE_2D, particle_tex);

	glBindBuffer(GL_ARRAY_BUFFER, SSBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
	glEnableVertexAttribArray(0);
	glPointSize(16);
	glDrawArrays(GL_POINTS, 0, particle_count);

	glfwSwapBuffers(window);

    dt = glfwGetTime() - t; // 0.016
}

void Renderer::particles_init(particle particles[])
{
    for (int i = 0; i < particle_count; i++)
    {
		float rnd = (float)rand();

		// particles[i].pos[0] = 1.0f;
		particles[i].pos[0] = cos(rnd) * (rnd / float(RAND_MAX));
		// particles[i].pos[1] = 0.0f;
		particles[i].pos[1] = sin(rnd) * (rnd / float(RAND_MAX));
		particles[i].pos[2] = cos(rnd) * sin(rnd) * (rnd / float(RAND_MAX));
		particles[i].pos[3] = 1.0f;

		particles[i].vel[0] = 0.0f;
		particles[i].vel[1] = 0.0f;
		particles[i].vel[2] = 0.0f;
		particles[i].vel[3] = 1.0f;
    }
}

void Renderer::gen_buffers()
{
    // No more default VAO with OpenGL 3.3+ core profile context,
	// so in order to make our SSBOs render create and bind a VAO
	// that's never used again
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    glGenBuffers(1, &SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

    particle particles[particle_count];
    particles_init(particles);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
}


std::string load_code(const char* path)
{
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try 
    {
        file.open(path);
        std::stringstream stream;

        stream << file.rdbuf();

        file.close();

        code = stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    return code;
}

void checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " 
                << type << "\n" << infoLog 
                << "\n-------------------------------------------------------" 
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " 
                << type << "\n" << infoLog 
                << "\n-------------------------------------------------------" 
                << std::endl;
        }
    }
}
