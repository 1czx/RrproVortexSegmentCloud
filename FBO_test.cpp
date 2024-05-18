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
    VortexSegmentCloud2D vcloud;
    int n_tracer = 1000;                                    // number of tracers = 5000
    Vector2d dft_set(0.0, 0.0);
    Vector3d dft_c(1.0, 0.0, 0.0);
    for(int i=0; i<n_tracer; i++) {
        glm::vec4 new_tracer = glm::vec4(0.0, 0.0, 0.0, 1.0);
        Vector2d new_vec(random_double(-0.5, -0.3), random_double(-0.1, 0.1));      // random position
        // Vector2d new_vec(random_double(-0.1, 0.1), random_double(-0.1, 0.1));      // random position
        new_tracer.x = new_vec(0);
        new_tracer.y = new_vec(1);
        VortexSegment2D seg(new_vec, dft_set, dft_c, 0);
        // vcloud.addTracer(seg);
        // vcloud.addSegment(seg);
    }
    
    // boundary???
    vector<Vector2d> bd;
    vector<Vector2d> bd_seg;
    vector<glm::vec4> cir;
    int n = 1280;
    // int n = 144;

    int na = 12;
    double radi = 0.08;
    double bd_x_off = -1.8;

    for(int i=0; i<n; i++) {
        Vector2d new_pt( radi * cos(3.14159 * 2 / n * i) + bd_x_off, radi * sin(3.14159 * 2 / n * i));
        cir.push_back(glm::vec4(new_pt(0), new_pt(1), 0, 1));
        bd.push_back(new_pt);
    }
    radi += 0.01;
    // radi += 0.001;
    for(int i=0; i<na; i++) {
        Vector2d new_pt( radi * cos(3.14159 * 2 / na * i) + bd_x_off, radi * sin(3.14159 * 2 / na * i));
        bd_seg.push_back(new_pt);
    }
    vcloud.setBoundary(bd, bd_seg);
    vcloud.set_back_vel(2.0, 0.0);


    vector<glm::vec4> bds;

    //////////////////////////////////////////////////////////////////////
    // about the FBO
    // screen quad VAO
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    unsigned int ScreenShader = get_shader("shader/fbo_test.vert", "shader/fbo_test.frag");
        


    // framebuffer configuration
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // bind the texture to the Framebuffer
    // if depth / other needed.......
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    // renderBuffer Obj
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // use a single renderbuffer object for both a depth AND stencil buffer.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);   // attach
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)                        // check 
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    ///////////////////////////////////////////////////////////////////////////
    // render loop, with frameBuffer used
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
        // bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // glEnable(GL_DEPTH_TEST);                             // depth not used, for now...
        // clear the frameBuffer Content
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // background color as black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the contents
        // projection and view (move of the camera)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        line_temp.draw(projection, view);

        // vcloud.oneStepTemporalEvolution(deltaTime);
        vcloud.oneStepTemporalEvolution(0.0333);
        vcloud.get_tracer(tracers);
        vcloud.get_seg(segms);
        // to draw: pass the vector of points. projection / view are given by camera
        draw_p.draw(tracers, projection, view, 1.0, 0.0, 0.0);
        draw_p.draw(segms, projection, view, 1.0, 1.0, 0.0);
        draw_p.draw(cir, projection, view, 0.0, 1.0, 0.0);
        printf("actual fps: 1/%f\t= %f\n", deltaTime, 1.0/deltaTime);



        ///////////////////////////////////////////////////////////////////////////////////////
        // then how to pass the frameBuffer to pic???
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ScreenShader);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);




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