#version 330 core
in  vec2 vUV;
out vec4 fColor;
uniform vec2 C;


void main() {
    vec2 c = vUV;
    const int MAX_ITER = 56;
    const float COLOR_SCLAE = 1.0f / (MAX_ITER - 1);
    int ret = MAX_ITER;
    for(int i = 0; i < MAX_ITER; i++) {
        if(ret == MAX_ITER) {
            vec2 sqr = vec2(c.x * c.x, c.y * c.y);
            c = vec2(sqr.x - sqr.y, c.x * c.y * 2) + C;
            if(sqr.x + sqr.y >= 2) {
                ret = i;
            }
        }
    }
    float color = COLOR_SCLAE * ret;
    fColor = vec4(color, color, color, 1.f);
}