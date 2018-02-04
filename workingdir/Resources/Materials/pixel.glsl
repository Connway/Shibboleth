#version 420

in vec2 tex_coord;
//in vec3 nrm;
// in vec3 pos;

layout(location = 0) out vec4 diffuse;
// layout(location = 1) out vec4 specular;
// layout(location = 2) out vec4 normal;
// layout(location = 3) out vec4 position;

layout(binding = 0) uniform sampler2D tex;

void main()
{
	//diffuse = vec4(1.0, 0.0, 0.0, 1.0);
	diffuse = texture(tex, tex_coord);
}
