#version 450 core

in vec4 in_Position;
in vec4 in_Color;

// uniform mat4 transformation_Matrix;

layout (std140, binding = 0) uniform TransformBlock {
	mat4 resizeMatrix, cameraTranslation,
		modelTranslation, modelRotation, modelScale;
} transfBlock;

out VS_OUT {
	vec4 color;
} vs_out;

void main(void) {
	//gl_Position = transformation_Matrix * in_Position;

	gl_Position = transfBlock.resizeMatrix 
		* transfBlock.cameraTranslation 
		* transfBlock.modelTranslation 
		* transfBlock.modelRotation
		* transfBlock.modelScale * in_Position;

	vs_out.color = in_Color;
} 
 