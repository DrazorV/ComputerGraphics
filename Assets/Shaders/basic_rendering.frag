#version 330 core
layout(location = 0) out vec4 out_color;

uniform vec3 uniform_diffuse;
uniform vec3 uniform_specular;
uniform float uniform_shininess;
uniform float uniform_has_texture;
uniform sampler2D diffuse_texture;

// Camera Properties
uniform vec3 uniform_camera_position;

// Light Source Properties
uniform mat4 uniform_light_projection_matrix;
uniform mat4 uniform_light_view_matrix;
uniform vec3 uniform_light_position;
uniform vec3 uniform_light_direction;
uniform vec3 uniform_light_color;
uniform float uniform_light_umbra;
uniform float uniform_light_penumbra;
uniform int uniform_cast_shadows;
uniform sampler2D shadowmap_texture;

float uniform_constant_bias = 0.0001;

in vec2 f_texcoord;
in vec3 f_position_wcs;
in vec3 f_normal;

#define PI 3.14159

float shadow_nearest(vec3 light_space_xyz)
{
	// sample shadow map
	float shadow_map_z = texture2D(shadowmap_texture, light_space_xyz.xy).r;

	// + shaded -> 0.0 
	// - lit -> 1.0
	return clamp(-sign((light_space_xyz.z - uniform_constant_bias) - shadow_map_z), 0.0, 1.0);
}

// 1 sample per pixel
float shadow(vec3 pwcs)
{
	vec4 plcs = uniform_light_projection_matrix * uniform_light_view_matrix * vec4(pwcs, 1.0);
	plcs /= plcs.w;
	plcs.xy = (plcs.xy + 1) * 0.5;
	
	// check that we are inside light clipping frustum
	//if (plcs.x < 0.0) return 0.0; if (plcs.y < 0.0) return 0.0; if (plcs.x > 1.0) return 0.0; if (plcs.y > 1.0) return 0.0;
	if ((clamp(plcs.xy, vec2(0,0), vec2(1,1)) - plcs.xy) != vec2(0,0)) return 0.0;

	// set scale of light space z vaule to [0, 1]
	plcs.z = 0.5 * plcs.z + 0.5;

	
	// sample shadow map
	return shadow_nearest(plcs.xyz);
}

float compute_spotlight(vec3 vertex_to_light_direction)
{
	float light_cosine_umbra = cos(radians(0.5 * uniform_light_umbra));
	float light_cosine_penumbra = cos(radians(0.5 * uniform_light_penumbra));
	float spoteffect = 1;
	float angle_vertex_spot_dir = dot(normalize(-vertex_to_light_direction), uniform_light_direction);

	if (angle_vertex_spot_dir > light_cosine_umbra) 
	{
		spoteffect = 1;
	}
	else if(angle_vertex_spot_dir > light_cosine_penumbra) 
	{
		spoteffect = smoothstep(light_cosine_penumbra, light_cosine_umbra, angle_vertex_spot_dir);
	}
	else 
		spoteffect = 0;
	
	return spoteffect;
}

void main(void) 
{
	vec3 normal = normalize(f_normal);
	
	vec4 diffuseColor = vec4(uniform_diffuse.rgb, 1);
	// if we provide a texture, multiply color with the color of the texture
	diffuseColor = mix( diffuseColor, diffuseColor * texture(diffuse_texture, f_texcoord), uniform_has_texture);
	
	// compute the direction to the light source
	vec3 vertex_to_light_direction = normalize(uniform_light_position - f_position_wcs.xyz);
	// compute the distance to the light source
	float dist = distance(uniform_light_position, f_position_wcs.xyz) + 0.000001; // add small offset to avoid division with zero
	
	// direction to the camera
	vec3 viewDirection = normalize(uniform_camera_position - f_position_wcs.xyz);	
	vec3 halfVector = normalize(viewDirection + vertex_to_light_direction);
	
	float NdotL = max(dot(vertex_to_light_direction, normal), 0.0);
	float NdotH = max(dot(halfVector, normal), 0.0);
	
	// check if we have shadows
	float shadow_value = (uniform_cast_shadows == 1)? shadow(f_position_wcs.xyz) : 1.0;
	// compute the spot effect
	float spotEffect = compute_spotlight(vertex_to_light_direction);
	//THIS IS THE SHADOW, IT IS REMOVED, PUT BACK LATER
	float shadow = shadow_value * spotEffect;
	// compute the incident radiance
	vec3 irradiance = uniform_light_color * NdotL / dist;

	vec3 diffuseReflection = irradiance * diffuseColor.rgb / PI;
	
	float specularNormalization = (uniform_shininess + 8) / (8 * PI);
	vec3 specularReflection = (NdotL > 0.0)? irradiance * specularNormalization * uniform_specular * pow( NdotH, uniform_shininess + 0.001) : vec3(0);
	
	out_color = vec4( diffuseReflection + specularReflection, 1.0);
}

