#version 330 core
in  vec2 vUV;
out vec4 fColor;
uniform vec2 C;

const int MAX_ITER = 85;

vec2 complex_sqr(vec2 p) {
    return vec2(p.x * p.x - p.y * p.y, 2 * p.x * p.y);
}

void main() {
    vec2 c = vUV;
    fColor = vec4(c.x, c.y, 1.f, 1.f);
    int ret = MAX_ITER;
    for(int i = 0; i < MAX_ITER; i++) {
        if(ret == MAX_ITER) {
            c = complex_sqr(c) + C;
            if(c.x * c.x + c.y * c.y >= 2) {
                ret = i;
            }
        }
    }
    float color = 1.0f / (MAX_ITER - 1) * ret;
    fColor = vec4(color, color, color, 1.f);
}