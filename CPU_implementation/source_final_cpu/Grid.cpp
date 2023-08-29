#include "Grid.h"

TYPE dot_vec2(vec2 a, vec2 b) {
	return a.x * b.x + a.z * b.z;
}

vec2 sum_vec2(vec2 a, vec2 b) {
	return { a.x + b.x, a.z + b.z };
}

vec2 scale_vec2(TYPE t, vec2 a) {
	return { t * a.x, t * a.z };
}

// Default Constructor //
Grid::Grid(TYPE timer) {
	TYPE A = 0.0, w = 0.8, phi = 1, Q = 0;
	TYPE q = Q / (w * A);
	vec2 D = { 0,1 }, p;
	for (int x = 0; x < x_size; x++) {
		for (int z = 0; z < z_size; z++) {
			p = { x* length,z*length};
			TYPE y = A * sin(w * dot_vec2(D, p) + phi * timer);
			//p = sum_vec2(p, scale_vec2(q * A * cos(w * dot_vec2(D, p) + phi * timer), D));
			height_map.insert(make_pair(make_pair(x, z), y));
		}
	}
}

plane Grid::plane_in(vec2 xz) {
	TYPE x_divisions = x_size - 1;
	TYPE z_divisions = z_size - 1;
	TYPE comp = x_divisions * length;
	TYPE larg = z_divisions * length;

	TYPE pi_x = -comp / 2;
	TYPE pi_z = -larg / 2;

	TYPE x = (xz.x - pi_x) / length;
	TYPE z = (xz.z - pi_z) / length;

	int i = floor(x);
	int j = floor(z);

	TYPE xmax = pi_x + length * (i + 1);
	TYPE xmin = pi_x + length * i;
	TYPE zmax = pi_z + length * (j + 1);
	TYPE zmin = pi_z + length * j;

	TYPE quad_level = (x-i) + (z-j) - 1;
	plane pl;
	vec3 A, B, C;

	if (quad_level < 0) {
		//primeiro triangulo
		A = { xmin, height_map.find(make_pair(i,j))->second,zmin };
		B = { xmin, height_map.find(make_pair(i,j + 1))->second,zmax };
		C = { xmax, height_map.find(make_pair(i+1,j))->second,zmin };
	}
	else {
		//segundo triangulo
		A = { xmax, height_map.find(make_pair(i+1,j+1))->second,zmax };
		B = { xmax, height_map.find(make_pair(i + 1,j))->second,zmin };
		C = { xmin, height_map.find(make_pair(i,j + 1))->second,zmax };
	}
	pl = points_plane(A, B, C);

	return pl;
}

TYPE Grid::height(vec2 xz) {
	plane pl = this->plane_in(xz);
	return -(pl.a * xz.x + pl.c * xz.z + pl.d) / pl.b;
}

TYPE Grid::dist_surface(vec3 point) {
	vec2 xz = { point.x,point.z };
	plane pl = this->plane_in(xz);
	TYPE res = apply(pl, point);
	return res;
}

void Grid::draw() {
	triangle t;

	for (int i = 0; i < x_size-1; i++) {
		for (int j = 0; j < z_size-1; j++) {
			glColor3f(1, 0, 0);

			TYPE x_divisions = x_size - 1;
			TYPE z_divisions = z_size - 1;
			TYPE comp = x_divisions * length;
			TYPE larg = z_divisions * length;
			

			TYPE pi_x = -comp / 2;
			TYPE pi_z = -larg / 2;

			TYPE xmax = pi_x + length * (i + 1);
			TYPE xmin = pi_x + length * i;
			TYPE zmax = pi_z + length * (j + 1);
			TYPE zmin = pi_z + length * j;


			t.A = { xmax, height_map.find(make_pair(i+1,j))->second, zmin };
			t.B = { xmin, height_map.find(make_pair(i,j))->second, zmin };
			t.C = { xmin, height_map.find(make_pair(i,j+1))->second, zmax };

			glBegin(GL_TRIANGLES);
			glVertex3f(t.A.x, t.A.y, t.A.z);
			glVertex3f(t.B.x, t.B.y, t.B.z);
			glVertex3f(t.C.x, t.C.y, t.C.z);
			glEnd();

			t.A = { xmin, height_map.find(make_pair(i,j+1))->second, zmax };
			t.B = { xmax, height_map.find(make_pair(i + 1,j+1))->second, zmax };
			t.C = { xmax, height_map.find(make_pair(i + 1,j))->second, zmin };

			glBegin(GL_TRIANGLES);
			glVertex3f(t.A.x, t.A.y, t.A.z);
			glVertex3f(t.B.x, t.B.y, t.B.z);
			glVertex3f(t.C.x, t.C.y, t.C.z);
			glEnd();
		}
	}
}