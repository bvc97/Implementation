#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices=9) out;

uniform sampler2D texUnit;
uniform int fboSize;
// uniform float boat_mass;
// uniform float boat_ATotal;
// uniform float boat_length;
uniform int max_vertices_per_boat;

/* layout (std430, binding = 9) buffer height_map{
    float heights[38][38];
}; */

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

/* layout (std430, binding = 7) buffer boat_length{
	float boat_len;
}; */

layout (std430, binding = 8) buffer torques{
    vec4 torque[];
};

layout (std430, binding = 10) buffer boats_mass{
	float boat_mass[];
};

layout (std430, binding = 11) buffer boats_ATotal{
	float boat_ATotal[];
};

layout (std430, binding = 12) buffer boats_length{
	float boat_length[];
};

in int instanceID[3];

float G = 9.8, DENS = 1.0;
uniform int boat_idx; //= instanceID[0];

int j = (gl_PrimitiveIDIn * 3); //+ max_vertices_per_boat * boat_idx;
int idx = (gl_PrimitiveIDIn * 9); //+ max_vertices_per_boat * boat_idx;

vec3 t_center(vec3 A, vec3 B, vec3 C) {
    return (A + B + C) / 3.0;
}

vec2 grid_coord(vec2 a){
    return vec2((a.x + 32)/64 ,(a.y + 32)/64);
}

vec3 normal(vec3 A, vec3 B, vec3 C) {
    vec3 U = B - A;
    vec3 V = C - A;

    vec3 N = cross(U, V);

	if(N == vec3(0.0)) return vec3(0.0);
    else return normalize(N);
}

vec4 points_plane(vec3 a, vec3 b, vec3 c) {
	vec3 normal = normal(a,b,c);

    float d = -(normal.x * a.x + normal.y * a.y + normal.z * a.z);

    return vec4(normal.x, normal.y, normal.z, d);
}

vec4 plane_in(vec2 xz){
	float i = floor(xz.x);
	float j = floor(xz.y);

	float xmax = i + 1;
	float xmin = i;
	float zmax = j + 1;
	float zmin = j;

	float quad_level = (xz.y-j) - (xz.x-i);
	vec4 pl;
	vec3 A, B, C;

    if (quad_level < 0) {
		//primeiro triangulo
		A = vec3(xmin, texture(texUnit, grid_coord(vec2(i,j))).r,zmin);
		B = vec3(xmin, texture(texUnit, grid_coord(vec2(i,j+1))).r,zmax);
		C = vec3(xmax, texture(texUnit, grid_coord(vec2(i+1,j))).r,zmin);
	}
	else {
		//segundo triangulo
		A = vec3(xmax, texture(texUnit, grid_coord(vec2(i+1,j+1))).r,zmax);
		B = vec3(xmax, texture(texUnit, grid_coord(vec2(i+1,j))).r,zmin);
		C = vec3(xmin, texture(texUnit, grid_coord(vec2(i,j+1))).r,zmax);
	}

	pl = points_plane(A, B, C);

	return pl;
}

float apply(vec4 pl, vec3 a){
    return (a.x * pl.x) + (a.y * pl.y) + (a.z * pl.z) + pl.w;
}

float dist_surface(vec3 a){
    vec2 xz = vec2(a.x,a.z);
	vec4 pl = plane_in(xz);
	float res = apply(pl, a);
	return res;
}

