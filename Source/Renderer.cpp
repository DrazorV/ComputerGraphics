#include "Renderer.h"
#include "GeometryNode.h"
#include "Tools.h"
#include <algorithm>
#include "ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OBJLoader.h"

// RENDERER
Renderer::Renderer() 
{
	m_corridors_geometry = new GeometryNode * [4];
	for (int i = 0; i < 4; i++) { 
		m_corridors_geometry[i] = nullptr;
	}
	m_wall_geometry = new GeometryNode * [2];
	for(int i = 0; i<2; i++){
		m_wall_geometry[i] = nullptr;
	}
	m_wall_transformation_matrix = new glm::mat4[2];
	m_wall_transformation_normal_matrix = new glm::mat4[2];

	m_corridors_transformation_matrix = new glm::mat4[4];
	m_corridors_transformation_normal_matrix = new glm::mat4[4];

	m_cannon_geometry = nullptr;
	

	m_vbo_fbo_vertices = 0;
	m_vao_fbo = 0;
	m_cloister_geometry = nullptr;
	m_rock_geometry = nullptr;

	m_fbo = 0;
	m_fbo_texture = 0;

	m_rendering_mode = RENDERING_MODE::TRIANGLES;
	m_continous_time = 0.0;
	m_camera_position = glm::vec3(1, 3, -6);
	m_camera_target_position = glm::vec3(0, 0, 0);
	m_camera_up_vector = glm::vec3(0, 1, 0);
}

Renderer::~Renderer()
{
	// delete g_buffer
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteFramebuffers(1, &m_fbo);
	delete m_cannon_geometry;

	// delete common data
	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);

	delete m_rock_geometry;
	delete m_cloister_geometry;

	for (int i = 0; i < 4; i++) {
		delete m_corridors_geometry[i];
	}

	delete m_corridors_geometry;

	for (int i = 0; i < 2; i++) {
		delete m_wall_geometry[i];
	}

	delete m_wall_geometry;
}

bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	// Initialize OpenGL functions

	//Set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//This enables depth and stencil testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	// glClearDepth sets the value the depth buffer is set at, when we clear it

	// Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// open the viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //we set up our viewport

	bool techniques_initialization = InitRenderingTechniques();
	bool buffers_initialization = InitIntermediateShaderBuffers();
	bool items_initialization = InitCommonItems();
	bool lights_sources_initialization = InitLightSources();
	bool meshes_initialization = InitGeometricMeshes();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	//If everything initialized
	return techniques_initialization && items_initialization && buffers_initialization;
}

void Renderer::Update(float dt)
{
	float movement_speed = 2.0f;
	glm::vec3 direction = glm::normalize(m_camera_target_position - m_camera_position);

	m_camera_position += m_camera_movement.x *  movement_speed * direction * dt;
	m_camera_target_position += m_camera_movement.x * movement_speed * direction * dt;

	glm::vec3 right = glm::normalize(glm::cross(direction, m_camera_up_vector));
	m_camera_position += m_camera_movement.y *  movement_speed * right * dt;
	m_camera_target_position += m_camera_movement.y * movement_speed * right * dt;

	glm::mat4 rotation = glm::mat4(1.0f);
	float angular_speed = glm::pi<float>() * 0.0025f;
	
	rotation *= glm::rotate(glm::mat4(1.0), m_camera_look_angle_destination.y * angular_speed, right);
	rotation *= glm::rotate(glm::mat4(1.0), -m_camera_look_angle_destination.x  * angular_speed, m_camera_up_vector);
	m_camera_look_angle_destination = glm::vec2(0);
	
	direction = rotation * glm::vec4(direction, 0);
	float dist = glm::distance(m_camera_position, m_camera_target_position);
	m_camera_target_position = m_camera_position + direction * dist;

	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);

	m_continous_time += dt;

	// update meshes tranformations
	glm::mat4 ball_translation = glm::translate(glm::mat4(1.0), glm::vec3(2, 1, 0));
	glm::mat4 ball_rotation = glm::rotate(glm::mat4(1.0), 1.5f*m_continous_time, glm::vec3(0, 1, 0));
	glm::mat4 ball_scale = glm::scale(glm::mat4(1.0), glm::vec3(0.0625));
	m_rock_transformation_matrix = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0))* ball_rotation * ball_translation * ball_scale;
	m_rock_transformation_normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_rock_transformation_matrix))));
}

