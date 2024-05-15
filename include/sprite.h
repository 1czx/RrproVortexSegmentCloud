#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include "Myshader.h"

#include <vector>

class PointSprite {
public:
    PointSprite() {
        glGenVertexArrays(1, &VAO);
        shader = get_shader("shader/sprite_shader.vert", "shader/sprite_shader.frag");
        printf("init sprite\n");
    }

    
    void getPos(unsigned int VBO_in, int num);
    void draw(glm::mat4 projection, glm::mat4 view, float R, float G, float B);

private:
    unsigned int VAO, VBO;
    unsigned int shader;
    int n;
};

// num = vector.size()
void PointSprite::getPos(unsigned int VBO_in, int num) {
    n = num;
    VBO = VBO_in;

    int VBO_size = num * 4 * sizeof(float);        // vec4
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // outside
    // glBufferData(GL_ARRAY_BUFFER, VBO_size, &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void PointSprite::draw(glm::mat4 projection, glm::mat4 view, float R, float G, float B) {
    if (n != 0) {
        glEnable(GL_PROGRAM_POINT_SIZE);                            // gl_PointSize
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
        glm::mat4 model = glm::mat4(1.0f);
        glUseProgram(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(shader, "point_size"), 1.0f);
        glUniform4f(glGetUniformLocation(shader, "inColor"), R, G, B, 1.0);

        glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, n);
		glBindVertexArray(0);

        glDisable(GL_PROGRAM_POINT_SIZE);
    } 
}

