#version 440

in vec4 position;

out flat int instanceID;

void main()
{
    gl_Position = position;
    instanceID = gl_InstanceID;
} 