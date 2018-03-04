#version 440

// *********************************
// Define the output colour for the shader here
uniform vec4 colour_one;
uniform vec4 colour_two;
// *********************************

// Outgoing colour for the shader
layout ( location = 0 ) out vec4 colour_out;

void main ( )
{
  // *********************************
  // Set outgoing colour
  colour_out = colour_one + colour_two;
  // *********************************
}