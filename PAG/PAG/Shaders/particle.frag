#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;
//uniform sampler2D gPosition;


void main()
{
	//float Input = gPosition.z;
    color = (texture(sprite, TexCoords) * ParticleColor);
	if (color.a <= 0.0)
		discard;
	//float Output = 0.5*pow(saturate(2*(( Input > 0.5) ? 1-Input : Input)), ContrastPower);
	//Output = ( Input > 0.5) ? 1-Output : Output;
}
