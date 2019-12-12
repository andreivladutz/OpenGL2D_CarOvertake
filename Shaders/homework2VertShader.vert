#version 450 core

in vec4 in_Position;
in vec4 in_Color;

uniform mat4 transform_Matrix, rotate_Matrix,
	translate_M, translate_NegM;

uniform bool should_Rotate;


out VS_OUT {
	vec4 color;
} vs_out;

void main(void) {
	// rotatia "intre poligoane"
	if (should_Rotate) {
		mat4 temp_Transform_Matrix = transform_Matrix * translate_M * rotate_Matrix * translate_NegM;
		gl_Position = temp_Transform_Matrix * in_Position;
	} else {
		gl_Position = transform_Matrix * in_Position;
    }

	vs_out.color = in_Color;
} 
 