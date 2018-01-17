#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out float gReflection;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 ourNormal;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D height;
    sampler2D normal;
    float shininess;
};
uniform Material material;
uniform bool refraction;
void main()
{
	//if(texture(material.diffuse, TexCoord).a <= 0.0)
		//discard;
    // store the fragment position vector in the first gbuffer texture
    gPosition.rgb = FragPos;
	// store shininess
	gPosition.a = material.shininess;
    // also store the per-fragment normals into the gbuffer
    gNormal.rgb = normalize(ourNormal);
	// store refraction
	gNormal.a = refraction ? 1.0 : 0.0;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.diffuse, TexCoord).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.specular, TexCoord).r;
    // store reflection intensity in gReflection
    gReflection = texture(material.height, TexCoord).r;
}
