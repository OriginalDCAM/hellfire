#include "DCraft/Graphics/GL/glsl.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

char* glsl::readFile(const char* filename)
{
    // Open the file using C++ streams for better error handling
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filename << std::endl;
        return nullptr;
    }
    
    // Read the entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string str = buffer.str();
    
    // Allocate memory for the char array (+1 for null terminator)
    char* contents = new char[str.length() + 1];
    
    // Copy the string contents and add null terminator
    std::copy(str.begin(), str.end(), contents);
    contents[str.length()] = '\0';
    
    return contents;
}

bool glsl::compiledStatus(GLint shaderID)
{
    GLint compiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
    if (compiled) {
        return true;
    }
    
    GLint logLength;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> msgBuffer(logLength);
    glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer.data());
    std::cerr << "Shader compilation error: " << msgBuffer.data() << std::endl;
    return false;
}

GLuint glsl::makeVertexShader(const char* shaderSource)
{
    if (!shaderSource) {
        std::cerr << "Null vertex shader source provided" << std::endl;
        return 0;
    }
    
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &shaderSource, NULL);
    glCompileShader(vertexShaderID);
    bool compiledCorrectly = compiledStatus(vertexShaderID);
    if (compiledCorrectly) {
        return vertexShaderID;
    }
    
    // Clean up on failure
    glDeleteShader(vertexShaderID);
    return 0;
}

GLuint glsl::makeFragmentShader(const char* shaderSource)
{
    if (!shaderSource) {
        std::cerr << "Null fragment shader source provided" << std::endl;
        return 0;
    }
    
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &shaderSource, NULL);
    glCompileShader(fragmentShaderID);
    bool compiledCorrectly = compiledStatus(fragmentShaderID);
    if (compiledCorrectly) {
        return fragmentShaderID;
    }
    
    // Clean up on failure
    glDeleteShader(fragmentShaderID);
    return 0;
}

GLuint glsl::makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
{
    if (vertexShaderID == 0 || fragmentShaderID == 0) {
        std::cerr << "Invalid shader IDs provided to makeShaderProgram" << std::endl;
        return 0;
    }
    
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    
    // Check linking status
    GLint linked;
    glGetProgramiv(programID, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> msgBuffer(logLength);
        glGetProgramInfoLog(programID, logLength, NULL, msgBuffer.data());
        std::cerr << "Shader program linking error: " << msgBuffer.data() << std::endl;
        
        // Clean up
        glDeleteProgram(programID);
        return 0;
    }
    
    // Detach shaders after successful linking
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    std::clog << "Shader program created successfully id: " << programID << std::endl;
    
    return programID;
}