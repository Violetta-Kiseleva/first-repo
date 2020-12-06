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


    // Установка вершин (буфер) и указателей
    /* ТРЕУГОЛЬНИК
    GLfloat vertices[] = {
            // Позиция           // Цвета
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,      // правый низ
       -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,      // левый низ
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f       // Вершина 
    };
    */

    // Устновка вершин (буфер) и указателей
    GLfloat vertices[] = {
        // Позиции              // Цвета           // Текстуры
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // правый верх
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // правый низ
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // левый низ
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // левый верх 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // первый треугольник
        1, 2, 3  // второй треугольник
    };

    GLuint VBO, VAO; 
    GLuint EBO;                     // прямоугольник
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);          // прямоугольник

    // связываем объект массива и устанавливаем буфера и аттрибуты вершин
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    // для прямоугольника 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Позиция атрибута
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Цвет атрибута
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); 

    // Текстура
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);                           // отвязываем VAO 

    // загружаем и создаем текстуру
    GLuint texture1; 
    GLuint texture2;
    //// текстура первая
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);          // все 2д операции влияют на этот объект
    
    // устанавливает текстуру
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // текстура фильтрации параметров
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // загружаем картинку, создаем текстуру
    int width, height;
    unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    
    if (!image)
    {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }
    // генерирую данные текстуры
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);        // привязка к текстуре изображения
    glGenerateMipmap(GL_TEXTURE_2D);

    // освобождение участка памяти 
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);                // отвязка объекта текстуры

    // вторая текстура 
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


    ////////////////////////////////////////////////////////        ИГРОВОЙ ЦИКЛ    //////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        // проверка на события (мышь, нажатие клавиши) 
        glfwPollEvents();

        // очищаем буфер цвета
        glClearColor(0.9f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // привязка текстуры
        // glBindTexture(GL_TEXTURE_2D, texture); 

        // рисуем прямоугольник
        file_Shader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(file_Shader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(file_Shader.Program, "ourTexture2"), 1);


        glBindVertexArray(VAO);  
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      //  glDrawArrays(GL_TRIANGLES, 0, 3);   // треугольник
        glBindVertexArray(0);

        // Меняем местами буфера экрана
        glfwSwapBuffers(window);
    }
    // удаляем все ресурсы после завершения их работы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // окончание работы GLFW, очищаем выделенные ресурсы
    glfwTerminate();
    return 0;
}

// вызывается, когда нажата отпущена клавиша GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}