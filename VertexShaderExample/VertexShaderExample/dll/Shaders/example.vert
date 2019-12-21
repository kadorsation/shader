#version 430

layout(location = 0) in vec3 position;
layout(location = 0) in vec3 normal;
uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec3 Light;
uniform vec3 Viewer;
uniform float Ks;
uniform float Kd;
uniform float Ka;
uniform float alpha;
uniform vec3 la;
uniform vec3 ld;
uniform vec3 ls;



varying vec3 color;


void main() {
    gl_Position = Projection * ModelView * vec4(position, 1.0) ;
	mat3 G = transpose(inverse(mat3(ModelView)));
	vec3 i = normalize(mat3(ModelView) * (Light - position));
	vec3 n = normalize(G * normal);
	float c1 = max(dot(i, n), 0);
	vec3 v = normalize(mat3(ModelView) * (Viewer - position));
	vec3 h = normalize(i + v);
	float c2 = max(dot(n, h), 0);
    color = Ka * la + Kd * ld * c1 + Ks * ls * pow(c2, alpha);
}
