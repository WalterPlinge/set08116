#include <glm/glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

mesh m;
effect eff;
target_camera cam;

bool load_content ( )
{
	// Construct geometry object
	geometry geom;
	// Create triangle data
	// Positions
	vector<vec3> positions{ vec3 ( 0.0f, 1.0f, 0.0f ), vec3 ( -1.0f, -1.0f, 0.0f ), vec3 ( 1.0f, -1.0f, 0.0f ) };
	// Colours
	vector<vec4> colours{ vec4 ( 1.0f, 0.0f, 0.0f, 1.0f ), vec4 ( 1.0f, 0.0f, 0.0f, 1.0f ), vec4 ( 1.0f, 0.0f, 0.0f, 1.0f ) };
	// Add to the geometry
	geom.add_buffer ( positions, POSITION_BUFFER );
	geom.add_buffer ( colours, COLOUR_BUFFER );

	// *********************************
	// Create mesh object here
	m = mesh ( geom );
	// *********************************

	// Load in shaders
	eff.add_shader ( "shaders/basic.vert", GL_VERTEX_SHADER );
	eff.add_shader ( "shaders/basic.frag", GL_FRAGMENT_SHADER );
	// Build effect
	eff.build ( );

	// Set camera properties
	cam.set_position ( vec3 ( 10.0f, 10.0f, 10.0f ) );
	cam.set_target ( vec3 ( 0.0f, 0.0f, 0.0f ) );
	auto aspect = static_cast<float>( renderer::get_screen_width ( ) ) / static_cast<float>( renderer::get_screen_height ( ) );
	cam.set_projection ( quarter_pi<float> ( ), aspect, 2.414f, 1000.0f );
	return true;
}

bool update ( float delta_time )
{
	// Use keys to update transform values
	// WSAD - movement
	// arrow keys - rotation
	// O decrease scale, P increase scale
	// Use the mesh functions, I've left two of the IFs as a hint
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_W ) )
	{
		m.get_transform ( ).position -= vec3 ( 0.0f, 0.0f, 5.0f ) * delta_time;
	}
	// *********************************
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_S ) )
	{
		m.get_transform ( ).position += vec3 ( 0.0f, 0.0f, 5.0f ) * delta_time;
	}
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_A ) )
	{
		m.get_transform ( ).position -= vec3 ( 5.0f, 0.0f, 0.0f ) * delta_time;
	}
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_D ) )
	{
		m.get_transform ( ).position += vec3 ( 5.0f, 0.0f, 0.0f ) * delta_time;
	}
	// *********************************
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_UP ) )
	{
		m.get_transform ( ).rotate ( vec3 ( -pi<float> ( ) * delta_time, 0.0f, 0.0f ) );
	}
	// *********************************
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_DOWN ) )
	{
		m.get_transform ( ).rotate ( vec3 ( pi<float> ( ) * delta_time, 0.0f, 0.0f ) );
	}
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_LEFT ) )
	{
		m.get_transform ( ).rotate ( vec3 ( 0.f, 0.f, -pi<float> ( ) * delta_time ) );
	}
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_RIGHT ) )
	{
		m.get_transform ( ).rotate ( vec3 ( 0.f, 0.f, pi<float> ( ) * delta_time ) );
	}
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_O ) )
	{
		m.get_transform ( ).scale += vec3 ( 5.f ) * delta_time;
	}
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_P ) )
	{
		if ( m.get_transform ( ).scale.x > 0 && m.get_transform ( ).scale.y > 0 && m.get_transform ( ).scale.z > 0 )
		{
			m.get_transform ( ).scale -= vec3 ( 5.f ) * delta_time;
		}
		else
		{
			m.get_transform ( ).scale = vec3 ( 1 );
		}
	}
	// *********************************
	// Update the camera
	cam.update ( delta_time );
	return true;
}

bool render ( )
{
	// Bind effect
	renderer::bind ( eff );
	mat4 M;
	// *********************************
	// Get the model transform from the mesh
	M = m.get_transform ( ).get_transform_matrix ( );
	// *********************************
	// Create MVP matrix
	auto V = cam.get_view ( );
	auto P = cam.get_projection ( );
	auto MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( MVP ) );
	// *********************************
	// Render the mesh here
	renderer::render ( m );
	// *********************************
	return true;
}

void main ( )
{
	// Create application
	app application ( "25_Transforming_Meshes" );
	// Set load content, update and render methods
	application.set_load_content ( load_content );
	application.set_update ( update );
	application.set_render ( render );
	// Run application
	application.run ( );
}