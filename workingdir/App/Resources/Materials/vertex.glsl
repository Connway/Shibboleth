#version 420

layout(binding = 0) uniform MatrixBuffer
{
	mat4 world_matrix;
	mat4 view_matrix;
	mat4 proj_matrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;
layout(location = 5) in uvec4 blend_indices;
layout(location = 6) in vec4 blend_weights;

out vec2 tex_coord;
//out vec3 nrm;
out vec3 pos;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	pos = (view_matrix * world_matrix * vec4(position, 1.0)).xyz;
	gl_Position = proj_matrix * vec4(pos, 1.0);
	tex_coord = uv;
	//nrm = (view_matrix * world_matrix * vec4(normal, 0.0)).xyz;
}
