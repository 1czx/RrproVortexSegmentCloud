#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../glm/glm.hpp"
#include "Myshader.h"

// the line coordinate
float lines[] = {
    0.0f, 0.0f, 0.0f, 
    10.0f, 0.0f, 0.0f
};


class line_coord {
public:
    line_coord() {};
    void init();
    void draw(glm::mat4 projection, glm::mat4 view);
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int shaderProgram;
};


void line_coord::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    shaderProgram = get_shader("shader/shader.vs", "shader/shader.fs");
}

void line_coord::draw(glm::mat4 projection, glm::mat4 view) {
    glUseProgram(shaderProgram);
    for(int i=0; i<3; i++) {
        // say about projection & view...
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "inColor");
        // render the line
        glm::mat4 model = glm::mat4(1.0f);
        if (i == 0) {
            model = glm::rotate(model, 1.57f, glm::vec3(1.0f, 0.0f, 0.0f));             // X: no rotation
            glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);                   // red
        } else if (i == 1) {
            model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));             // -Z
            glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);                   // green
        } else {
            model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 0.0f, 1.0f));             // Y
            glUniform4f(vertexColorLocation, 0.0f, 0.0f, 1.0f, 1.0f);                   // blue
        }
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);

    }
}