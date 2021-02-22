#version 330 core
layout(location = 0) out vec4 out_color;

in vec2 f_texcoord;
in vec3 f_position_wcs;
in mat3 f_TBN;

#define _PI_ 3.14159

uniform vec3 uniform_camera_pos;
uniform vec3 uniform_camera_dir;

uniform vec3 uniform_diffuse;
uniform vec3 uniform_specular;
uniform vec3 uniform_ambient;
uniform float uniform_shininess;

uniform int uniform_has_tex_diffuse;
uniform int uniform_has_tex_normal;
uniform int uniform_is_tex_bumb;
uniform sampler2D uniform_tex_diffuse;
uniform sampler2D uniform_tex_normal;

uniform vec3 uniform_light_color;
uniform vec3 uniform_light_pos;
uniform vec3 uniform_light_dir;

uniform float uniform_light_umbra;
uniform float uniform_light_penumbra;

uniform mat4 uniform_light_projection_view;
uniform int uniform_cast_shadows;
uniform float uniform_constant_bias = 0.0002;

uniform sampler2D uniform_shadow_map;

float compute_spotlight(const in vec3 pSurfToLight)
{
	float cos_umbra = cos(radians(0.5 * uniform_light_umbra));
	float cos_penumbra = cos(radians(0.5 * uniform_light_penumbra));
	float spoteffect = 1;
	float angle_vertex_spot_dir = dot(-pSurfToLight, uniform_light_dir);

	if (angle_vertex_spot_dir > cos_umbra) 
	{
		spoteffect = 1;
	}
	else if(angle_vertex_spot_dir >= cos_penumbra) 
	{
		spoteffect = smoothstep(cos_penumbra, cos_umbra, angle_vertex_spot_dir);
	}
	else
	{
		spoteffect = 0;
	}
	
	return spoteffect;
}

float shadow_pcf2x2_weighted(vec3 light_space_xyz)
{
	ivec2 shadow_map_size = textureSize(uniform_shadow_map, 0);
	float xOffset = 1.0 / shadow_map_size.x;
    float yOffset = 1.0 / shadow_map_size.y;

	// compute the weights of the neighboring pixels
	vec2 uv = light_space_xyz.xy - vec2(xOffset, yOffset);
	float u_ratio = mod(uv.x, xOffset) / xOffset;
	float v_ratio = mod(uv.y, yOffset) / yOffset;
	float u_opposite = 1 - u_ratio;
	float v_opposite = 1 - v_ratio;

	// compute the distance with a small bias
	float z = light_space_xyz.z - uniform_constant_bias;

	// compute the shadow percentage
	float bottomLeft = (texture(uniform_shadow_map, uv).r > z) ? u_opposite : 0.0;
	float bottomRight = (texture(uniform_shadow_map, uv + vec2(xOffset, 0)).r > z) ? u_ratio : 0.0; 
	float topLeft = (texture(uniform_shadow_map, uv + vec2(0, yOffset), 0).r > z) ? u_opposite : 0.0;
	float topRight = texture(uniform_shadow_map, uv + vec2(xOffset, yOffset)).r > z ? u_ratio : 0.0;
	float factor = (bottomLeft + bottomRight) * v_opposite + (topLeft + topRight) * v_ratio;

    return factor;
}

float shadow_pcf2x2_mean(vec3 light_space_xyz)
{
	ivec2 shadow_map_size = textureSize(uniform_shadow_map, 0);
	float xOffset = 1.0 / shadow_map_size.x;
    float yOffset = 1.0 / shadow_map_size.y;
	vec2 texelSize = vec2(xOffset, yOffset);

	vec2 uv_bl = light_space_xyz.xy + vec2(-1.0, -1.0) * texelSize;
	vec2 uv_br = light_space_xyz.xy + vec2(0, -1.0) * texelSize;
	vec2 uv_tl = light_space_xyz.xy + vec2(-1.0, 0) * texelSize;
	vec2 uv_tr = light_space_xyz.xy + vec2(0, 0) * texelSize;

	float z = light_space_xyz.z - uniform_constant_bias;

	// compute the shadow percentage
	float bottomLeft = (texture(uniform_shadow_map, uv_bl).r > z) ? 1.0 : 0.0;
	float bottomRight = (texture(uniform_shadow_map, uv_br).r > z) ? 1.0 : 0.0;
	float topLeft = (texture(uniform_shadow_map, uv_tl).r > z) ? 1.0 : 0.0;
	float topRight = texture(uniform_shadow_map, uv_tr).r > z ? 1.0 : 0.0;
	float factor = (bottomLeft + bottomRight + topLeft + topRight) / 4.0;

    return factor;
}

