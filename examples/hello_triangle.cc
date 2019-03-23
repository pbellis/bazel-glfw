#include <array>
#include <iostream>
#include <ostream>

#include <glad/glad.h> // must go before glfw3.h
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

struct Vertex {
  glm::vec2 position;
  glm::vec3 color;
};


void error_callback(int error, const char* description) {
  std::cerr << "error[" << error << "]:" << description << std::endl;
}

int main(int argc, char **argv) {
  constexpr GLchar* vertex_shader_text = R"vert(
    uniform mat4 MVP;
    attribute vec3 vCol;
    attribute vec2 vPos;
    varying vec3 color;
    
    void main() {
      gl_Position = MVP * vec4(vPos, 0.0, 1.0);
      color = vCol;
    }
  )vert";

  constexpr GLchar* fragment_shader_text = R"frag(
    varying vec3 color;
    void main() {
      gl_FragColor = vec4(color, 1.0);
    }
  )frag";

  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  
  window = glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr);
  
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  std::array<Vertex, 3> vertices {
    glm::vec2(-0.6f, -0.4f), glm::vec3(1.f, 0.f, 0.f),
    glm::vec2(0.6f, -0.4f), glm::vec3(0.f, 1.f, 0.f),
    glm::vec2(0.f,   0.6f), glm::vec3(0.f, 0.f, 1.f)
  };

  for (const auto& vertex : vertices) {
    std::cout << "vertex.position: " << glm::to_string(vertex.position) << ",";
    std::cout << "vertex.color: " << glm::to_string(vertex.color) << std::endl;
  }

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
  
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint mvp_location = glGetUniformLocation(program, "MVP");
  GLint vpos_location = glGetAttribLocation(program, "vPos");
  GLint vcol_location = glGetAttribLocation(program, "vCol");
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(float) * 5, nullptr);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(float) * 5, (void *)(sizeof(float) * 2));
  while (!glfwWindowShouldClose(window)) {    
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);

    float ratio = width / static_cast<float>(height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glm::mat4 m(1.0f);
    m = glm::rotate(m, (float)glfwGetTime(), glm::vec3(0, 0, 1));
    
    glm::mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, -1.f, 1.f);

    glm::mat4 mvp = p * m;
    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE,  glm::value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
