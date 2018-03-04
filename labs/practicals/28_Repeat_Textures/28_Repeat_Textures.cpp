#include <glm/glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

mesh m, m2;
effect eff;
target_camera cam;
texture tex;

bool load_content ( )
{
  // Construct geometry object
  geometry geom;
  // Create triangle data
  // Positions
  vector<vec3> positions{ vec3 ( 0.0f, 1.0f, 0.0f ), vec3 ( -1.0f, -1.0f, 0.0f ), vec3 ( 1.0f, -1.0f, 0.0f ) };
  // *********************************
  // Define texture coordinates for triangle
  vector<vec2> tex_coords{ vec2 ( 0, 10 ), vec2 ( -10, -10 ), vec2 ( 10, -10 ) };
  // *********************************
  // Add to the geometry
  geom.add_buffer ( positions, POSITION_BUFFER );
  // *********************************
  // Add texture coordinate buffer to geometry
  geom.add_buffer ( tex_coords, TEXTURE_COORDS_0 );
  // *********************************

  // Create mesh object
  m = mesh ( geom );

  ///
  geometry geom2;
  geom2.add_buffer ( positions, POSITION_BUFFER );
  geom2.add_buffer ( tex_coords, TEXTURE_COORDS_0 );
  m2 = mesh ( geom2 );
  m2.get_transform ( ).orientation = eulerAngleY ( pi<float> ( ) / 2.f );
  m2.get_transform ( ).position = vec3 ( -1, 0, 1 );;
  ///

  // Load in texture shaders, !Note that are pulling in shader file from previous project!
  eff.add_shader ( "27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER );
  eff.add_shader ( "27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER );

  // *********************************
  // Build effect
  eff.build ( );
  // Load texture "textures/sign.jpg"
  tex = texture ( "textures/sign.jpg" );
  // *********************************

  // Set camera properties
  cam.set_position ( vec3 ( 4.0f, 4.0f, 4.0f ) );
  cam.set_target ( vec3 ( 0.0f, 0.0f, 0.0f ) );
  auto aspect = static_cast<float>( renderer::get_screen_width ( ) ) / static_cast<float>( renderer::get_screen_height ( ) );
  cam.set_projection ( quarter_pi<float> ( ), aspect, 1.0f, 1000.0f );

  return true;
}

bool update ( float delta_time )
{
  // Update the camera
  cam.update ( delta_time );
  return true;
}

bool render ( )
{
  // Bind effect
  renderer::bind ( eff );
  // Create MVP matrix
  auto M = m.get_transform ( ).get_transform_matrix ( );
  auto V = cam.get_view ( );
  auto P = cam.get_projection ( );
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), // Location of uniform
    1,                               // Number of values - 1 mat4
    GL_FALSE,                        // Transpose the matrix?
    value_ptr ( MVP ) );                 // Pointer to matrix data

// *********************************
// Bind texture to renderer
  renderer::bind ( tex, 0 );
  // Set the texture value for the shader here
  glUniform1i ( eff.get_uniform_location ( "tex" ), 0 );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // *********************************

  // Render the mesh
  renderer::render ( m );
  MVP = P * V * m2.get_transform ( ).get_transform_matrix ( );
  glUniformMatrix4fv ( eff.get_uniform_location ( "MVP" ), 1, GL_FALSE, value_ptr ( MVP ) );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
  renderer::render ( m2 );

  return true;
}

void main ( )
{
  // Create application
  app application ( "28_Repeat_Textures" );
  // Set load content, update and render methods
  application.set_load_content ( load_content );
  application.set_update ( update );
  application.set_render ( render );
  // Run application
  application.run ( );
}