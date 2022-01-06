#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <ratio>

#define GLEW_STATIC 1
#include <GL/glew.h>

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#define __gl_h_
#include <GLUT/glut.h>
#define MACOSX_COMPATIBILITY GLUT_3_2_CORE_PROFILE
#else
#include <GL/glut.h>
#define MACOSX_COMPATIBILITY 0
#endif
#include "glhelper.h"
#include "camera.h"

const int NB_PARTICULES = 10000;
const int dt_test = 16;

GLuint VAO;
GLuint buffers[2];

Camera cam;

GLuint program_id;
GLuint program_shader;

GLuint nframe;
std::chrono::high_resolution_clock::time_point start;


void init()
{
  program_id = glhelper::create_program_from_file("shaders/basic.vert", "shaders/basic.frag");
  program_shader = glhelper::create_program_compute_shader("shaders/freefall.comp");


  std::vector<GLfloat> positions(NB_PARTICULES*3);
  std::vector<GLfloat> vitesses(NB_PARTICULES*3);

  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(-0.3,0.3);
  for(auto i = 0u; i < NB_PARTICULES*3; ++i)
  {
    positions[i] = 0.;
    vitesses[i] = ((i+2)%3) == 0 ? std::fabs(distribution(generator)) : distribution(generator);
  }
  std::cout << "Size particules in RAM : " 
    << float(sizeof(float) * positions.size())/std::mega::num  << " Mbytes" << std::endl;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(2, buffers);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*positions.size(), positions.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
  glEnableVertexAttribArray(0); 

  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vitesses.size(),vitesses.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); 
  glEnableVertexAttribArray(1); 

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glPointSize(3);

  start = std::chrono::high_resolution_clock::now();
}

void compute_fps()
{
 ++nframe;
  if(nframe == 100)
  {
    auto stop = std::chrono::high_resolution_clock::now();
    auto diff = (stop-start).count();
    std::cout << 100./(float(diff) / std::nano::den) << std::endl;
    nframe = 0;
    start = stop;
  }
}

void set_uniform_mvp(GLuint program)
{
  GLint mvp_id = glGetUniformLocation(program, "MVP");
  GLint current_prog_id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_prog_id);
  glUseProgram(program);
  if (mvp_id != -1)
  {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = cam.projection()*cam.view()*model;
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);
  }
  glUseProgram(current_prog_id);
}

static void display_callback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program_id);
  set_uniform_mvp(program_id);
  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, NB_PARTICULES);

  glUseProgram(program_shader);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1 , buffers[0]);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2 , buffers[1]);
  glDispatchCompute(NB_PARTICULES/100,1,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  glBindVertexArray(0);
  glutSwapBuffers ();
  compute_fps();
  //only needed for benchmark
  // glutPostRedisplay();
}

static void keyboard_callback(unsigned char key, int, int)
{
  int viewport[4];

  switch (key)
  {
    case 'p':
      glGetIntegerv(GL_VIEWPORT, viewport);
      glhelper::print_screen(viewport[2], viewport[3]);
      break;
    case 'q':
    case 'Q':
    case 27:
              exit(0);
  }
}

static void reshape_callback(int width, int height)
{
  cam.common_reshape(width,height);

  glViewport(0,0, width, height);
}
static void mouse_callback (int button, int action, int x, int y)
{
  cam.common_mouse(button, action, x, y);
}

static void motion_callback(int x, int y)
{
  cam.common_motion(x, y);
}

static void timer_callback(int)
{
  glutTimerFunc(dt_test, timer_callback, 0);
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | MACOSX_COMPATIBILITY);
  glutInitWindowSize(cam.width(), cam.height());
  glutCreateWindow("opengl");
  glutDisplayFunc(display_callback);
  glutMotionFunc(motion_callback);
  glutMouseFunc(mouse_callback);
  glutKeyboardFunc(keyboard_callback);
  glutReshapeFunc(reshape_callback);
  glutTimerFunc(25, timer_callback, 0);
  glewExperimental=true;
  glewInit();

  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  std::cout << "OpenGL Version: " << major <<"."<< minor << std::endl;

  init();
  glutMainLoop();

  return 0;
}
