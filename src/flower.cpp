#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <unordered_map>

#define _USE_MATH_DEFINES
#include <cmath>

#include <ShaderProgram.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void addPoint1(float* vertices, int i);
void addPoint2(float* vertices, int i);
float changeTime1(int mode);
float changeTime2(int mode);
bool isTransition(int mode[2]);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const unsigned int NUM_SEGS = 1000;
const double UPPER = 2 * M_PI;
const float COLORS[] = {255.0f, 192.0f, 203.0f,
                       255.0f, 0.0f, 255.0f,
                       230.0f, 230.0f, 250.0f,
                       147.0f, 112.0f, 219.0f,
                       30.0f, 144.0f, 255.0f,
                       255.0f, 255.0f, 0.0f};
int currColor = 0;
const int NUM_COLORS = 3;
float getColorComp(int color, int index);
const char* VERTEX_PATH = "src/shaders/vertex.vs";
const char* FRAG_PATH = "src/shaders/fragment.fs";
int oldMode[2] = {0, 0};
int mode[2] = {0, 0};
int newMode[2] = {0, 0};
std::unordered_map<int, bool> pressed;

int main(int argc, char** argv)
{
    if(argc > 1) {
        currColor = std::__cxx11::stoi(argv[1]) * 2;
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Blooming", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ShaderProgram shader(VERTEX_PATH, FRAG_PATH);

    float flower1[NUM_SEGS * 5] = {};
    float flower2[NUM_SEGS * 5] = {};

    for(int i = 0; i < NUM_SEGS; i++) {
        addPoint1(flower1, i);
        addPoint2(flower2, i);
    }
    
    unsigned int VAO1, VAO2;
    unsigned int VBO1, VBO2;

    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flower1), flower1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flower2), flower2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    double time1;
    double time2;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        /*
        * mode -2 = reset to blank
        * mode -1 = match to new pattern
        */

        if (mode[0] != -2 && mode[0] != -1) {
            time1 = changeTime1(    mode[0]);
        } else if (mode[0] == -2) {
            if(fabs(time1) < 0.01) {
                mode[0] = -1;
            } else {
                time1 = changeTime1(oldMode[0]);
            }
        } else {
            if(fabs(changeTime1(newMode[0])) < 0.01) {
                mode[0] = newMode[0];
                time1 = changeTime1(mode[0]);
            }
        }

        if(mode[1] != -1 && mode[1] != -2) {
            time2 = changeTime2(mode[1]);
        } else if(mode[1] == -2) {
            if(fabs(time2) < 0.01) {
                mode[1] = -1;
            } else {
                time2 = changeTime2(oldMode[1]);
            }
        } else {
            if(fabs(changeTime2(newMode[1])) < 0.01) {
                mode[1] = newMode[1];
                time2 = changeTime2(mode[1]);
            }
        }

        shader.setFloat("timeFactor", time1);

        for(int i = 0; i < 3; i++) {
            shader.setFloat("color" + std::to_string(i+1), getColorComp(currColor*2, i));
        }

        glBindVertexArray(VAO1);
        glDrawArrays(GL_LINE_LOOP, 0, NUM_SEGS);

        shader.setFloat("timeFactor", time2);

        for(int i = 0; i < 3; i++) {
            shader.setFloat("color" + std::to_string(i+1), getColorComp(currColor*2+1, i));
        }

        glBindVertexArray(VAO2);
        glDrawArrays(GL_LINE_LOOP, 0, NUM_SEGS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !isTransition(mode)) {
        oldMode[0] = mode[0];
        mode[0] = -2;

        oldMode[1] = mode[1];
        mode[1] = -2;

        newMode[0] = 0;
        newMode[1] = 0;
    } if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !isTransition(mode)) {
        oldMode[0] = mode[0];
        mode[0] = -2;

        oldMode[1] = mode[1];
        mode[1] = -2;

        newMode[0] = 1;
        newMode[1] = 1;
    } if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !isTransition(mode)) {
        oldMode[0] = mode[0];
        mode[0] = -2;

        oldMode[1] = mode[1];
        mode[1] = -2;

        newMode[0] = 2;
        newMode[1] = 2;
    } if(glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && (pressed.find(GLFW_KEY_GRAVE_ACCENT) == pressed.end() || !(pressed.find(GLFW_KEY_GRAVE_ACCENT)->second))) {
        currColor = (currColor + 1) % NUM_COLORS;
        pressed.insert_or_assign(GLFW_KEY_GRAVE_ACCENT, true);
    } if(glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) != GLFW_PRESS) {
        pressed.insert_or_assign(GLFW_KEY_GRAVE_ACCENT, false);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void addPoint1(float* vertices, int i) {
    //vertices[2*i] = cos(i * UPPER / NUM_SEGS);
    //vertices[2*i + 1] = sin(i * UPPER / NUM_SEGS);
    vertices[5*i] = cos(7 * i * UPPER / NUM_SEGS) * cos(i * UPPER / NUM_SEGS);
    vertices[5*i+1] = cos(7 * i * UPPER / NUM_SEGS) * sin(i * UPPER / NUM_SEGS);

    vertices[5*i+2] = COLORS[6*currColor] / 255.0f;
    vertices[5*i+3] = COLORS[6*currColor + 1] / 255.0f;
    vertices[5*i+4] = COLORS[6*currColor + 2] / 255.0f;
}

void addPoint2(float* vertices, int i) {
    vertices[5*i] = -cos(7 * i * UPPER / NUM_SEGS) * cos(i * UPPER / NUM_SEGS);
    vertices[5*i+1] = -cos(7 * i * UPPER / NUM_SEGS) * sin(i * UPPER / NUM_SEGS);

    vertices[5*i+2] = COLORS[6*currColor + 3] / 255.0f;
    vertices[5*i+3] = COLORS[6*currColor + 4] / 255.0f;
    vertices[5*i+4] = COLORS[6*currColor + 5] / 255.0f;
}

float changeTime1(int mode) {
    switch(mode) {
        case 0:
            return sin(glfwGetTime());
        case 1:
            return sin(glfwGetTime());
        case 2:
            return sin(glfwGetTime()) < 0 ? -sin(glfwGetTime()) : sin(glfwGetTime());
        default:
            return 0;
    }
}

float changeTime2(int mode) {
    switch(mode) {
        case 0:
            return sin(glfwGetTime());
        case 1:
            return cos(glfwGetTime());
        case 2:
            return cos(glfwGetTime()) < 0 ? -cos(glfwGetTime()) : cos(glfwGetTime());
        default:
            return 0;
    }
}

float getColorComp(int color, int index) {
    switch(index) {
        case 0:
            return COLORS[3*color] / 255.0f;
        case 1:
            return COLORS[3*color+1] / 255.0f;
        case 2:
            return COLORS[3*color+2] / 255.0f;
        default:
            throw std::invalid_argument("value is not between 0 and 2");
            return 0;
    }
}

bool isTransition(int mode[2]) {
    return mode[0] == -1 || mode[1] == -1 || mode[0] == -2 || mode[1] == -2;
}