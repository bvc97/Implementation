#version 440

// layout (std430, binding = 9) buffer height_map{
//     float heights[38][38];
// };

uniform mat4 m_pvm;
uniform mat4 m_pv;
uniform mat3 m_normal;
uniform mat4 m_view;
uniform mat4 m_proj;
uniform mat4 m_model;

uniform float timer;

// A = amplitude, w = frequencia, phi = fase da onda, Q = cristas, D = direcao do vento
uniform float A, w, phi; //Q
uniform vec2 D;

in vec4 position;

out Data {
	vec4 pos;
} DataOut;

void main() {

	//float q = Q/(w*A);
	vec4 p = m_model * position;

	p.y = A*sin(w*dot(D, p.xz) + phi*timer);
	
	//p.xz = p.xz + q*A*D* cos(w*dot(D, p.xz) + phi*timer);
	
	//DataOut.normal.x = -(D.x*w*A*cos(w*dot(D,position.xz) + phi*timer/1000));
	//DataOut.normal.z = -(D.y*w*A*cos(w*dot(D,position.xz) + phi*timer/1000));
	//DataOut.normal.y = 1 - (q*w*A*sin(w*dot(D,position.xz) + phi*timer/1000));
	
	//DataOut.normal = normalize(m_normal * DataOut.normal);
	//DataOut.l_dir = vec3(normalize(- (m_view * l_dir)));
	gl_Position = m_pvm * position;
	DataOut.pos = p;
}