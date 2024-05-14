#version 430 core
layout (location = 0) in vec4 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float point_size;


void main()
{
	gl_PointSize = point_size * 3;
	// gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_Position = projection * view * model * aPos;
}