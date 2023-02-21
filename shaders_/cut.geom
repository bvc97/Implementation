#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices=9) out;

layout (std430, binding = 0) buffer vertices{
	vec4 vertex[];
};

layout (std430, binding = 1) buffer colors{
	vec4 color[];
};

layout (std430, binding = 3) buffer position{
    vec4 pos[];
};

layout (std430, binding = 6) buffer angular_position{
    vec4 theta[];
};

in int instanceID[3];

out vec3 colour;

uniform mat4 m_pvm;
uniform mat4 m_pv;
uniform mat3 m_normal;
uniform mat4 m_view;
uniform mat4 m_proj;
uniform mat4 m_model;

uniform sampler2D texUnit;
uniform int fboSize;
uniform int max_vertices_per_boat;

int x_size = 38, z_size = 38;

int boat_idx = instanceID[0];

int idx = (gl_PrimitiveIDIn * 9) + max_vertices_per_boat * boat_idx;

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

vec2 grid_coord(vec2 a){
    return vec2((a.x + 32)/64,(a.y + 32)/64);
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
	//float res = apply(pl, a);
    float expected_y = -((a.x * pl.x) + (a.z * pl.z) + pl.w)/pl.y; //pl.y nunca vai ser igual a 0
    float res = a.y - expected_y;
	return res;
}

bool equal_sign_vec3(vec3 a, vec3 b) {
    return (a.x > 0) == (b.x > 0) && (a.y > 0) == (b.y > 0) && (a.z > 0) == (b.z > 0);
}

bool comp_hand(vec3 A, vec3 B, vec3 C, vec3 H, vec3 M, vec3 L){
    return equal_sign_vec3(cross(B - A, C - A), cross(M - H, L - H));
}

