#include "forces.h"

//// Metodo 2 para calcular o torque (Artigo - ANEXO A) ////
/*
void static_center_up(plane pl, vec3* C, triangle t, vec3 H, vec3 M, vec3 L) {
	vec3 z_axis = normalize_vec3({ pl.a, pl.b, pl.c });
	vec3 x_axis = normalize_vec3(diff_vec3(M, L));
	vec3 y_axis = cross_vec3(x_axis, z_axis);

	vec3 H_ = { dot_vec3(x_axis, H), dot_vec3(z_axis, H) + pl.d, dot_vec3(y_axis, H) };
	vec3 M_ = { dot_vec3(x_axis, M), dot_vec3(z_axis, M) + pl.d, dot_vec3(y_axis, M) };
	vec3 L_ = { dot_vec3(x_axis, L), dot_vec3(z_axis, L) + pl.d, dot_vec3(y_axis, L) };

	TYPE a = min(M_.x, L_.x);
	TYPE b = max(M_.x, L_.x);
	TYPE c = M_.z;

	TYPE z0 = H_.y;
	TYPE d_z0 = abs(z0);
	TYPE h = M_.y - z0;
	TYPE d_h = abs(h);
	TYPE alpha = atan(c / h);
	TYPE tc = (6 * d_z0 + 4 * d_h) == 0 ? 0 : ((4 * d_z0 + 3 * d_h) / (6 * d_z0 + 4 * d_h));

	*C = sum_vec3({ 0, z0, 0 }, scale_vec3(tc, { (b - a) / 2, h, c }));
	//vec3 F = scale_vec3(G * t_center(t).y * DENS, normal(t));
	//vec3 F = scale_vec3(C.y, normal(t));
}


void static_center_down(plane pl, vec3* C,triangle t, vec3 H, vec3 M, vec3 L) {
	vec3 z_axis = normalize_vec3({ pl.a, pl.b, pl.c });
	vec3 x_axis = normalize_vec3(diff_vec3(H, M));
	vec3 y_axis = cross_vec3(x_axis, z_axis);

	vec3 H_ = { dot_vec3(x_axis, H), dot_vec3(z_axis, H) + pl.d, dot_vec3(y_axis, H) };
	vec3 M_ = { dot_vec3(x_axis, M), dot_vec3(z_axis, M) + pl.d, dot_vec3(y_axis, M) };
	vec3 L_ = { dot_vec3(x_axis, L), dot_vec3(z_axis, L) + pl.d, dot_vec3(y_axis, L) };

	TYPE a = min(M_.x, H_.x);
	TYPE b = max(M_.x, H_.x);
	TYPE c = M_.z;

	TYPE z0 = L_.y;
	TYPE d_z0 = abs(z0);
	TYPE h = M_.y - z0;
	TYPE d_h = abs(h);
	TYPE alpha = atan(c / h);
	TYPE tc = (6 * d_z0 + 2 * d_h) == 0 ? 0 : ((2 * d_z0 + d_h) / (6 * d_z0 + 2 * d_h));

	*C = sum_vec3({ 0, z0, 0 }, scale_vec3(tc, { (b - a) / 2.0, h, c }));

	//vec3 F = scale_vec3(G * t_center(t).y * DENS, normal(t));
	//vec3 F = scale_vec3(C.y, normal(t));
}

void static_center_up(plane pl, vec3* C,triangle t, vec3 H, vec3 M, vec3 L) {
	TYPE bma, c, z0, h, tc;

	bma = norm_vec3(diff_vec3(M, L));
	c = abs(dot_vec3(normalize_vec3(cross_vec3({ pl.a,pl.b,pl.c }, diff_vec3(M, L))), diff_vec3(M, H)));
	z0 = abs(apply(pl, H));
	h = abs(apply(pl, M)) - z0;

	tc = (6 * z0 + 4 * h) == 0 ? 0 : ((4 * z0 + 3 * h) / (6 * z0 + 4 * h));

	*C = sum_vec3({ 0,0,z0 }, scale_vec3(tc, { bma / 2, c, h }));
}

void static_center_down(plane pl, vec3* C,triangle t, vec3 H, vec3 M, vec3 L) {
	TYPE bma, c, z0, h, tc;

	bma = norm_vec3(diff_vec3(M, H));
	c = abs(dot_vec3(normalize_vec3(cross_vec3({pl.a,pl.b,pl.c}, diff_vec3(H,M))), diff_vec3(M, L)));
	z0 = abs(apply(pl, L));
	h = abs(apply(pl, M)) - z0;

	tc = (6 * z0 + 2 * h) == 0 ? 0 : ((2 * z0 + h) / (6 * z0 + 2 * h));

	*C = sum_vec3({ 0,0,z0 }, scale_vec3(tc, { bma / 2, c, h }));
}

void static_centers(triangle t, plane pl, maybe_vec3* C1, maybe_vec3* C2, triangle* up, triangle* down) {

	triangle_ord to = H_M_L(t, pl);

	vec3 H = to.HML.A;
	vec3 M = to.HML.B;
	vec3 L = to.HML.C;

	plane pl_m = { pl.a, pl.b, pl.c, pl.d - apply(pl, M) };

	maybe_vec3 res_intersect = intersect(H, L, pl_m);

	//vec3 F_final;
	if (!res_intersect.exists) {
		*up = t;
		*down = { L,M,L };
		C1->exists = true;
		static_center_up(pl,&(C1->v),*up, H, M, L);
		C2->exists = false;
		return;
	}
	vec3 ponto_HL = res_intersect.v;

	if (equal_vec3(ponto_HL, L)) {
		*up = t;
		*down = { L,M,L };
		C1->exists = true;
		static_center_up(pl, &(C1->v), *up, H, M, L);
		C2->exists = false;
		return;
	}

	if (equal_vec3(ponto_HL, H)) {
		*up = { H,M,H };
		*down = t;
		C1->exists = false;
		C2->exists = true;
		static_center_down(pl, &(C2->v),*down, H, M, L);
		return;
	}

	if (comp_hand(t, { H, M, L })) {
		*up = { H, M, ponto_HL };
		*down = { M, L, ponto_HL };
	}
	else {
		*up = { H, ponto_HL, M };
		*down = { ponto_HL, L, M };
	}

	C1->exists = true;
	C2->exists = true;
	static_center_up(pl, &(C1->v),*up, H, M, ponto_HL);
	static_center_down(pl, &(C2->v),*down, ponto_HL, M, L);

}

vec3 triangle_torque(triangle t, plane pl, vec3 center_of_mass) {
	TYPE sum_area, area_up = 0, area_down = 0, ratio_up, ratio_down;
	vec3 F1, F2, r1, r2, T1 = { 0.0,0.0,0.0 }, T2 = { 0.0,0.0,0.0 };
	maybe_vec3 C1, C2;
	triangle up, down;
	static_centers(t, pl, &C1, &C2, &up, &down);

	if (C1.exists) {
		area_up = area(up);
		F1 = triangle_force(up, pl);
		r1 = diff_vec3(C1.v, center_of_mass);
		T1 = cross_vec3(r1, F1);
	}
	if (C2.exists) {
		area_down = area(down);
		F2 = triangle_force(down, pl);
		r2 = diff_vec3(C2.v, center_of_mass);
		T2 = cross_vec3(r2, F2);
	}

	sum_area = area_up + area_down;
	ratio_up = sum_area == 0 ? 0 : (area_up / sum_area);
	ratio_down = sum_area == 0 ? 0 : (area_down / sum_area);

	return sum_vec3(scale_vec3(ratio_up, T1), scale_vec3(ratio_down, T2));
}
*/


