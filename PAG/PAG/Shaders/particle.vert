#version 330 core
layout (location = 0) in vec4 vertex_tex;

out vec2 TexCoords;
out vec4 ParticleColor;
out vec4 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 2.0f;
    TexCoords = vertex_tex.zw;
    ParticleColor = color;
	vec3 pos = offset + camera_right*vertex_tex.x*scale + camera_up * vertex_tex.y * scale;
	FragPos = view * vec4(pos, 1.0);
    gl_Position = projection * FragPos;
}
