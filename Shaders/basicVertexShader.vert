#version 450 core

in vec4 in_Position;
in vec4 in_Color;

uniform mat4 transformation_Matrix;

out VS_OUT {
	vec4 color;
} vs_out;

void main(void) {
	gl_Position = transformation_Matrix * in_Position;

	vs_out.color = in_Color;
} 
 