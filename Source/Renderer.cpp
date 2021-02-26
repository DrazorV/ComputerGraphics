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
	//Allocation Spotlight
	m_spotlight_node = new SpotLightNode[5];

	//allocation corridors
	m_corridors_geometry = new GeometryNode * [10];
	for (int i = 0; i < 10; i++) {
		m_corridors_geometry[i] = nullptr;
	}
	m_corridors_transformation_matrix = new glm::mat4[10];
	m_corridors_transformation_normal_matrix = new glm::mat4[10];

	//left corridor
	m_corridor_left_geometry = new GeometryNode * [5];
	for (int i = 0; i < 5; i++) {
		m_corridor_left_geometry[i] = nullptr;
	}
	m_corridor_left_transformation_matrix = new glm::mat4[5];
	m_corridor_left_transformation_normal_matrix = new glm::mat4[5];
	//right corridor
	m_corridor_right_geometry = new GeometryNode * [5];
	for (int i = 0; i < 5; i++) {
		m_corridor_right_geometry[i] = nullptr;
	}
	m_corridor_right_transformation_matrix = new glm::mat4[5];
	m_corridor_right_transformation_normal_matrix = new glm::mat4[5];

	//fork corridor
	m_corridor_fork_geometry = new GeometryNode * [7];
	for (int i = 0; i < 7; i++) {
		m_corridor_fork_geometry[i] = nullptr;
	}
	m_corridor_fork_transformation_matrix = new glm::mat4[7];
	m_corridor_fork_transformation_normal_matrix = new glm::mat4[7];

	//alocation walls

	m_wall_geometry = new GeometryNode * [15];
	for (int i = 0; i < 15; i++) {
		m_wall_geometry[i] = nullptr;
	}
	m_wall_transformation_matrix = new glm::mat4[15];
	m_wall_transformation_normal_matrix = new glm::mat4[15];

	//allocation cannon
	m_cannon_geometry = new GeometryNode * [3];
	for (int i = 0; i < 3; i++) {
		m_cannon_geometry[i] = nullptr;
	}
	m_cannon_transformation_matrix = new glm::mat4[3];
	m_cannon_transformation_normal_matrix = new glm::mat4[3];

	//allocation cannon mounts	
	m_cannon_mount_geometry = new GeometryNode * [4];
	for (int i = 0; i <4; i++) {
		m_cannon_mount_geometry[i] = nullptr;
	}
	m_cannon_mount_transformation_matrix = new glm::mat4[4];
	m_cannon_mount_transformation_normal_matrix = new glm::mat4[4];
	//pipes
	m_pipe_geometry = new GeometryNode * [24];
	for (int i = 0; i < 24; i++) {
		m_pipe_geometry[i] = nullptr;
	}
	m_pipe_transformation_matrix = new glm::mat4[24];
	m_pipe_transformation_normal_matrix = new glm::mat4[24];

	//allocation iris
	m_iris_geometry = new GeometryNode * [10];
	for (int i = 0; i < 10; i++) {
		m_iris_geometry[i] = nullptr;
	}
	m_iris_transformation_matrix = new glm::mat4[10];
	m_iris_transformation_normal_matrix = new glm::mat4[10];
	//beam
	m_beam_geometry = new GeometryNode * [8];
	for (int i = 0; i < 8; i++) {
		m_beam_geometry[i] = nullptr;
	}
	m_beam_transformation_matrix = new glm::mat4[8];
	m_beam_transformation_normal_matrix = new glm::mat4[8];

	m_fbo = 0;
	m_fbo_texture = 0;

	m_rendering_mode = RENDERING_MODE::TRIANGLES;
	m_continous_time = 0.0;
	m_camera_position = glm::vec3(0, 4, -10);
	m_camera_target_position = glm::vec3(0, 1, 1);
	m_camera_up_vector = glm::vec3(0, 1, 0);
}

