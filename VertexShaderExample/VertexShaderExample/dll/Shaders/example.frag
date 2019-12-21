#version 430
out vec4 frag_color;
varying vec3 color;

void main() {
    frag_color = vec4(color.x, color.y, color.z, 1);
}