//// Calcula as forças hidrostaticas (Artigo) ////

// Calcula a forca de cada triangulo //

vec3 buoyancy_force(triangle t, Grid water) {
	TYPE h_center = water.dist_surface(t_center(t));

	vec3 F = scale_vec3(G * h_center * DENS * area(t), normal(t));
	F.x = 0.0;
	F.z = 0.0;

	return F;
}

// Coloca a forca de buoyancy de cada triangulo num array //

vector<vec3> buoyancy_forces(vector<triangle> trs, Grid water) {
	vector<vec3> res;

	for (triangle t : trs) {
		res.push_back(buoyancy_force(t, water));
	}

	return res;
}

// Calcula a forca total de buoyancy dadas as forcas dos triangulos //

vec3 buoyancy_force(vector<vec3> buoyancy_forces) {
	vec3 totalForce = { 0, 0, 0 };

	for (int i = 0; i < buoyancy_forces.size(); i++) {
		totalForce = sum_vec3(totalForce, buoyancy_forces[i]);
	}

	return totalForce;
}

//// Metodo 1 para calcular o torque ////
// Calcula o torque de um triangulo //
vec3 triangle_torque(triangle t, vec3 center_of_mass, vec3 force_i) {
	vec3 r = diff_vec3(t_center(t), center_of_mass);
	return cross_vec3(r, force_i);
}


