#version 330
layout(location = 0) out vec4 out_color;

uniform sampler2D uniform_texture;
uniform float uniform_time;

in vec2 f_texcoord;

void main(void)
{
	vec3 color = texture2D(uniform_texture, f_texcoord).rgb;
	vec2 uv = f_texcoord;
	vec2 pos = uv - vec2(0.5); 
	float dist = sqrt(pos.x * pos.x + pos.y * pos.y); 
	if (dist < 0.005) color = vec3(1.0);
	if (dist < 0.004) color = vec3(0.0);


	out_color = vec4(color, 1.0);
}