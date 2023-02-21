#version 440

in vec4 position;

uniform mat4 m_pvm;
uniform mat4 m_pv;
uniform mat3 m_normal;
uniform mat4 m_view;
uniform mat4 m_proj;
uniform mat4 m_model;

uniform sampler2D texUnit;
uniform int fboSize;

out Data{
	vec4 pos;
} DataOut;

void main() {
	vec4 p = m_model * position;
	
	vec2 gridCoord = vec2((p.x + 32)/64,(p.z + 32)/64);

	p.y = texture(texUnit, gridCoord).r;

	gl_Position = m_pv * p;

	DataOut.pos = p;
}