float norm(vec3 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

float area(vec3 A, vec3 B, vec3 C) {
    return norm(cross(B - A, C - A)) / 2;
}

vec3 buoyancy_force(vec3 A, vec3 B, vec3 C){
	float h_center = dist_surface(t_center(A,B,C));

	vec3 F = G * h_center * DENS * area(A,B,C) * normal(A,B,C);
	F.x = 0.0;
	F.z = 0.0;

	return F;
}

// Calcula a velocidade de um triangulo //

vec3 triangle_velocity(vec3 A, vec3 B, vec3 C){
	//The connection formula for velocities (directly translated from Swedish)
	// v_A = v_B + omega_B cross r_BA
	// v_A - velocity in point A
	// v_B - velocity in point B
	// omega_B - angular velocity in point B
	// r_BA - vector between A and B

	vec3 v_B = vel[boat_idx].xyz;

	vec3 omega_B = vel_angular[boat_idx].xyz;

	vec3 r_BA = t_center(A, B, C) - pos[boat_idx].xyz;

	vec3 v_A = v_B + cross(omega_B, r_BA);

	return v_A;
}

// Calcula o numero de Reynolds //

// preciso da area submergida e do volume submergido

float reynolds_number(vec3 v) {
	//Reynolds number

	// Rn = (V * L) / nu
	// V - speed of the body
	// L - length of the sumbmerged body
	// nu - viscosity of the fluid [m^2 / s]

	//A viscosidade varia com a temperatura, a 20 graus celsius:
	float nu = 0.000001;
	//A 30 graus celsius: nu = 0.0000008f;

	//Reynolds number
	float velocity = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	float Rn = (velocity * boat_length[boat_idx]) / nu;

	return Rn;
}

// Calcula log10 de um numero //
float log10(float x){
	return log(x) / log(10);
}

float resistance_coefficient(float Rn){
	//The resistance coefficient
	float Cf = 0.075f / pow((log10(Rn) - 2.0f), 2.0f);

	return Cf;
}

// Calcula a resistencia viscosa da agua //

vec3 viscous_water_resistance(vec3 A, vec3 B, vec3 C, float Cf){
	//Viscous resistance occurs when water sticks to the boat's surface and the boat has to drag that water with it

	// F = 0.5 * rho * v^2 * S * Cf
	// rho - density of the medium you have
	// v - speed
	// S - surface area
	// Cf - Coefficient of frictional resistance

	vec3 n = normal(A, B, C);

	float w_magnitude = norm(triangle_velocity(A, B, C));

	float n_magnitude = norm(n);

	float m = (n_magnitude == 0) ? 1.0 : 1.0/n_magnitude;

	vec3 v_tangent = m * cross(normal(A, B, C), m * cross(triangle_velocity(A, B, C), normal(A, B, C)));

	float v_tangent_magnitude = norm(v_tangent);

	float m_vel = (v_tangent_magnitude == 0.0) ? 1.0 : 1.0/v_tangent_magnitude;

	vec3 tangential_dir = m_vel * v_tangent;

	vec3 v_f = w_magnitude * tangential_dir;

	float v_f_magnitude = norm(v_f);

	vec3 viscous_force = 0.5 * DENS * Cf * area(A, B, C) * v_f_magnitude * v_f;
	
	return viscous_force;
}

// Calcula o cos_theta de um triangulo //

float cos_theta(vec3 A, vec3 B, vec3 C) {
	vec3 normalized_triangle_vel = (triangle_velocity(A, B, C) == vec3(0.0))?vec3(0.0):normalize(triangle_velocity(A, B, C));
	return dot(normalized_triangle_vel, normal(A, B, C));
}


// Calcula as pressure drag forces //

vec3 pressure_drag_forces(vec3 A, vec3 B, vec3 C) {
	float a = pow(triangle_velocity(A, B, C).x, 2) + pow(triangle_velocity(A, B, C).y, 2) + pow(triangle_velocity(A, B, C).z, 2);
	float v = a < 0 ? 0 : sqrt(a);

	float v_ref = v;

	v == 0 ? 1 : v / v_ref;

	vec3 drag_force = { 0.0, 0.0, 0.0 };

	float cos_theta_i = cos_theta(A, B, C);

	if (cos_theta_i > 0) {
		float C_PD1 = 1;
		float C_PD2 = 1;
		float f_P = 0.5;

		drag_force = -(C_PD1 * v + C_PD2 * (v * v)) * area(A, B, C) * pow(cos_theta_i, f_P) * normal(A, B, C);
	}
	else {
		float C_SD1 = 1;
		float C_SD2 = 1;
		float f_S = 0.5;

		drag_force = (C_SD1 * v + C_SD2 * (v * v)) * area(A, B, C) * pow(abs(cos_theta_i), f_S) * normal(A, B, C);
	}

	return drag_force;
}

// Calcula a area do trianglo submerso perpendicular à velocidade //

float submerged_triangle_area(vec3 A, vec3 B, vec3 C) {
	float submerged_triangle_area;

	if (cos_theta(A, B, C) < 0) {
		submerged_triangle_area = 0.0;
	}
	else{
		submerged_triangle_area = area(A, B, C);
	}

	return submerged_triangle_area;
}

vec3 slamming_force(vec3 A, vec3 B, vec3 C){
	return boat_ATotal[boat_idx] == 0 ? vec3(0.0) : ((2 * boat_mass[boat_idx] * submerged_triangle_area(A, B, C) * cos_theta(A, B, C)) / boat_ATotal[boat_idx]) * vel[boat_idx].xyz;
}

vec3 triangle_torque(vec3 A, vec3 B, vec3 C, vec3 center_of_mass, vec3 force_i) {
	vec3 r = t_center(A, B, C) - center_of_mass;
	return cross(r, force_i);
}

void main(){
	vec3 A0 = vertex[idx].xyz;
	vec3 B0 = vertex[idx+1].xyz;
	vec3 C0 = vertex[idx+2].xyz;
    vec3 A1 = vertex[idx+3].xyz;
	vec3 B1 = vertex[idx+4].xyz;
	vec3 C1 = vertex[idx+5].xyz;
    vec3 A2 = vertex[idx+6].xyz;
	vec3 B2 = vertex[idx+7].xyz;
	vec3 C2 = vertex[idx+8].xyz;

	vec3 blue_color = vec3(0.0,0.0,1.0);
	
	vec3 F0 = vec3(0.0);
    vec3 F1 = vec3(0.0);
    vec3 F2 = vec3(0.0);

	vec3 T0 = vec3(0.0);
    vec3 T1 = vec3(0.0);
    vec3 T2 = vec3(0.0);

	if(color[idx].rgb == blue_color && color[idx+1].rgb == blue_color && color[idx+2].rgb == blue_color){
		F0 = buoyancy_force(A0,B0,C0) + 
			 viscous_water_resistance(A0,B0,C0, resistance_coefficient(reynolds_number(vel[boat_idx].xyz))) + 
			 pressure_drag_forces(A0, B0, C0) + 
			 slamming_force(A0, B0, C0);
		T0 = triangle_torque(A0,B0,C0,pos[boat_idx].xyz,F0);
	}

    if(color[idx+3].rgb == blue_color && color[idx+4].rgb == blue_color && color[idx+5].rgb == blue_color){
		F1 = buoyancy_force(A1,B1,C1) + 
			 viscous_water_resistance(A1,B1,C1, resistance_coefficient(reynolds_number(vel[boat_idx].xyz))) + 
			 pressure_drag_forces(A1, B1, C1) + 
			 slamming_force(A1, B1, C1);
		T1 = triangle_torque(A1,B1,C1,pos[boat_idx].xyz,F1);
	}

    if(color[idx+6].rgb == blue_color && color[idx+7].rgb == blue_color && color[idx+8].rgb == blue_color){
		F2 = buoyancy_force(A2,B2,C2) + 
			 viscous_water_resistance(A2,B2,C2, resistance_coefficient(reynolds_number(vel[boat_idx].xyz))) + 
			 pressure_drag_forces(A2, B2, C2) + 
			 slamming_force(A2, B2, C2);
		T2 = triangle_torque(A2,B2,C2,pos[boat_idx].xyz,F2);
	}

	force[j] = vec4(F0,0);
    force[j+1] = vec4(F1,0);
    force[j+2] = vec4(F2,0);

	torque[j] = vec4(T0,0);
    torque[j+1] = vec4(T1,0);
    torque[j+2] = vec4(T2,0);
}