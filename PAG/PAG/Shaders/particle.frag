#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
in vec4 FragPos;
out vec4 color;

uniform sampler2D sprite;
uniform sampler2D gPosition;

const float ContrastPower = 4.0;

void main()
{
	float Depth = texture(gPosition, TexCoords).z;
	if(Depth < FragPos.z)
		discard;
	float Input = Depth - FragPos.z;
    color = (texture(sprite, TexCoords) * ParticleColor);
	
	float Output = 0.5*pow(clamp(2*(( Input > 0.5) ? 1-Input : Input), 0.0, 1.0), ContrastPower);
	Output = ( Input > 0.5) ? 1-Output : Output;
	color.a = color.a*Output;
	if (color.a <= 0.0)
		discard;
}
