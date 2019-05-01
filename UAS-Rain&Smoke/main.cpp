#include "glad.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "camera.h"

#include "filesystem.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

#define MAX_PARTICLES 1000
#define RAIN    0
#define SNOW    1
#define HAIL    2

float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

int loop;
int fall = 0;

//floor colors
float r = 0.0;
float g = 1.0;
float b = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;

typedef struct {
  // Life
  bool alive;   // is the particle alive?
  float life;   // particle lifespan
  float fade; // decay
  // color
  float red;
  float green;
  float blue;
  // Position/direction
  float xpos;
  float ypos;
  float zpos;
  // Velocity/Direction, only goes down in y dir
  float vel;
  // Gravity
  float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

// Initialize/Reset Particles - give them their attributes
void initParticles(int i) {
    par_sys[i].alive = true;
    par_sys[i].life = 1.0;
    par_sys[i].fade = float(rand()%100)/1000.0f+0.003f;

    par_sys[i].xpos = (float) (rand() % 21) - 10;
    par_sys[i].ypos = 10.0;
    par_sys[i].zpos = (float) (rand() % 21) - 10;

    par_sys[i].red = 0.5;
    par_sys[i].green = 0.5;
    par_sys[i].blue = 1.0;

    par_sys[i].vel = velocity;
    par_sys[i].gravity = -0.8;//-0.8;
}

void init( ) {
    // Initialize particles
    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        initParticles(loop);
    }
}

// For Rain
void drawRain() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(0.5, 0.5, 1.0);
      glBegin(GL_LINES);
        glVertex3f(x, y, z);
        glVertex3f(x, y+0.5, z);
      glEnd();

      // Update values
      //Move
      // Adjust slowdown for speed!
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= -10) {
        par_sys[loop].life = -1.0;
      }
      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// Draw Particles
