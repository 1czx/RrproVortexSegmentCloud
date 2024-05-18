#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "include/Myshader.h"
#include "include/lines.h"
#include "include/camera.h"
#include "include/particles.h"

#include "include/VortexSegmentCloud.h"

#include <vector>
#include <iostream>
// #include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// // settings
// const unsigned int SCR_WIDTH = 2560;
// const unsigned int SCR_HEIGHT = 1600;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // above: inits
    // glEnable(GL_DEPTH_TEST);


    // say: Coordinate, no use anyway
    line_coord line_temp;
    line_temp.init();

    DrawParticle draw_p;


    std::vector<glm::vec4> tracers;
    std::vector<glm::vec4> segms;
    VortexSegmentCloud2D vcloud(1.6, 1);
    // int n_tracer = 1000;                                    // number of tracers = 5000
    Vector2d dft_set(0.0, 0.0);
    Vector3d dft_c(1.0, 0.0, 0.0);
    // for(int i=0; i<n_tracer; i++) {
    //     glm::vec4 new_tracer = glm::vec4(0.0, 0.0, 0.0, 1.0);
    //     Vector2d new_vec(random_double(-0.5, -0.3), random_double(-0.1, 0.1));      // random position
    //     // Vector2d new_vec(random_double(-0.1, 0.1), random_double(-0.1, 0.1));      // random position
    //     new_tracer.x = new_vec(0);
    //     new_tracer.y = new_vec(1);
    //     VortexSegment2D seg(new_vec, dft_set, dft_c, 0);
    //     // vcloud.addTracer(seg);
    //     // vcloud.addSegment(seg);
    // }
    
    // boundary???
    vector<Vector2d> bd;
    vector<Vector2d> bd_seg;
    vector<glm::vec4> cir;
    int n = 10;
    int nb = 1440;

    int na = 30;
    double radi = 0.08;
    for(int i=0; i<n; i++) {
        Vector2d new_pt( radi * cos(3.14159 * 2 / n * i)-1.6, radi * sin(3.14159 * 2 / n * i));
        bd.push_back(new_pt);
    }
    for(int i=0; i<nb; i++) {
        Vector2d new_pt( radi * cos(3.14159 * 2 / n * i)-1.6, radi * sin(3.14159 * 2 / n * i));
        cir.push_back(glm::vec4(new_pt(0), new_pt(1), 0, 1));
    }
    
    radi += 0.01;
    // radi += 0.001;
    for(int i=0; i<na; i++) {
        if(1){ Vector2d new_pt( radi * cos(3.14159 * 2 / na * i+3.14159/4)-1.6, radi * sin(3.14159 * 2 / na * i+3.14159/4));
        bd_seg.push_back(new_pt);
    }}
    vcloud.setBoundary(bd, bd_seg);
    vcloud.set_back_vel(8.0, 0.0);


    vector<glm::vec4> bds;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        // 30 fps???
        if (deltaTime < 0.0333) {
            continue;
        }
        lastFrame = currentFrame;


        // process the inputs mouse / keyboard ---> update camera
        processInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // background color as black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // projection and view (move of the camera)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        line_temp.draw(projection, view);


        // vcloud.oneStepTemporalEvolution(deltaTime);
        // vcloud.addTracer(tra);
        vcloud.oneStepTemporalEvolution(0.01,2);
        vcloud.get_tracer(tracers);
        vcloud.get_seg(segms);
        // printf("%d\n", segms.size());
        // vcloud.get_bd(bds);


        // to draw: pass the vector of points. projection / view are given by camera
        draw_p.draw(tracers, projection, view, 1.0, 0.0, 0.0);
        draw_p.draw(segms, projection, view, 1.0, 1.0, 0.0);
        // draw_p.draw(bds, projection, view, 0.0, 1.0, 0.0);

        draw_p.draw(cir, projection, view, 0.0, 1.0, 0.0);
        printf("actual fps: 1/%f\t= %f\n", deltaTime, 1.0/deltaTime);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // gldelete....

    glfwTerminate();
    return 0;
}










////////////////////////////////////////////////////////////////////
// process callback
// process the inputs
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)          // exit
        glfwSetWindowShouldClose(window, true);

    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)               // w
    //     camera.ProcessKeyboard(FORWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)               // s
    //     camera.ProcessKeyboard(BACKWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)               // a
    //     camera.ProcessKeyboard(LEFT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)               // d
    //     camera.ProcessKeyboard(RIGHT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)           // space
    //     camera.ProcessKeyboard(UP, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)      // left_shift
    //     camera.ProcessKeyboard(DOWN, deltaTime);
        
}

// re-size for the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // float xpos = static_cast<float>(xposIn);
    // float ypos = static_cast<float>(yposIn);

    // if (firstMouse)
    // {
    //     lastX = xpos;
    //     lastY = ypos;
    //     firstMouse = false;
    // }

    // float xoffset = xpos - lastX;
    // float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    // lastX = xpos;
    // lastY = ypos;

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}