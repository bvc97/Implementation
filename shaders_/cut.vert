#version 440

uniform mat4 m_pvm;
uniform mat4 m_pv;
uniform mat3 m_normal;
uniform mat4 m_view;
uniform mat4 m_proj;
uniform mat4 m_model;

in vec4 position;

out flat int instanceID;

void main()
{
    gl_Position = position;
    instanceID = gl_InstanceID;
} 