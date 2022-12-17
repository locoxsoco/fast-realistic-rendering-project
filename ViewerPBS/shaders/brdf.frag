#version 330 core
layout (location = 0) out vec4 frag_lightning;
layout (location = 1) out vec4 frag_color;
in vec2 TexCoords;
in vec3 eye_vertex;
in vec3 eye_normal;

// material parameters
uniform float metalness;
uniform float roughness;

// IBL and material textures
uniform samplerCube diffuse_map;
uniform samplerCube specular_map;
uniform sampler2D texture_color;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metalness;

// light
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 fresnel;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(vec3 F0, vec3 L, vec3 H)
{
    return F0 + (1.0 - F0) * pow(max(dot(L, H), 0.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{    
    float metalness_t = texture(texture_roughness, TexCoords).r;
    float roughness_t = texture(texture_metalness, TexCoords).r;

    // Light vector
    vec3 L = normalize(light_position-eye_vertex);
    // Normal vector
    vec3 N = normalize(eye_normal);
    // Eye view vector
    vec3 V = normalize(-eye_vertex);
    // Half-way vector
    vec3 H = normalize(V + L);

    // F0 Gold vec3(1.022,0.782,0.344)
    // F0 Silver vec3(0.972,0.960,0.915)
    vec3 F0 = fresnel;

    // reflectance equation
    vec3 Lo = vec3(0.0);

    /**** calculate light radiance ****/
    float distance = length(light_position - eye_vertex);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light_color * attenuation;

    // Diffuse term
    vec3 f_diff = texture(texture_color, TexCoords).rgb;

    // Specular term
    vec3 F = fresnelSchlick(F0,L,H);
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    //float NDF = DistributionGGX(N, H, roughness_t);
    //float G   = GeometrySmith(N, V, L, roughness_t);
    vec3 numerator    = NDF * G * F;
    // + 0.0001 to prevent divide by zero
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 f_spec = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD = kD * (1.0 - metalness);
    //kD = kD * (1.0 - metalness_t);

    Lo += (kD * f_diff / PI + f_spec) * radiance;
    /************************************/



    /**** Calculate Ambient Term using IBL ****/
    kS = fresnelSchlick(F0,L,H);
    kD = 1.0 - kS;
    kD = kD * (1.0 - metalness);
    //kD = kD * (1.0 - metalness_t);
    vec3 diffuse_irradiance = texture(diffuse_map, N).rgb;
    vec3 diffuse = diffuse_irradiance * texture(texture_color, TexCoords).rgb;
    vec3 specular_irradiance = texture(specular_map, N).rgb;
    vec3 specular = specular_irradiance * texture(texture_color, TexCoords).rgb;
    vec3 ambient = kD * diffuse + kS * specular;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    frag_color = vec4(color , 1.0);

    frag_lightning = vec4(1.f,1.f,1.f, 1.0);
}
