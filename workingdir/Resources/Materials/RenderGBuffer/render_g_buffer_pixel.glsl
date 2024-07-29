#version 420

in vec2 tex_coord;

layout(location = 0) out vec4 diffuse;

layout(binding = 0) uniform sampler2D tex;

void main()
{
	diffuse = texture(tex, tex_coord);
}