// Coloca o torque total de cada triangulo num array //

vector<vec3> torques(Boat boat, vector<triangle> trs, Grid water, vec3 center_of_mass, TYPE Cf) {
	vector<vec3> res;
	for (triangle t : trs) {
		TYPE cos_theta_i;
		vec3 b_i = buoyancy_force(t, water);
		vec3 v_i = viscous_water_resistance(boat, t, Cf);
		vec3 d_i = pressure_drag_forces(boat, t);
		vec3 s_i = scale_vec3(boat.ATotal == 0 ? 0 : (2 * boat.mass * submerged_triangle_area(boat, t, &cos_theta_i)) / boat.ATotal, boat.v);
		vec3 force_i = b_i;
		force_i = sum_vec3(force_i, v_i);
		force_i = sum_vec3(force_i, d_i);
		force_i = sum_vec3(force_i, s_i);
		res.push_back(triangle_torque(t, center_of_mass, force_i));
	}
	return res;
}

// Calcula o torque total dados os torques dos triangulos //

vec3 total_torque(vector<vec3> torques) {
	vec3 total_torque = { 0, 0, 0 };

	for (int i = 0; i < torques.size(); i++) {
		total_torque = sum_vec3(total_torque, torques[i]);
	}

	return total_torque;
}


/*
// Calcula uma aproximacao do centroide da parte submersa do objeto //

vec3 centroidAproximation(vector<triangle> trs, vec3 default) {
	if (trs.size() == 0) return default;
	vec3 centroid = { 0,0,0 };
	TYPE volume_tetras = volume_submerged(trs, default);
	for (triangle t : trs) {
		centroid = sum_vec3(centroid, scale_vec3(tetra_volume(t, default), tetra_centroid(t, default)));
		//centroid = sum_vec3(centroid, scale_vec3(area(t), t_center(t)));
	}
	//centroid = scale_vec3(1 / (trs.size() * totalArea(trs)), centroid);
	//centroid = scale_vec3(1/totalArea(trs), centroid);
	centroid = scale_vec3(volume_tetras == 0.0 ? 0.0 : 1.0 / volume_tetras, centroid);
	return centroid;
}


// Calcula o torque dada a forca total, o centroide da parte submersa e o centro de massa //

vec3 torque(vec3 totalForce, vec3 centroid, vec3 center_of_mass) {
	vec3 r = diff_vec3(centroid, center_of_mass);
	return cross_vec3(r, totalForce);
}

// Calcula um threshold para ser aplicado ao torque do objeto //

vec3 threshold_force(vec3 force) {
	vec3 new_force = force;
	if (abs(force.x) < pow(10, -5)) new_force.x = 0;
	if (abs(force.y) < pow(10, -5)) new_force.y = 0;
	if (abs(force.z) < pow(10, -5)) new_force.z = 0;
	return new_force;
}
*/


//// Calcula as forças hidrodinamicas (Artigo) ////


// Calcula as forcas de damping com base numa constante Cdamp //

vec3 damping_force(TYPE ASubmerged, TYPE ATotal, vec3 w, TYPE cdamp) {
	return scale_vec3(-cdamp * (ASubmerged / ATotal), w);
}

// Calcula a velocidade de um triangulo //

vec3 triangle_velocity(Boat boat, triangle t){
	//The connection formula for velocities (directly translated from Swedish)
	// v_A = v_B + omega_B cross r_BA
	// v_A - velocity in point A
	// v_B - velocity in point B
	// omega_B - angular velocity in point B
	// r_BA - vector between A and B

	vec3 v_B = boat.v;

	vec3 omega_B = boat.w;

	vec3 r_BA = diff_vec3(t_center(t), boat.pos);

	vec3 v_A = sum_vec3(v_B, cross_vec3(omega_B, r_BA));

	return v_A;
}



// Calcula o numero de Reynolds //

