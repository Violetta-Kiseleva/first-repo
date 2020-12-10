#include <iostream> 
#define GLEW_STATIC

#include <GLAD/include/glad/glad.h>

// GLFW
#include <GLFW/glfw3.h> 

#include <stb_image/stb_image.h>

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

///////     ���������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void processInput(GLFWwindow* window);
void mouse_moving(GLFWwindow* window, double xpos, double ypos);
void update_camera();
void scroll_moving(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

const GLuint WIDTH = 800, HEIGHT = 600;

///////     ������   
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];                    // ��� ������������, ����� ������� � ��� ������
bool firstMouse = true;
GLfloat lastTime = 0.0f;            // ����� ������ ���������� �����
GLfloat deltaTime = 0.0f;           // �����, ��������� ����� ��������� � ������� ������

GLfloat lastX = WIDTH / 2.0;        // last ���������� ��������� �����
GLfloat lastY = HEIGHT / 2.0;
// ��������� ��������� �����
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


///////////////////////////////////////////////////////                   MAIN        ////////////////////////////////////////
int main(void)
{
    // ����������� GLFW
    if (!glfwInit()) {
        std::cout << "ERROR WITH GLFW \n" << std::endl;
        return -1;
    }
    // ������������� ��� ����������� ��������� ��� GLFW ��� ������ 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // ������� ������ GLFW 
    GLFWwindow* window;
    
    // ������� ���� � ������� � ��� �������� OpenGL
    window = glfwCreateWindow(WIDTH, HEIGHT, "Grafichs_openGL", NULL, NULL);
    if (!window)
    {
        std::cout << "ERROR WITH OPEN THE WINDOW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // �������� �� ����� �������� �������
    glfwSetKeyCallback(window, key_callback);
    
    // ������ ������� � ������� ��������� �����
    glfwSetCursorPosCallback(window, mouse_moving);
    glfwSetScrollCallback(window, scroll_moving);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ������������� GLAD, ����� �������� �����-���� ������� �� openGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // �������� ������� (����� �� ���� ��������� ������)
    glEnable(GL_DEPTH_TEST);

    Shader lamp_Shader("light_vertex_shader.vs", "light_fragment_shader.vs"); // ������ ��������
    Shader light_cube_Shader("vertex_shader.vs", "fragment_shader.vs");
    
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
    // �������            // �������           // ��������
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    }; 

    // ������� ��� ��� �� 10 
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),      // 2.4
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // ������� ������� ������  !!! ���� ������ ���������� ��������� � �������, ��������������� ����������� ������.
    //glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);
    // ������ �������� � ������� ����� �����
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // ����������� ������
    //glm::vec3 cameraDirection = glm::vec3(cameraPosition - cameraTarget);
    //  ������ ����������� ����������� ����� 
    //glm::vec3 upvec = glm::vec3(0.0f, 1.0f, 0.0f);
    // ������ ��������� ������������ � ������� ������ ����������������  - ������������� ����������� ��� �
    //glm::vec3 cameraRight = glm::normalize(glm::cross(upvec, cameraDirection));
    // ��� ������ �����
    //glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

    /*GLuint indices[] = {  // ������� ���������
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle
    };*/

    GLuint  VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);
    
    // ��������� ������ �������, ������������� ������ � ��������� ������  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // �������������
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    // ������� ��������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // ���� ��������
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // ��������
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    
    // VAO ��� �����
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // ��� ��� VBO �������-���������� ��� �������� ��� ����������� ������, 
    // �� ��� ����� ������ ������� � ��� ����� VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // ����������� �������� (����� ����� ����������� ������ ���������� ������)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    /////////////////////////            �������� �������
    /*GLuint texture1;

    //// �������� ������
    ===================================
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);          // ��� 2� �������� ������ �� ���� ������


    // ��������� ��������, ������� ��������
    int width, height, nrComponents;
    unsigned char* image = SOIL_load_image("container2.png", &width, &height, &nrComponents, SOIL_LOAD_RGB);
    if (!image)
    {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }

    GLenum format;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    // ��������� ������ ��������
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);        // �������� � �������� �����������
    glGenerateMipmap(GL_TEXTURE_2D);

    // ������������� ��������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // �������� ���������� ����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ������������ ������� ������
    SOIL_free_image_data(image);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);   */             // ������� ������� ��������

    unsigned int texture_diffuse = loadTexture("container2.png");
    unsigned int texture_specular = loadTexture("container2_specular.png");
    light_cube_Shader.Use();
    light_cube_Shader.setInt("material.diffuse", 0);  
    light_cube_Shader.setInt("material.specular", 1);
        
    // ������ �������� 
    // ===================
    /*GLuint texture2;
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
    glBindTexture(GL_TEXTURE_2D, 0);*/

    ////////////////////////////////////////////////////                ������� ����        ////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // �������� �� ������� (����, ������� �������)
        processInput(window);
        glfwPollEvents();

        update_camera();

        // ������� ����� �����
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // ������� ����� ������� (����� �� �������� �����)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // �������� ��������
        // glBindTexture(GL_TEXTURE_2D, texture); 

        light_cube_Shader.Use();
        
        //light_cube_Shader.setVec3("light.position", lightPos);
        light_cube_Shader.setVec3("viewPos", camera.Position);


        //glm::vec3 lightColor;
        /*lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);*/ 

        //lightColor.x = 2.0f;
        //lightColor.y = 0.7f;
        //lightColor.z = 1.3f;
        //glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);                  // decrease the influence
        //glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);                // low influence
        //light_cube_Shader.setVec3("light.ambient", ambientColor);
        //light_cube_Shader.setVec3("light.diffuse", diffuseColor);
        
        light_cube_Shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        light_cube_Shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        light_cube_Shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        light_cube_Shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);      // ������ ��������� ����

        light_cube_Shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        light_cube_Shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        light_cube_Shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        light_cube_Shader.setFloat("material.shininess", 64.0f);

        /*glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(file_Shader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(file_Shader.Program, "ourTexture2"), 1);*/


        /*
        light_Shader.setVec3("light.ambient", ambientColor);
        light_Shader.setVec3("light.diffuse", diffuseColor);*/

        // �������������
        // ��� ��������� ������� (��������������� ��� ������� � ���������� ������� ������������)
        //glm::mat4 model;
        // ������ ��������: �������
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(1.0f, 0.5f, 0.0f));

        // ��� ������� ���� (�������� ����� �����)
        /*glm::mat4 view;
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); */

        // �������� �� ������ ������� LookAt
        glm::mat4 view;
        view = camera.GetViewMatrix();

        // ������� �������� (���������� ������������� ��������)
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

        light_cube_Shader.setMat4("projection", projection);
        light_cube_Shader.setMat4("view", view); 

        // ��� ������ ������
        glm::mat4 model = glm::mat4(1.0f);
        light_cube_Shader.setMat4("model", model);

 
        // ���������� ������� � ������
        /*GLint modelLoc = glGetUniformLocation(light_Shader.Program, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(light_Shader.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLoc = glGetUniformLocation(light_Shader.Program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)); */

        // ��� ��������� �����
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_diffuse); 

        // ��� �������� �����
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_specular);
        
        
        glBindVertexArray(cubeVAO);
        /*glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);*/

        // ����� �����!!! :3                glm::radians(angle)
         for (GLuint i = 0; i < 9; i++)
         {
             glm::mat4 model = glm::mat4(1.0f);
             model = glm::translate(model, cubePositions[i]);
             float angle = 20.0f * i;
             model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
             light_cube_Shader.setMat4("model", model);

             glDrawArrays(GL_TRIANGLES, 0, 36);
         }

        ////// ����������� �����-���
        /*lamp_Shader.Use();

        lamp_Shader.setMat4("projection", projection);
        lamp_Shader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

        lamp_Shader.setMat4("model", model);

        // ��������� ���� ����� 
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);*/

        // ������ ������� ������ ������
        glfwSwapBuffers(window);
    }
    // ������� ��� ������� ����� ���������� �� ������
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // ������� ������� � ���������� ������
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void update_camera() {
    GLfloat cameraSpeed = 0.5 * deltaTime;
    if (keys[GLFW_KEY_W]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (keys[GLFW_KEY_S]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (keys[GLFW_KEY_A]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (keys[GLFW_KEY_D]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

}

// ���������� ��� ������ ����������� �����
void mouse_moving(GLFWwindow* window, double xpos, double ypos) { // xpos,ypos - ������� ���������� 
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // �������� ������� ��������� ������ ��� ������� Y-���������� ���������� � ����� ���� 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// ��� �������� �����
void scroll_moving(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// ��� ���������� wasd
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// ������� �������� ��������
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
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

    return textureID;
}