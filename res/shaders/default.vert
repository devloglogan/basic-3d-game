#version 330 core

layout (location = 0) in vec3 a_pos;

out vec3 v_col;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
  v_col = vec3(1.0, 0.0, 0.0);
  gl_Position = u_projection * u_view * u_model * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
}
