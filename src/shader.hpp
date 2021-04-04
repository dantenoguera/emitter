#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

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

void check_compile_errors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
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

unsigned int load_base_shader(const char* vertexPath, const char* fragmentPath)
{

    unsigned int base_program = glCreateProgram();

    std::string vertex_code = load_code(vertexPath);
    std::string fragment_code = load_code(fragmentPath);
    const char* vShaderCode = vertex_code.c_str();
    const char * fShaderCode = fragment_code.c_str();
    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    base_program = glCreateProgram();
    glAttachShader(base_program, vertex);
    glAttachShader(base_program, fragment);
    glLinkProgram(base_program);

    check_compile_errors(base_program, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return base_program;
}

unsigned int load_compute_shader(const char* compute_path)
{
    unsigned int compute_program = glCreateProgram();

    std::string compute_code = load_code(compute_path);
    const char *compute_src = compute_code.c_str();

    GLuint compute = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(compute, 1, &compute_src, NULL);
    glCompileShader(compute);
    check_compile_errors(compute, "COMPUTE");

    compute_program = glCreateProgram();
    glAttachShader(compute_program, compute);
    glLinkProgram(compute_program);

    check_compile_errors(compute_program, "PROGRAM");

    glDeleteShader(compute);

    return compute_program;
}