bool Renderer::InitCommonItems()
{
	glGenVertexArrays(1, &m_vao_fbo);
	glBindVertexArray(m_vao_fbo);

	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,
	};

	glGenBuffers(1, &m_vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	return true;
}

bool Renderer::InitRenderingTechniques()
{
	bool initialized = true;

	std::string vertex_shader_path = "Assets/Shaders/basic_rendering.vert";
	std::string fragment_shader_path = "Assets/Shaders/basic_rendering.frag";
	m_geometry_rendering_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_geometry_rendering_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_geometry_rendering_program.CreateProgram();
	m_geometry_rendering_program.LoadUniform("uniform_projection_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_view_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_model_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_normal_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_diffuse");
	m_geometry_rendering_program.LoadUniform("uniform_specular");
	m_geometry_rendering_program.LoadUniform("uniform_shininess");
	m_geometry_rendering_program.LoadUniform("uniform_has_texture");
	m_geometry_rendering_program.LoadUniform("diffuse_texture");
	m_geometry_rendering_program.LoadUniform("uniform_camera_position");
	// Light Source Uniforms
	m_geometry_rendering_program.LoadUniform("uniform_light_projection_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_light_view_matrix");
	m_geometry_rendering_program.LoadUniform("uniform_light_position");
	m_geometry_rendering_program.LoadUniform("uniform_light_direction");
	m_geometry_rendering_program.LoadUniform("uniform_light_color");
	m_geometry_rendering_program.LoadUniform("uniform_light_umbra");
	m_geometry_rendering_program.LoadUniform("uniform_light_penumbra");
	m_geometry_rendering_program.LoadUniform("uniform_cast_shadows");
	m_geometry_rendering_program.LoadUniform("shadowmap_texture");

	vertex_shader_path = "Assets/Shaders/postproc.vert";
	fragment_shader_path = "Assets/Shaders/postproc.frag";
	m_postprocess_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_postprocess_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_postprocess_program.CreateProgram();
	m_postprocess_program.LoadUniform("uniform_texture");
	m_postprocess_program.LoadUniform("uniform_time");

	vertex_shader_path = "Assets/Shaders/shadow_map_rendering.vert";
	fragment_shader_path = "Assets/Shaders/shadow_map_rendering.frag";
	m_spot_light_shadow_map_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_spot_light_shadow_map_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_spot_light_shadow_map_program.CreateProgram();
	m_spot_light_shadow_map_program.LoadUniform("uniform_projection_matrix");
	m_spot_light_shadow_map_program.LoadUniform("uniform_view_matrix");
	m_spot_light_shadow_map_program.LoadUniform("uniform_model_matrix");
	
	return initialized;
}

bool Renderer::ReloadShaders()
{
	bool reloaded = true;
	// rendering techniques
	m_geometry_rendering_program.ReloadProgram();

	m_postprocess_program.ReloadProgram();

	m_spot_light_shadow_map_program.ReloadProgram();

	return reloaded;
}

bool Renderer::InitIntermediateShaderBuffers()
{
	// out texture	
	glGenTextures(1, &m_fbo_texture);
	// depth texture
	glGenTextures(1, &m_fbo_depth_texture);

	// framebuffer to link everything together
	glGenFramebuffers(1, &m_fbo);

	return ResizeBuffers(m_screen_width, m_screen_height);
}

bool Renderer::ResizeBuffers(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// depth texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_screen_width, m_screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_projection_matrix = glm::perspective(glm::radians(60.f), width / (float)height, 0.1f, 1500.0f);
	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);

	return true;
}

bool Renderer::InitLightSources()
{
	m_spotlight_node.SetPosition(glm::vec3(6, 7, -3));
	m_spotlight_node.SetTarget(glm::vec3(0, 2, 0));

	m_spotlight_node.SetColor(40.0f * glm::vec3(0.79, 0.89, 1.0));
	m_spotlight_node.SetConeSize(50, 70);
	m_spotlight_node.CastShadow(true);

	return true;
}

