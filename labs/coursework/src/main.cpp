#include <glm/glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

free_camera cam1;
target_camera cam0;
unsigned char cam = 0;
map<string, mesh> meshes;
map<string, effect> effects;
map<string, texture> textures;
vector<mesh> cubes;
vector<mesh> cubes2;

bool initialise ( )
{
	glfwSetInputMode ( renderer::get_window ( ), GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPos ( renderer::get_window ( ), 0.0, 0.0 );
	return true;
}

bool load_content ( )
{
	meshes["ground"] = mesh ( geometry_builder::create_disk ( 127, vec2 ( 64 ) ) );
	meshes["stadium"] = mesh ( geometry_builder::create_torus ( 64, 64, 8, 40 ) );
	meshes["freecam"] = mesh ( geometry_builder::create_sphere ( 64, 64 ) );

	effects["basic_textured"].add_shader ( "res/shaders/shader.vert", GL_VERTEX_SHADER );
	effects["basic_textured"].add_shader ( "res/shaders/shader.frag", GL_FRAGMENT_SHADER );
	effects["basic_textured"].build ( );

	textures["grass"] = texture ( "res/textures/grass.jpg" );
	textures["smiley"] = texture ( "res/textures/smiley.png" );
	textures["checker"] = texture ( "res/textures/checker.png" );
	textures["check_1"] = texture ( "res/textures/check_1.png" );
	textures["check_2"] = texture ( "res/textures/check_2.png" );

	for ( auto a = 0; a < 32; ++a )
	{
		cubes.insert ( cubes.end ( ), mesh ( geometry_builder::create_box ( ) ) );
		cubes[a].get_transform ( ).position = vec3 ( 1, 0, 0 );
	}
	cubes[0].get_transform ( ).position = vec3 ( 0, 1, 0 );
	for ( auto a = 0; a < 24; ++a )
	{
		cubes2.insert ( cubes2.end ( ), mesh ( geometry_builder::create_box ( ) ) );
		cubes2[a].get_transform ( ).position = vec3 ( 1, 0, 0 );
	}
	cubes2[0].get_transform ( ).position = vec3 ( 0, 2, 0 );

	cam1.set_position ( vec3 ( 5 ) );
	cam1.set_up ( vec3 ( -1, 1, -1 ) );
	cam1.set_projection ( quarter_pi<float> ( ), renderer::get_screen_aspect ( ), 0.1f, 1000.f );

	// Set camera properties
	cam0.set_position ( vec3 ( 5 ) );
	cam0.set_target ( vec3 ( 0 ) );
	cam0.set_projection ( quarter_pi<float> ( ), renderer::get_screen_aspect ( ), 0.1f, 1000.0f );
	return true;
}

bool update ( float delta_time )
{
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_1 ) )
		cam = 0;
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_2 ) )
		cam = 1;

	float speed;
	if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_LEFT_SHIFT ) )
		speed = 5;
	else
		speed = 1;

	if ( cam )
	{
		static double ratio_width = quarter_pi<float> ( ) / static_cast<float>( renderer::get_screen_width ( ) );
		static double ratio_height = quarter_pi<float> ( ) / static_cast<float>( renderer::get_screen_height ( ) );

		double current_x;
		double current_y;

		glfwGetCursorPos ( renderer::get_window ( ), &current_x, &current_y );

		glfwSetCursorPos ( renderer::get_window ( ), 0.0, 0.0 );

		auto delta_x = current_x * ratio_width;
		auto delta_y = current_y * ratio_height;

		cam1.rotate ( delta_x, -delta_y );

		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_W ) )
			cam1.set_position ( cam1.get_position ( ) + cam1.get_forward ( ) * 5.f * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_S ) )
			cam1.set_position ( cam1.get_position ( ) - cam1.get_forward ( ) * 5.f * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_A ) )
			cam1.set_position ( cam1.get_position ( ) - cross ( cam1.get_forward ( ), cam1.get_up ( ) ) * 5.f * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_D ) )
			cam1.set_position ( cam1.get_position ( ) + cross ( cam1.get_forward ( ), cam1.get_up ( ) ) * 5.f * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_SPACE ) )
			cam1.set_position ( cam1.get_position ( ) + cam1.get_up ( ) * 5.f * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_LEFT_CONTROL ) )
			cam1.set_position ( cam1.get_position ( ) - cam1.get_up ( ) * 5.f * speed * delta_time );

		cam1.update ( delta_time );
	}
	else
	{
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_UP ) )
			cam0.set_position ( cam0.get_position ( ) - normalize ( cam0.get_target ( ) - cam0.get_position ( ) ) * distance ( cam0.get_position ( ), cam0.get_target ( ) ) * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_DOWN ) && distance ( cam0.get_position ( ), cam0.get_target ( ) ) > 1 )
			cam0.set_position ( cam0.get_position ( ) + normalize ( cam0.get_target ( ) - cam0.get_position ( ) ) * distance ( cam0.get_position ( ), cam0.get_target ( ) ) * speed * delta_time );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_LEFT ) )
			cam0.set_position ( vec4 ( cam0.get_position ( ), 1 ) * eulerAngleY ( quarter_pi<float> ( ) * speed * delta_time ) );
		if ( glfwGetKey ( renderer::get_window ( ), GLFW_KEY_RIGHT ) )
			cam0.set_position ( vec4 ( cam0.get_position ( ), 1 ) * eulerAngleY ( -quarter_pi<float> ( ) * speed * delta_time ) );

		cam1.set_up ( cam0.get_up ( ) );
		cam1.set_position ( cam0.get_position ( ) );
		cam1.set_target ( cam0.get_target ( ) );
		cam1.update ( delta_time );

		cam0.update ( delta_time );
	}

	meshes["freecam"].get_transform ( ).position = cam1.get_position ( );

	for ( auto a = 0; a < cubes.size ( ); ++a )
	{
		cubes[a].get_transform ( ).rotate ( vec3 ( a ? delta_time : 0, !a ? delta_time / 4 : 0, 0 ) );
	}
	for ( auto a = 0; a < cubes2.size ( ); ++a )
	{
		cubes2[a].get_transform ( ).rotate ( vec3 ( a ? -delta_time / 4 : 0, !a ? delta_time : 0, 0 ) );
	}

	// Update the camera
	return true;
}

