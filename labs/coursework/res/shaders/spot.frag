#version 440

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

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate direction to the light
  vec3 direction = normalize ( spot.position - position );

  // Calculate distance to light
  float dist = length ( spot.position - position );

  vec3 view_dir = position - eye_pos;
  vec4 tex_colour = texture ( tex, tex_coord );

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
  colour = normalize ( primary * tex_colour + specular );
  colour.a = 1;
}