bool Renderer::InitGeometricMeshes()
{
	bool initialized = true;
	OBJLoader loader;


	GeometricMesh* mesh = loader.load("Assets/Objects/Corridor_Straight.obj");
	for (int i = 0; i < 4; i++) {
		
		if (mesh != nullptr)
		{
			m_corridors_geometry[i] = new GeometryNode();
			m_corridors_geometry[i]->Init(mesh);
			
			glm::mat4 corridors_trans = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, i * 20));
			
			m_corridors_transformation_matrix[i] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * corridors_trans;
			m_corridors_transformation_normal_matrix[i] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridors_transformation_matrix[i]))));
		}
		else
			initialized = false;
		
	}
	delete mesh;

	mesh = loader.load("Assets/Objects/Wall.obj");
	for (int i = 0; i < 2; i++) {
		if (mesh != nullptr)
		{
			m_wall_geometry[i] = new GeometryNode();
			m_wall_geometry[i]->Init(mesh);

		}
		else
			initialized = false;
	}

	glm::mat4 ball_translation = glm::translate(glm::mat4(1.0), glm::vec3(-3, 0, -20));
	m_wall_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * ball_translation;
	m_wall_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[0]))));

	ball_translation = glm::translate(glm::mat4(1.0), glm::vec3(3, 0, -20));
	m_wall_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * ball_translation;
	m_wall_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[1]))));
	delete mesh;

	mesh = loader.load("Assets/Objects/Cannon.obj"); 
	if (mesh != nullptr)
	{
		m_rock_geometry = new GeometryNode();
		m_rock_geometry->Init(mesh);
	}
	else
		initialized = false;
	
	ball_translation = glm::translate(glm::mat4(1.0), glm::vec3(3, 0, -20));
	m_wall_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * ball_translation;
	m_wall_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[1]))));
	delete mesh;


	
	
	
	
	delete mesh;
	

	return initialized;
}

void Renderer::SetRenderingMode(RENDERING_MODE mode)
{
	m_rendering_mode = mode;
}

void Renderer::Render()
{
	// Draw the geometry to the shadow maps
	RenderShadowMaps();

	// Draw the geometry
	RenderGeometry();

	// render to screen                    **********************************************
	RenderToOutFB();

	GLenum error = Tools::CheckGLError();
	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}
}

void Renderer::RenderShadowMaps()
{
	// if the light source casts shadows
	if (m_spotlight_node.GetCastShadowsStatus())
	{
		int m_depth_texture_resolution = m_spotlight_node.GetShadowMapResolution();

		glBindFramebuffer(GL_FRAMEBUFFER, m_spotlight_node.GetShadowMapFBO());
		glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
		GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawbuffers);
		
		// clear depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		m_spot_light_shadow_map_program.Bind();
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetProjectionMatrix()));
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetViewMatrix()));
		


		// draw the Array of Corridors
		for (int i = 0; i < 4; i++) {
			glBindVertexArray(m_corridors_geometry[i]->m_vao);
			glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridors_transformation_matrix[i]));
			for (int j = 0; j < m_corridors_geometry[i]->parts.size(); j++)
			{
				glDrawArrays(GL_TRIANGLES, m_corridors_geometry[i]->parts[j].start_offset, m_corridors_geometry[i]->parts[j].count);
			}
		}

		// draw the Door
		for (int i = 0; i < 2; i++) {
			glBindVertexArray(m_wall_geometry[i]->m_vao);
			glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_wall_transformation_matrix[i]));
			for (int j = 0; j < m_rock_geometry->parts.size(); j++)
			{
				glDrawArrays(GL_TRIANGLES, m_wall_geometry[i]->parts[j].start_offset, m_wall_geometry[i]->parts[j].count);
			}
		}
		
		// draw the spinning rock
		glBindVertexArray(m_rock_geometry->m_vao);
		glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_rock_transformation_matrix));
		for (int j = 0; j < m_rock_geometry->parts.size(); j++)
		{
			glDrawArrays(GL_TRIANGLES, m_rock_geometry->parts[j].start_offset, m_rock_geometry->parts[j].count);
		}
		
		glBindVertexArray(0);
		m_spot_light_shadow_map_program.Unbind();
		
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


