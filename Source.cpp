#include <iostream> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#define GLEW_STATIC
#include <GLFW/glew.h>

// GLFW
#include <GLFW/glfw3.h> 

// SOIL
#include <SOIL/SOIL.h>

// SHADER
#include "Shader.h"


// ��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

/////////////////////////////////////////////////////////////////        MAIN        ///////////////////////////////////////////
int main()
{
    // ����������� GLFW
    if (!glfwInit())
    {
        std::cout << "Smth goes wrong with GLFW \n" << std::endl;
        return -1;
    }

    // ������������� ��� ����������� ��������� ��� GLFW ��� ������ 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // ������� ������ GLFW 
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Grafichs_openGL", nullptr, nullptr);
    
    if (window == nullptr)
    {
        std::cout << "Smth goes wrong that to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 

    // �������� �� ����� �������� �������
    glfwSetKeyCallback(window, key_callback);

    // ������������� GLEW, ����� �������� �����-���� ������� �� openGL
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }  
    glViewport(0, 0, WIDTH, HEIGHT);    // ������� ����


    // �������� � ����������� ��������������� ����
    Shader file_Shader("vertex_shader.vs", "fragment_shader.vs");


    // ��������� ������ (�����) � ����������
    /* �����������
    GLfloat vertices[] = {
            // �������           // �����
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,      // ������ ���
       -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,      // ����� ���
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f       // ������� 
    };
    */

    // �������� ������ (�����) � ����������
    GLfloat vertices[] = {
        // �������              // �����           // ��������
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // ������ ����
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // ������ ���
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // ����� ���
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // ����� ���� 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // ������ �����������
        1, 2, 3  // ������ �����������
    };

    GLuint VBO, VAO; 
    GLuint EBO;                     // �������������
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);          // �������������

    // ��������� ������ ������� � ������������� ������ � ��������� ������
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    // ��� �������������� 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ������� ��������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // ���� ��������
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); 

    // ��������
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);                           // ���������� VAO 

    // ��������� � ������� ��������
    GLuint texture1; 
    GLuint texture2;
    //// �������� ������
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);          // ��� 2� �������� ������ �� ���� ������
    
    // ������������� ��������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // �������� ���������� ����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ��������� ��������, ������� ��������
    int width, height;
    unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    
    if (!image)
    {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }
    // ��������� ������ ��������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);        // �������� � �������� �����������
    glGenerateMipmap(GL_TEXTURE_2D);

    // ������������ ������� ������ 
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);                // ������� ������� ��������

    // ������ �������� 
    // ===================
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    image = SOIL_load_image("awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB); 

    if (!image)
    {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);


    ////////////////////////////////////////////////////////        ������� ����    //////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        // �������� �� ������� (����, ������� �������) 
        glfwPollEvents();

        // ������� ����� �����
        glClearColor(0.9f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // �������� ��������
        // glBindTexture(GL_TEXTURE_2D, texture); 

        // ������ �������������
        file_Shader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(file_Shader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(file_Shader.Program, "ourTexture2"), 1);


        glBindVertexArray(VAO);  
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      //  glDrawArrays(GL_TRIANGLES, 0, 3);   // �����������
        glBindVertexArray(0);

        // ������ ������� ������ ������
        glfwSwapBuffers(window);
    }
    // ������� ��� ������� ����� ���������� �� ������
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // ��������� ������ GLFW, ������� ���������� �������
    glfwTerminate();
    return 0;
}

// ����������, ����� ������ �������� ������� GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}