float shadow_nearest(vec3 light_space_xyz)
{
	// sample shadow map
	float shadow_map_z = texture2D(uniform_shadow_map, light_space_xyz.xy).r;

	// + shaded -> 0.0 
	// - lit -> 1.0
	// < closer to the light
	return ((light_space_xyz.z - uniform_constant_bias) < shadow_map_z) ? 1.0 : 0.0;
}

// 1 sample per pixel
float shadow(vec3 pwcs)
{
	// project the pwcs to the light source point of view
	vec4 plcs = uniform_light_projection_view * vec4(pwcs, 1.0);
	// perspective division
	plcs /= plcs.w;
	// convert from [-1 1] to [0 1]
	plcs.xy = (plcs.xy + 1) * 0.5;

	// check that we are inside light clipping frustum
	if (plcs.x < 0.0) return 0.0;
	if (plcs.y < 0.0) return 0.0;
	if (plcs.x > 1.0) return 0.0;
	if (plcs.y > 1.0) return 0.0;

	// set scale of light space z vaule to [0, 1]
	plcs.z = 0.5 * plcs.z + 0.5;

	// sample shadow map
	//return shadow_nearest(plcs.xyz);
	return shadow_pcf2x2_weighted(plcs.xyz);
	//return shadow_pcf2x2_mean(plcs.xyz);
}

vec3 blinn_phong(const in vec3 pSurfToEye, const in vec3 pSurfToLight)
{
	vec3 normal = f_TBN[2];

	if(uniform_has_tex_normal == 1)
	{
		vec3 nmap = texture(uniform_tex_normal, f_texcoord).rgb;

		if(uniform_is_tex_bumb == 1)
		{
			float heigh_prev_U = textureOffset(uniform_tex_normal, f_texcoord, ivec2(-1, 0)).r;
			float heigh_prev_V = textureOffset(uniform_tex_normal, f_texcoord, ivec2(0, -1)).r;
			normal = normal - f_TBN[0] * (nmap.r - heigh_prev_U) - f_TBN[1] * (nmap.r - heigh_prev_V);
		}
		else
		{
			nmap = nmap * 2.0 - 1.0;
			normal = normalize(f_TBN * nmap);
		}
	}

	vec3 halfVector = normalize(pSurfToEye + pSurfToLight);

	float NdotL = max(dot(normal, pSurfToLight), 0.0);
	float NdotH = max(dot(normal, halfVector), 0.0);

	vec3 albedo = uniform_has_tex_diffuse == 1 ? texture(uniform_tex_diffuse, f_texcoord).rgb : uniform_diffuse;

	vec3 kd = albedo / _PI_;
	vec3 ks = uniform_specular;

	float fn =
		((uniform_shininess + 2) * (uniform_shininess + 4)) /
		(8 * _PI_ * (uniform_shininess + 1.0 / pow(2, uniform_shininess / 2.0)));

	vec3 diffuse = kd * NdotL;
	vec3 specular = NdotL > 0.0 ? ks * fn * pow(NdotH, uniform_shininess) : vec3(0.0);

	return (diffuse + specular) * uniform_light_color + uniform_ambient;
}

void main(void)
{
	vec3 surfToEye = normalize(uniform_camera_pos - f_position_wcs);
	vec3 surfToLight = normalize(uniform_light_pos - f_position_wcs);

	// check if we have shadows
	float shadow_value = (uniform_cast_shadows == 1) ? shadow(f_position_wcs) : 1.0;

	vec3 brdf = blinn_phong(surfToEye, surfToLight);
	float spotEffect =  compute_spotlight(surfToLight);
	float dist = distance(uniform_light_pos, f_position_wcs);

	out_color = vec4(shadow_value * brdf * spotEffect / pow(dist, 2), 1.0);
}