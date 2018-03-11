#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Directional light data
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 direction;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light being used in the scene
uniform point_light[2] points;
// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
uniform sampler2D nm_tex;
uniform int use_normal_map;
uniform int use_dissolve;
uniform sampler2D alpha_map;
// Shadow map to sample from
uniform sampler2D shadow_map;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec2 tex_coord;
// Incoming texture coordinate
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;
// Incoming light space position
layout(location = 5) in vec4 light_space_pos;

// Outgoing colour
layout(location = 0) out vec4 colour;

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // *********************************
  // Get distance between point light and vertex
  float dist = length ( position - point.position );

  // Calculate attenuation factor
  float att = 1 / ( point.constant + point.linear * dist + point.quadratic * dist * dist );

  // Calculate light colour
  vec4 light_colour = att * point.light_colour;

  // Calculate light dir
  vec3 light_dir = normalize ( point.position - position );

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k = max ( dot ( normal, light_dir ), 0.0 );
  vec4 diffuse = k * ( mat.diffuse_reflection * light_colour );

  // Calculate half vector
  vec3 half_vector = normalize ( light_dir + view_dir );

  // Calculate specular component
  k = pow ( max ( dot ( normal, half_vector ), 0 ), mat.shininess );
  vec4 specular = k * mat.specular_reflection * light_colour;

  // Calculate primary colour component
  vec4 primary = mat.emissive + diffuse;

  // Calculate final colour - remember alpha
  vec4 return_colour = primary * tex_colour + specular;
  return_colour.a = 1;

  // Return colour
  return return_colour;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
  vec3 direction = normalize ( spot.position - position );

  // Calculate distance to light
  float dist = length ( spot.position - position );

  // Calculate attenuation value
  float att = 1 / ( spot.constant + spot.linear * dist + spot.quadratic * dist * dist );

  // Calculate spot light intensity
  float light_intensity = pow ( max ( dot ( -spot.direction, direction ), 0 ), spot.power );

  // Calculate light colour
  vec4 light_colour = light_intensity * att * spot.light_colour;

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  float k = max ( dot ( normal, direction ), 0 );
  vec4 diffuse = k * ( mat.diffuse_reflection * light_colour );

  // Calculate half vector
  vec3 half_vector = normalize ( direction + view_dir );

  // Calculate specular component
  k = pow ( max ( dot ( normal, half_vector ), 0 ), mat.shininess );
  vec4 specular = k * mat.specular_reflection * light_colour;

  // Calculate primary colour component
  vec4 primary = mat.emissive + diffuse;

  // Calculate final colour - remember alpha
  vec4 return_colour = normalize ( primary * tex_colour + specular );
  return_colour.a = 1;

  // Return colour
  return return_colour;
}

vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord)
{
	// *********************************
	// Ensure normal, tangent and binormal are unit length (normalized)
	normal = normalize ( normal );
	tangent = normalize ( tangent );
	binormal = normalize ( binormal );
	// Sample normal from normal map
	vec3 sampled_normal = vec3 ( texture ( nm_tex, tex_coord ) );
	// *********************************
	// Transform components to range [0, 1]
	sampled_normal = 2.0 * sampled_normal - vec3(1.0, 1.0, 1.0);
	// Generate TBN matrix
	mat3 TBN = mat3(tangent, binormal, normal);
	// Return sampled normal transformed by TBN
	return normalize(TBN * sampled_normal);
}

// Calculates the shadow factor of a vertex
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos) {
  // Get light screen space coordinate
  vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
  // Use this to calculate texture coordinates for shadow map
  vec2 shadow_tex_coords;
  // This is a bias calculation to convert to texture space
  shadow_tex_coords.x = 0.5 * proj_coords.x + 0.5;
  shadow_tex_coords.y = 0.5 * proj_coords.y + 0.5;
  // Check shadow coord is in range
  if (shadow_tex_coords.x < 0 || shadow_tex_coords.x > 1 || shadow_tex_coords.y < 0 || shadow_tex_coords.y > 1) {
    return 1.0;
  }
  float z = 0.5 * proj_coords.z + 0.5;
  // *********************************
  // Now sample the shadow map, return only first component (.x/.r)
  float depth = texture ( shadow_map, shadow_tex_coords ).x;
  // *********************************
  // Check if depth is in range.  Add a slight epsilon for precision
  if (depth == 0.0) {
    return 1.0;
  } else if (depth < z + 0.001) {
    return 0.5;
  } else {
    return 1.0;
  }
}

void main() {

  colour = vec4(0.0, 0.0, 0.0, 1.0);

  if ( use_dissolve == 1 )
	if ( texture ( alpha_map, tex_coord ).r > 0.8 )
	  discard;

  float shade_factor = calculate_shadow ( shadow_map, light_space_pos );

  // *********************************
  // Calculate view direction
  vec3 view_dir = normalize ( eye_pos - position );

  // Sample texture
  vec4 tex_colour = texture ( tex, tex_coord );

  // Calculate normal map
  vec3 new_normal = normal;
  if ( use_normal_map == 1 )
	new_normal = calc_normal ( normal, tangent, binormal, nm_tex, tex_coord );

  // Sum lights
  colour += calculate_spot ( spot, mat, position, new_normal, view_dir, tex_colour );
  for ( int a = 0; a < 2; ++a )
	colour += calculate_point ( points[a], mat, position, new_normal, view_dir, tex_colour );
  colour *= shade_factor;
  colour.a = 1;
}
