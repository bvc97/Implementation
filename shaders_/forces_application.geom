#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices=9) out;

layout (std430, binding = 0) buffer vertices{
	vec4 vertex[];
};

layout (std430, binding = 1) buffer colors{
	vec4 color[];
};

layout (std430, binding = 2) buffer forces{
    vec4 force[];
};

layout (std430, binding = 3) buffer position{
    vec4 pos[];
};

layout (std430, binding = 4) buffer velocity{
    vec4 vel[];
};

layout (std430, binding = 5) buffer angular_velocity{
    vec4 vel_angular[];
};

layout (std430, binding = 6) buffer angular_position{
    vec4 theta[];
};

layout (std430, binding = 8) buffer torques{
    vec4 torque[];
};

layout (std430, binding = 9) buffer boats_inv_inertia{
	mat4 boat_inv_inertia[];
};

layout (std430, binding = 10) buffer boats_mass{
	float boat_mass[];
};

layout (std430, binding = 20) buffer auxiliar{
	vec4 aux[];
};

uniform mat4 m_pvm;
uniform mat4 m_pv;
uniform mat3 m_normal;
uniform mat4 m_view;
uniform mat4 m_proj;
uniform mat4 m_model;

uniform int max_vertices_per_boat;
uniform int num_boats;
//uniform float boat_mass;
uniform float delta_t;

in int instanceID[3];

int boat_idx = instanceID[0];
int idx = (gl_PrimitiveIDIn * 9) + max_vertices_per_boat * boat_idx;

out vec3 colour;

vec3 translate(vec3 trans, vec3 a){
    vec3 result = a + trans;
    return result;
}

float norm(vec3 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3 rotate(vec3 rot, vec3 a){
    float d = norm(rot); // d e o angulo
    float c = cos(d);
    float s = sin(d);

    float x = (d == 0 ? 0 : rot.x / d);
    float y = (d == 0 ? 0 : rot.y / d);
    float z = (d == 0 ? 0 : rot.z / d);

    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float yy = y * y;
    float yz = y * z;
    float zz = z * z;

    mat3 m_rot = mat3(xx + (1 - xx) * c,    xy * (1 - c) + z * s,   xz * (1 - c) - y * s,
                      xy * (1 - c) - z * s, yy + (1 - yy) * c,      yz * (1 - c) + x * s,
                      xz * (1 - c) + y * s, yz * (1 - c) - x * s,   zz + (1 - zz) * c);

    a = translate(-pos[boat_idx].xyz, a);

    a = m_rot * a;

    a = translate(pos[boat_idx].xyz, a);

    return a;
}

vec3 next_t_velocity(vec3 v, vec3 F, float d_t){
    vec3 a = (1.0/boat_mass[boat_idx]) * F;
    return v + d_t * a; 
}

vec3 next_t_position(vec3 pos, vec3 v, vec3 new_v, float d_t) {
    return pos + 0.5 * d_t * (v + new_v);
}

vec4 next_a_velocity(vec3 w, vec4 T, float d_t) {
	vec4 a_angular = boat_inv_inertia[boat_idx] * T;
	return vec4(w,0) + d_t * a_angular;
}

vec3 next_a_position(vec3 theta, vec3 w, vec3 new_w, float d_t) {
    return theta + 0.5 * d_t * (w + new_w);
}

void main()
{
    //total_force = force[boat_idx * max_vertices_per_boat];
    //total_torque = torque[boat_idx * max_vertices_per_boat];
    //total_torque = vec4(0);

    aux[0] = theta[0];
    // aux[1] = torque[max_vertices_per_boat];
    // aux[2] = torque[2 * max_vertices_per_boat];
    // aux[3] = torque[3 * max_vertices_per_boat];

    vec3 A0 = vertex[idx].xyz;
    vec3 B0 = vertex[idx+1].xyz;
    vec3 C0 = vertex[idx+2].xyz;
    vec3 A1 = vertex[idx+3].xyz;
    vec3 B1 = vertex[idx+4].xyz;
    vec3 C1 = vertex[idx+5].xyz;
    vec3 A2 = vertex[idx+6].xyz;
    vec3 B2 = vertex[idx+7].xyz;
    vec3 C2 = vertex[idx+8].xyz;

    vec3 c_A0 = color[idx].xyz;
    vec3 c_B0 = color[idx+1].xyz;
    vec3 c_C0 = color[idx+2].xyz;
    vec3 c_A1 = color[idx+3].xyz;
    vec3 c_B1 = color[idx+4].xyz;
    vec3 c_C1 = color[idx+5].xyz;
    vec3 c_A2 = color[idx+6].xyz;
    vec3 c_B2 = color[idx+7].xyz;
    vec3 c_C2 = color[idx+8].xyz;

    vec3 gravity = boat_mass[boat_idx] * vec3(0.0, -9.8, 0.0);
    
    vec3 F = force[max_vertices_per_boat * boat_idx].xyz + gravity;

    vec3 new_v = next_t_velocity(vel[boat_idx].xyz, F, delta_t);

    vec3 new_pos = next_t_position(pos[boat_idx].xyz, vel[boat_idx].xyz, new_v, delta_t);

    vec3 trans = (new_pos-pos[boat_idx].xyz);

    A0 = translate(trans, A0);
    B0 = translate(trans, B0);
    C0 = translate(trans, C0);
    A1 = translate(trans, A1);
    B1 = translate(trans, B1);
    C1 = translate(trans, C1);
    A2 = translate(trans, A2);
    B2 = translate(trans, B2);
    C2 = translate(trans, C2);

    
    vec4 new_w = next_a_velocity(vel_angular[boat_idx].xyz, torque[max_vertices_per_boat * boat_idx], delta_t);
    
    vec3 new_theta = next_a_position(theta[boat_idx].xyz, vel_angular[boat_idx].xyz, new_w.xyz, delta_t);

    vec3 rot = vec3(new_theta-theta[boat_idx].xyz);

    A0 = rotate(rot, A0);
    B0 = rotate(rot, B0);
    C0 = rotate(rot, C0);
    A1 = rotate(rot, A1);
    B1 = rotate(rot, B1);
    C1 = rotate(rot, C1);
    A2 = rotate(rot, A2);
    B2 = rotate(rot, B2);
    C2 = rotate(rot, C2);

    pos[boat_idx] = vec4(new_pos,1.0);
    vel[boat_idx] = vec4(new_v,0.0);

    vel_angular[boat_idx] = new_w;
    theta[boat_idx] = vec4(new_theta,1.0);

    gl_Position = m_pv * vec4(A0, 1.0);
    colour = c_A0;
    EmitVertex();

    gl_Position = m_pv * vec4(B0, 1.0);
    colour = c_B0;
    EmitVertex();

    gl_Position = m_pv * vec4(C0, 1.0);
    colour = c_C0;
    EmitVertex();

    EndPrimitive();

    gl_Position = m_pv * vec4(A1, 1.0);
    colour = c_A1;
    EmitVertex();

    gl_Position = m_pv * vec4(B1, 1.0);
    colour = c_B1;
    EmitVertex();

    gl_Position = m_pv * vec4(C1, 1.0);
    colour = c_C1;
    EmitVertex();

    EndPrimitive();

    gl_Position = m_pv * vec4(A2, 1.0);
    colour = c_A2;
    EmitVertex();

    gl_Position = m_pv * vec4(B2, 1.0);
    colour = c_B2;
    EmitVertex();

    gl_Position = m_pv * vec4(C2, 1.0);
    colour = c_C2;
    EmitVertex();

    EndPrimitive();
    
} 