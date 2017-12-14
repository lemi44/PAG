#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
out vec3 ourNormal;
out vec2 TexCoord;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(position, 1.0f);
    ourNormal = normal;
    TexCoord = texCoord;
}