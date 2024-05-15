#include "sprite.h"
#include <vector>
#include <random>

class DrawParticle {
public:
    DrawParticle() {};
    // maybe we need to avoid passing vector as parameter...????
    void draw(std::vector<glm::vec4> positions, glm::mat4 projection, glm::mat4 view, float R, float G, float B);
private:
    PointSprite sprite;
};

// generate buffer holding the positions of tracer particles, and draw
void DrawParticle::draw(std::vector<glm::vec4> positions, glm::mat4 projection, glm::mat4 view, float R, float G, float B) {
    unsigned int sprVBO;
	glGenBuffers(1, &sprVBO);
	int VBO_size = positions.size() * 4 * sizeof(float);        // vec4
	glBindBuffer(GL_ARRAY_BUFFER, sprVBO);
	glBufferData(GL_ARRAY_BUFFER, VBO_size, &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	sprite.getPos(sprVBO, positions.size());

	sprite.draw(projection, view, R, G, B);

	glDeleteBuffers(1, &sprVBO);
}

inline double random_double(double a, double b) {
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<double> dis(a, b);
	return dis(eng);
}