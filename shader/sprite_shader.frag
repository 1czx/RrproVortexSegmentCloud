#version 430 core
// out vec4 FragColor;


// layout(location = 0) out vec4 FragColor;
out vec4 FragColor;
uniform vec4 inColor;		// uniform inColor as input parameter


void main()
{
	FragColor = inColor;	// set the color
}
