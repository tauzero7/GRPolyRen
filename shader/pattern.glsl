
uniform vec2 patFreq;

const float PI = 3.14159265;

const vec3 col_red_1 = vec3(1.0, 0.7, 0.7);
const vec3 col_red_2 = vec3(0.8, 0.2, 0.0);
const vec3 col_blue_1 = vec3(0.0, 0.2, 0.8);
const vec3 col_blue_2 = vec3(0.6, 0.6, 0.9);

vec3 HSVtoRGB(vec3 hsv) {
    float F, M, N, K;
    int   I;

    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    vec3 rgb = vec3(0.0);

    if (s == 0.0) {
        rgb = vec3(v);
    }
    else {
        if (h >= 1.0) {
            h = 0.0;
        }
        else {
            h = h * 6;
        }

        I = int(h);    // should be in the range 0..5
        F = h - I;     // fractional part

        M = v * (1 - s);
        N = v * (1 - s * F);
        K = v * (1 - s * (1.0 - F));

        if (I == 0) {rgb.r = v; rgb.g = K; rgb.b = M; }
        if (I == 1) {rgb.r = N; rgb.g = v; rgb.b = M; }
        if (I == 2) {rgb.r = M; rgb.g = v; rgb.b = K; }
        if (I == 3) {rgb.r = M; rgb.g = N; rgb.b = v; }
        if (I == 4) {rgb.r = K; rgb.g = M; rgb.b = v; }
        if (I == 5) {rgb.r = v; rgb.g = M; rgb.b = N; }
    }
    return rgb;
}

float checkerboard(vec2 tc) {
    vec4 bgcol = vec4(tc,0,1);

    float xpatFreq = patFreq.x;
    float ypatFreq = patFreq.y;
    float baseVal = 0.5;
    float stepVal = 0.5;
    
    bool xpat = (mod(tc.x * xpatFreq * 0.5, 1.0) < 0.5);
    bool ypat = (mod(tc.y * ypatFreq * 0.5, 1.0) < 0.5);

    float val = (xpat ^^ ypat) ? baseVal - stepVal : baseVal + stepVal;
    return val;
}

vec3 checkered_disk(vec2 texCoords) {
    vec2 tc = texCoords - vec2(0.25);

    if (tc.y > 0.25) tc.y -= 0.5;
    float r = length(tc);
    float phi = atan(tc.y, tc.x);
    float val = checkerboard(vec2(r, phi / PI));

    vec3 color1 = col_red_1;
    vec3 color2 = col_red_2;

    if (texCoords.y > 0.5) {
        color1 = col_blue_1;
        color2 = col_blue_2;
    }

    vec3 col = mix(color1, color2, val);

    if (tc.x > 0.25) {
        col = vec3(0.2,0.3,0.2);
    }
    return col;
}

vec3 checkered_sphere(vec2 texCoords) {
    vec2 tc = texCoords * 1.0;
    float val = checkerboard(tc);
    return mix(col_red_1, col_red_2, val);
}

vec3 color_checkered_sphere(vec2 texCoords) {
    vec2 tc = texCoords * 1.0;
    float val = checkerboard(tc);
    float fac = 0.5;
    vec3 col = HSVtoRGB(vec3(tc, 1.0 - fac + fac * val));
    return col;
}

vec3 checkered_plane(vec2 texCoords) {
    float val = checkerboard(texCoords);
    return mix(col_red_1, col_red_2, val);
}