void drawScene( ) {
  int i, j;
  float x, y, z;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();


  glRotatef(pan, 0.0, 1.0, 0.0);
  glRotatef(tilt, 1.0, 0.0, 0.0);

  // GROUND?!
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
    // along z - y const
    for (i = -10; i+1 < 11; i++) {
      // along x - y const
      for (j = -10; j+1 < 11; j++) {
        glColor3fv(ground_colors[i+10][j+10]);
        glVertex3f(ground_points[j+10][i+10][0],
              ground_points[j+10][i+10][1],
              ground_points[j+10][i+10][2] + zoom);
        glColor3fv(ground_colors[i+10][j+1+10]);
        glVertex3f(ground_points[j+1+10][i+10][0],
              ground_points[j+1+10][i+10][1],
              ground_points[j+1+10][i+10][2] + zoom);
        glColor3fv(ground_colors[i+1+10][j+1+10]);
        glVertex3f(ground_points[j+1+10][i+1+10][0],
              ground_points[j+1+10][i+1+10][1],
              ground_points[j+1+10][i+1+10][2] + zoom);
        glColor3fv(ground_colors[i+1+10][j+10]);
        glVertex3f(ground_points[j+10][i+1+10][0],
              ground_points[j+10][i+1+10][1],
              ground_points[j+10][i+1+10][2] + zoom);
      }

    }
  glEnd();
  // Which Particles
  if (fall == RAIN) {
    drawRain();
  }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, (float) w / (float) h, .1, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hujan Berasap", NULL, NULL);
    if (window == NULL)
    {
        //std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        //std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Kan kene iso
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("7.4.camera.vs", "7.4.camera.fs");
    Shader lightingShader("2.1.basic_lighting.vs", "2.1.basic_lighting.fs");
    Shader lampShader("2.1.lamp.vs", "2.1.lamp.fs");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // KOTAK 1
        //belakang
        -1.0f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         1.0f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         1.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         1.0f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        -1.0f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        -1.0f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

        //depan
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         1.0f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         1.0f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
         1.0f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -1.0f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

        // kiri
        -1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        -1.0f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
        -1.0f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
        -1.0f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
        -1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,

        //kanan
         1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
         1.0f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
         1.0f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
         1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,

         //bawah
        -1.0f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
         1.0f, -0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
        -1.0f, -0.5f, -0.5f,  0.0f, 1.0f, 0.5f,

        // atas
        -1.0f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,
         1.0f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,
         1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        -1.0f,  0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
        -1.0f,  0.5f, -0.5f,  0.0f, 1.0f, 0.5f,

        // ===========KOTAK 2===========
        //belakang
        -1.0f, -0.5f, -1.5f,  0.0f, 0.0f, 0.5f,
         1.0f, -0.5f, -1.5f,  1.0f, 0.0f, 0.5f,
         1.0f,  1.5f, -1.5f,  1.0f, 1.0f, 0.5f,
         1.0f,  1.5f, -1.5f,  1.0f, 1.0f, 0.5f,
        -1.0f,  1.5f, -1.5f,  0.0f, 1.0f, 0.5f,
        -1.0f, -0.5f, -1.5f,  0.0f, 0.0f, 0.5f,

        //depan
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f,  1.5f,  0.5f,  1.0f, 1.0f, 0.5f,
         1.0f,  1.5f,  0.5f,  1.0f, 1.0f, 0.5f,
        -1.0f,  1.5f,  0.5f,  0.0f, 1.0f, 0.5f,
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,

        // kiri
        -1.0f,  1.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        -1.0f,  1.5f, -1.5f,  1.0f, 1.0f, 0.5f,
        -1.0f, -0.5f, -1.5f,  0.0f, 1.0f, 0.5f,
        -1.0f, -0.5f, -1.5f,  0.0f, 1.0f, 0.5f,
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
        -1.0f,  1.5f,  0.5f,  1.0f, 0.0f, 0.5f,

        //kanan
         1.0f,  1.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f,  1.5f, -1.5f,  1.0f, 1.0f, 0.5f,
         1.0f, -0.5f, -1.5f,  0.0f, 1.0f, 0.5f,
         1.0f, -0.5f, -1.5f,  0.0f, 1.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
         1.0f,  1.5f,  0.5f,  1.0f, 0.0f, 0.5f,

         //bawah
        -1.0f, -0.5f, -1.5f,  0.0f, 1.0f, 0.5f,
         1.0f, -0.5f, -1.5f,  1.0f, 1.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f, -0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        -1.0f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,
        -1.0f, -0.5f, -1.5f,  0.0f, 1.0f, 0.5f,

        // atas
        -1.0f,  1.5f, -1.5f,  0.0f, 1.0f, 0.5f,
         1.0f,  1.5f, -1.5f,  1.0f, 1.0f, 0.5f,
         1.0f,  1.5f,  0.5f,  1.0f, 0.0f, 0.5f,
         1.0f,  1.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        -1.0f,  1.5f,  0.5f,  0.0f, 0.0f, 0.5f,
        -1.0f,  1.5f, -1.5f,  0.0f, 1.0f, 0.5f,

        // ===========KOTAK 3===========
        // KOTAK 3
        //belakang
        -0.15f, -0.25f, -0.25f,  0.0f, 0.0f, 0.5f,
         0.15f, -0.25f, -0.25f,  1.0f, 0.0f, 0.5f,
         0.15f,  0.25f, -0.25f,  1.0f, 1.0f, 0.5f,
         0.15f,  0.25f, -0.25f,  1.0f, 1.0f, 0.5f,
        -0.15f,  0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
        -0.15f, -0.25f, -0.25f,  0.0f, 0.0f, 0.5f,

        //depan
        -0.15f, -0.25f,  0.25f,  0.0f, 0.0f, 0.5f,
         0.15f, -0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
         0.15f,  0.25f,  0.25f,  1.0f, 1.0f, 0.5f,
         0.15f,  0.25f,  0.25f,  1.0f, 1.0f, 0.5f,
        -0.15f,  0.25f,  0.25f,  0.0f, 1.0f, 0.5f,
        -0.15f, -0.25f,  0.25f,  0.0f, 0.0f, 0.5f,

        // kiri
        -0.15f,  0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
        -0.15f,  0.25f, -0.25f,  1.0f, 1.0f, 0.5f,
        -0.15f, -0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
        -0.15f, -0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
        -0.15f, -0.25f,  0.25f,  0.0f, 0.0f, 0.5f,
        -0.15f,  0.25f,  0.25f,  1.0f, 0.0f, 0.5f,

        //kanan
         0.15f,  0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
         0.15f,  0.25f, -0.25f,  1.0f, 1.0f, 0.5f,
         0.15f, -0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
         0.15f, -0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
         0.15f, -0.25f,  0.25f,  0.0f, 0.0f, 0.5f,
         0.15f,  0.25f,  0.25f,  1.0f, 0.0f, 0.5f,

         //bawah
        -0.15f, -0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
         0.15f, -0.25f, -0.25f,  1.0f, 1.0f, 0.5f,
         0.15f, -0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
         0.15f, -0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
        -0.15f, -0.25f,  0.25f,  0.0f, 0.0f, 0.5f,
        -0.15f, -0.25f, -0.25f,  0.0f, 1.0f, 0.5f,

        // atas
        -0.15f,  0.25f, -0.25f,  0.0f, 1.0f, 0.5f,
         0.15f,  0.25f, -0.25f,  1.0f, 1.0f, 0.5f,
         0.15f,  0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
         0.15f,  0.25f,  0.25f,  1.0f, 0.0f, 0.5f,
        -0.15f,  0.25f,  0.25f,  0.0f, 0.0f, 0.5f,
        -0.15f,  0.25f, -0.25f,  0.0f, 1.0f, 0.5f,

        //======================================
        //hujan
        //belakang
        -0.01f, -0.1f, -0.01f,  0.0f, 0.0f, 0.0f,
         0.01f, -0.1f, -0.01f,  1.0f, 0.0f, 0.0f,
         0.01f,  0.1f, -0.01f,  1.0f, 1.0f, 0.0f,
         0.01f,  0.1f, -0.01f,  1.0f, 1.0f, 0.0f,
        -0.01f,  0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
        -0.01f, -0.1f, -0.01f,  0.0f, 0.0f, 0.0f,

        //depan
        -0.01f, -0.1f,  0.01f,  0.0f, 0.0f, 0.0f,
         0.01f, -0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
         0.01f,  0.1f,  0.01f,  1.0f, 1.0f, 0.0f,
         0.01f,  0.1f,  0.01f,  1.0f, 1.0f, 0.0f,
        -0.01f,  0.1f,  0.01f,  0.0f, 1.0f, 0.0f,
        -0.01f, -0.1f,  0.01f,  0.0f, 0.0f, 0.0f,

        // kiri
        -0.01f,  0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
        -0.01f,  0.1f, -0.01f,  1.0f, 1.0f, 0.0f,
        -0.01f, -0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
        -0.01f, -0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
        -0.01f, -0.1f,  0.01f,  0.0f, 0.0f, 0.0f,
        -0.01f,  0.1f,  0.01f,  1.0f, 0.0f, 0.0f,

        //kanan
         0.01f,  0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
         0.01f,  0.1f, -0.01f,  1.0f, 1.0f, 0.0f,
         0.01f, -0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
         0.01f, -0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
         0.01f, -0.1f,  0.01f,  0.0f, 0.0f, 0.0f,
         0.01f,  0.1f,  0.01f,  1.0f, 0.0f, 0.0f,

         //bawah
        -0.01f, -0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
         0.01f, -0.1f, -0.01f,  1.0f, 1.0f, 0.0f,
         0.01f, -0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
         0.01f, -0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
        -0.01f, -0.1f,  0.01f,  0.0f, 0.0f, 0.0f,
        -0.01f, -0.1f, -0.01f,  0.0f, 1.0f, 0.0f,

        // atas
        -0.01f,  0.1f, -0.01f,  0.0f, 1.0f, 0.0f,
         0.01f,  0.1f, -0.01f,  1.0f, 1.0f, 0.0f,
         0.01f,  0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
         0.01f,  0.1f,  0.01f,  1.0f, 0.0f, 0.0f,
        -0.01f,  0.1f,  0.01f,  0.0f, 0.0f, 0.0f,
        -0.01f,  0.1f, -0.01f,  0.0f, 1.0f, 0.0f,

        // Bidang Datar
        //-5.0f,  0.0f, -5.0f,  0.0f, 1.0f, 0.0f,
        // 5.0f,  0.0f, -5.0f,  1.0f, 1.0f, 0.0f,
        // 5.0f,  0.0f,  5.0f,  1.0f, 0.0f, 0.0f,
        // 5.0f,  0.0f,  5.0f,  1.0f, 0.0f, 0.0f,
        //-5.0f,  0.0f,  5.0f,  0.0f, 0.0f, 0.0f,
        //-5.0f,  0.0f, -5.0f,  0.0f, 1.0f, 0.0f,
    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  -0.5f,  -4.0f),
        glm::vec3( 0.0f,  -0.5f,  -5.0f),
        glm::vec3( 0.9f,  -1.0f, -4.0f),
        glm::vec3( -0.9f,  -1.0f, -4.0f),
        glm::vec3( 0.9f,  -1.0f, -6.0f),
        glm::vec3( -0.9f,  -1.0f, -6.0f),

        glm::vec3(0.0f, -2.2f, 0.0f),
        glm::vec3(0.0f, 4.0f, 0.0f),

        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        //std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.bmp").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        //std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    // HUJAN PRIVATE ROOM ======================================

    float incr = 0.5f;
    float x,y,z;
    int idx = 0;

    // HUJAN 1
    glm::vec3 hujanPositions[64];
    
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 2
    glm::vec3 hujanPositions1[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions1[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 3
    glm::vec3 hujanPositions2[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions2[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 4
    glm::vec3 hujanPositions3[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions3[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 5
    glm::vec3 hujanPositions4[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions4[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 6
    glm::vec3 hujanPositions5[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions5[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 7
    glm::vec3 hujanPositions6[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions6[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 8
    glm::vec3 hujanPositions7[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions7[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 9
    glm::vec3 hujanPositions8[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -1.0f;
            while (z < 1.0f) {
                hujanPositions8[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // ===============
    // HUJAN 11
    glm::vec3 hujanPositions10[64];
    
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions10[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 12
    glm::vec3 hujanPositions11[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions11[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 13
    glm::vec3 hujanPositions12[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions12[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 14
    glm::vec3 hujanPositions13[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions13[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 15
    glm::vec3 hujanPositions14[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions14[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 16
    glm::vec3 hujanPositions15[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions15[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 17
    glm::vec3 hujanPositions16[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions16[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 18
    glm::vec3 hujanPositions17[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions17[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 19
    glm::vec3 hujanPositions18[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -3.0f;
            while (z < -1.0f) {
                hujanPositions18[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // ===============
    // HUJAN 21
    glm::vec3 hujanPositions20[64];
    
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions20[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 22
    glm::vec3 hujanPositions21[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions21[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 23
    glm::vec3 hujanPositions22[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions22[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 24
    glm::vec3 hujanPositions23[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions23[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 25
    glm::vec3 hujanPositions24[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions24[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 26
    glm::vec3 hujanPositions25[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions25[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 27
    glm::vec3 hujanPositions26[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions26[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 28
    glm::vec3 hujanPositions27[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions27[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 29
    glm::vec3 hujanPositions28[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -5.0f;
            while (z < -3.0f) {
                hujanPositions28[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // ===============
    // HUJAN 21
    glm::vec3 hujanPositions30[64];
    
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions30[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 22
    glm::vec3 hujanPositions31[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions31[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 23
    glm::vec3 hujanPositions32[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = -1.0f;
        while (y < 1.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions32[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 24
    glm::vec3 hujanPositions33[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions33[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 25
    glm::vec3 hujanPositions34[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions34[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 26
    glm::vec3 hujanPositions35[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 1.0f;
        while (y < 3.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions35[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 27
    glm::vec3 hujanPositions36[64];
      
    idx = 0;
    x = -1.0f;
    while (x < 1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions36[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }
    printf("index = %d\n", idx);

    // HUJAN 28
    glm::vec3 hujanPositions37[64];
    
    idx = 0;
    x = -3.0f;
    while (x < -1.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions37[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    // HUJAN 39
    glm::vec3 hujanPositions38[64];
    
    idx = 0;
    x = 1.0f;
    while (x < 3.0f) {
        y = 3.0f;
        while (y < 5.0f) {
            z = -7.0f;
            while (z < -5.0f) {
                hujanPositions38[idx] = glm::vec3(x,y,z);
                z += incr;
                idx += 1;
            }
            y += incr;
        }
        x += incr;
    }

    float yhujan = 4.0f;
    float yhujan1 = 6.0f;
    float yhujan2 = 8.0f;
    float decr = 0.05f;
  
    init();
    drawScene();
  
    // ======================================================
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glm::vec3 poshujan(0.0f, yhujan, -2.5f);
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        ourShader.setMat4("projection", projection);

        // camera/view transformation
        ourShader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 2310; i++)
        {
            int mboh = 0;
            int startidx;
            if (i == 0) {
                startidx = 0;
            } else if (i == 1){
                startidx = 36;
            } else if ((i > 1) && (i < 6)) {
                startidx = 72;
            } else {
                mboh = 1;
                startidx = 108;
            }
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            if (mboh == 0) {
                model = glm::translate(model, cubePositions[i]);
            } else {
                if (i > 70) {
                    model = glm::translate(model, hujanPositions1[i-70]);
                } else if (i > 134) {
                    model = glm::translate(model, hujanPositions2[i-134]);
                } else if (i > 198 ) {
                    model = glm::translate(model, hujanPositions3[i-198]);
                } else if (i > 262 ) {
                    model = glm::translate(model, hujanPositions4[i-262]);
                } else if (i > 326 ) {
                    model = glm::translate(model, hujanPositions5[i-326]);
                } else if (i > 390 ) {
                    model = glm::translate(model, hujanPositions6[i-390]);
                } else if (i > 454 ) {
                    model = glm::translate(model, hujanPositions7[i-454]);
                } else if (i > 518 ) {
                    model = glm::translate(model, hujanPositions8[i-518]);
                } else if (i > 582 ) {
                    model = glm::translate(model, hujanPositions10[i-582]);
                } else if (i > 646 ) {
                    model = glm::translate(model, hujanPositions11[i-646]);
                } else if (i > 710 ) {
                    model = glm::translate(model, hujanPositions12[i-710]);
                } else if (i > 774 ) {
                    model = glm::translate(model, hujanPositions13[i-774]);
                } else if (i > 838 ) {
                    model = glm::translate(model, hujanPositions14[i-838]);
                } else if (i > 902 ) {
                    model = glm::translate(model, hujanPositions15[i-902]);
                } else if (i > 966 ) {
                    model = glm::translate(model, hujanPositions16[i-966]);
                } else if (i > 1030 ) {
                    model = glm::translate(model, hujanPositions17[i-1030]);
                } else if (i > 1094 ) {
                    model = glm::translate(model, hujanPositions18[i-1094]);
                } else {
                    model = glm::translate(model, hujanPositions[i-6]);
                }
            }
            
            lightingShader.setMat4("model", model);
            
            float angle = 20.0f * i;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, startidx, startidx+36);
        }

        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, cubePositions[6]);

        //lightingShader.setMat4("model", model);

        //ourShader.setMat4("model", model);

        //glDrawArrays(GL_TRIANGLES, 144, 150);
        // =================================
        if (yhujan < -2.0f) {
            yhujan = 4.0f;
        }
        if (yhujan1 < -2.0f) {
            yhujan1 = 4.0f;
        }
        if (yhujan2 < -2.0f) {
            yhujan2 = 4.0f;
        }
        yhujan -= decr;
        yhujan1 -= decr;
        yhujan2 -= decr;

        // HUJAN 1
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 2
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions1[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 3
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions2[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 4
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions3[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 5
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions4[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 6
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions5[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 7
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions6[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 8
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions7[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 9
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -1.0f;
                while (z < 1.0f) {
                    hujanPositions8[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 11
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions10[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 12
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions11[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 13
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions12[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 14
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions13[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 15
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions14[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 16
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions15[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 17
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions16[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 18
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions17[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 19
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -3.0f;
                while (z < -1.0f) {
                    hujanPositions18[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 21
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions20[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 22
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions21[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 23
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions22[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 24
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions23[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 25
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions24[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 26
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions25[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 27
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions26[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 28
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions27[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 29
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -5.0f;
                while (z < -3.0f) {
                    hujanPositions28[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 21
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions30[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 22
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions31[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 23
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions32[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 24
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions33[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 25
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions34[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 26
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan1;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions35[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 27
        idx = 0;
        x = -1.0f;
        while (x < 1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions36[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 28
        idx = 0;
        x = -3.0f;
        while (x < -1.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions37[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }

        // HUJAN 29
        idx = 0;
        x = 1.0f;
        while (x < 3.0f) {
            y = yhujan2;
            int count = 0;
            while (count < 4) {
                z = -7.0f;
                while (z < -5.0f) {
                    hujanPositions38[idx] = glm::vec3(x,y,z);
                    z += incr;
                    idx += 1;
                }
                count += 1;
                y += incr;
            }
            x += incr;
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        //printf("in\n");
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}