TYPE reynolds_number(vec3 v, TYPE ASubmerged, TYPE VSubmerged) {
	//Reynolds number

	// Rn = (V * L) / nu
	// V - speed of the body
	// L - length of the sumbmerged body
	// nu - viscosity of the fluid [m^2 / s]

	//A viscosidade varia com a temperatura, a 20 graus celsius:
	TYPE nu = 0.000001;
	//A 30 graus celsius: nu = 0.0000008f;

	//Reynolds number
	TYPE velocity = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	TYPE length = VSubmerged * ASubmerged;
	TYPE Rn = (velocity * length) / nu;

	return Rn;
}

// Calcula o coeficiente de resistencia da agua //

TYPE resistance_coefficient(TYPE Rn){
	//The resistance coefficient
	TYPE Cf = 0.075f / pow((log10(Rn) - 2.0f), 2.0f);

	return Cf;
}

// Calcula a resistencia viscosa da agua //

vec3 viscous_water_resistance(Boat boat, triangle t, TYPE Cf){
	//Viscous resistance occurs when water sticks to the boat's surface and the boat has to drag that water with it

	// F = 0.5 * rho * v^2 * S * Cf
	// rho - density of the medium you have
	// v - speed
	// S - surface area
	// Cf - Coefficient of frictional resistance

	vec3 n = normal(t);

	TYPE w_magnitude = sqrt(pow(triangle_velocity(boat, t).x, 2) + pow(triangle_velocity(boat, t).y, 2) + pow(triangle_velocity(boat, t).z, 2));

	TYPE n_magnitude = sqrt(pow(n.x, 2) + pow(n.y, 2) + pow(n.z, 2));

	vec3 v_tangent = scale_vec3(n_magnitude == 0 ? 1.0 : 1.0/n_magnitude, cross_vec3(normal(t),scale_vec3(n_magnitude == 0 ? 1.0 : 1.0 / n_magnitude, cross_vec3(triangle_velocity(boat, t),normal(t))))); //dot_vec3(diff_vec3(w, w), normal(tr));

	TYPE v_tangent_magnitude = sqrt(pow(v_tangent.x, 2) + pow(v_tangent.y, 2) + pow(v_tangent.z, 2));

	vec3 tangential_dir = scale_vec3(v_tangent_magnitude == 0.0 ? 1.0 : 1.0/v_tangent_magnitude, v_tangent);

	vec3 v_f = scale_vec3(w_magnitude, tangential_dir);

	TYPE v_f_magnitude = sqrt(pow(v_f.x, 2) + pow(v_f.y, 2) + pow(v_f.z, 2));

	vec3 viscous_force = scale_vec3(0.5 * DENS * Cf * area(t) * v_f_magnitude, v_f);
	
	return viscous_force;
}

// Coloca a forca de viscosidade de cada triangulo num array //

vector<vec3> viscous_forces(Boat boat, vector<triangle> trs, TYPE Cf) {
	vector<vec3> res;

	for (triangle t : trs) {
		res.push_back(viscous_water_resistance(boat, t, Cf));
	}

	return res;
}


// Calcula a forca total de viscosidade dadas as forcas dos triangulos //

vec3 viscous_force(vector<vec3> viscous_forces) {
	vec3 totalForce = { 0, 0, 0 };

	for (int i = 0; i < viscous_forces.size(); i++) {
		totalForce = sum_vec3(totalForce, viscous_forces[i]);
	}

	return totalForce;
}

// Calcula o cos_theta de um triangulo //

TYPE cos_theta(Boat boat, triangle t) {
	return dot_vec3(normalize_vec3(triangle_velocity(boat, t)), normal(t));
}


// Calcula as pressure drag forces //

vec3 pressure_drag_forces(Boat boat, triangle t) {
	TYPE a = pow(triangle_velocity(boat, t).x, 2) + pow(triangle_velocity(boat, t).y, 2) + pow(triangle_velocity(boat, t).z, 2);
	TYPE v = a < 0 ? 0 : sqrt(a);

	TYPE v_ref = v;

	v == 0 ? 1 : v / v_ref;

	vec3 drag_force = { 0.0, 0.0, 0.0 };

	TYPE cos_theta_i = cos_theta(boat, t);

	if (cos_theta_i > 0) {
		TYPE C_PD1 = 1;
		TYPE C_PD2 = 1;
		TYPE f_P = 0.5;

		drag_force = scale_vec3(-(C_PD1 * v + C_PD2 * (v * v)) * area(t) * pow(cos_theta_i, f_P), normal(t));
	}
	else {
		TYPE C_SD1 = 1;
		TYPE C_SD2 = 1;
		TYPE f_S = 0.5;

		drag_force = scale_vec3((C_SD1 * v + C_SD2 * (v * v)) * area(t) * pow(abs(cos_theta_i), f_S), normal(t));
	}

	return drag_force;
}

