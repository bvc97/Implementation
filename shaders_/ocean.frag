#version 440

out vec4 out2;

in Data{
	vec4 pos;
}DataIn;

void main() {
	out2 = vec4(0.2, 0.4, 0.8, 1.0) * (1 + DataIn.pos.y);
}
	