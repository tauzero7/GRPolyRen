
const float SCHW_PI = 3.1415926;
const float rs = 2.0;

uniform sampler2D lutTex0;
uniform sampler2D lutTex1;
uniform float xmin;
uniform float xscale;

/**
 * Read distance and angle from lookup table
 * @param iorder  order of light ray (0,1)
 * @param r  actual position (radius)
 * @param phi  actual position (azimuth angle)
 * @param dist  apparent distance (light-travel-time)
 * @param ksi   apparent viewing angle
 */
void lookupCoords(in float iorder, in float r, in float phi, out float dist, out float ksi) {
    float x = rs / r;
    float t = (x - xmin) * xscale;

    float s = phi / SCHW_PI;

    vec2 npos_first = texture(lutTex0, vec2(s, t)).xy;
    vec2 npos_second = texture(lutTex1, vec2(s, t)).xy;

    vec2 npos = mix(npos_first, npos_second, iorder);
    dist = npos.y;
    ksi = npos.x;
}

/**
 * Calculate apparent position of q wrt p
 * @param p  main point
 * @param q  view point
 * @param iorder  order of light ray (0,1)
 * @param distScale  Scale factor for distance
 * @return apparent position in pseudo-cartesian coordinates
 */
vec3 calcApparentPos(vec3 p, vec3 q, float iorder, float distScale = 1.0) {
    vec3 e1 = normalize(p);
    vec3 n = cross(e1, q);
    vec3 e2 = normalize(cross(n, e1));

    float x = dot(q, e1);
    float y = dot(q, e2);
    float r = sqrt(x*x + y*y);
    float phi = atan(y, x);

    float dist = r;
    float ksi = phi;
    lookupCoords(iorder, r, phi, dist, ksi);

    float s = 2.0 * (0.5 - iorder);

    return distScale * dist * (cos(ksi) * e1 + s * sin(ksi) * e2) + p;
}

/**
 *
 */
void lookupDirsAndDist(in float iorder, in float r, in float phi, out float ux, out float uy, out float dist) {
    float x = rs / r;
    float t = (x - xmin) * xscale;

    float s = phi / SCHW_PI;

    vec3 ndir_first = texture(lutTex0, vec2(s, t)).zwy;
    vec3 ndir_second = texture(lutTex1, vec2(s, t)).zwy;
    
    vec2 ndir = mix(ndir_first.xy, ndir_second.xy, iorder);
    ux = ndir.x;
    uy = ndir.y;

    dist = mix(ndir_first.z, ndir_second.z, iorder);
}


/**
 *
 */
void calcApparentDirAndDist(vec3 p, vec3 q, float iorder, out vec3 dir, out float dist) {
    vec3 e1 = normalize(p);
    vec3 n = cross(e1, q);
    vec3 e2 = normalize(cross(n, e1));

    float x = dot(q, e1);
    float y = dot(q, e2);
    float r = sqrt(x*x + y*y);
    float phi = atan(y, x);

    float ux, uy;
    lookupDirsAndDist(iorder, r, phi, ux, uy, dist);
    float s = 2.0 * (0.5 - iorder);

    dir = -(ux * e1 + s * uy * e2);
}