void main()
{
    vec3 A = rotate(theta[boat_idx].xyz, (m_model * gl_in[0].gl_Position).xyz + pos[boat_idx].xyz);
    vec3 B = rotate(theta[boat_idx].xyz, (m_model * gl_in[1].gl_Position).xyz + pos[boat_idx].xyz);
    vec3 C = rotate(theta[boat_idx].xyz, (m_model * gl_in[2].gl_Position).xyz + pos[boat_idx].xyz);

    int is_A_under = bool(dist_surface(A) < 0)?1:0;
    int is_B_under = bool(dist_surface(B) < 0)?1:0;
    int is_C_under = bool(dist_surface(C) < 0)?1:0;

    

    switch (is_A_under + is_B_under + is_C_under){
        case 0: {
            vec3 vertexPos = A;
            gl_Position = m_pv * vec4(vertexPos, 1.0);
            vertex[idx] = vec4(vertexPos,1.0);
            colour = vec3(1.0, 1.0, 1.0);
            color[idx] = vec4(colour,1.0);
            EmitVertex();

            vertexPos = B;
            gl_Position = m_pv * vec4(vertexPos, 1.0);
            vertex[idx+1] = vec4(vertexPos,1.0);
            colour = vec3(1.0, 1.0, 1.0);
            color[idx+1] = vec4(colour,1.0);
            EmitVertex();

            vertexPos = C;
            gl_Position = m_pv * vec4(vertexPos, 1.0);
            vertex[idx+2] = vec4(vertexPos,1.0);
            colour = vec3(1.0, 1.0, 1.0);
            color[idx+2] = vec4(colour,1.0);
            EmitVertex();

            EndPrimitive();

            for(int i = 3; i < 9; i++){
                vertex[idx+i] = vec4(vec3(0.0),1.0);
                color[idx+i] = vec4(vec3(0.0),1.0);
            }

        }
            break;

        case 1: {
            vec3 L = bool(is_A_under) ? A : bool(is_B_under) ? B : C;
            vec3 M = bool(is_A_under) ? (B.y < C.y ? B : C) : bool(is_B_under) ? (A.y < C.y ? A : C) : (A.y < B.y ? A : B);
            vec3 H = bool(is_A_under) ? (B.y >= C.y ? B : C) : bool(is_B_under) ? (A.y >= C.y ? A : C) : (A.y >= B.y ? A : B);

            float hH = dist_surface(H);
            float hM = dist_surface(M);
            float hL = dist_surface(L);

            float tH = (-hL) / (hH - hL);
            vec3 JH = L + (tH * (H - L));

            float tM = (-hL) / (hM - hL);
            vec3 JM = L + (tM * (M - L)); 


            if (comp_hand(A, B, C, H, M, L)) {
                vec3 vertexPos = H;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = M;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+1] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+1] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+2] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+2] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = JM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+3] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+3] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JH;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+4] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+4] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = H;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+5] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+5] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = JM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+6] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+6] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = L;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+7] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+7] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JH;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+8] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+8] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

            }
            else {
                vec3 vertexPos = M;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = H;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+1] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+1] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+2] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+2] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = JH;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+3] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+3] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+4] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+4] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = H;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+5] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+5] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = L;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+6] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+6] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+7] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+7] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = JH;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+8] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+8] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

            }
        }
            break;
        case 2: {
            vec3 H = !bool(is_A_under) ? A : !bool(is_B_under) ? B : C;
            vec3 L = !bool(is_A_under) ? (B.y < C.y ? B : C) : !bool(is_B_under) ? (A.y < C.y ? A : C) : (A.y < B.y ? A : B);
            vec3 M = !bool(is_A_under) ? (B.y >= C.y ? B : C) : !bool(is_B_under) ? (A.y >= C.y ? A : C) : (A.y >= B.y ? A : B);

            float hH = dist_surface(H);
            float hM = dist_surface(M);
            float hL = dist_surface(L);

            float tL = (-hL) / (hH - hL);
            vec3 IL = L + (tL * (H - L));

            float tM = (-hM) / (hH - hM);
            vec3 IM = M + (tM * (H - M));

            if (comp_hand(A, B, C, H, M, L)) {
                vec3 vertexPos = H;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+1] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+1] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IL;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+2] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+2] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = IM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+3] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+3] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = M;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+4] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+4] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = L;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+5] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+5] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = IM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+6] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+6] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = L;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+7] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+7] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IL;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+8] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+8] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

            }
            else {
                vec3 vertexPos = IM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = H;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+1] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+1] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IL;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+2] = vec4(vertexPos,1.0);
                colour = vec3(1.0, 1.0, 1.0);
                color[idx+2] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = M;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+3] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+3] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+4] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+4] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = L;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+5] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+5] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

                vertexPos = L;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+6] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+6] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IM;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+7] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+7] = vec4(colour,1.0);
                EmitVertex();

                vertexPos = IL;
                gl_Position = m_pv * vec4(vertexPos, 1.0);
                vertex[idx+8] = vec4(vertexPos,1.0);
                colour = vec3(0.0, 0.0, 1.0);
                color[idx+8] = vec4(colour,1.0);
                EmitVertex();

                EndPrimitive();

            }
        }
            break;
        case 3: {
            vec3 vertexPos = A;
            gl_Position = m_pv * vec4(vertexPos, 1.0);
            vertex[idx] = vec4(vertexPos,1.0);
            colour = vec3(0.0, 0.0, 1.0);
            color[idx] = vec4(colour,1.0);
            EmitVertex();

            vertexPos = B;
            gl_Position = m_pv * vec4(vertexPos, 1.0);
            vertex[idx+1] = vec4(vertexPos,1.0);
            colour = vec3(0.0, 0.0, 1.0);
            color[idx+1] = vec4(colour,1.0);
            EmitVertex();

            vertexPos = C;
            gl_Position = m_pv * vec4(vertexPos, 1.0);
            vertex[idx+2] = vec4(vertexPos,1.0);
            colour = vec3(0.0, 0.0, 1.0);
            color[idx+2] = vec4(colour,1.0);
            EmitVertex();

            EndPrimitive();

            for(int i = 3; i < 9; i++){
                vertex[idx+i] = vec4(vec3(0.0),1.0);
                color[idx+i] = vec4(vec3(0.0),1.0);
            }

        }
            break;
        default: break;
    }
    
    
}