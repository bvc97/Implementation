#version 440

in Data {
	vec4 pos;
} DataIn;

layout (location = 0) out float out1;

void main() {
	
	out1 = DataIn.pos.y;
}
	