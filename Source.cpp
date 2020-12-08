#include <iostream> 

// GLEW
#define GLEW_STATIC
#include <GLFW/glew.h>

// GLFW
#include <GLFW/glfw3.h> 

// SOIL
#include <SOIL/SOIL.h> 

// SHADER
#include "Shader.h"

// CAMERA 
#include "Camera.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




// ���������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void Do_Movement(GLFWwindow* window);                                                         // ��� ���������� ������ 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);          // ��� ������ (�����)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);   // �������� �����

// �������
const GLuint WIDTH = 800, HEIGHT = 600; 

// CAMERA info
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];                                // ��� ������������, ����� ������� � ��� ������
GLfloat deltaTime = 0.0f;	                    // �����, ��������� ����� ��������� � ������� ������
GLfloat lastFrame = 0.0f;  	                    // ����� ������ ���������� �����
GLfloat lastX = 400, lastY = 300;               // last ���������� ��������� �����
bool firstMouse = true;

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
    glfwWindowHint(GLFW_SAMPLES, 4);

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

    // ������ ������� � ������� ��������� �����  
    glfwSetCursorPosCallback(window, mouse_callback);               // ������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    
    glfwSetScrollCallback(window, scroll_callback);                 // ��������

    // ������������� GLEW, ����� �������� �����-���� ������� �� openGL
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }  
    glViewport(0, 0, WIDTH, HEIGHT);    // ������� ����

    glEnable(GL_DEPTH_TEST);

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

    // �������������
    /*
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
    }; */

    GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    }; 

    // ������� ��� ��� �� 10 
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    }; 

    GLuint VBO, VAO; 
    //GLuint EBO;                     // �������������
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);          // �������������

    // ��������� ������ ������� � ������������� ������ � ��������� ������
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    // ��� �������������� 
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ������� ��������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // ���� ��������
    /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); */

    // ��������
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);                           // ���������� VAO 

    // ��������� � ������� ��������
    GLuint texture1; 
    
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
    GLuint texture2;
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
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // �������� �� ������� (����, ������� �������) 
        glfwPollEvents(); 

        Do_Movement(window);

        // ������� ����� �����
        glClearColor(0.9f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

       
        // �������������
        // ��� ��������� ������� (��������������� ��� ������� � ���������� ������� ������������)
        //glm::mat4 model;
        // ������ ��������: �������
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(1.0f, 0.5f, 0.0f));

        // �������� �� ������ ������� LookAt
        glm::mat4 view;
        view = camera.GetViewMatrix();
        
        // ��� ������� ���� (�������� ����� �����)
        /*glm::mat4 view; 
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); */

        // ������� �������� (���������� ������������� ��������)
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (float)WIDTH/(float)HEIGHT, 0.1f, 1000.0f);

        // ���������� ������� � ������
        GLint modelLoc = glGetUniformLocation(file_Shader.Program, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 

        GLint viewLoc = glGetUniformLocation(file_Shader.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); 

        GLint projectionLoc = glGetUniformLocation(file_Shader.Program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);  
       // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      //  glDrawArrays(GL_TRIANGLES, 0, 3);   // �����������
        //glDrawArrays(GL_TRIANGLES, 0, 36);      // ���� ���

        
        
        // ����� �����!!! :3 
        for (GLuint i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // ������ ������� ������ ������
        glfwSwapBuffers(window);
    }
    // ������� ��� ������� ����� ���������� �� ������
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    // ��������� ������ GLFW, ������� ���������� �������
    glfwTerminate();
    return 0;
}

// ����������, ����� ������ �������� ������� GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // esc
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)       // ������
            keys[key] = true;
        else if (action == GLFW_RELEASE)    // ��������
            keys[key] = false;
    }
}

void Do_Movement(GLFWwindow* window)
{
    // �������� ������ WASD
    //std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
} 


// ���������� ��� ������ ����������� �����
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // ��������� �������� ������
    if (firstMouse) // ��� ���������� ���� ������������������� ��������� true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // �������� ������� ��������� ������ ��� ������� Y-���������� ���������� � ����� ���� 
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);          // !!
} 

// ��� �������� �����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);     // !!
}