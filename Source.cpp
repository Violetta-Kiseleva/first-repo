#include <iostream> 
#define GLEW_STATIC
#include <map>
#include <vector>

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



/////////     ПРОТОТИПЫ
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void control_keyboard(GLFWwindow* window);
void mouse_moving(GLFWwindow* window, double xpos, double ypos);
void control_camera();
void scroll_moving(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);
void normal_function();
void parallax_function();

/////////       КАМЕРА   
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];                                // для отслеживания, какие клавиши у нас нажаты
bool firstMouse = true;
float lastTime = 0.0f;                          // время вывода последнего кадра
float deltaTime = 0.0f;                         // время, прошедшее между последним и текущим кадром



/////////       КОНСТАНТЫ        
glm::vec3 lightPos(-3.0f, 1.0f, 5.0f);          // положение источника света 
const GLuint WIDTH = 800, HEIGHT = 600;         // размеры окна
float heightScale = 0.1;                        // "глубина" рельефа для эффекта parallax mapping
float lastX = WIDTH / 2.0;                      // last координаты указателя мышки
float lastY = HEIGHT / 2.0;





///////////////////////////////////////////////////////                   MAIN                  ///////////////////////////////////////////////////////

int main(void)
{
    // настраиваем GLFW
    if (!glfwInit()) {
        std::cout << "ERROR WITH GLFW \n" << std::endl;
        return -1;
    }

    // устанавливаем все необходимые параметры для GLFW 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // создаем объект GLFW 
    GLFWwindow* window;
    
    // открыть окно и создать в нем контекст OpenGL
    window = glfwCreateWindow(WIDTH, HEIGHT, "Grafichs_openGL", NULL, NULL);
    if (!window)
    {
        std::cout << "ERROR WITH OPEN THE WINDOW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 
    glfwSetKeyCallback(window, key_callback);           // проверка на вызов обратных функций
    
    // захват курсора и скрытие указателя мышки
    glfwSetCursorPosCallback(window, mouse_moving);
    glfwSetScrollCallback(window, scroll_moving);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "FAILED TO INITIALIZE GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);                        // вкл. тест глубины
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);                      // вкл. трафаретное тестирование  
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);            // смотрим, если входящее значение != 1, то мимо
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 

    // для прозрачных окон
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    ////////////////////////////////////////////////////                ШЕЙДЕРЫ                     //////////////////////////////////////////////////

    Shader lamp_Shader("lamp_v_shader.vs", "lamp_f_shader.fs");                 // для лампы
    Shader light_cube_Shader("cube_v_shader.vs", "cube_f_shader.fs");           // для кубиков
    Shader contour_Shader("contour_v_shader.vs", "contour_color.fs");           // для обводки
    Shader window_Shader("contour_v_shader.vs", "contour_f_shader.fs");         // для окон
    Shader skybox_Shader("skybox_v_shader.vs", "skybox_f_shader.fs");           // для skybox
    Shader mirror_cube_Shader("mirror_v_shader.vs", "mirror_f_shader.fs");      // для зеркального куба
    Shader normall_Shader("real_cube_v_shader.vs", "real_cube_f_shader.fs");    // normall mapping
    Shader parallax_Shader("parallax_v_shader.vs", "parallax_f_shader.fs");     // parallax relief mapping


    ////////////////////////////////////////////////////            КООРДИНАТЫ ВЕРШИН               //////////////////////////////////////////////////

    //      для обычных кубиков
    float cube_vertices[] = {
    // позиции            // нормали           // текстуры
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

    // для зеркального куба
    float mirror_cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    //      для пола
    float floor_vertices[] = {
        // позиции            // нормали           // текстуры (>1, чтобы повторялась текстура пола)
         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  1.0f,

         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  1.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   1.0f,  1.0f
    };

    //      для окон
    float window_vertices[] = {
        // координаты         // текстурные координаты (y-координаты поменялись местами, потому что текстура перевернута вверх ногами)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    }; 

    //      для skybox
    float skybox_vertices[] = {
        // позиции          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


    // умножим наш куб на 10 
    /*glm::vec3 cubePositions[] = {
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
    };*/




    ////////////////////////////////////////////////////                    VAO и VBO                    ///////////////////////////////////////////
    
    //      для кубиков         ////////////////////////

    GLuint  cube_VBO, cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);
    
    glBindVertexArray(cube_VAO);
    // связываем объект массива, устанавливаем буфера и аттрибуты вершин  
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    
    // Позиция атрибута
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
 
    // Цвет атрибута
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Текстура
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    

    //      для пола             ///////////////////////
    GLuint  floor_VBO, floor_VAO;
    glGenVertexArrays(1, &floor_VAO);
    glGenBuffers(1, &floor_VBO);

    glBindVertexArray(floor_VAO);
    // связываем объект массива, устанавливаем буфера и аттрибуты вершин  
    glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), &floor_vertices, GL_STATIC_DRAW);
    
    // Позиция атрибута
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Цвет атрибута
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));   
    glEnableVertexAttribArray(1);
    
    // Текстура
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    //      для лампы               ////////////////////
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Так как VBO объекта-контейнера уже содержит все необходимые данные, 
    // то нам нужно только связать с ним новый VAO
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    
    // Настраиваем атрибуты (нашей лампе понадобятся только координаты вершин)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); 

    //      для окон                ////////////////////
    unsigned int window_VAO, window_VBO;
    glGenVertexArrays(1, &window_VAO);
    glGenBuffers(1, &window_VBO);
    glBindVertexArray(window_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, window_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(window_vertices), window_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0); 

    //      для зеркального кубика      ////////////////////
    unsigned int  mirror_cube_VBO, mirror_cube_VAO;
    glGenVertexArrays(1, &mirror_cube_VAO);
    glGenBuffers(1, &mirror_cube_VBO);

    glBindVertexArray(mirror_cube_VAO);
    // связываем объект массива, устанавливаем буфера и аттрибуты вершин  
    glBindBuffer(GL_ARRAY_BUFFER, mirror_cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirror_cube_vertices), &mirror_cube_vertices, GL_STATIC_DRAW);

    // Позиция атрибута
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    // Цвет атрибута
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);



    //        VAO и VBO skybox          ///////////////////

    unsigned int skybox_VBO, skybox_VAO;
    glGenVertexArrays(1, &skybox_VAO);
    glGenBuffers(1, &skybox_VBO);
    glBindVertexArray(skybox_VAO);

    // связываем объект массива, устанавливаем буфера и аттрибуты вершин  
    glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);

    // Позиция атрибута
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);



    //          грани куба skybox               //////////////////////

    std::vector<std::string> faces
    {
            "forest_right.jpg",
            "forest_left.jpg",
            "forest_top.jpg",
            "forest_bottom.jpg",
            "forest_front.jpg",
            "forest_back.jpg"
    }; 


    //          VECROR  ОКНО                    ////////////////////////

    std::vector<glm::vec3> windows
    {
            //glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3(1.5f, 0.0f, 0.51f),
            glm::vec3(0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3(0.5f, 0.0f, -0.6f)
    };


    /////////////////////////////////////////////////////                   ТЕКСТУРЫ                ///////////////////////////////////////////////////////      


    unsigned int texture_cube = loadTexture("container2.png");                  //  первый куб
    unsigned int texture_floor = loadTexture("mramor.jpg");                     //  пол
    unsigned int specular_Map = loadTexture("container2_specular.png");         //  для отражения материалов
    unsigned int texture_window = loadTexture("kkk.png");                       //  полупрозрачное стекло
    unsigned int skybox_texture = loadCubemap(faces);                           //  загружаем skybox

    unsigned int texture_real_cube = loadTexture("vio_d.jpg");                  //  текстура кирпичной стены
    unsigned int texture_normal_cube = loadTexture("vio_norm.jpg");             //  текстура нормалей 

    unsigned int texture_parallax_diffuse = loadTexture("toy_diffuse.png");     //  тестура игрушки
    unsigned int texture_parallax_normal = loadTexture("toy_normal.png");       //  текстура нормалей
    unsigned int texture_parallax_height = loadTexture("toy_disp.png");         //  текстура глубины
    
    light_cube_Shader.Use();  
    light_cube_Shader.setInt("material.diffuse", 0);       
    light_cube_Shader.setInt("material.specular", 1);

    skybox_Shader.Use();
    skybox_Shader.setInt("skybox", 0);

    mirror_cube_Shader.Use();
    mirror_cube_Shader.setInt("skybox", 0);

    normall_Shader.Use();
    normall_Shader.setInt("texture_real_cube", 0);
    normall_Shader.setInt("texture_normal_cube", 1);

    parallax_Shader.Use();
    parallax_Shader.setInt("texture_parallax_diffuse", 0);
    parallax_Shader.setInt("texture_parallax_normal", 1);
    parallax_Shader.setInt("texture_parallax_height", 2);

        

    //////////////////////////////////////////////////////                ИГРОВОЙ ЦИКЛ              /////////////////////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;
 
        control_keyboard(window);       // управление клавишами
        glfwPollEvents();               // проверка на события

        control_camera();               // управление камерой


        //      делаем сортировку для наших окон (чтобы сквозь было видно других)
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // очищаем буфер цвета, глубины и трафаретный буфер (грязь от прошлого кадра)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection; 

        ///////////////////////////////////////////////////                     ЭФФЕКТЫ                ///////////////////////////////////////////////////////////



        //      эффект parallax relief mapping      ///////////////////                                         

        // настраиваем матрицы вида и проекции
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        parallax_Shader.Use();
        parallax_Shader.setMat4("projection", projection);
        parallax_Shader.setMat4("view", view);

        // настраиваем четырехугольник
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 1.0f, -3.5f)); 
        parallax_Shader.setMat4("model", model);
        parallax_Shader.setVec3("viewPos", camera.Position);
        parallax_Shader.setVec3("lightPos", lightPos);
        parallax_Shader.setFloat("heightScale", heightScale); 

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_parallax_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_parallax_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_parallax_height);
        parallax_function();

        // настраиваем свет
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        parallax_Shader.setMat4("model", model);
        parallax_function();
        
        
        //      эффект normal mapping           /////////////////////////
        //   делаем матрицу вида и проекции                 
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        normall_Shader.Use();
        normall_Shader.setMat4("projection", projection);
        normall_Shader.setMat4("view", view);
        
        // настраиваем четырехугольник
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 2.0f, 2.0f)); 
        normall_Shader.setMat4("model", model);
        normall_Shader.setVec3("viewPos", camera.Position);
        normall_Shader.setVec3("lightPos", lightPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_real_cube);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_normal_cube);
        normal_function();


        // настраиваем свет
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        normall_Shader.setMat4("model", model);
        normal_function();

        

        //       настройка обводки               /////////////////////////
        contour_Shader.Use();
        // настраиваем модель, матрицу вида и проекции
        model = glm::mat4(1.0f);                            
        view = camera.GetViewMatrix();                         
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);   
        
        contour_Shader.setMat4("projection", projection);
        contour_Shader.setMat4("view", view);

        //          кубики                      /////////////////////////
        light_cube_Shader.Use();
        light_cube_Shader.setMat4("projection", projection);
        light_cube_Shader.setMat4("view", view);
        light_cube_Shader.setVec3("light.position", lightPos);
        light_cube_Shader.setVec3("viewPos", camera.Position);
        
        
        
    //////////////////////////////////////////////////                      ИСТОЧНИК СВЕТА                    ///////////////////////////////////////////////////

        light_cube_Shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        light_cube_Shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        light_cube_Shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        light_cube_Shader.setFloat("light.constant", 1.0f);
        light_cube_Shader.setFloat("light.linear", 0.09f);
        light_cube_Shader.setFloat("light.quadratic", 0.032f);

        light_cube_Shader.setFloat("material.shininess", 64.0f);


        /////////////////            ПРОЖЕКТОР
        /*light_cube_Shader.setVec3("light.direction", camera.Front);
        light_cube_Shader.setVec3("light.position", camera.Position);
        
        // задаем постоянный, линейный и квадратичный коэффициенты
        light_cube_Shader.setFloat("light.constant", 1.0f);
        light_cube_Shader.setFloat("light.linear", 0.09f);
        light_cube_Shader.setFloat("light.quadratic", 0.032f);

        // для прожектора
        light_cube_Shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));     // передаем cos (вычислительные ресурсы)
        light_cube_Shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.0f)));*/

        
     ///////////////////////////////////////////////////                    ОБВОДКА                     /////////////////////////////////////////////////////  

        glStencilMask(0x00);                        // каждый бит становится 0 в трафаретном буфере
        
        // настраиваем пол
        glBindVertexArray(floor_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_floor);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_floor);
        light_cube_Shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // тест трафарета
        glStencilFunc(GL_ALWAYS, 1, 0xFF);          // каждый бит обновит трафаретный буфер
        glStencilMask(0xFF);                        // запись в трафаретный буфер

        // настраиваем для первого куба
        glBindVertexArray(cube_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_cube); 
        glActiveTexture(GL_TEXTURE1);                           
        glBindTexture(GL_TEXTURE_2D, specular_Map);                 

        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f)); 
        light_cube_Shader.setMat4("model", model);                    
        glDrawArrays(GL_TRIANGLES, 0, 36); 

        // настраиваем для второго куба
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        light_cube_Shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);        // мы рисуем только те части объектов, которые не равны единице
        glStencilMask(0x00);                        // выкл. запись в трафаретный буфер
        glDisable(GL_DEPTH_TEST);                   // выкл. тест глубины (чтобы поло не было перезаписано)
        contour_Shader.Use();
        float scale = 1.1;
        
        //      рисуем кубы
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        contour_Shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        contour_Shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);


        // много кубов!!! :3                
         /*for (GLuint i = 0; i < 10; i++)
         {
             glm::mat4 model = glm::mat4(1.0f);
             model = glm::translate(model, cubePositions[i]);
             float angle = 20.0f * i;
             model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
             light_cube_Shader.setMat4("model", model);

             glDrawArrays(GL_TRIANGLES, 0, 36);
         }*/

        //////////////////////////////////////////                         НАСТРОЙКА ЛАМПЫ                     ////////////////////////////////////////
        lamp_Shader.Use();
        lamp_Shader.setMat4("projection", projection);
        lamp_Shader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));     // размер

        lamp_Shader.setMat4("model", model);

        // рисование куба лампы 
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0); 


        //      рисуем зеркальный куб       /////////////////////////
        mirror_cube_Shader.Use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-10.0f, 3.0f, -2.0f));
        view = camera.GetViewMatrix();
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        mirror_cube_Shader.setMat4("model", model);
        mirror_cube_Shader.setMat4("view", view);
        mirror_cube_Shader.setMat4("projection", projection);
        mirror_cube_Shader.setVec3("cameraPos", camera.Position);

        glBindVertexArray(mirror_cube_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0); 



        ///////////////////////////////////////////                         ЗАГРУЗКА SKYBOX                    ////////////////////////////////////////////////
          
        glDepthFunc(GL_LEQUAL);
        skybox_Shader.Use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skybox_Shader.setMat4("projection", projection);
        skybox_Shader.setMat4("view", view);

        glBindVertexArray(skybox_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        ///////////////////////////////////////                      ПОЛУПРОЗРАЧНЫЕ ОКНА                    ///////////////////////////////////////////////////

        window_Shader.Use();
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        window_Shader.setMat4("view", view);
        window_Shader.setMat4("projection", projection);
        glBindVertexArray(window_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_window);
        model = glm::mat4(1.0f);
        
        //          вывод
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            window_Shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Меняем местами буфера экрана
        glfwSwapBuffers(window);
    }
    // удаляем все ресурсы после завершения их работы
    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteVertexArrays(1, &mirror_cube_VAO);
    glDeleteVertexArrays(1, &floor_VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &window_VAO);
    glDeleteVertexArrays(1, &skybox_VAO);
    glDeleteBuffers(1, &cube_VBO);
    glDeleteBuffers(1, &window_VBO);
    glDeleteBuffers(1, &mirror_cube_VBO);
    glDeleteBuffers(1, &floor_VBO);
    glDeleteBuffers(1, &skybox_VBO);
    //glDeleteBuffers(1, &EBO);
    
    // окончание работы GLFW, очищаем выделенные ресурсы
    glfwTerminate();
    return 0;
}