Renderer::~Renderer()
{
	//delete light sources

	delete m_spotlight_node;
	// delete g_buffer
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteFramebuffers(1, &m_fbo);


	// delete common data
	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);
	//delete corridors

	for (int i = 0; i < 10; i++) {
		delete m_iris_geometry[i];
	}
	delete m_iris_geometry;
	delete m_iris_transformation_matrix;
	delete m_iris_transformation_normal_matrix;

	for (int i = 0; i < 10; i++) {
		delete m_corridors_geometry[i];
	}
	delete m_corridors_geometry;
	delete m_corridors_transformation_matrix;
	delete m_corridors_transformation_normal_matrix;

	for (int i = 0; i < 7; i++) {
		delete m_corridor_fork_geometry[i];
	}
	delete m_corridor_fork_geometry;
	delete m_corridor_fork_transformation_matrix;
	delete m_corridor_fork_transformation_normal_matrix;

	//delete left 
	for (int i = 0; i < 5; i++) {
		delete m_corridor_left_geometry[i];
	}
	delete m_corridor_left_geometry;
	delete m_corridor_left_transformation_matrix;
	delete m_corridor_left_transformation_normal_matrix;
	//right corridor

	for (int i = 0; i < 5; i++) {
		delete m_corridor_right_geometry[i];
	}
	delete m_corridor_right_geometry;
	delete m_corridor_right_transformation_matrix;
	delete m_corridor_right_transformation_normal_matrix;

	//delete walls
	for (int i = 0; i < 15; i++) {
		delete m_wall_geometry[i];
	}
	delete m_wall_geometry;
	delete m_wall_transformation_matrix;
	delete m_wall_transformation_normal_matrix;

	//delete cannons
	for (int i = 0; i < 3; i++) {
		delete m_cannon_geometry[i];
	}
	delete m_cannon_geometry;
	delete m_cannon_transformation_matrix;
	delete m_cannon_transformation_normal_matrix;
	//delete cannon mounts
	for (int i = 0; i < 4; i++) {
		delete m_cannon_mount_geometry[i];
	}
	delete m_cannon_mount_geometry;
	delete m_cannon_mount_transformation_matrix;
	delete m_cannon_mount_transformation_normal_matrix;
	//delete pipes
	for (int i = 0; i < 24; i++) {
		delete m_pipe_geometry[i];
	}
	delete m_pipe_geometry;
	delete m_pipe_transformation_matrix;
	delete m_pipe_transformation_normal_matrix;
	//delete beams
	for (int i = 0; i < 8; i++) {
		delete m_beam_geometry[i];
	}
	delete m_beam_geometry;
	delete m_beam_transformation_matrix;
	delete m_beam_transformation_normal_matrix;
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
	float movement_speed = 25.5f;
	glm::vec3 direction = glm::normalize(m_camera_target_position - m_camera_position);

	m_camera_position += m_camera_movement.x * movement_speed * direction * dt;
	m_camera_target_position += m_camera_movement.x * movement_speed * direction * dt;

	glm::vec3 right = glm::normalize(glm::cross(direction, m_camera_up_vector));
	m_camera_position += m_camera_movement.y * movement_speed * right * dt;
	m_camera_target_position += m_camera_movement.y * movement_speed * right * dt;

	glm::mat4 rotation = glm::mat4(1.0f);
	float angular_speed = glm::pi<float>() * 0.0025f;

	rotation *= glm::rotate(glm::mat4(1.0), m_camera_look_angle_destination.y * angular_speed, right);
	rotation *= glm::rotate(glm::mat4(1.0), -m_camera_look_angle_destination.x * angular_speed, m_camera_up_vector);
	m_camera_look_angle_destination = glm::vec2(0);

	direction = rotation * glm::vec4(direction, 0);
	float dist = glm::distance(m_camera_position, m_camera_target_position);
	m_camera_target_position = m_camera_position + direction * dist;

	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);

	m_continous_time += dt;

	// update meshes tranformations
	glm::mat4 ball_rotation = glm::rotate(glm::mat4(1.0), 1.5f * m_continous_time, glm::vec3(0, 1, 0));

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
	for (int i = 0; i < 5; i++) {
		m_spotlight_node[i].SetPosition(glm::vec3(6, 7, -3 + i * 20));
		m_spotlight_node[i].SetTarget(glm::vec3(0, 2, 0 + i * 20));
		m_spotlight_node[i].SetColor(40.0f * glm::vec3(1.0, 1.0, 1.0));
		m_spotlight_node[i].SetConeSize(100, 170);
		m_spotlight_node[i].CastShadow(true);
	}
	return true;
}

