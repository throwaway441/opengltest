//#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace std;

//window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, IBO, shader, uniformModel;
const float toRadians = 3.14159265f / 180.0f;

bool xdirection = true;
bool ydirection = true;
bool sizedirection = true;
float curSize = 0.4f; //current size
float maxSize = 0.8f;
float minSize = 0.2f;
float trixOffset = 0.0f;
float triyOffset = 0.0f;
float triMaxoffset = 0.7f;
//float triIncrement = 0.02f;

float curAngle = 0.0f;

//VERTEX SHADER
static const char* vShader = "       \n\
#version 330                        \n\
                                    \n\
layout (location = 0) in vec3 pos;  \n\
                                    \n\
out vec4 vertexcol;                                    \n\
uniform mat4 model;                 \n\
                                    \n\
                                    \n\
void main()                         \n\
{                                   \n\
    gl_Position = model * vec4(pos, 1.0);             \n\
    vertexcol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);                               \n\
}";


//FRAGMENT SHADER
static const char* fShader = "       \n\
#version 330                        \n\
                                    \n\
in vec4 vertexcol;                        \n\
out vec4 color;                     \n\
                                    \n\
void main()                         \n\
{                                   \n\
    color = vertexcol;             \n\
                                    \n\
}";


void CreateTriangle()
{
    unsigned int indices[] =
    {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };



    GLfloat vertices[] =
    {
    -1.0f, -1.0f, -1.0f,
    0.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    0.0f, 1.0f, 0.0f
    };


    //ASSIGNING VAO ID AND BINDING IT
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //ASSIGNING IBO/EBO ID AND BINDING IT
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //ASSIGNING VBO ID AND BINDING IT
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //UNBINDING VBO, VAO AND IBO/EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void addShader(GLuint Shaderprogram, const char* shadercode, GLenum shadertype)
{
    GLuint theshader = glCreateShader(shadertype);
    const GLchar* theCode[1];
    theCode[0] = shadercode;

    GLint codeLength[1];
    codeLength[0] = strlen(shadercode);

    glShaderSource(theshader, 1, theCode, codeLength);
    glCompileShader(theshader);

    GLint result = 0;
    GLchar eLog[1024] = {0};

    glGetShaderiv(theshader, GL_COMPILE_STATUS, &result);
    if(!result)
    {
        glGetProgramInfoLog(theshader, sizeof(eLog), NULL, eLog);
        printf("ERROR COMPILING THE %d SHADER '%s' /n", shadertype, eLog);
        return;
    }

    glAttachShader(Shaderprogram, theshader);
}


void compileShaders()
{
    shader = glCreateProgram();

    if (!shader)
    {
        printf("FAILED TO CREATE SHADER PROGRAM\n");
        return;
    }

    addShader(shader, vShader, GL_VERTEX_SHADER);
    addShader(shader, fShader, GL_FRAGMENT_SHADER);

    //RESULT OF ADDSHADER AND ERROR LOG FOR ADDSHADER FOR DEBUGGING
    GLint result = 0;
    GLchar eLog[1024] = {0};

    glLinkProgram(shader);
    //CHECK IF SHADER PROGRAM WORKS
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if(!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("ERROR LINKING PROGRAM '%s' /n", eLog);
        return;
    }

    glValidateProgram(shader);

    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if(!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("ERROR VALIDATING PROGRAM '%s' /n", eLog);
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model");



}


int main()
{
    //INITIALIZE GLFW

    //NEVER DOUBT THAT PRINTF DOESNT WORK PROPERLY BECAUSE IT DOES WORK


    if(!glfwInit())
    {
        printf("GLFW INIT FAILED\n");
        glfwTerminate();
        return 1;
    }

    //SET GLFW VERSION (VERSION 3 TO VERSION 3 HERE)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //CORE PROFILE = NO BACKWARDS COMPATIABILITY, NO DEPRECATED FEATURES
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //ENABLE FORWARD COMPAT
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //WINDOW OBJECT CREATION
    GLFWwindow *mainWin = glfwCreateWindow(WIDTH, HEIGHT, "test", NULL, NULL);
    if(!mainWin)
    {
        printf("GLFW WINDOW FAILED\n");
        glfwTerminate();
        return 1;
    }

    //GET BUFFER SIZE INFORMATION
    int bufferW, bufferH;
    glfwGetFramebufferSize(mainWin, &bufferW, &bufferH);

    //SET CONTEXT FOR GLFW TO USE
    glfwMakeContextCurrent(mainWin);

    //ALLOW MODERN EXTENSION FEATURES
    glewExperimental = GL_TRUE;

    if(glewInit() != GLEW_OK)
    {
        printf("GLEW INIT FAILED\n");
        glfwDestroyWindow(mainWin);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    //SET VIEWPORT SIZE
    glViewport(0, 0, bufferW, bufferH);

    CreateTriangle();
    compileShaders();


    //LOOP
    while(!glfwWindowShouldClose(mainWin))
    {
        //HANDLE USER EVENTS
        glfwPollEvents();

        if(xdirection)
        {
                trixOffset += 0.001;
        }
        else
        {
                trixOffset -= 0.001;
        }

        if(abs(trixOffset) >= triMaxoffset)
        {
                xdirection = !xdirection;
        }

        if(ydirection)
        {
                triyOffset += 0.01f;
        }
        else
        {
                triyOffset -= 0.01f;
        }

        if(abs(triyOffset) >= triMaxoffset)
        {
                ydirection = !ydirection;
        }

		curAngle += 0.6f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

        if(sizedirection)
        {
                curSize += 0.002f;
        }
        else
        {
                curSize -= 0.002f;
        }

        if(curSize >= maxSize || curSize <= minSize)
        {
            sizedirection = !sizedirection;
        }

        //CLEAR WINDOW - RED, GREEN, BLUE, ALPHA
        glClearColor(0.1f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //USE FRAGMENT AND VERTEX SHADER
        glUseProgram(shader);

        glm::mat4 model(1.0f);

        // ORDER SHOULD ALWAYS BE TRANSLATE, ROTATE, SCALE
        model = glm::translate(model, glm::vec3(trixOffset, triyOffset, 0.0f));
        model = glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));

//        glUniform1f(uniformModel, trixOffset);
//        glUniform1f(uniformModel, triyOffset);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glUseProgram(0);

        glfwSwapBuffers(mainWin);
    }


    printf("I love OpenGL\n");
    return 0;
}
