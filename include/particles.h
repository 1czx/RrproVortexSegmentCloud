#include "sprite.h"
#include <vector>


class DrawParticle {
public:
    DrawParticle() {};
    // maybe we need to avoid passing vector as parameter...????
    void draw(std::vector<glm::vec4> positions, glm::mat4 projection, glm::mat4 view);
private:
    PointSprite sprite;
};

// generate buffer holding the positions of tracer particles, and draw
void DrawParticle::draw(std::vector<glm::vec4> positions, glm::mat4 projection, glm::mat4 view) {
    unsigned int sprVBO;
	glGenBuffers(1, &sprVBO);
	int VBO_size = positions.size() * 4 * sizeof(float);        // vec4
	glBindBuffer(GL_ARRAY_BUFFER, sprVBO);
	glBufferData(GL_ARRAY_BUFFER, VBO_size, &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	sprite.getPos(sprVBO, positions.size());

	sprite.draw(projection, view);

	glDeleteBuffers(1, &sprVBO);
}
