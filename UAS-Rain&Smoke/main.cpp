// Std. Includes
#include <string>
#include <algorithm>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Texture.hpp"
#include "shader.hpp"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void processInput(GLFWwindow *window);

// Camera
Camera  camera(glm::vec3( 0.0f, 3.0f, 24.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat middlePoint[] = {0, 0, 0};
GLfloat rad = 0.00f;
const GLfloat CAR_RADIUS_COLLISION = 3.0f;
const GLfloat OFFSET_SMOKE = 3.5f;
const GLfloat CAR_SPEED_FAST = 0.3f;
const GLfloat CAR_SPEED_MEDIUM = 0.2f;
const GLfloat CAR_SPEED_SLOW = 0.1f;
const int carLastIndex = 36 * 12 * 8; // 3456
const int objectLastIndex = 36 * 15 * 8;
GLfloat vertices[21000] =
    {
        // KOTAK 1
        //belakang
        -1.0f, -0.5f, 6.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         1.0f, -0.5f, 6.5f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         1.0f,  0.5f, 6.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         1.0f,  0.5f, 6.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f,  0.5f, 6.5f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, -0.5f, 6.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        //depan
        -1.0f, -0.5f,  7.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         1.0f, -0.5f,  7.5f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         1.0f,  0.5f,  7.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         1.0f,  0.5f,  7.5f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f,  0.5f,  7.5f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, -0.5f,  7.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        // kiri
        -1.0f,  0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  0.5f, 6.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f,  7.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,

        //kanan
         1.0f,  0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  0.5f, 6.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  7.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,

         //bawah
        -1.0f, -0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 6.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f,  7.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,

        // atas
        -1.0f,  0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  0.5f, 6.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  0.5f,  7.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  0.5f,  7.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  0.5f, 6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,

        // ===========KOTAK 2===========
        //belakang
        -1.0f, -0.5f, 4.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 4.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f, 4.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f, 4.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  1.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 4.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,

        //depan
        -1.0f, -0.5f,  6.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f,  6.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f,  6.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  1.5f,  6.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f,  6.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,

        // kiri
        -1.0f,  1.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  1.5f, 4.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f,  6.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  1.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,

        //kanan
         1.0f,  1.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f, 4.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  6.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,

         //bawah
        -1.0f, -0.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f, 4.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f, -0.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f,  6.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f, -0.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,

        // atas
        -1.0f,  1.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f, 4.5f,  1.0f, 1.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
         1.0f,  1.5f,  6.5f,  1.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  1.5f,  6.5f,  0.0f, 0.0f, 0.5f, 1.0f, 1.0f,
        -1.0f,  1.5f, 4.5f,  0.0f, 1.0f, 0.5f, 1.0f, 1.0f,
    };

struct SmokeParticle{
    glm::vec3 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, weight;
    float life; // Remaining life of the SmokeParticle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const SmokeParticle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

const int MAX_SMOKE_PARTICLES = 100000;
SmokeParticle ParticlesContainer[MAX_SMOKE_PARTICLES];
int LastUsedParticle = 0;

float accum = -32.0f;
float slowdown = 2.0;
float velocity = 5.0;
float zoom = -1.0f;
bool starter = true;
int loop;
void initParticles(int i, bool starter);
void init(bool starter);
void drawRain(bool starter);

typedef struct {
    // Life
    bool alive; // is the particle alive?
    float life; // particle lifespan
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
const int MAX_RAIN_PARTICLES = 600;
// Paticle System
particles par_sys[MAX_RAIN_PARTICLES]; 
int idxRain = objectLastIndex;
GLfloat inc_x_rain[MAX_RAIN_PARTICLES] = { 0.0f };
GLfloat inc_y_rain[MAX_RAIN_PARTICLES] = { 0.0f };
GLfloat inc_z_rain[MAX_RAIN_PARTICLES] = { 0.0f };
GLfloat init_x_rain[MAX_RAIN_PARTICLES] = { 0.0f };
GLfloat init_z_rain[MAX_RAIN_PARTICLES] = { 0.0f };




// Finds a SmokeParticle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){

    for(int i=LastUsedParticle; i<MAX_SMOKE_PARTICLES; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }

    for(int i=0; i<LastUsedParticle; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void SortParticles(){
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MAX_SMOKE_PARTICLES]);
}

// The MAIN function, from here we start our application and run our Game loop
int main( )
{
    // Init GLFW
    glfwInit( );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "Hujan Agak Deras", nullptr, nullptr ); // Windowed
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }

    init(starter);
    drawRain(starter);
    starter = false;
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    glfwSetScrollCallback( window, ScrollCallback );
    
    // Options, removes the mouse cursor for a more immersive experience
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // Setup some OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    // enable alpha support
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // Setup and compile our shaders
    Shader lightShader( "res/shaders/light.vs", "res/shaders/light.frag" );
    
    glm::vec3 cubePositions[] =
    {
        glm::vec3( 0.0f, 0.0f, 0.0f ),
        glm::vec3( 2.0f, 5.0f, -15.0f ),
        glm::vec3( -1.5f, -2.2f, -2.5f ),
        glm::vec3( -3.8f, -2.0f, -12.3f ),
        glm::vec3( 2.4f, -0.4f, -3.5f ),
        glm::vec3( -1.7f, 3.0f, -7.5f ),
        glm::vec3( 1.3f, -2.0f, -2.5f ),
        glm::vec3( 1.5f, 2.0f, -2.5f ),
        glm::vec3( 1.5f, 0.2f, -1.5f ),
        glm::vec3( -1.3f, 1.0f, -1.5f )
    };
    
    GLuint diffuseMap[10], specularMap[10], emissionMap[10];


    GLuint VBO, VAO;
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray( VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    
    // Position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray( 1 );
    // TexCoord attribute
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )( 6 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );
    
    glBindVertexArray( 0 ); // Unbind VAO
    
    // Load and create a texture
    int width, height;
    unsigned char *image;
    
    lightShader.Use();
    glUniform1i(glGetUniformLocation(lightShader.Program, "material.diffuse"),  0);
    glUniform1i(glGetUniformLocation(lightShader.Program, "material.specular"), 1);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "res/shaders/SmokeParticle.vs", "res/shaders/SmokeParticle.frag" );

    // Vertex shader
    GLuint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
    GLuint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
    GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    // fragment shader
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    static GLfloat* g_particule_position_size_data = new GLfloat[MAX_SMOKE_PARTICLES * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[MAX_SMOKE_PARTICLES * 4];

    for(int i=0; i<MAX_SMOKE_PARTICLES; i++){
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }

    GLuint Texture = loadDDS("res/images/smoke.jpg");

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat smoke_vertices[] = { 
         -0.2f, -0.2f, 0.0f,
          0.2f, -0.2f, 0.0f,
         -0.2f,  0.2f, 0.0f,
          0.2f,  0.2f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(smoke_vertices), smoke_vertices, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_SMOKE_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_SMOKE_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    // Game loop
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while( !glfwWindowShouldClose( window ) )
    {
     // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            //printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        // Check and call events
        glfwPollEvents( );
        
        // Clear the colorbuffer
        glClearColor( 0.4f, 0.6f, 0.6f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Draw
        lightShader.Use( );
        GLint lightDirLoc = glGetUniformLocation(lightShader.Program, "light.direction");
        GLint viewPosLoc  = glGetUniformLocation(lightShader.Program, "viewPos");
        glUniform3f(lightDirLoc, -0.2f, -1.0f, -0.3f);
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        // Set lights properties
        glUniform3f(glGetUniformLocation(lightShader.Program, "light.ambient"),  0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(lightShader.Program, "light.diffuse"),  0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(lightShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
        // Set material properties
        glUniform1f(glGetUniformLocation(lightShader.Program, "material.shininess"), 100.0f);



        
        
        glm::mat4 projection;
        projection = glm::perspective(camera.GetZoom( ), (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
        
        // Create camera transformation
        glm::mat4 view;
        view = camera.GetViewMatrix( );

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation( lightShader.Program, "model" );
        GLint viewLoc = glGetUniformLocation( lightShader.Program, "view" );
        GLint projLoc = glGetUniformLocation( lightShader.Program, "projection" );
        
        // We will need the camera's position in order to sort the particles
        // w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp, 
        // but this works too.
        glm::vec3 CameraPosition(glm::inverse(view)[3]);

        glm::mat4 ViewProjectionMatrix = projection * view;

        // Pass the matrices to the shader
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( VAO );
        
        //for( GLuint i = 0; i < 10; i++ )
        //{
            // Calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate( model, cubePositions[0] );
            GLfloat angle = 20.0f * 0;
            model = glm::rotate(model, angle, glm::vec3( 1.0f, 0.3f, 0.5f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );

            glDrawArrays( GL_TRIANGLES, 0, 36 * 2 );
        //}
        
        float x1, y1, z1;
        int start = 36*15;
        drawRain(false);
        for (loop = 0; loop < MAX_RAIN_PARTICLES; loop++) {
            if (par_sys[loop].alive == true) {
                x1 = par_sys[loop].xpos;
                y1 = par_sys[loop].ypos;
                z1 = par_sys[loop].zpos + zoom;

                // Bind Textures using texture units
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap[5]);
                glBindVertexArray(VAO);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, specularMap[5]);
                glBindTexture( GL_TEXTURE_2D, VAO );
                glUniform3f(glGetUniformLocation(lightShader.Program, "inc_rain"), inc_x_rain[loop], inc_y_rain[loop], inc_z_rain[loop]);
                glUniform1i(glGetUniformLocation(lightShader.Program, "is_rain"), 1);
                glUniform1i( glGetUniformLocation( lightShader.Program, "metal" ), 5 );
                glDrawArrays( GL_LINES, start, start + 2);
                glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
                start += 2;
                
                // Update values
                //Move 
                // Adjust slowdown for speed!
                par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
                inc_y_rain[loop] += par_sys[loop].vel / (slowdown*1000);
                par_sys[loop].vel += par_sys[loop].gravity;
                // Decay
                par_sys[loop].life -= par_sys[loop].fade;
                
                if (par_sys[loop].ypos <= accum) {
                    par_sys[loop].life = -1.0;
                }
                //Revive 
                if (par_sys[loop].life < 0.0) {
                    initParticles(loop, starter);
                }
            }
        }
        
        // Generate 10 new particule each millisecond,
        // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
        // newparticles will be huge and the next frame even longer.
        int newparticles = (int)(deltaTime*7000.0);
        if (newparticles > (int)(0.016f*7000.0))
            newparticles = (int)(0.016f*7000.0);
        
        for(int i=0; i<newparticles; i++){
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = 0.8f; // This SmokeParticle will live n seconds.
            ParticlesContainer[particleIndex].pos = glm::vec3(middlePoint[0]+OFFSET_SMOKE*sin(rad),middlePoint[1],middlePoint[2]+OFFSET_SMOKE*cos(rad)); // spawn location

            float spread = 1.5f;
            glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
            glm::vec3 randomdir = glm::vec3(
                (rand()%2000 - 1000.0f)/1000.0f,
                (rand()%2000 - 1000.0f)/1000.0f,
                (rand()%2000 - 1000.0f)/1000.0f
            );
            
            ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;
            ParticlesContainer[particleIndex].r = rand() % 256;
            ParticlesContainer[particleIndex].g = rand() % 256;
            ParticlesContainer[particleIndex].b = rand() % 256;
            ParticlesContainer[particleIndex].a = (rand() % 256) / 3;
            ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
        }

        // Simulate all particles
        int ParticlesCount = 0;
        for(int i=0; i<MAX_SMOKE_PARTICLES; i++){

            SmokeParticle& p = ParticlesContainer[i]; // shortcut

            if(p.life > 0.0f){

                // Decrease life
                p.life -= deltaTime;
                if (p.life > 0.0f){

                    // Simulate simple physics : gravity only, no collisions
                    p.speed += glm::vec3(0.0f,0.05f, 0.0f) * (float)deltaTime * 0.05f;
                    p.pos += p.speed * (float)deltaTime;
                    p.cameradistance = glm::length2( p.pos - CameraPosition );
                    //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                    // Fill the GPU buffer
                    g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                    g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                    g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;
                                                   
                    g_particule_position_size_data[4*ParticlesCount+3] = p.size;
                                                   
                    g_particule_color_data[4*ParticlesCount+0] = p.r;
                    g_particule_color_data[4*ParticlesCount+1] = p.g;
                    g_particule_color_data[4*ParticlesCount+2] = p.b;
                    g_particule_color_data[4*ParticlesCount+3] = p.a;

                }else{
                    // Particles that just died will be put at the end of the buffer in SortParticles();
                    p.cameradistance = -1.0f;
                }

                ParticlesCount++;
            }
        }

        SortParticles();

        // Update the buffers that OpenGL uses for rendering.
        // There are much more sophisticated means to stream data from the CPU to the GPU, 
        // but this is outside the scope of this tutorial.
        // http://www.opengl.org/wiki/Buffer_Object_Streaming
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, MAX_SMOKE_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MAX_SMOKE_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use our shader
        glUseProgram(programID);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, view[0][0], view[1][0], view[2][0]);
        glUniform3f(CameraUp_worldspace_ID   , view[0][1], view[1][1], view[2][1]);

        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(
            3,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        
        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            4,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(5);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(
            5,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE,                 // type
            GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(3, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(4, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(5, 1); // color : one per quad                                  -> 1

        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);

        glBindVertexArray( 0 );
        
        // Swap the buffers
        glBindBuffer( GL_ARRAY_BUFFER, VBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
        glfwSwapBuffers( window );
        glfwPollEvents();
    }
    
    delete[] g_particule_position_size_data;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    //glDeleteVertexArrays(1, &VertexArrayID);


    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glfwTerminate( );
    
    return EXIT_SUCCESS;
}

bool CheckPositionLowerSpeed(GLfloat move) {
    return (middlePoint[0] + move < 22.0f) && (middlePoint[0] - move > 8.0f) && (middlePoint[2] + move < 10.0f) && (middlePoint[2] - move > -10.0f);
}

bool CheckPositionCanMoveForward(GLfloat move) {
    return (middlePoint[0] + move * sin(rad) < 38.0f) && (middlePoint[2] + move * cos(rad) < 20.0f) && (middlePoint[0] + move * sin(rad) > -8.0f) && (middlePoint[2] + move * cos(rad) > -20.0f);
}
bool CheckPositionCanMoveBackward(GLfloat move) {
    return (middlePoint[0] - move * sin(rad) < 38.0f) && (middlePoint[2] - move * cos(rad) < 20.0f) && (middlePoint[0] - move * sin(rad) > -8.0f) && (middlePoint[2] - move * cos(rad) > -20.0f);
}

// Moves/alters the camera positions based on user input
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

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if( action == GLFW_RELEASE )
        {
            bool up = keys[GLFW_KEY_UP];
            bool down = keys[GLFW_KEY_DOWN];
            
            keys[key] = false;

            if (up && !keys[GLFW_KEY_UP]) {
                
            }
            if (down && !keys[GLFW_KEY_DOWN]) {
                
            }
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}	


void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset )
{
    camera.ProcessMouseScroll( yOffset );
}


void initParticles(int idx, bool starter) {
    par_sys[idx].alive = true;
    par_sys[idx].life = 1.0;
    par_sys[idx].fade = float(rand()%100)/10000.0f+0.0003f;
                   
    par_sys[idx].xpos = (float) (rand() % 21*3) - 13;
    par_sys[idx].ypos = 18.0f;
    par_sys[idx].zpos = (float) (rand() % 21*3) - 10;

    if (!starter) {
        inc_x_rain[idx] =  par_sys[idx].xpos - init_x_rain[idx];
        inc_z_rain[idx] = par_sys[idx].zpos - init_z_rain[idx];
        inc_y_rain[idx] = 0.0f;
    } else {
        init_x_rain[loop] = par_sys[idx].xpos;
        init_z_rain[loop] = par_sys[idx].zpos;
    }
    
    par_sys[idx].red = 0.5;
    par_sys[idx].green = 0.5;
    par_sys[idx].blue = 1.0;
                    
    par_sys[idx].vel = velocity;
    par_sys[idx].gravity = -0.8;//-0.8;
}

void init(bool starter) {
    for (loop = 0; loop < MAX_RAIN_PARTICLES; loop++) {
        initParticles(loop, starter);
    }
}

void drawRain(bool starter) {
    float x, y, z;

    for (loop = 0; loop < MAX_RAIN_PARTICLES; loop++) {
        if (par_sys[loop].alive == true) {          
             x = par_sys[loop].xpos;
             y = par_sys[loop].ypos;
             z = par_sys[loop].zpos + zoom;

             if (starter) {
                init_x_rain[loop] = x;
                init_z_rain[loop] = z;
             }

            vertices[idxRain++] = x;
            vertices[idxRain++] = y;
            vertices[idxRain++] = z;

            vertices[idxRain++] = 1.0f;
            vertices[idxRain++] = 1.0f;
            vertices[idxRain++] = 1.0f;

            vertices[idxRain++] = 1.0f;
            vertices[idxRain++] = 0.0f;

            vertices[idxRain++] = x;
            vertices[idxRain++] = y + 0.5f;
            vertices[idxRain++] = z;

            vertices[idxRain++] = 1.0f;
            vertices[idxRain++] = 1.0f;
            vertices[idxRain++] = 1.0f;

            vertices[idxRain++] = 0.0f;
            vertices[idxRain++] = 0.0f;
            
            // Update values
            //Move
            // Adjust slowdown for speed!
            par_sys[loop].ypos += par_sys[loop].vel / (slowdown*100);
            inc_y_rain[loop] += par_sys[loop].vel / (slowdown*100);
            par_sys[loop].vel += par_sys[loop].gravity;
            // Decay
            par_sys[loop].life -= par_sys[loop].fade;
            
            if (par_sys[loop].ypos <= accum) {
                par_sys[loop].life = -1.0;
            }
            //Revive 
            if (par_sys[loop].life < 0.0) {
                initParticles(loop, starter);
            }
        }
    }
    idxRain = objectLastIndex;
}