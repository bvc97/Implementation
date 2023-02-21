#version 440

in vec3 colour;

layout (location = 1) out vec4 out3;

void main(){

    out3 = vec4(colour, 1.0);

}