// вызывается, когда нажата отпущена клавиша GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // esc
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // смотрим нажатые и отпущенные клавиши
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

void control_camera() {
    float cameraSpeed = 0.5 * deltaTime;
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

// вызывается при каждом перемещении мышки
void mouse_moving(GLFWwindow* window, double xpos, double ypos) {  
    if (firstMouse)                             // xpos,ypos - текущие координаты
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;   // обратный порядок вычитания потому что оконные Y-координаты возрастают сверху вниз 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// для колесика мышки
void scroll_moving(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// для управления клавиатурой
void control_keyboard(GLFWwindow* window)
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
    }
}

// функция загрузки текстуры
unsigned int loadTexture(char const* path)
{
    unsigned int my_texture;
    glGenTextures(1, &my_texture);

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

        glBindTexture(GL_TEXTURE_2D, my_texture);
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
        std::cout << "FAILED TO LOAD TEXTURE: " << path << std::endl;
        stbi_image_free(data);
    }

    return my_texture;
}

// загрузка текстур для skybox 
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int current_texture;
    glGenTextures(1, &current_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, current_texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "ERROR TO LOAD TEXTURE FOR SKYBOX:  " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return current_texture;
}

unsigned int rect_VAO = 0;
unsigned int rect_VBO;

// реализация эффекта normal mapping
void normal_function()
{
    if (rect_VAO == 0)
    {
        // координаты вершин
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        
        // координаты текстуры
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        
        // вектор нормали
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        
        //          ПЕРВЫЙ ТРЕУГОЛЬНИК       ////////////////
        // ищем вектора, описывающие грани 
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        // ищем касательную и бикасательную по формалам
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        //          ВТОРОЙ ТРЕУГОЛЬНИК       ////////////////
        // аналогично, что с первым
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        float rect_vertices[] = {
            // позиции              // нормали        // текстуры   // касательная                      // бикасательная
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };

        // настраиваем наш прямоугольник
        glGenVertexArrays(1, &rect_VAO);
        glGenBuffers(1, &rect_VBO);
        glBindVertexArray(rect_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, rect_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), &rect_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(rect_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
} 

// реализация parallax relief mapping
void parallax_function()
{
    if (rect_VAO == 0)
    {
        // координаты
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        
        // текстурные координаты
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        
        // векторы нормалей
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // вычисляем касательные и бикасательные наших треугольников
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        
        //          ПЕРВЫЙ ТРЕУГОЛЬНИК       ////////////////
        // ищем вектора, описывающие грани
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        //          ВТОРОЙ ТРЕУГОЛЬНИК       ////////////////
        // аналогично, что с первым
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float rect_vertices[] = {
            // координаты           // нормали        // текстуры   // касательные                      // бикасательные
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };

        // настраиваем наш прямоугольник
        glGenVertexArrays(1, &rect_VAO);
        glGenBuffers(1, &rect_VBO);
        glBindVertexArray(rect_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, rect_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), &rect_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(rect_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}