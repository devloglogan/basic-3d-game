#version 330 core

in vec3 v_col;

out vec4 o_col;

void main()
{
  o_col = vec4(v_col, 1.0);
}
