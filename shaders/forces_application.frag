#version 440

in vec3 colour;

out vec4 out5;

void main(){

    out5 = vec4(colour, 1.0);

}