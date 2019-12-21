#version 430
out vec4 frag_color;
uniform mat4 ModelView;
varying vec3 position_v;

void main() {
    frag_color = vec4(position_v, 1);
}
