#version 330 core
out vec4 frag_color;

in vec2 TexCoords;
in vec2 Vertex;

uniform sampler2D texture_ssao_albedo;
uniform sampler2D texture_ssao_normal;
uniform sampler2D texture_ssao_depth;
uniform sampler2D texture_ssao_ssao;
uniform int ssao_render_mode;

float twopi = 6.28318384f;

float distance(float x, float y, float i, float j) {
    return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

float gaussian(float x, float sigma) {
    return exp(-(pow(x, 2))/(2 * pow(sigma, 2))) / (2 * twopi * pow(sigma, 2));
}

void main()
{
    int diameter = 7;
    float iFiltered = 0;
    float wP = 0;
    float neighbor_x = 0;
    float neighbor_y = 0;
    int halfF = diameter / 2;
    vec2 texelSize = 1.0 / vec2(textureSize(texture_ssao_ssao, 0));

    for(int i = 0; i < diameter; i++) {
        for(int j = 0; j < diameter; j++) {
            neighbor_x = TexCoords.x - (halfF - i)*texelSize.x;
            neighbor_y = TexCoords.y - (halfF - j)*texelSize.y;
            float gi = gaussian(texture(texture_ssao_ssao, vec2(neighbor_x,neighbor_y)).r - texture(texture_ssao_ssao, TexCoords).r, 0.12f);
            float gs = gaussian(distance(TexCoords.x, TexCoords.y, neighbor_x, neighbor_y), 16.f);
            float w = gi * gs;
            iFiltered = iFiltered + texture(texture_ssao_ssao, vec2(neighbor_x,neighbor_y)).r * w;
            wP = wP + w;
        }
    }
    iFiltered = iFiltered / wP;
    frag_color = vec4(iFiltered,iFiltered,iFiltered, 1.0);



    /*vec2 texelSize = 1.0 / vec2(textureSize(texture_ssao_ssao, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(texture_ssao_ssao, TexCoords + offset).r;
        }
    }
    result = result / (4.0 * 4.0);


    frag_color = vec4(result,result,result, 1.0);*/

}
