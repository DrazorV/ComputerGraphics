#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "ShaderProgram.h"
#include "GeometryNode.h"
#include "CollidableNode.h"
#include "LightNode.h"

class Renderer
{
public:
	// Empty

protected:
	int												m_screen_width, m_screen_height;
	glm::mat4										m_world_matrix;
	glm::mat4										m_view_matrix;
	glm::mat4										m_projection_matrix;
	glm::vec3										m_camera_position;
	glm::vec3										m_camera_target_position;
	glm::vec3										m_camera_up_vector;
	glm::vec2										m_camera_movement;
	glm::vec2										m_camera_look_angle_destination;
	
	float m_continous_time;

	// Protected Functions
	bool InitShaders();
	bool InitGeometricMeshes();
	bool InitCommonItems();
	bool InitLights();
	bool InitIntermediateBuffers();
	void BuildWorld();
	void InitCamera();
	void RenderGeometry();
	void RenderStaticGeometry();
	void RenderCollidableGeometry();
	void RenderShadowMaps();
	void RenderPostProcess();

	enum OBJECS
	{
		BUNNY = 0,
		BALL,
		CHAIR,
		FLOOR,
		WALLS,
		BEAM,
		SIZE_ALL
	};

	std::vector<GeometryNode*> m_nodes;
	std::vector<CollidableNode*> m_collidables_nodes;

	LightNode									m_light;
	ShaderProgram								m_geometry_program;
	ShaderProgram								m_post_program;
	ShaderProgram								m_spot_light_shadow_map_program;

	GLuint m_fbo;
	GLuint m_fbo_texture;
	GLuint m_fbo_depth_texture;
	GLuint m_vao_fbo;
	GLuint m_vbo_fbo_vertices;

public:

	Renderer();
	~Renderer();
	bool										Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										Update(float dt);
	bool										ResizeBuffers(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										UpdateGeometry(float dt);
	void										UpdateCamera(float dt);
	bool										ReloadShaders();
	void										Render();

	void										CameraMoveForward(bool enable);
	void										CameraMoveBackWard(bool enable);
	void										CameraMoveLeft(bool enable);
	void										CameraMoveRight(bool enable);
	void										CameraLook(glm::vec2 lookDir);
};

#endif
