#version 330 core

in vec2 v_tex_coord;

uniform sampler2D u_texture;

out vec4 o_col;

void main()
{
  o_col = texture(u_texture, v_tex_coord);
}
