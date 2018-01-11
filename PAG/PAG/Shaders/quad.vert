#version 330 core
const vec2 quadVertices[6] = vec2[]( vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0) );
const vec2 quadTexCoords[6] = vec2[]( vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0) );
out vec2 TexCoords;
void main()
{
    gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
	TexCoords = quadTexCoords[gl_VertexID];
}