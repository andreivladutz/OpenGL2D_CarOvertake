// Shader-ul de fragment / Fragment shader  
 
 #version 450 core

in vec4 ex_Color;
out vec4 out_Color;

in VS_OUT {
	vec4 color;
} vs_in;

void main(void) {
	out_Color = vs_in.color;
}
 