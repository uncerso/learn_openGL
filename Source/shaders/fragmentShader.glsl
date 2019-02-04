R"~(
#version 130
uniform float alpha;
varying float arg_shift;
void main(){
	vec3 a = vec3(-0.5f, 0.0f, 0.5f);
	vec3 b = vec3(0.0f, 0.5f, 0.0f);
	vec3 c = vec3(-0.25f, 0.25f, -0.25f);
	vec3 d = vec3(0.75f, 0.75f, 0.75f);
	float arg = alpha + arg_shift; 
	gl_FragColor = vec4(a*cos(arg) + b*sin(arg) + c*cos(2*arg) + d, 1.0f);
}
)~"