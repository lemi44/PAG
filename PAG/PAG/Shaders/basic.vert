#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
out vec3 FragPos;
out vec3 ourNormal;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;
void main()
{
	vec4 viewPos = view * model * vec4(position, 1.0);
    FragPos = viewPos.xyz;
    ourNormal = normalMat * normal;
    TexCoord = texCoord;
    
    gl_Position = projection * viewPos;
}
