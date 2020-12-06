#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GLFW/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// SHADER
#include "Shader.h"


// Прототип
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

/////////////////////////////////////////////////////////////////        MAIN        ///////////////////////////////////////////
int main()
{
    // настраиваем GLFW
    if (!glfwInit())
    {
        std::cout << "Smth goes wrong with GLFW \n" << std::endl;
        return -1;
    }

    // устанавливаем все необходимые параметры для GLFW для версии 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // создаем объект GLFW 
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Grafichs_openGL", nullptr, nullptr);
    
    if (window == nullptr)
    {
        std::cout << "Smth goes wrong that to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 

    // проверка на вызов обратных функций
    glfwSetKeyCallback(window, key_callback);

    // инициализация GLEW, чтобы вызывать какие-либо функции из openGL
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }  
    glViewport(0, 0, WIDTH, HEIGHT);    // размеры окна


    // вызываем и компилируем нашзаголовочный файл
    Shader file_Shader("vertex_shader.vs", "fragment_shader.vs");


    // Устновка вершин (буфер) и указателей
    GLfloat vertices[] = {
            // Позиция           // Цвета
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,      // правый низ
       -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,      // левый низ
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f       // Вершина 
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO


    ////////////////////////////////////////////////////////        ИГРОВОЙ ЦИКЛ    //////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.9f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        file_Shader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}