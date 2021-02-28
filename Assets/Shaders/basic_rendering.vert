#version 330 core
layout(location = 0) in vec3 coord3d;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 v_tangent;
layout(location = 4) in vec3 v_bitangent;

uniform mat4 uniform_model_matrix;
uniform mat4 uniform_normal_matrix;

uniform mat4 uniform_view_matrix;
uniform mat4 uniform_projection_matrix;

out vec2 f_texcoord;
out vec3 f_position_wcs;
out mat3 f_TBN;

void main(void) 
{
	f_TBN = mat3(
		normalize(vec3(uniform_normal_matrix * vec4(v_tangent, 0.0))),
		normalize(vec3(uniform_normal_matrix * vec4(v_bitangent, 0.0))),
		normalize(vec3(uniform_normal_matrix * vec4(v_normal, 0.0))));

	vec4 position_wcs = uniform_model_matrix * vec4(coord3d, 1.0);
	f_position_wcs = position_wcs.xyz;
	f_texcoord = texcoord;
	gl_Position = uniform_projection_matrix * uniform_view_matrix * position_wcs;
}
