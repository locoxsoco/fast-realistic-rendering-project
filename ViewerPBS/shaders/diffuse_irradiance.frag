#version 330

smooth in vec3 world_vertex;

uniform samplerCube environment_map;

out vec4 frag_color;

const float PI = 3.14159265359;

void main (void) {
    vec3 N = normalize(world_vertex);

    vec3 irradiance = vec3(0.0);

    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    float sample_delta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sample_delta) {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sample_delta) {
            // spherical to cartesian (in tangent space)
            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * N;
            // Obtaining irradiance from sample
            irradiance += texture(environment_map, sample_vec).rgb * cos(theta) * sin(theta);
            nrSamples = nrSamples + 1.0 ;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    frag_color = vec4(irradiance, 1.0);
}
