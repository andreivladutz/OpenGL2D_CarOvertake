
// Shader-ul de fragment / Fragment shader  
 
 #version 450 core

in vec4 ex_Color;
out vec4 out_Color;

uniform int color_Code;

in VS_OUT {
	vec4 color;
} vs_in;

void main(void) {
	// culoarea poligonului P1
	if (color_Code == 1) {
		out_Color = vec4(1, 0, 1, 1);
	} else if (color_Code == 2) {
		out_Color = vec4(0, 1, 0, 1);
	} else if (color_Code == 3) {
		out_Color = vec4(1, 0, 0, 1);
	} else if (color_Code == 4) {
		out_Color = vec4(0, 0, 1, 1);
	} else if (color_Code == 5) {
		out_Color = vec4(1, 1, 0, 1);
	} else if (color_Code == 0) {
		out_Color = vs_in.color;
	}
}
 