#version 330 core
out vec4 frag_color;

in vec2 TexCoords;
in vec2 Vertex;

uniform sampler2D texture_ssao_albedo;
uniform sampler2D texture_ssao_normal;
uniform sampler2D texture_ssao_depth;
uniform sampler2D texture_ssao_ssao;
uniform sampler2D texture_ssao_ssao_blur;
uniform sampler2D texture_ssao_lightning;
uniform int ssao_render_mode;

void main()
{

    vec3 col = texture(texture_ssao_albedo, TexCoords).rgb;
    vec3 normal = texture(texture_ssao_normal, TexCoords).rgb;
    float depth = texture(texture_ssao_depth, TexCoords).r;
    float ssao = texture(texture_ssao_ssao, TexCoords).r;
    float ssao_blur = texture(texture_ssao_ssao_blur, TexCoords).r;
    vec3 lighting = texture(texture_ssao_lightning, TexCoords).rgb;

    if(ssao_render_mode == 0){ // Normal
        frag_color = vec4(normal, 1.0);
    } else if(ssao_render_mode == 1){ // Albedo
        frag_color = vec4(col, 1.0);
    }
    else if(ssao_render_mode == 2){ // Depth
        frag_color = vec4(depth,depth,depth, 1.0);
    }
    else if(ssao_render_mode == 3){ // SSAO
        frag_color = vec4(ssao,ssao,ssao, 1.0);
    }
    else if(ssao_render_mode == 4){ // SSAO Blur
        frag_color = vec4(ssao_blur,ssao_blur,ssao_blur, 1.0);
    }
    else if(ssao_render_mode == 5){ // SSAO Blur + lighting
        frag_color = vec4(col, 1.0)*vec4(lighting, 1.0)*vec4(ssao_blur,ssao_blur,ssao_blur, 1.0);
    }
}
