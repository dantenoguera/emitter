#include <GL/glew.h>
#include <cstdlib>
#include <time.h>

#include "renderer.hpp"

extern Camera camera;
extern GLFWwindow *window;
extern float dt;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;


Renderer::Renderer(int particle_count, unsigned int particle_tex, unsigned int base_program, unsigned int compute_program)
    : particle_count { particle_count },
    particle_tex { particle_tex },
    base_program { base_program },
    compute_program { compute_program }
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

Renderer::~Renderer()
{}

void Renderer::render()
{
    glUseProgram(compute_program);
	glUniform1f(glGetUniformLocation(compute_program, "dt"), dt); 
    
	glUniform3f(glGetUniformLocation(compute_program, "attr"), 0.0f, -0.8f, 0.0f);

    int workgroups = particle_count / 16;
    glDispatchCompute(workgroups, 1, 1);

#if 0
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << std::hex << err << '\n';
    }
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
#endif

	glUseProgram(base_program);
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(base_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    glUniformMatrix4fv(glGetUniformLocation(base_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(base_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindTexture(GL_TEXTURE_2D, particle_tex);

	glBindBuffer(GL_ARRAY_BUFFER, SSBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(particle), (void *) (offsetof(particle, lt)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glPointSize(16);
	glDrawArrays(GL_POINTS, 0, particle_count);
}

void Renderer::particles_init(particle particles[])
{
    srand(time(NULL));

    for (int i = 0; i < particle_count; i++)
    {
        float low = -0.5f;
        float high = 0.5f;

		particles[i].pos[0] = 0.0f;
		particles[i].pos[1] = 0.5f;
		particles[i].pos[2] = 0.0f;
		particles[i].pos[3] = 1.0f;

		particles[i].vel[0] = low + rand() / float(RAND_MAX) * (high - low);
		particles[i].vel[1] = low + rand() / float(RAND_MAX) * (high - low);
		particles[i].vel[2] = low + rand() / float(RAND_MAX) * (high - low);

        particles[i].lt = 1.0f;
    }
}