bool render ( )
{
	auto V = cam ? cam1.get_view ( ) : cam0.get_view ( );
	auto P = cam ? cam1.get_projection ( ) : cam0.get_projection ( );
	auto PV = P * V;

	auto eff = effects["basic_textured"];
	renderer::bind ( eff );

	auto tex = textures["grass"];
	renderer::bind ( tex, 0 );
	auto mesh = meshes["ground"];
	auto M = mesh.get_transform ( ).get_transform_matrix ( );
	glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( PV * M ) );
	glUniform1i ( eff.get_uniform_location ( "tex" ), 0 );
	renderer::render ( mesh );

	tex = textures["checker"];
	renderer::bind ( tex, 0 );
	mesh = meshes["stadium"];
	M = mesh.get_transform ( ).get_transform_matrix ( );
	glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( PV * M ) );
	glUniform1i ( eff.get_uniform_location ( "tex" ), 0 );
	renderer::render ( mesh );

	tex = textures["smiley"];
	renderer::bind ( tex, 0 );
	mesh = meshes["freecam"];
	M = mesh.get_transform ( ).get_transform_matrix ( );
	glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( PV * M ) );
	glUniform1i ( eff.get_uniform_location ( "tex" ), 0 );
	renderer::render ( mesh );

	for ( auto a = 0; a < cubes.size ( ); ++a )
	{
		if ( a % 6 )
		{
			tex = textures["check_1"];
			renderer::bind ( tex, 0 );
		}
		else
		{
			tex = textures["check_2"];
			renderer::bind ( tex, 0 );
		}
		mesh = cubes[a];
		M = mesh.get_transform ( ).get_transform_matrix ( );
		for ( auto b = a; b > 0; --b )
		{
			M = cubes[b - 1].get_transform ( ).get_transform_matrix ( ) * M;
		}
		glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( PV * M ) );
		glUniform1i ( eff.get_uniform_location ( "tex" ), 0 );
		renderer::render ( mesh );
	}
	for ( auto a = 0; a < cubes2.size ( ); ++a )
	{
		if ( ( a + 4 ) % 7 )
		{
			tex = textures["check_1"];
			renderer::bind ( tex, 0 );
		}
		else
		{
			tex = textures["check_2"];
			renderer::bind ( tex, 0 );
		}
		mesh = cubes2[a];
		M = mesh.get_transform ( ).get_transform_matrix ( );
		for ( auto b = a; b > 0; --b )
		{
			M = cubes2[b - 1].get_transform ( ).get_transform_matrix ( ) * M;
		}
		glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( PV * M ) );
		glUniform1i ( eff.get_uniform_location ( "tex" ), 0 );
		renderer::render ( mesh );
	}

	return true;
}

void main ( )
{
	// Create application
	app application ( "Graphics Coursework" );
	// Set load content, update and render methods
	application.set_load_content ( load_content );
	application.set_initialise ( initialise );
	application.set_update ( update );
	application.set_render ( render );
	// Run application
	application.run ( );
}