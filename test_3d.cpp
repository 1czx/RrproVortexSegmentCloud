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
#include <opencv2/opencv.hpp>

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
// Camera camera(glm::vec3(3.0f, 0.0f, 0.0f));



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

    std::vector<glm::vec4> segms_p;
    std::vector<glm::vec4> segms_n;


    // VortexSegmentCloud2D vcloud(1.6, 1);
    VortexSegmentCloud3D vcloud(10.0, 10.0, 10.0);          // ???

    int n_tracer = 1000;                                    // number of tracers
    Vector3d dft_set(0.0, 0.0, 0.0);
    Vector3d dft_c(1.0, 0.0, 0.0);

    int n_seg = 12;                                         // num of segment (per each ring)
    float radi = 0.15;                                      // radius of the bigger ring
    float radii = 0.05;                                     // radius of the smaller
    for(int i=0; i<n_seg; i++ ) {
        Vector3d pos(0.0, radi * sin(3.14159 * 2 / n_seg * i), radi * cos(3.14159 * 2 / n_seg * i));
        Vector3d pos_i(0.0, radii * sin(3.14159 * 2 / n_seg * i), radii * cos(3.14159 * 2 / n_seg * i));

        float len_radi = 0.1;                               // distance between x+ x-
        Vector3d len(0.0, -len_radi * pos(2), len_radi * pos(1));
        Vector3d len_i(0.0, -len_radi * pos_i(2), len_radi * pos_i(1));



        float vor_temp = 5.0;                               // vor: may influence the speed...

        vcloud.addSegment(VortexSegment3D(pos + len, pos - len, dft_set, dft_set, dft_c, vor_temp));
        vcloud.addSegment(VortexSegment3D(pos_i + len_i, pos_i - len_i, dft_set, dft_set, dft_c, vor_temp));

    }
    for(int i=0; i<n_tracer; i++) {
        glm::vec4 new_tracer = glm::vec4(0.0, 0.0, 0.0, 1.0);
        Vector3d new_vec(random_double(-radii, radii), random_double(-radii, radii), random_double(-radii, radii));      // random position
        new_tracer.x = new_vec(0);
        new_tracer.y = new_vec(1);
        new_tracer.z = new_vec(2);
        VortexSegment3D seg(new_vec, dft_set, dft_set, dft_set, dft_c, 0);
        vcloud.addTracer(seg);
    }


    //////////////////////////////////////////////////////////////////////////////////////////
    // buffer...
    unsigned int frame_buffer;
    unsigned int frame_texture;
    glGenBuffers(1, &frame_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, frame_buffer);
    int buffer_size = 3 * SCR_HEIGHT * SCR_WIDTH;
    glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STREAM_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &frame_texture);
    glBindTexture(GL_TEXTURE_2D, frame_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // about the video
    int numFrames = 100;
    cv::VideoWriter video("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(SCR_WIDTH, SCR_HEIGHT));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
    double total_time = 0.0;
    // for(int i=0; i<numFrames; i++) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        total_time += deltaTime;
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
        // vcloud.get_seg_p(segms_p);               // for x+ x-
        // vcloud.get_seg_n(segms_n);

        // to draw: pass the vector of points. projection / view are given by camera
        draw_p.draw(tracers, projection, view, 1.0, 0.0, 0.0);
        draw_p.draw(segms, projection, view, 1.0, 1.0, 0.0);

        // draw_p.draw(segms_p, projection, view, 0.0, 1.0, 0.0);               // for x+ x-
        // draw_p.draw(segms_n, projection, view, 1.0, 0.0, 1.0);



        // bind the buffer to GL_PIXEL_PACK_BUFFER
        glBindBuffer(GL_PIXEL_PACK_BUFFER, frame_buffer);
        // write the data into the buffer (in GL_PIXEL_PACK_BUFFER)
        glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        void *frame_data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);

        if (frame_data) {                                           // add to the frame...
            cv::Mat frame(SCR_HEIGHT, SCR_WIDTH, CV_8UC3, frame_data);        
            video.write(frame);
        } else {
        }
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);                        // unmap
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);                      // unbind


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // gldelete....
    video.release();                                                // release the video

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)               // w
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)               // s
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)               // a
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)               // d
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)           // space
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)      // left_shift
        camera.ProcessKeyboard(DOWN, deltaTime);
        
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