bool Renderer::InitGeometricMeshes()
{
	bool initialized = true;
	OBJLoader loader;

	//load corridor
	GeometricMesh* mesh = loader.load("Assets/Objects/Corridor_Straight.obj");
	for (int i = 0; i < 6; i++) {
		if (mesh != nullptr)
		{
			m_corridors_geometry[i] = new GeometryNode();
			m_corridors_geometry[i]->Init(mesh);
		}
		else
			initialized = false;
	}
	for (int i = 0; i < 4; i++) {
		glm::mat4 corridors_trans = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, i * 20));

		m_corridors_transformation_matrix[i] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * corridors_trans;
		m_corridors_transformation_normal_matrix[i] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridors_transformation_matrix[i]))));
	}
	
	m_corridors_transformation_matrix[4] = glm::translate(glm::mat4(1.0), glm::vec3(9, 0, 120));
	m_corridors_transformation_normal_matrix[4] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridors_transformation_matrix[4]))));

	m_corridors_transformation_matrix[5] = glm::translate(glm::mat4(1.0), glm::vec3(9, 0, 140));
	m_corridors_transformation_normal_matrix[5] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridors_transformation_matrix[5]))));

	delete mesh;
	//load cannon mounts
	mesh = loader.load("Assets/Objects/CannonMount.obj");

	for (int i = 0; i <4; i++) {
		if (mesh != nullptr)
		{
			m_cannon_mount_geometry[i] = new GeometryNode();
			m_cannon_mount_geometry[i]->Init(mesh);
		}
		else
			initialized = false;
	}
	glm::mat4 RotY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

	glm::mat4 cannonMount_translation = glm::translate(glm::mat4(1.0), glm::vec3(1, 2, 78.8));
	m_cannon_mount_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(1, 1, 1)) * cannonMount_translation * RotY;
	m_cannon_mount_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_cannon_mount_transformation_matrix[0]))));
	
	RotY = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
	cannonMount_translation = glm::translate(glm::mat4(1.0), glm::vec3(-2.25, -1.75, 13));
	m_cannon_mount_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 1)) * cannonMount_translation * RotY;
	m_cannon_mount_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_cannon_mount_transformation_matrix[1]))));
	
	m_cannon_mount_transformation_matrix[2] = glm::translate(glm::mat4(1.0), glm::vec3(0,0,10))* RotY;
	m_cannon_mount_transformation_normal_matrix[2] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_cannon_mount_transformation_matrix[2]))));

	
	
	
	//glm::translate(glm::mat4(1.0), glm::vec3(2.2, -12, 120));

	delete mesh;

	//cannon
	mesh = loader.load("Assets/Objects/Cannon.obj");
	for (int i = 0; i < 3; i++) {
		if (mesh != nullptr)
		{
			m_cannon_geometry[i] = new GeometryNode();
			m_cannon_geometry[i]->Init(mesh);
		}
		else
			initialized = false;
	}
	glm::mat4 RotX = glm::rotate(glm::mat4(1.f), glm::radians(35.f), glm::vec3(1.f, 0.f, 0.f));
	RotY = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 RotZ = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f));

	glm::mat4 cannon_translation = glm::translate(glm::mat4(1.0), glm::vec3(1, 3, 78.7));
	m_cannon_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 0)) * cannon_translation * RotY * RotX;
	m_cannon_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_cannon_transformation_matrix[0]))));

	RotX = glm::rotate(glm::mat4(1.f), glm::radians(-2.5f), glm::vec3(1.f, 0.f, 0.f));
	cannon_translation = glm::translate(glm::mat4(1.0), glm::vec3(-3, -0.5, 14));
	m_cannon_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 0)) * cannon_translation * RotY * RotX* ball_rotation;
	m_cannon_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_cannon_transformation_matrix[1]))));

	cannon_translation = glm::translate(glm::mat4(1.0), glm::vec3(-1.5, -0.5, 14));
	m_cannon_transformation_matrix[2] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 0)) * cannon_translation * RotY * RotX;
	m_cannon_transformation_normal_matrix[2] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_cannon_transformation_matrix[2]))));
	delete mesh;

	//load pipes
	mesh = loader.load("Assets/Objects/Pipe.obj");
	for (int i = 0; i < 24; i++) {

		if (mesh != nullptr)
		{
			m_pipe_geometry[i] = new GeometryNode();
			m_pipe_geometry[i]->Init(mesh);
		}
		else
			initialized = false;

	}
	int j = 0;
	for (int i = -10; i < 70; i = i + 10) {

		glm::mat4 pipe_translation = glm::translate(glm::mat4(1.0), glm::vec3(1.5, -3, i));
		m_pipe_transformation_matrix[j] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 0)) * pipe_translation;
		m_pipe_transformation_normal_matrix[j] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_pipe_transformation_matrix[j]))));
		j++;
	}

	for (int i = -10; i < 70; i = i + 10) {

		glm::mat4 pipe_translation = glm::translate(glm::mat4(1.0), glm::vec3(-1.5, 5.0, i));
		m_pipe_transformation_matrix[j] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 0)) * pipe_translation;
		m_pipe_transformation_normal_matrix[j] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_pipe_transformation_matrix[j]))));
		j++;
	}

	for (int i = -10; i < 70; i = i + 10) {

		glm::mat4 pipe_translation = glm::translate(glm::mat4(1.0), glm::vec3(-2.5, 5.0, i));
		m_pipe_transformation_matrix[j] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0.0, 0.0)) * pipe_translation;
		m_pipe_transformation_normal_matrix[j] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_pipe_transformation_matrix[j]))));
		j++;
	}

	delete mesh;

	//load walls
	mesh = loader.load("Assets/Objects/Wall.obj");
	for (int i = 0; i < 13; i++) {
		if (mesh != nullptr)
		{
			m_wall_geometry[i] = new GeometryNode();
			m_wall_geometry[i]->Init(mesh);

		}
		else
			initialized = false;
	}

	glm::mat4 wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(-3, 0, -20));
	m_wall_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[0]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(3, 0, -20));
	m_wall_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[1]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(3, 0, 80));
	m_wall_transformation_matrix[2] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[2] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[2]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(11, 0, 80));
	m_wall_transformation_matrix[3] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[3] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[3]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(-19, 0, 120));
	m_wall_transformation_matrix[4] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[4] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[4]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(-13, 0, 120));
	m_wall_transformation_matrix[5] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[5] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[5]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(2.5, -11, 120));
	m_wall_transformation_matrix[6] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[6] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[6]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(8.5, -11, 120));
	m_wall_transformation_matrix[7] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[7] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[7]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(8.5, -6, 180));
	m_wall_transformation_matrix[8] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[8] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[8]))));

	wall_trans = glm::translate(glm::mat4(1.0), glm::vec3(2.5, -6, 180));
	m_wall_transformation_matrix[9] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * wall_trans;
	m_wall_transformation_normal_matrix[9] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[9]))));

	m_wall_transformation_matrix[10] = glm::translate(glm::mat4(1.0), glm::vec3(5, 0, 110));
	m_wall_transformation_normal_matrix[10] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[10]))));

	m_wall_transformation_matrix[11] = glm::translate(glm::mat4(1.0), glm::vec3(-5, 0, 110));
	m_wall_transformation_normal_matrix[11] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[11]))));

	m_wall_transformation_matrix[12] = glm::translate(glm::mat4(1.0), glm::vec3(-7, 0, 130));
	m_wall_transformation_normal_matrix[12] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_wall_transformation_matrix[12]))));
	delete mesh;

	//load fork
	mesh = loader.load("Assets/Objects/Corridor_Fork.obj");
	for (int i = 0; i < 6; i++) {
		if (mesh != nullptr)
		{
			m_corridor_fork_geometry[i] = new GeometryNode();
			m_corridor_fork_geometry[i]->Init(mesh);
		}
		else
			initialized = false;
	}

	glm::mat4 corridorFork_trans = glm::translate(glm::mat4(1.0), glm::vec3(0 , 0, 60)); // scaling factor for each axis
	glm::mat4 RotFork = glm::rotate(glm::mat4(1.f), glm::radians(-180.f), glm::vec3(0.f, 1.f, 0.f));
	//sta pip es
	m_corridor_fork_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * corridorFork_trans * RotFork;
	m_corridor_fork_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_fork_transformation_matrix[0]))));
	//deksia 
	m_corridor_fork_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(-11, 0, 100))* RotFork;
	m_corridor_fork_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_fork_transformation_matrix[1]))));
	//merge monopatia
	m_corridor_fork_transformation_matrix[2] = glm::translate(glm::mat4(1.0), glm::vec3(4, 0, 160)) ;
	m_corridor_fork_transformation_normal_matrix[2] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_fork_transformation_matrix[2]))));
	//katifora
	glm::mat4 RotForkX = glm::rotate(glm::mat4(1.f), glm::radians(-180.f), glm::vec3(1.f, 1.f, 0.f));
	m_corridor_fork_transformation_matrix[3] = glm::translate(glm::mat4(1.0), glm::vec3(4, -5, 180))*RotForkX*RotFork;
	m_corridor_fork_transformation_normal_matrix[3] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_fork_transformation_matrix[3]))));
	//spread meta ti katifora
	m_corridor_fork_transformation_matrix[4] = glm::translate(glm::mat4(1.0), glm::vec3(4, -10, 160));
	m_corridor_fork_transformation_normal_matrix[4] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_fork_transformation_matrix[4]))));
	//mazema tou spread
	RotForkX = glm::rotate(glm::mat4(1.f), glm::radians(-180.f), glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 RotForkY = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	m_corridor_fork_transformation_matrix[5] = glm::translate(glm::mat4(1.0), glm::vec3(4, -10, 120))* RotForkX;
	m_corridor_fork_transformation_normal_matrix[5] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_fork_transformation_matrix[5]))));

	

	delete mesh;
	//load left
	mesh = loader.load("Assets/Objects/Corridor_Left.obj");
	for (int i = 0; i < 2; i++) {

		if (mesh != nullptr)
		{
			m_corridor_left_geometry[i] = new GeometryNode();
			m_corridor_left_geometry[i]->Init(mesh);
		}
		else
			initialized = false;
	}

	//glm::mat4 corridorL_trans = glm::translate(glm::mat4(1.0), glm::vec3(9, 0, 100)); // scaling factor for each axis
	m_corridor_left_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(9, 0, 100));
	m_corridor_left_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_left_transformation_matrix[0]))));

	m_corridor_left_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 140));
	m_corridor_left_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_left_transformation_matrix[1]))));

	
	delete mesh;

	//load corridor right(right on first fork)
	mesh = loader.load("Assets/Objects/Corridor_Right.obj");
	if (mesh != nullptr)
	{
		m_corridor_right_geometry[0] = new GeometryNode();
		m_corridor_right_geometry[0]->Init(mesh);
	}
	else
		initialized = false;

	glm::mat4 rc_trans = glm::translate(glm::mat4(1.0), glm::vec3(-10, 0, 100));
	m_corridor_right_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(-1, 0, 0)) * rc_trans;
	m_corridor_right_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_corridor_right_transformation_matrix[0]))));
	delete mesh;

	//load beams
	mesh = loader.load("Assets/Objects/Beam.obj");
	for (int i = 0; i < 7; i++) {
		if (mesh != nullptr)
		{
			m_beam_geometry[i] = new GeometryNode();
			m_beam_geometry[i]->Init(mesh);

		}
		else
			initialized = false;
	}

	glm::mat4 beam_trans = glm::translate(glm::mat4(1.0), glm::vec3(-5, -3.5, 10));
	RotY = glm::rotate(glm::mat4(1.f), glm::radians(135.f), glm::vec3(0.f, 1.f, 0.f));
	RotX = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	m_beam_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(1, 0, 1)) * beam_trans * RotY * RotX;
	m_beam_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[0]))));


	RotZ = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	m_beam_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(4, 0, 70)) * beam_trans * RotZ; //* Rotb * Rotc;
	m_beam_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[1]))));


	RotZ = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	m_beam_transformation_matrix[2] = glm::translate(glm::mat4(1.0), glm::vec3(-11, 0, 120)) * RotZ; //* Rotb * Rotc;
	m_beam_transformation_normal_matrix[2] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[2]))));

	RotZ = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	m_beam_transformation_matrix[3] = glm::translate(glm::mat4(1.0), glm::vec3(9, 5, 130)) * beam_trans * RotZ; //* Rotb * Rotc;
	m_beam_transformation_normal_matrix[3] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[3]))));

	m_beam_transformation_matrix[4] = glm::translate(glm::mat4(1.0), glm::vec3(-5, 3, 100)) * beam_trans * RotZ*RotX; //* Rotb * Rotc;
	m_beam_transformation_normal_matrix[4] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[4]))));

	RotY = glm::rotate(glm::mat4(1.f), glm::radians(135.f), glm::vec3(0.f, 1.f, 0.f));
	RotX = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	m_beam_transformation_matrix[5] = glm::translate(glm::mat4(1.0), glm::vec3(-5, -3.5, 100)) * RotY * RotX;
	m_beam_transformation_normal_matrix[5] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[5]))));

	m_beam_transformation_matrix[6] = glm::translate(glm::mat4(1.0), glm::vec3(2.2, -12, 120));
	m_beam_transformation_normal_matrix[6] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_beam_transformation_matrix[6]))));
	delete mesh;


	//load iris
	mesh = loader.load("Assets/Objects/Iris.obj");

	for (int i = 0; i < 5; i++) {
		if (mesh != nullptr)
		{
			m_iris_geometry[i] = new GeometryNode();
			m_iris_geometry[i]->Init(mesh);
		}
		else
			initialized = false;
	}
	RotY = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));

	m_iris_transformation_matrix[0] = glm::translate(glm::mat4(1.0), glm::vec3(2, -3.1, 79.9)) *RotY;
	m_iris_transformation_normal_matrix[0] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_iris_transformation_matrix[0]))));

	m_iris_transformation_matrix[1] = glm::translate(glm::mat4(1.0), glm::vec3(2, -0.1, 79.9)) * RotY;
	m_iris_transformation_normal_matrix[1] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_iris_transformation_matrix[1]))));

	m_iris_transformation_matrix[2] = glm::translate(glm::mat4(1.0), glm::vec3(-14, -3.1, 119.8)) * RotY;
	m_iris_transformation_normal_matrix[2] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_iris_transformation_matrix[2]))));

	m_iris_transformation_matrix[3] = glm::translate(glm::mat4(1.0), glm::vec3(-14, 3.1 , 119.8)) * RotY;
	m_iris_transformation_normal_matrix[3] = glm::mat4(glm::transpose(glm::inverse(glm::mat3(m_iris_transformation_matrix[3]))));

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
	for (int i = 0; i < 5; i++) {
		if (m_spotlight_node[i].GetCastShadowsStatus())
		{
			int m_depth_texture_resolution = m_spotlight_node[i].GetShadowMapResolution();

			glBindFramebuffer(GL_FRAMEBUFFER, m_spotlight_node[i].GetShadowMapFBO());
			glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
			GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawbuffers);

			// clear depth buffer
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			m_spot_light_shadow_map_program.Bind();
			glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node[i].GetProjectionMatrix()));
			glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node[i].GetViewMatrix()));



			//draw the Array of Corridors
			for (int i = 0; i < 6; i++) {
				glBindVertexArray(m_corridors_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridors_transformation_matrix[i]));
				for (int j = 0; j < m_corridors_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_corridors_geometry[i]->parts[j].start_offset, m_corridors_geometry[i]->parts[j].count);
				}
			}
			//draw iris
			for (int i = 0; i < 4; i++) {
				glBindVertexArray(m_iris_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_iris_transformation_matrix[i]));
				for (int j = 0; j < m_iris_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_iris_geometry[i]->parts[j].start_offset, m_iris_geometry[i]->parts[j].count);
				}
			}
			// draw the wall
			for (int i = 0; i < 13; i++) {
				glBindVertexArray(m_wall_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_wall_transformation_matrix[i]));
				for (int j = 0; j < m_wall_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_wall_geometry[i]->parts[j].start_offset, m_wall_geometry[i]->parts[j].count);
				}
			}
			//draw beam
			for (int i = 0; i < 7; i++) {
				glBindVertexArray(m_beam_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_beam_transformation_matrix[i]));
				for (int j = 0; j < m_beam_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_beam_geometry[i]->parts[j].start_offset, m_beam_geometry[i]->parts[j].count);
				}
			}
			//cannon
			for (int i = 0; i < 3; i++) {
				glBindVertexArray(m_cannon_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_transformation_matrix[i]));
				for (int j = 0; j < m_cannon_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_cannon_geometry[i]->parts[j].start_offset, m_cannon_geometry[i]->parts[j].count);
				}
			}


			// draw the cannon mount
			for (int i = 0; i < 3; i++) {
				glBindVertexArray(m_cannon_mount_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_mount_transformation_matrix[i]));
				for (int j = 0; j < m_cannon_mount_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_cannon_mount_geometry[i]->parts[j].start_offset, m_cannon_mount_geometry[i]->parts[j].count);
				}
			}
			// draw the pipe
			for (int i = 0; i < 24; i++) {
				glBindVertexArray(m_pipe_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_pipe_transformation_matrix[i]));
				for (int j = 0; j < m_pipe_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_pipe_geometry[i]->parts[j].start_offset, m_pipe_geometry[i]->parts[j].count);
				}
			}
			//draw fork
			for (int i = 0; i < 6; i++) {
				glBindVertexArray(m_corridor_fork_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_fork_transformation_matrix[i]));
				for (int j = 0; j < m_corridor_fork_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_corridor_fork_geometry[i]->parts[j].start_offset, m_corridor_fork_geometry[i]->parts[j].count);
				}
			}
			//draw left
			for (int i = 0; i < 2; i++) {
				glBindVertexArray(m_corridor_left_geometry[i]->m_vao);
				glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_left_transformation_matrix[i]));
				for (int j = 0; j < m_corridor_left_geometry[i]->parts.size(); j++)
				{
					glDrawArrays(GL_TRIANGLES, m_corridor_left_geometry[i]->parts[j].start_offset, m_corridor_left_geometry[i]->parts[j].count);
				}
			}
			//right corridor 
			for (int i = 0; i < 1; i++) {}
			glBindVertexArray(m_corridor_right_geometry[0]->m_vao);
			glUniformMatrix4fv(m_spot_light_shadow_map_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_transformation_matrix[0]));
			for (int j = 0; j < m_corridor_right_geometry[0]->parts.size(); j++)
			{
				glDrawArrays(GL_TRIANGLES, m_corridor_right_geometry[0]->parts[j].start_offset, m_corridor_right_geometry[0]->parts[j].count);
			}

			glBindVertexArray(0);
			m_spot_light_shadow_map_program.Unbind();

			glDisable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
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
	for (int i = 0; i < 5; i++) {
		glm::vec3 light_position = m_spotlight_node[i].GetPosition();
		glm::vec3 light_direction = m_spotlight_node[i].GetDirection();
		glm::vec3 light_color = m_spotlight_node[i].GetColor();
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_projection_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node[i].GetProjectionMatrix()));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_light_view_matrix"], 1, GL_FALSE, glm::value_ptr(m_spotlight_node[i].GetViewMatrix()));
		glUniform3f(m_geometry_rendering_program["uniform_light_position"], light_position.x, light_position.y, light_position.z);
		glUniform3f(m_geometry_rendering_program["uniform_light_direction"], light_direction.x, light_direction.y, light_direction.z);
		glUniform3f(m_geometry_rendering_program["uniform_light_color"], light_color.x, light_color.y, light_color.z);
		glUniform1f(m_geometry_rendering_program["uniform_light_umbra"], m_spotlight_node[i].GetUmbra());
		glUniform1f(m_geometry_rendering_program["uniform_light_penumbra"], m_spotlight_node[i].GetPenumbra());
		glUniform1i(m_geometry_rendering_program["uniform_cast_shadows"], (m_spotlight_node[i].GetCastShadowsStatus()) ? 1 : 0);
		glUniform1i(m_geometry_rendering_program["shadowmap_texture"], 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, (m_spotlight_node[i].GetCastShadowsStatus()) ? m_spotlight_node[i].GetShadowMapDepthTexture() : 0);
	}



	// Enable Texture Unit 0
	glUniform1i(m_geometry_rendering_program["uniform_diffuse_texture"], 0);
	glActiveTexture(GL_TEXTURE0);


	// draw the Array of Corridors
	for (int i = 0; i < 6; i++) {

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
	for (int i = 0; i < 13; i++) {
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
	// draw the pipe
	for (int i = 0; i < 24; i++) {
		glBindVertexArray(m_pipe_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_pipe_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_pipe_transformation_normal_matrix[i]));
		for (int j = 0; j < m_pipe_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_pipe_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_pipe_geometry[i]->parts[j].specularColor;
			float shininess = m_pipe_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_pipe_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_pipe_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_pipe_geometry[i]->parts[j].start_offset, m_pipe_geometry[i]->parts[j].count);
		}
	}
	//draw beam
	for (int i = 0; i < 7; i++) {
		glBindVertexArray(m_beam_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_beam_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_beam_transformation_normal_matrix[i]));
		for (int j = 0; j < m_beam_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_beam_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_beam_geometry[i]->parts[j].specularColor;
			float shininess = m_beam_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_beam_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_beam_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_beam_geometry[i]->parts[j].start_offset, m_beam_geometry[i]->parts[j].count);
		}
	}
	//draw cannon mount
	for (int i = 0; i < 3; i++) {
		glBindVertexArray(m_cannon_mount_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_mount_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_mount_transformation_normal_matrix[i]));
		for (int j = 0; j < m_cannon_mount_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_cannon_mount_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_cannon_mount_geometry[i]->parts[j].specularColor;
			float shininess = m_cannon_mount_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_cannon_mount_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_cannon_mount_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_cannon_mount_geometry[i]->parts[j].start_offset, m_cannon_mount_geometry[i]->parts[j].count);
		}
	}

	//draw cannon
	for (int i = 0; i < 3; i++) {
		glBindVertexArray(m_cannon_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_cannon_transformation_normal_matrix[i]));
		for (int j = 0; j < m_cannon_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_cannon_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_cannon_geometry[i]->parts[j].specularColor;
			float shininess = m_cannon_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_cannon_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_cannon_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_cannon_geometry[i]->parts[j].start_offset, m_cannon_geometry[i]->parts[j].count);
		}
	}

	//draw iris
	for (int i = 0; i < 4; i++) {
		glBindVertexArray(m_iris_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_iris_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_iris_transformation_normal_matrix[i]));
		for (int j = 0; j < m_iris_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_iris_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_iris_geometry[i]->parts[j].specularColor;
			float shininess = m_iris_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_iris_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_iris_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_iris_geometry[i]->parts[j].start_offset, m_iris_geometry[i]->parts[j].count);
		}
	}
	//draw a fork
	for (int i = 0; i < 6; i++) {
		glBindVertexArray(m_corridor_fork_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_fork_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_fork_transformation_normal_matrix[i]));
		for (int j = 0; j < m_corridor_fork_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_corridor_fork_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_corridor_fork_geometry[i]->parts[j].specularColor;
			float shininess = m_corridor_fork_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_corridor_fork_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_corridor_fork_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_corridor_fork_geometry[i]->parts[j].start_offset, m_corridor_fork_geometry[i]->parts[j].count);
		}
	}
	//draw left
	for (int i = 0; i < 2; i++) {
		glBindVertexArray(m_corridor_left_geometry[i]->m_vao);
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_left_transformation_matrix[i]));
		glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_left_transformation_normal_matrix[i]));
		for (int j = 0; j < m_corridor_left_geometry[i]->parts.size(); j++)
		{
			glm::vec3 diffuseColor = m_corridor_left_geometry[i]->parts[j].diffuseColor;
			glm::vec3 specularColor = m_corridor_left_geometry[i]->parts[j].specularColor;
			float shininess = m_corridor_left_geometry[i]->parts[j].shininess;
			glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
			glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_corridor_left_geometry[i]->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, m_corridor_left_geometry[i]->parts[j].textureID);

			glDrawArrays(GL_TRIANGLES, m_corridor_left_geometry[i]->parts[j].start_offset, m_corridor_left_geometry[i]->parts[j].count);
		}
	}
	//draw right 
	glBindVertexArray(m_corridor_right_geometry[0]->m_vao);
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_right_transformation_matrix[0]));
	glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(m_corridor_right_transformation_normal_matrix[0]));
	for (int j = 0; j < m_corridor_right_geometry[0]->parts.size(); j++)
	{
		glm::vec3 diffuseColor = m_corridor_right_geometry[0]->parts[j].diffuseColor;
		glm::vec3 specularColor = m_corridor_right_geometry[0]->parts[j].specularColor;
		float shininess = m_corridor_right_geometry[0]->parts[j].shininess;
		glUniform3f(m_geometry_rendering_program["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
		glUniform3f(m_geometry_rendering_program["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
		glUniform1f(m_geometry_rendering_program["uniform_shininess"], shininess);
		glUniform1f(m_geometry_rendering_program["uniform_has_texture"], (m_corridor_right_geometry[0]->parts[j].textureID > 0) ? 1.0f : 0.0f);
		glBindTexture(GL_TEXTURE_2D, m_corridor_right_geometry[0]->parts[j].textureID);

		glDrawArrays(GL_TRIANGLES, m_corridor_right_geometry[0]->parts[j].start_offset, m_corridor_right_geometry[0]->parts[j].count);
	}
	glBindVertexArray(0);
	m_geometry_rendering_program.Unbind();

	glDisable(GL_DEPTH_TEST);
	if (m_rendering_mode != RENDERING_MODE::TRIANGLES)
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
	m_camera_movement.x = (enable) ? 1 : 0;
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
