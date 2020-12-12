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

#include <map>
#include <vector>

///////     ПРОТОТИПЫ
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void processInput(GLFWwindow* window);
void mouse_moving(GLFWwindow* window, double xpos, double ypos);
void update_camera();
void scroll_moving(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);

// положение источника света
glm::vec3 lightPos(1.0f, 5.0f, 5.0f);
// размеры окна
const GLuint WIDTH = 800, HEIGHT = 600;

///////     КАМЕРА   
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];                    // для отслеживания, какие клавиши у нас нажаты
bool firstMouse = true;
GLfloat lastTime = 0.0f;            // время вывода последнего кадра
GLfloat deltaTime = 0.0f;           // время, прошедшее между последним и текущим кадром

GLfloat lastX = WIDTH / 2.0;        // last координаты указателя мышки
GLfloat lastY = HEIGHT / 2.0;



///////////////////////////////////////////////////////                   MAIN        ////////////////////////////////////////
int main(void)
{
    // настраиваем GLFW
    if (!glfwInit()) {
        std::cout << "ERROR WITH GLFW \n" << std::endl;
        return -1;
    }
    // устанавливаем все необходимые параметры для GLFW для версии 3.3
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
    
    // проверка на вызов обратных функций
    glfwSetKeyCallback(window, key_callback);
    
    // захват курсора и скрытие указателя мышки
    glfwSetCursorPosCallback(window, mouse_moving);
    glfwSetScrollCallback(window, scroll_moving);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // инициализация GLAD, чтобы вызывать какие-либо функции из openGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "FAILED TO INITIALIZE GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // трафаретное тестирование
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    // смотрим, если входящее значение != 1, то мимо
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ////////////////////////////////////////////////////            ШЕЙДЕРЫ         //////////////////////////////////////////////////

    Shader lamp_Shader("lamp_v_shader.vs", "lamp_f_shader.fs");       // для лампы
    Shader light_cube_Shader("cube_v_shader.vs", "cube_f_shader.fs");         // для кубиков
    Shader shaderSingleColor("contour_v_shader.vs", "contour_color.fs");         // для обводки
    Shader window_Shader("contour_v_shader.vs", "fragment_shader_1.vs");
    Shader skybox_Shader("skybox_v_shader.vs", "skybox_f_shader.fs");



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


    float floor_vertices[] = {
        // позиции            // нормали           // текстуры (>1, чтобы повторялась текстура пола)
         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  1.0f,

         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  1.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   1.0f,  1.0f
    };

    float transparentVertices[] = {
        // координаты         // текстурные координаты (y-координаты поменялись местами, потому что текстура перевернута вверх ногами)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    }; 

    float skybox_vertices[] = {
        // positions          
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

    // получим позицию камеры  !!! этот вектор фактически указывает в сторону, противоположную направлению камеры.
    //glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);
    // камера нацелена в базовую точку сцены
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // направление камеры
    //glm::vec3 cameraDirection = glm::vec3(cameraPosition - cameraTarget);
    //  вектор указывающий направление вверх 
    //glm::vec3 upvec = glm::vec3(0.0f, 1.0f, 0.0f);
    // найдем векторное произведение и получим вектор перпендикулярный  - положительное направление оси Х
    //glm::vec3 cameraRight = glm::normalize(glm::cross(upvec, cameraDirection));
    // ось камеры вверх
    //glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));



    ////////////////////////////////////////////               VAO и VBO для кубиков         //////////////////////////////////////////

    GLuint  cube_VBO, cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);
    //glGenBuffers(1, &EBO);
    glBindVertexArray(cube_VAO);
    // связываем объект массива, устанавливаем буфера и аттрибуты вершин  
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);

    // прямоугольник
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Позиция атрибута
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
 
    // Цвет атрибута
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Текстура
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
   // glBindVertexArray(0);
    

    /////////////////////////////////////////////                   VAO и VBO для пола          //////////////////////////////////////
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

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));   
    glEnableVertexAttribArray(1);
    // Текстура
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);


    ////////////////////////////////////////////                VAO для лампы               //////////////////////////////////////////
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Так как VBO объекта-контейнера уже содержит все необходимые данные, 
    // то нам нужно только связать с ним новый VAO
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    
    // Настраиваем атрибуты (нашей лампе понадобятся только координаты вершин)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    //glBindVertexArray(0); 

    //////////////////////////////////////////////                  VAO прозрачности            ////////////////////////////////////
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0); 

    ///////////////////////////////////////////////                 VAO и VBO skybox            /////////////////////////////////////

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



    /////////////////////////////////////////////////////           грани куба skybox       ////////////////////////

    std::vector<std::string> faces
    {
            "forest_right.jpg",
            "forest_left.jpg",
            "forest_top.jpg",
            "forest_bottom.jpg",
            "forest_front.jpg",
            "forest_back.jpg"
    };


    /////////////////////////////////////////////////////            ТЕКСТУРЫ       /////////////////////////////////       


    unsigned int texture_cube = loadTexture("container2.png");                  //  куб
    unsigned int texture_floor = loadTexture("mramor.jpg");                     //  пол
    unsigned int specular_Map = loadTexture("container2_specular.png");         //  для отражения материалов
    unsigned int transparentTexture = loadTexture("transparent_window.png");
    unsigned int skybox_texture = loadCubemap(faces);   // загружаем skybox
    
    
    light_cube_Shader.Use();
    //light_cube_Shader.setInt("texture1", 0);  
    light_cube_Shader.setInt("material.diffuse", 0);       //1
    light_cube_Shader.setInt("material.specular", 1);

    skybox_Shader.Use();
    skybox_Shader.setInt("skybox", 0);
        
    /////////////////////////////////////////////////////         VECROR  ОКНО            ////////////////////////////////// 

    std::vector<glm::vec3> windows
    {
            glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3(1.5f, 0.0f, 0.51f),
            glm::vec3(0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3(0.5f, 0.0f, -0.6f)
    }; 
    


    ////////////////////////////////////////////////////                ИГРОВОЙ ЦИКЛ        ////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // проверка на события (мышь, нажатие клавиши)
        processInput(window);
        glfwPollEvents();

        update_camera(); 

        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }

        // очищаем буфер цвета
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // очищаем буфер глубины (грязь от прошлого кадра)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        

        /////////////////////       обводка
        shaderSingleColor.Use();
        model = glm::mat4(1.0f);                            // настраиваем модель
        view = camera.GetViewMatrix();                      // вызываем из класса матрицу LookAT     
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);   // матрица проекции (используем перспективную проекцию)
        
        shaderSingleColor.setMat4("projection", projection);
        shaderSingleColor.setMat4("view", view);

        ///////////////     кубики
        light_cube_Shader.Use();
        light_cube_Shader.setMat4("projection", projection);
        light_cube_Shader.setMat4("view", view);
        

        // освещение
        light_cube_Shader.setVec3("light.position", lightPos);
        light_cube_Shader.setVec3("viewPos", camera.Position);
        
        
        /*glm::vec3 lightColor;
        /*lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        lightColor.x = 2.0f;
        lightColor.y = 0.7f;
        lightColor.z = 1.3f;
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);                  // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);                // low influence
        light_cube_Shader.setVec3("light.ambient", ambientColor);
        light_cube_Shader.setVec3("light.diffuse", diffuseColor);*/
        
    //////////////////////////////////////////////////            НАПРАВЛЕННЫЙ СВЕТ            /////////////////////////////////

        light_cube_Shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        light_cube_Shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        light_cube_Shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // light_cube_Shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        light_cube_Shader.setFloat("light.constant", 1.0f);
        light_cube_Shader.setFloat("light.linear", 0.09f);
        light_cube_Shader.setFloat("light.quadratic", 0.032f);

        light_cube_Shader.setFloat("material.shininess", 64.0f);

        
        //light_cube_Shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        //light_cube_Shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        //light_cube_Shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);

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


      

        // Трансформации
        // для модельной матрицы (преобразовываем все вершины в глобальное мировое пространство)
        //glm::mat4 model;
        // второй аругмент: вращаем
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 50.0f, glm::vec3(1.0f, 0.5f, 0.0f));

        // для матрицы вида (сдвигаем сцену назад)
        /*glm::mat4 view;
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); */


        

        
     ///////////////////////////////////////////////////            ОБВОДКА         //////////////////////////////////////////////   
        glStencilMask(0x00);
        
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
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        // настраиваем для первого куба
        glBindVertexArray(cube_VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_cube); 
        glActiveTexture(GL_TEXTURE1);                           ///////////////////
        glBindTexture(GL_TEXTURE_2D, specular_Map);                 /////////////////////

        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f)); ///////////////////
        light_cube_Shader.setMat4("model", model);                    //////////////////
        glDrawArrays(GL_TRIANGLES, 0, 36); 

        // настраиваем для второго куба
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        light_cube_Shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        shaderSingleColor.Use();
        float scale = 1.1;
        
        // строим кубы
        //glBindVertexArray(cube_VAO);
        //glBindTexture(GL_TEXTURE_2D, texture_cube);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        //glBindVertexArray(cubeVAO);

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

        //////////////////////////////          НАСТРОЙКА ЛАМПЫ             ////////////////////////////////
        lamp_Shader.Use();

        lamp_Shader.setMat4("projection", projection);
        lamp_Shader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

        lamp_Shader.setMat4("model", model);

        // рисование куба лампы 
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0); 


        ////////////////////////////                загружаем skybox            ///////////////////////////////
        
        
        glDepthFunc(GL_LEQUAL);
        skybox_Shader.Use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        // матрица проекции (используем перспективную проекцию)
        //projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        skybox_Shader.setMat4("projection", projection);
        skybox_Shader.setMat4("view", view);
        glBindVertexArray(skybox_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        //////////////////////////////         ПОЛУПРОЗРАЧНЫЕ ОКНА     ///////////////////////////////////

        window_Shader.Use();
        projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        window_Shader.setMat4("view", view);
        window_Shader.setMat4("projection", projection);
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        model = glm::mat4(1.0f);
        
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
    glDeleteVertexArrays(1, &floor_VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &cube_VBO);
    glDeleteBuffers(1, &floor_VBO);
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

    // смотрим нажатые и отпущенные камеры
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

// вызывается при каждом перемещении мышки
void mouse_moving(GLFWwindow* window, double xpos, double ypos) { // xpos,ypos - текущие координаты 
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // обратный порядок вычитания потому что оконные Y-координаты возрастают с верху вниз 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// для колесика мышки
void scroll_moving(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// для управления wasd
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

// функция загрузки текстуры
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

// загрузка skybox 
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

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
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}