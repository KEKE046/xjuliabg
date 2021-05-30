#version 330 core
in  lowp vec2 vUV;
out lowp vec4 fColor;
uniform lowp vec2 C;


lowp vec2 cmul(in vec2 a, in vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
lowp vec2 cdiv(in vec2 a, in vec2 b) {
    return vec2(a.x * b.x + a.y * b.y, -a.x * b.y + a.y * b.x) / dot(b, b);
}
lowp vec2 cpow2(in vec2 a) {return cmul(a, a);}
lowp vec2 cpow3(in vec2 a) {return cmul(cpow2(a), a);}
lowp vec2 cpow4(in vec2 a) {return cpow2(cpow2(a));}
lowp vec2 cpow5(in vec2 a) {return cmul(cpow4(a), a);}


lowp vec2 next_value(vec2 p) {
    return cpow2(p) + C;
}


void main() {
    lowp vec2 c = vUV;
    const int MAX_ITER = 56;
    const lowp float COLOR_SCLAE = 1.0f / (MAX_ITER - 1);
    int ret = MAX_ITER;
    for(int i = 0; i < MAX_ITER; i++) {
        if(ret == MAX_ITER) {
            if(c.x * c.x + c.y * c.y >= 4) {
                ret = i;
            }
            c = next_value(c);
        }
    }
    lowp float color = COLOR_SCLAE * ret;
    fColor = vec4(color, color, color, 1.f);
}