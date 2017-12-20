#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
out vec3 FragPos;
out vec3 ourNormal;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 wvp;
uniform mat3 normalMat;
void main()
{
    FragPos = vec3(model * vec4(position, 1.0));
    ourNormal = normalMat * normal;
    TexCoord = texCoord;
    
    gl_Position = wvp * vec4(FragPos, 1.0);
}
