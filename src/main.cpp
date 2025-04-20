#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/trigonometric.hpp"
#include "utils.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#define SCREEN_WIDTH 640.0f
#define SCREEN_HEIGHT 480.0f
#define TIME_SEC (float)SDL_GetTicks() / 1000.0f

// clang-format off
float vertices[] =
{
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

float colors[] =
{
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f
};
// clang-format on

enum VBO
{
  VBO_POSITION,
  VBO_COLOR,
  VBO_MAX
};

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
    std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_GLContext opengl_context = SDL_GL_CreateContext(window);
  if (opengl_context == nullptr)
  {
    std::cout << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  if (glewInit() != GLEW_OK)
  {
    std::cout << "GLEW could not be initialized! GLEW_Error: " << glewGetErrorString(glGetError()) << std::endl;
    return 1;
  }

  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo[VBO_MAX];
  glGenBuffers(VBO_MAX, &vbo[0]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(VBO_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VBO_POSITION);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_COLOR]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
  glVertexAttribPointer(VBO_COLOR, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(VBO_COLOR);

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
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
    std::cout << info_log << std::endl;
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
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
    std::cout << info_log << std::endl;
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
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl;
    std::cout << info_log << std::endl;
  }

  glUseProgram(shader_program);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  glm::mat4 view { 1.0f };
  view = glm::translate(view, glm::vec3 { 0.0f, 0.0f, -3.0f });
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

    glUseProgram(shader_program);
    glBindVertexArray(vao);

    glm::mat4 model { 1.0f };
    model = glm::translate(model, paddle_pos);
    model = glm::scale(model, glm::vec3 { 1.0f, 0.25f, 0.5f });
    int u_model = glGetUniformLocation(shader_program, "u_model");
    glUniformMatrix4fv(u_model, 1, false, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 36);

    SDL_GL_SwapWindow(window);
  }

  return 0;
}
