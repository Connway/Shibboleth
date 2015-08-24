#version 420

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;

out vec2 tex_coord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(position, 1.0);
	tex_coord = uv;
}
