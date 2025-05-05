#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/trigonometric.hpp"
#include "utils.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <vector>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

using namespace tinygltf;

#define SCREEN_WIDTH 640.0f
#define SCREEN_HEIGHT 480.0f
#define TIME_SEC (float)SDL_GetTicks() / 1000.0f

// TODO: Make this not global
unsigned int texture_id;

// TODO: Create Vertex struct and use glm vectors
struct MeshData
{
  std::vector<float> positions;
  std::vector<float> tex_coords;
  std::vector<uint32_t> indices;
};

enum VBO
{
  VBO_POSITION,
  VBO_TEX_COORD,
  VBO_MAX
};

// TODO: Update variables to const refs where applicable
MeshData load_mesh_data(const std::string& filepath)
{
  Model model;
  TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);

  if (!warn.empty()) std::cerr << "Warning: " << warn << std::endl;
  if (!err.empty()) std::cerr << "Error: " << err << std::endl;
  if (!ret)
  {
    std::cerr << "Failed to load glTF!" << std::endl;
    return MeshData {};
  }

  // Assuming mesh 0, primitive 0, one material.
  Primitive primitive = model.meshes[0].primitives[0];
  Material material = model.materials[primitive.material];
  Texture texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
  Image image = model.images[texture.source];

  // Position
  Accessor pos_accessor = model.accessors[primitive.attributes.at("POSITION")];
  BufferView pos_buffer_view = model.bufferViews[pos_accessor.bufferView];
  Buffer pos_buffer = model.buffers[pos_buffer_view.buffer];
  const unsigned char* pos_data = pos_buffer.data.data() + pos_buffer_view.byteOffset + pos_accessor.byteOffset;
  std::vector<float> positions(pos_accessor.count * 3);
  memcpy(positions.data(), pos_data, pos_accessor.count * 3 * sizeof(float));

  // Tex Coord
  Accessor tc_accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
  BufferView tc_buffer_view = model.bufferViews[tc_accessor.bufferView];
  Buffer tc_buffer = model.buffers[tc_buffer_view.buffer];
  const unsigned char* tc_data = tc_buffer.data.data() + tc_buffer_view.byteOffset + tc_accessor.byteOffset;
  std::vector<float> tex_coords(tc_accessor.count * 2);
  memcpy(tex_coords.data(), tc_data, tc_accessor.count * 2 * sizeof(float));

  // Indices
  Accessor idx_accessor = model.accessors[primitive.indices];
  BufferView idx_buffer_view = model.bufferViews[idx_accessor.bufferView];
  Buffer idx_buffer = model.buffers[idx_buffer_view.buffer];
  const unsigned char* idx_data = idx_buffer.data.data() + idx_buffer_view.byteOffset + idx_accessor.byteOffset;
  std::vector<uint32_t> indices(idx_accessor.count);
  if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
  {
    const uint16_t* src = reinterpret_cast<const uint16_t*>(idx_data);
    for (size_t i = 0; i < indices.size(); ++i) indices[i] = src[i];
  }
  else if (idx_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
  {
    const uint32_t* src = reinterpret_cast<const uint32_t*>(idx_data);
    for (size_t i = 0; i < indices.size(); ++i) indices[i] = src[i];
  }
  else
  {
    std::cerr << "Unsupported glTF index type!" << std::endl;
  }

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  GLenum format = GL_RGB;
  if (image.component == 1)
    format = GL_RED;
  else if (image.component == 3)
    format = GL_RGB;
  else if (image.component == 4)
    format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.image.data());

  glGenerateMipmap(GL_TEXTURE_2D);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return { positions, tex_coords, indices };
}

int main(int argc, char* argv[])
{
  SDL_Window* window;
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  window = SDL_CreateWindow("Breakout 3D",
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      SDL_WINDOW_OPENGL);

  if (window == nullptr)
  {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_GLContext opengl_context = SDL_GL_CreateContext(window);
  if (opengl_context == nullptr)
  {
    std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  if (glewInit() != GLEW_OK)
  {
    std::cerr << "GLEW could not be initialized! GLEW_Error: " << glewGetErrorString(glGetError()) << std::endl;
    return 1;
  }

  MeshData mesh_data = load_mesh_data("../res/models/pyramid/pyramid.gltf");

  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo[VBO_MAX];
  glGenBuffers(VBO_MAX, &vbo[0]);

  // TODO: Use offsetof function when Vertex struct is implemented
  glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, mesh_data.positions.size() * sizeof(float), mesh_data.positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(VBO_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VBO_POSITION);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_TEX_COORD]);
  glBufferData(GL_ARRAY_BUFFER, mesh_data.tex_coords.size() * sizeof(float), mesh_data.tex_coords.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(VBO_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VBO_TEX_COORD);

  unsigned int ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size() * sizeof(uint32_t), mesh_data.indices.data(), GL_STATIC_DRAW);

  int success;
  char info_log[512];

  unsigned int vert_shader;
  std::string vert_string = Utils::load_file_source("../res/shaders/default.vert");
  const char* vert_src = vert_string.c_str();
  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_src, nullptr);
  glCompileShader(vert_shader);
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vert_shader, 512, nullptr, info_log);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
    std::cerr << info_log << std::endl;
  }

  unsigned int frag_shader;
  std::string frag_string = Utils::load_file_source("../res/shaders/default.frag");
  const char* frag_src = frag_string.c_str();
  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_src, nullptr);
  glCompileShader(frag_shader);
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(frag_shader, 512, nullptr, info_log);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
    std::cerr << info_log << std::endl;
  }

  unsigned int shader_program;
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl;
    std::cerr << info_log << std::endl;
  }

  glUseProgram(shader_program);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  glm::mat4 view { 1.0f };
  view = glm::translate(view, glm::vec3 { 0.0f, 0.0f, -4.0f });
  int u_view = glGetUniformLocation(shader_program, "u_view");
  glUniformMatrix4fv(u_view, 1, false, glm::value_ptr(view));

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
  int u_projection = glGetUniformLocation(shader_program, "u_projection");
  glUniformMatrix4fv(u_projection, 1, false, glm::value_ptr(projection));

  glm::vec3 paddle_pos = glm::vec3 { 0.0f, -0.75f, 0.0f };

  const bool* keystates = SDL_GetKeyboardState(nullptr);

  while (!done)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      // Handle SDL events.
    }

    if (keystates[SDL_SCANCODE_ESCAPE])
    {
      done = true;
    }
    // TODO: Account for delta time in paddle movement (and everywhere I guess lol)
    if (keystates[SDL_SCANCODE_D])
    {
      paddle_pos.x += 0.0003;
    }
    if (keystates[SDL_SCANCODE_A])
    {
      paddle_pos.x -= 0.0003;
    }
    paddle_pos.x = glm::clamp(paddle_pos.x, -1.5f, 1.5f);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    glUseProgram(shader_program);
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(glGetUniformLocation(shader_program, "u_texture"), 0);

    glm::mat4 model { 1.0f };
    model = glm::translate(model, paddle_pos);
    int u_model = glGetUniformLocation(shader_program, "u_model");
    glUniformMatrix4fv(u_model, 1, false, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, mesh_data.indices.size(), GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
  }

  return 0;
}
