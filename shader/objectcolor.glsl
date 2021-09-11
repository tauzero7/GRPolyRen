
#define OBJ_TEXTURE_NONE      0
#define OBJ_TEXTURE_DISK      1
#define OBJ_TEXTURE_SPHERE    2
#define OBJ_TEXTURE_COLSPHERE 3
#define OBJ_TEXTURE_TRIANGLE  4

uniform int obj_texture;

vec4 objectcolor(vec2 tc, vec3 normal) {
    vec4 color = vec4(tc, 0.0, 1.0);
    
    if (obj_texture == OBJ_TEXTURE_DISK) {
        color = vec4(checkered_disk(tc), 1.0);
    }
    else if (obj_texture == OBJ_TEXTURE_SPHERE) {
        color = vec4(checkered_sphere(tc), 1.0);
    }
    else if (obj_texture == OBJ_TEXTURE_COLSPHERE) {
        color = vec4(color_checkered_sphere(tc), 1.0);
    }
    else if (obj_texture == OBJ_TEXTURE_TRIANGLE) {
        color = vec4(checkered_plane(tc + vec2(0.1)), 1.0);
    }
    
    //color.rgb = normal*0.5 + vec3(0.5);    
    //color.rgb = (normal + vec3(1) * checkered_plane(tc + vec2(0.1)));
    
    return color;
}