void Renderer::RenderGeometry()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_screen_width, m_screen_height);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);
	
	// clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glEnable(GL_DEPTH_TEST);
	
	switch (m_rendering_mode)
	{
	case RENDERING_MODE::TRIANGLES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case RENDERING_MODE::LINES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case RENDERING_MODE::POINTS:
		glPointSize(2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	};
	
	m_geometry_rendering_program.Bind();
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_projection_matrix));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_view_matrix));
	glUniform3f(m_geometry_rendering_program["uniform_camera_position"], m_camera_position.x, m_camera_position.y, m_camera_position.z);
	
	// pass the light source parameters
	glm::vec3 light_position = m_spotlight_node.GetPosition();
	glm::vec3 light_direction = m_spotlight_node.GetDirection();
	glm::vec3 light_color = m_spotlight_node.GetColor();
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetProjectionMatrix()));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node.GetViewMatrix()));
	glUniform3f(m_geometry_rendering_program["uniform_light_position"], light_position.x, light_position.y, light_position.z);
	glUniform3f(m_geometry_rendering_program["uniform_light_direction"], light_direction.x, light_direction.y, light_direction.z);
	glUniform3f(m_geometry_rendering_program["uniform_light_color"], light_color.x, light_color.y, light_color.z);
	glUniform1f(m_geometry_rendering_program["uniform_light_umbra"], m_spotlight_node.GetUmbra());
	glUniform1f(m_geometry_rendering_program["uniform_light_penumbra"], m_spotlight_node.GetPenumbra());
	glUniform1i(m_geometry_rendering_program["uniform_cast_shadows"], (m_spotlight_node.GetCastShadowsStatus())? 1 : 0);
	glUniform1i(m_geometry_rendering_program["shadowmap_texture"], 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, (m_spotlight_node.GetCastShadowsStatus()) ? m_spotlight_node.GetShadowMapDepthTexture() : 0);
	
	// Enable Texture Unit 0
	glUniform1i(m_geometry_rendering_program["uniform_diffuse_texture"], 0);
	glActiveTexture(GL_TEXTURE0);


	// draw the Array of Corridors
	for (int i = 0; i < 4; i++) {

		glBindVertexArray(m_corridors_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridors_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridors_transformation_normal_matrix[i]));
		for (int j = 0; j < m_corridors_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_corridors_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_corridors_geometry[i]->parts[j].specularColor;
			float shininess = m_corridors_geometry[i]->parts[j].shininess;

			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_corridors_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_corridors_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_corridors_geometry[i]->parts[j].start_offset, m_corridors_geometry[i]->parts[j].count);
		}
	}
	

	// draw the Door
	for (int i = 0; i < 2; i++) {
		glBindVertexArray(m_wall_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_wall_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_wall_transformation_normal_matrix[i]));
		for (int j = 0; j < m_wall_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_wall_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_wall_geometry[i]->parts[j].specularColor;
			float shininess = m_wall_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_wall_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_wall_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_wall_geometry[i]->parts[j].start_offset, m_wall_geometry[i]->parts[j].count);
		}
	}

	// draw the spinning rock
	glBindVertexArray(m_rock_geometry->m_vao);
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_rock_transformation_matrix));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_rock_transformation_normal_matrix));
	for (int j = 0; j < m_rock_geometry->parts.size(); j++)
	{
		glm::vec3 diffuseColor = m_rock_geometry->parts[j].diffuseColor;
		glm::vec3 specularColor = m_rock_geometry->parts[j].specularColor;
		float shininess = m_rock_geometry->parts[j].shininess;
		glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
		glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
		glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
		glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_rock_geometry->parts[j].textureID > 0) ? 1.0f : 0.0f);
		glBindTexture(GL_TEXTURE_2D, m_rock_geometry->parts[j].textureID);

		glDrawArrays(GL_TRIANGLES, m_rock_geometry->parts[j].start_offset, m_rock_geometry->parts[j].count);
	}
	glBindVertexArray(0);
	m_geometry_rendering_program.Unbind();

	glDisable(GL_DEPTH_TEST);
	if(m_rendering_mode != RENDERING_MODE::TRIANGLES)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderToOutFB()
{
	// render to screen                    **********************************************
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_screen_width, m_screen_height);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	m_postprocess_program.Bind();
	
	glBindVertexArray(m_vao_fbo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glUniform1i(m_postprocess_program["uniform_texture"], 0);
	glUniform1f(m_postprocess_program["uniform_time"], m_continous_time);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

	m_postprocess_program.Unbind();
}

void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable)? 1 : 0;
}
void Renderer::CameraMoveBackWard(bool enable)
{
	m_camera_movement.x = (enable) ? -1 : 0;
}

void Renderer::CameraMoveLeft(bool enable)
{
	m_camera_movement.y = (enable) ? -1 : 0;
}
void Renderer::CameraMoveRight(bool enable)
{
	m_camera_movement.y = (enable) ? 1 : 0;
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	m_camera_look_angle_destination = glm::vec2(1, -1) * lookDir;
}
