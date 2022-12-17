#version 330 core
out vec4 frag_color;

in vec2 TexCoords;
in vec2 Vertex;

uniform sampler2D texture_ssao_normal;
uniform sampler2D texture_ssao_depth;
uniform sampler2D texture_ssao_random;
uniform vec2 ssao_samples[64];

uniform int ssao_n_samples;
uniform float ssao_radius;
uniform float ssao_sigma; //intensity_scale
uniform float ssao_k; //contrast
uniform float ssao_beta; //bias_distance
uniform float ssao_epsilon;
uniform int ssao_render_mode;

float twopi = 6.28318384f;

void main()
{
    float n = 0.3f; // prob is variable
    float f = 2; // prob is variable
    float height = 600;
    float width = 600;

    vec3 normal = texture(texture_ssao_normal, TexCoords).rgb;
    float depth = texture(texture_ssao_depth, TexCoords).r;
    float random = texture(texture_ssao_random, TexCoords).r;


    float z_ndc = 2.0 * depth - 1.0;
    float z_eye = 2.0 * n * f / (f + n - z_ndc * (f - n));
    float z_near = n;

    //AB=z_near; DE=z_eye; BCx=ssao_sample_x; EFx=BCx*DE/AB
    //AB=z_near; DE=z_eye; BCy=ssao_sample_ y; EFy=BCy*DE/AB
    float sample_obscurance_sum = 0;
    for(int i=0;i<ssao_n_samples;i++){
        float ssao_sample_x = ssao_samples[i].x * cos(twopi*random) - ssao_samples[i].y * sin(twopi*random);
        float ssao_sample_y = ssao_samples[i].x * sin(twopi*random) - ssao_samples[i].y * cos(twopi*random);
        vec2 ssao_sample = normalize(vec2(ssao_sample_x,ssao_sample_y))*ssao_radius;
        ssao_sample_x = ssao_sample.x;
        ssao_sample_y = ssao_sample.y;

        /*float ssao_sample_x  = ssao_samples[i].x * ssao_radius;
        float ssao_sample_y  = ssao_samples[i].y * ssao_radius;*/

        float EFx = ssao_sample_x * z_eye / z_near;
        float EFy = ssao_sample_y * z_eye / z_near;

        float Fx = Vertex.x + EFx;
        float Fy = Vertex.y + EFy;
        if(Fx > 1.f || Fx < 0.f || Fy > 1.f || Fy < 0.f)
            continue;
        float depth_2 = texture(texture_ssao_depth, vec2(Fx,Fy)).r;
        float Fz = 2.0 * depth_2 - 1.0;
        Fz = 2.0 * n * f / (f + n - Fz * (f - n));

        vec3 sample_vector = vec3(EFx, EFy, -(Fz-z_eye));
        sample_obscurance_sum += max(0, dot(sample_vector,normal) - z_eye*ssao_beta)/(dot(sample_vector,sample_vector)+ssao_epsilon);
    }

    float sigma = ssao_sigma * ssao_radius*twopi;
    sample_obscurance_sum = pow(max(0,1-(2*sigma/ssao_n_samples)*sample_obscurance_sum),ssao_k);

    frag_color = vec4(sample_obscurance_sum,sample_obscurance_sum,sample_obscurance_sum, 1.0);

}
