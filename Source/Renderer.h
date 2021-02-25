#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "ShaderProgram.h"
#include "SpotlightNode.h"

class Renderer
{
public:
	enum RENDERING_MODE
	{
		TRIANGLES,
		LINES,
		POINTS
	};

protected:
	int												m_screen_width, m_screen_height;
	glm::mat4										m_view_matrix;
	glm::mat4										m_projection_matrix;
	glm::vec3										m_camera_position;
	glm::vec3										m_camera_target_position;
	glm::vec3										m_camera_up_vector;
	glm::vec2										m_camera_movement;
	glm::vec2										m_camera_look_angle_destination;

	// Geometry Rendering Intermediate Buffer
	GLuint m_fbo;
	GLuint m_fbo_depth_texture;
	GLuint m_fbo_texture;

	GLuint m_vao_fbo, m_vbo_fbo_vertices;

	float m_continous_time;

	// Rendering Mode
	RENDERING_MODE m_rendering_mode;

	// Lights
	//more spotlights apo edw meta sto cpp kai sto init lightsources

	SpotLightNode* m_spotlight_node;

	// Meshes	
	class GeometryNode** m_wall_geometry;
	glm::mat4* m_wall_transformation_matrix;
	glm::mat4* m_wall_transformation_normal_matrix;
	class GeometryNode** m_corridors_geometry;
	glm::mat4* m_corridors_transformation_matrix;
	glm::mat4* m_corridors_transformation_normal_matrix;

	class GeometryNode** m_cannon_geometry;
	glm::mat4* m_cannon_transformation_matrix;
	glm::mat4* m_cannon_transformation_normal_matrix;

	class GeometryNode** m_pipe_geometry;
	glm::mat4* m_pipe_transformation_matrix;
	glm::mat4* m_pipe_transformation_normal_matrix;

	class GeometryNode** m_beam_geometry;
	glm::mat4* m_beam_transformation_matrix;
	glm::mat4* m_beam_transformation_normal_matrix;

	class GeometryNode** m_cannon_mount_geometry;
	glm::mat4* m_cannon_mount_transformation_matrix;
	glm::mat4* m_cannon_mount_transformation_normal_matrix;

	class GeometryNode** m_corridor_fork_geometry;
	glm::mat4* m_corridor_fork_transformation_matrix;
	glm::mat4* m_corridor_fork_transformation_normal_matrix;

	class GeometryNode** m_corridor_left_geometry;
	glm::mat4* m_corridor_left_transformation_matrix;
	glm::mat4* m_corridor_left_transformation_normal_matrix;

	class GeometryNode** m_corridor_right_geometry;
	glm::mat4* m_corridor_right_transformation_matrix;
	glm::mat4* m_corridor_right_transformation_normal_matrix;

	class GeometryNode** m_iris_geometry;
	glm::mat4* m_iris_transformation_matrix;
	glm::mat4* m_iris_transformation_normal_matrix;

	// Protected Functions
	bool InitRenderingTechniques();
	bool InitIntermediateShaderBuffers();
	bool InitCommonItems();
	bool InitLightSources();
	bool InitGeometricMeshes();

	ShaderProgram								m_geometry_rendering_program;
	ShaderProgram								m_postprocess_program;
	ShaderProgram								m_spot_light_shadow_map_program;

public:
	Renderer();
	~Renderer();
	bool										Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										Update(float dt);
	bool										ResizeBuffers(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	bool										ReloadShaders();
	void										Render();

	// Passes
	void										RenderShadowMaps();
	void										RenderGeometry();
	void										RenderToOutFB();

	// Set functions
	void										SetRenderingMode(RENDERING_MODE mode);

	// Camera Function
	void										CameraMoveForward(bool enable);
	void										CameraMoveBackWard(bool enable);
	void										CameraMoveLeft(bool enable);
	void										CameraMoveRight(bool enable);
	void										CameraLook(glm::vec2 lookDir);
};

#endif
