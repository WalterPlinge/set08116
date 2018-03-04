#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/projection.hpp>
#include <iostream>

#define PI pi<float>( )

using namespace std;
using namespace glm;

int main ( )
{
  // 2D vectors
  vec2 v2_a ( 1, 0 );
  vec2 v2_b ( 0, 1 );

  // 3D vectors
  vec3 v3_a ( 1, 0, 0 );
  vec3 v3_b ( 0, 0, 1 );

  // 4D vectors
  vec4 v4_a ( 1, 0, 0, 0 );
  vec4 v4_b ( 0, 0, 0, 1 );

  // Length of vector A
  float l2 ( length ( v2_a ) );
  float l3 ( length ( v3_a ) );
  float l4 ( length ( v4_a ) );

  // Normalise vector A
  vec2 n2 ( normalize ( v2_a ) );
  vec3 n3 ( normalize ( v3_a ) );
  vec4 n4 ( normalize ( v4_a ) );

  // Dot product of A and B
  float d2 ( dot ( v2_a, v2_b ) );
  float d3 ( dot ( v3_a, v3_b ) );
  float d4 ( dot ( v4_a, v4_b ) );

  // Projection of A onto B
  vec2 p2 ( proj ( v2_a, v2_b ) );
  vec3 p3 ( proj ( v3_a, v3_b ) );
  vec4 p4 ( proj ( v4_a, v4_b ) );

  // Cross product of A and B
  vec3 c3 ( cross ( v3_a, v3_b ) );

  /// Matrices
  // Identity matrix
  mat4 m ( 1 );
  mat3 n ( m );

  // Scaled matrix
  mat4 o = n / 5.f;

  // Multiplication
  mat4 p = m * o;

  // Translation matrix
  mat4 mt = translate ( mat4 ( 1 ), vec3 ( 1 ) );
  vec3 u;

  // Translated vector
  vec3 v = mt * vec4 ( u, 1 );

  // Rotation vectors
  mat4 mrx = rotate ( mat4 ( 1 ), PI / 2, vec3 ( 1, 0, 0 ) );
  mat4 mry = rotate ( mat4 ( 1 ), PI / 2, vec3 ( 0, 1, 0 ) );
  mat4 mrz = rotate ( mat4 ( 1 ), PI / 2, vec3 ( 0, 0, 1 ) );
  vec3 mr = mrx * vec4 ( v3_a, 1 );

  // Euler angle
  mat4 R = eulerAngleXYZ ( PI / 2, PI / 2, PI / 2 );

  // Scale matrix
  mat4 sm = scale ( mat4 ( 1 ), vec3 ( 1, 2, 3 ) );

  /// Quarternions
  // Rotations
  quat qrx = rotate ( quat ( ), PI / 2, vec3 ( 1, 0, 0 ) );
  quat qry = rotate ( quat ( ), PI / 2, vec3 ( 0, 1, 0 ) );
  quat qrz = rotate ( quat ( ), PI / 2, vec3 ( 0, 0, 1 ) );
  quat qr = qrx * qry * qrz;
  mat4 mqr = mat4_cast(qr);
  vec3 mqrv = mqr * vec4 ( v3_a, 1 );



  // Testing
  mat4 transformation = sm * mrz * mt;
  vec3 out = transformation * vec4 ( v3_a, 1 );
  cout << out.x << " " << out.y << " " << out.z << " Should be -1 4 3" << endl;

  // END
  cout << "Press enter to continue...";
  getchar();
  return 0;
}