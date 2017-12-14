#version 330 core
in vec3 ourNormal;
in vec2 TexCoord;
out vec4 color;
struct Material
{
sampler2D texture_diffuse1;
sampler2D texture_diffuse2;
sampler2D texture_diffuse3;
sampler2D texture_specular1;
sampler2D texture_specular2;
};
uniform Material material;
float cutoff = 0.1;
void main()
{
color = texture(material.texture_diffuse1, TexCoord) * vec4(1.0f);
if (color.a < cutoff)
        // alpha value less than user-specified threshold?
    {
        discard; // yes: discard this fragment
    }
}