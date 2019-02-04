R"~(
#version 130
attribute vec3 position;
attribute float shift;
varying float arg_shift;
void main(){
	gl_Position = vec4(position, 1.0f);
	arg_shift = shift;
}
)~"