// Coloca a drag force de cada triangulo num array //

vector<vec3> drag_forces(Boat boat, vector<triangle> trs) {
	vector<vec3> res;

	for (triangle t : trs) {
		res.push_back(pressure_drag_forces(boat, t));
	}

	return res;
}

// Calcula a drag force total dadas as forcas dos triangulos //

vec3 drag_force(vector<vec3> drag_forces) {
	vec3 totalForce = { 0, 0, 0 };

	for (int i = 0; i < drag_forces.size(); i++) {
		totalForce = sum_vec3(totalForce, drag_forces[i]);
	}

	return totalForce;
}


// Calcula a area do trianglo submerso perpendicular à velocidade //

TYPE submerged_triangle_area(Boat boat, triangle t, TYPE* cos_theta_i) {
	*cos_theta_i = cos_theta(boat, t);

	if (cos_theta(boat, t) < 0) {
		return 0.0;
	}

	TYPE submerged_triangle_area = area(t) * (*cos_theta_i);

	return submerged_triangle_area;
}

// Coloca a slamming area de cada triangulo num array //

vector<TYPE> slamming_areas(Boat boat, vector<triangle> trs, TYPE* cos_theta_i) {
	vector<TYPE> res;

	for (triangle t : trs) {
		res.push_back(submerged_triangle_area(boat, t, cos_theta_i));
	}

	return res;
}

// Calcula a slamming force total dadas as forcas dos triangulos //

vec3 slamming_force(Boat boat, vector<TYPE> slamming_areas) {
	TYPE total_area = 0;

	vec3 totalForce = { 0, 0, 0 };

	for (int i = 0; i < slamming_areas.size(); i++) {
		total_area += slamming_areas[i];
	}

	totalForce = scale_vec3(boat.ATotal == 0 ? 0 : (2 * boat.mass * total_area) / boat.ATotal, boat.v);

	return totalForce;
}



//// Equacoes de movimento ////

// Calcula a proxima posicao do objeto //

vec3 next_dynamic_t_forces(Boat boat, vec3 F, TYPE d_t, vec3* new_v) {
	vec3 a = scale_vec3(1.0/boat.mass, F);

	*new_v = sum_vec3(boat.v, scale_vec3(d_t, a));

	vec3 res = sum_vec3(boat.pos, scale_vec3(0.5 * d_t, sum_vec3(boat.v, *new_v)));

	return res;
}

// Calcula o proximo theta do objeto //

vec3 next_dynamic_a_forces(Boat boat, vec3 T, TYPE d_t, vec3* new_w) {
	vec3 a_angular = mult_mat_vec3(boat.inv_inertia, T);

	*new_w = sum_vec3(boat.w, scale_vec3(d_t, a_angular));

	vec3 res = sum_vec3(boat.theta, scale_vec3(0.5 * d_t, sum_vec3(boat.w, *new_w)));

	return res;
}



/*
vec3 triangle_torque(triangle t, Grid water, vec3 center_of_mass) {
	vec3 P1 = diff_vec3(t.A, center_of_mass);
	vec3 P2 = diff_vec3(t.B, center_of_mass);
	vec3 P3 = diff_vec3(t.C, center_of_mass);

	vec3 C = t_center(t);

	//vec3 n_pl = { pl.a, pl.b, pl.c };

	TYPE P1_ = water.height({ P1.x,P1.z });
	TYPE P2_ = water.height({ P2.x,P2.z });
	TYPE P3_ = water.height({ P3.x,P3.z });

	TYPE C_ = water.height({ C.x,C.z });


	plane pl_C = water.plane_in({ C.x, C.z });

	return scale_vec3( DENS * G * area(t),
					  //cross_vec3(sum_vec3(scale_vec3(pl.d + 0.75 * C_,C),
						cross_vec3(sum_vec3(scale_vec3(pl_C.d + 0.75 * C_, C),
										  sum_vec3(sum_vec3(scale_vec3(1.0/12.0 * P1_, P1),
															scale_vec3(1.0 / 12.0 * P2_, P2)),
												   scale_vec3(1.0 / 12.0 * P3_, P3))),
					  normal(t)));
}
*/
