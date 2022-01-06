#include <chrono>
#include <iostream>
#include <fstream>
#include <random>

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

const int NB_PARTICULES = 10;

GLuint VAO;
GLuint VBO[1];

Camera cam;

GLuint program_id;

GLuint nframe;
std::chrono::high_resolution_clock::time_point start;


void init()
{
  program_id = glhelper::create_program_from_file("shaders/basic.vert", "shaders/basic.frag");

  std::vector<GLfloat> positions(NB_PARTICULES*3);
  //std::vector<float> buff(3*NB_PARTICULES);

  //glGetNamedBufferSubData(VAO, 0, 3*NB_PARTICULES*sizeof(float), buff.data()); //ça fait quoi ?

  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(-0.3,0.3);
  for(auto i = 0u; i < NB_PARTICULES*3; ++i)
  {
    positions[i] = 0.;
    //vitesses[i] = distribution(generator);
  }
  std::cout << "Size particules in RAM : " 
    << float(sizeof(float) * positions.size())/std::mega::num  << " Mbytes" << std::endl;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*positions.size(), positions.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
  glEnableVertexAttribArray(0);

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

// Variable globale dt
void set_uniform_dt(GLuint program)   
{
  GLint dt_id = glGetUniformLocation(program, "dt");
  GLint current_prog_id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_prog_id);
  glUseProgram(program);
  if (dt_id != -1)
  {
    glUniform1f( dt_id, 0.01f );
  }
  glUseProgram(current_prog_id);
}

static void display_callback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Redéfinition des VBO dans le VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);

  glUseProgram(program_id);
  set_uniform_mvp(program_id);
  set_uniform_dt(program_id); //variable globale dt
  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, NB_PARTICULES);

  glBindVertexArray(0);
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
  glutTimerFunc(25, timer_callback, 0);
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

// int major, minor;
// glGetIntegerv(GL_MAJOR_VERSION, &major);
// glGetIntegerv(GL_MINOR_VERSION, &minor);
// std::cout << "OpenGL Version: " << major <<"."<< minor << std::endl;

  init();
  glutMainLoop();

  return 0;
}
