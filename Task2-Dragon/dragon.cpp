#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

GLFWwindow * window;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
} 

void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
    //Make a window
    window = glfwCreateWindow(800, 600, "Dragon wa Maid-sama - Season 1", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    //Init glew
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return;
    }
}

//Vertex shader source
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aCol;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    " color = aCol;\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

//fragment shader source
const char *fragmentShaderSource = "#version 330 core\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "in vec4 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = color;\n"
    "}\0";

int main() {
    initWindow();

    //Rendering size
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Declare vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //compile vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //Declare fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    //compile fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //Declare shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    //Link vertex and fragment shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Delete vertex and fragment shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Vertices
    float vertices[] = {
        // Isi segitiga disini
        // format : x,y,z,r,g,b
        // triangle 1
        0.640f, 0.486f, 0.0f, 0.0f, 1.0f, 0.0f, // p1
        0.580f, 0.405f, 0.0f, 0.0f, 1.0f, 0.0f, // p2
        0.720f, 0.324f, 0.0f, 0.0f, 1.0f, 0.0f, // p3
        // triangle 2
        0.580f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f, // p2
        0.720f, 0.324f, 0.0f, 1.0f, 0.0f, 0.0f, // p3
        0.680f, 0.297f, 0.0f, 1.0f, 0.0f, 0.0f, // p4
        // triangle 3
        0.720f, 0.324f, 0.0f, 0.0f, 0.0f, 1.0f, // p3
        0.680f, 0.297f, 0.0f, 0.0f, 0.0f, 1.0f, // p4
        0.700f, 0.243f, 0.0f, 0.0f, 0.0f, 1.0f, // p5
        // triangle 4
        0.580f, 0.405f, 0.0f, 0.0f, 1.0f, 0.0f, // p2
        0.680f, 0.297f, 0.0f, 0.0f, 1.0f, 0.0f, // p4
        0.540f, 0.405f, 0.0f, 0.0f, 1.0f, 0.0f, // p7
        // triangle 5
        0.680f, 0.297f, 0.0f, 1.0f, 0.0f, 0.0f, // p4
        0.520f, 0.324f, 0.0f, 1.0f, 0.0f, 0.0f, // p6
        0.540f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f, // p7
        // traingle 6
        0.520f, 0.324f, 0.0f, 0.0f, 0.0f, 1.0f, // p6
        0.540f, 0.405f, 0.0f, 0.0f, 0.0f, 1.0f, // p7
        0.420f, 0.432f, 0.0f, 0.0f, 0.0f, 1.0f, // p9
        // triangle 7
        0.540f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f, // p7
        0.480f, 0.513f, 0.0f, 1.0f, 0.0f, 0.0f, // p8
        0.420f, 0.432f, 0.0f, 1.0f, 0.0f, 0.0f, // p9
        // triangle 8
        0.480f, 0.513f, 0.0f, 0.0f, 1.0f, 0.0f, // p8
        0.420f, 0.432f, 0.0f, 0.0f, 1.0f, 0.0f, // p9
        0.300f, 0.702f, 0.0f, 0.0f, 1.0f, 0.0f, // p10
        // traingle 9
        0.520f, 0.324f, 0.0f, 0.0f, 0.0f, 1.0f, // p6
        0.420f, 0.432f, 0.0f, 0.0f, 0.0f, 1.0f, // p9
        0.380f, 0.378f, 0.0f, 0.0f, 0.0f, 1.0f, // p12
        // triangle 10
        0.520f, 0.324f, 0.0f, 1.0f, 0.0f, 0.0f, // p6
        0.580f, 0.216f, 0.0f, 1.0f, 0.0f, 0.0f, // p11
        0.380f, 0.378f, 0.0f, 1.0f, 0.0f, 0.0f, // p12
        // t11
        0.420f, 0.432f, 0.0f, 0.0f, 0.0f, 1.0f, // p9
        0.360f, 0.567f, 0.0f, 0.0f, 0.0f, 1.0f, // p13
        0.240f, 0.540f, 0.0f, 0.0f, 0.0f, 1.0f, // p14
        // t12
        0.420f, 0.432f, 0.0f, 1.0f, 0.0f, 0.0f, // p9
        0.380f, 0.378f, 0.0f, 1.0f, 0.0f, 0.0f, // p12
        0.240f, 0.540f, 0.0f, 1.0f, 0.0f, 0.0f, // p14
        // t13
        0.380f, 0.378f, 0.0f, 1.0f, 0.0f, 0.0f, // p12
        0.240f, 0.540f, 0.0f, 1.0f, 0.0f, 0.0f, // p14
        0.120f, 0.459f, 0.0f, 1.0f, 0.0f, 0.0f, // p15
        // t14
        0.320f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f, // p16
        0.160f, 0.432f, 0.0f, 1.0f, 0.0f, 0.0f, // p17
        0.100f, 0.270f, 0.0f, 1.0f, 0.0f, 0.0f, // p18
        // t15
        0.320f, 0.405f, 0.0f, 1.0f, 0.0f, 0.0f, // p16
        0.100f, 0.270f, 0.0f, 1.0f, 0.0f, 0.0f, // p18
        0.280f, 0.243f, 0.0f, 1.0f, 0.0f, 0.0f,// p19
        // t16
        0.100f, 0.270f, 0.0f, 1.0f, 0.0f, 0.0f, // p18
        0.280f, 0.243f, 0.0f, 1.0f, 0.0f, 0.0f,// p19
        0.096f, 0.135f, 0.0f, 1.0f, 0.0f, 0.0f,// p20
        // t17
        0.280f, 0.243f, 0.0f, 1.0f, 0.0f, 0.0f,// p19
        0.096f, 0.135f, 0.0f, 1.0f, 0.0f, 0.0f,// p20
        0.200f, -0.081f, 0.0f, 1.0f, 0.0f, 0.0f, // p22
        // t18
        0.280f, 0.243f, 0.0f, 1.0f, 0.0f, 0.0f,// p19
        0.200f, -0.081f, 0.0f, 1.0f, 0.0f, 0.0f,// p22
        0.360f, 0.108f, 0.0f, 1.0f, 0.0f, 0.0f,// p24
        // t19
        0.200f, -0.081f, 0.0f, 1.0f, 0.0f, 0.0f,// p22
        0.360f, 0.108f, 0.0f, 1.0f, 0.0f, 0.0f,// p24
        0.480f, 0.054f, 0.0f, 1.0f, 0.0f, 0.0f,// p25
        // t20
        0.200f, -0.081f, 0.0f, 0.0f, 0.0f, 1.0f,// p22
        0.480f, 0.054f, 0.0f, 0.0f, 0.0f, 1.0f,// p25
        0.560f, -0.351f, 0.0f, 0.0f, 0.0f, 1.0f,// p27
        // t21
        0.480f, 0.054f, 0.0f, 0.0f, 1.0f, 0.0f,// p25
        0.560f, -0.351f, 0.0f, 0.0f, 1.0f, 0.0f,// p27
        0.560f, -0.081f, 0.0f, 0.0f, 1.0f, 0.0f,// p28
        // ==
        // t22
        0.280f, -0.108f, 0.0f, 0.0f, 1.0f, 0.0f,// p26
        0.560f, -0.351f, 0.0f, 0.0f, 1.0f, 0.0f,// p27
        0.300f, -0.216f, 0.0f, 0.0f, 1.0f, 0.0f,// p30
        // t23
        0.560f, -0.351f, 0.0f, 0.0f, 1.0f, 0.0f,// p27
        0.520f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p47
        0.300f, -0.216f, 0.0f, 0.0f, 1.0f, 0.0f, // p30
        // t24
        0.520f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p47
        0.336f, -0.270f, 0.0f, 0.0f, 1.0f, 0.0f, // p29
        0.320f, -0.486f, 0.0f, 0.0f, 1.0f, 0.0f, // p31
        // t25
        0.520f, -0.432f, 0.0f, 0.0f, 0.0f, 1.0f,// p47
        0.320f, -0.486f, 0.0f, 0.0f, 0.0f, 1.0f, // p31
        0.584f, -0.594f, 0.0f, 0.0f, 0.0f, 1.0f,// p32
        // t26
        0.320f, -0.486f, 0.0f, 0.0f, 1.0f, 0.0f, // p31
        0.440f, -0.540f, 0.0f, 0.0f, 1.0f, 0.0f,// p48
        0.300f, -0.594f, 0.0f, 0.0f, 1.0f, 0.0f,// p33
        // t27
        0.336f, -0.270f, 0.0f, 0.0f, 1.0f, 0.0f, // p29
        0.300f, -0.216f, 0.0f, 0.0f, 1.0f, 0.0f, // p30
        0.300f, -0.594f, 0.0f, 0.0f, 1.0f, 0.0f,// p33
        // t28
        0.300f, -0.216f, 0.0f, 0.0f, 1.0f, 0.0f, // p30
        0.300f, -0.594f, 0.0f, 0.0f, 1.0f, 0.0f,// p33
        0.160f, -0.567f, 0.0f, 0.0f, 1.0f, 0.0f,// p34
        // t29
        0.300f, -0.216f, 0.0f, 0.0f, 1.0f, 0.0f, // p30
        0.160f, -0.567f, 0.0f, 0.0f, 1.0f, 0.0f,// p34
        0.220f, -0.243f, 0.0f, 0.0f, 1.0f, 0.0f,// p35
        // t30
        0.160f, -0.567f, 0.0f, 0.0f, 1.0f, 0.0f,// p34
        0.220f, -0.243f, 0.0f, 0.0f, 1.0f, 0.0f,// p35
        0.032f, -0.081f, 0.0f, 0.0f, 1.0f, 0.0f,// p36
        // t31
        0.160f, -0.567f, 0.0f, 0.0f, 1.0f, 0.0f,// p34
        0.032f, -0.081f, 0.0f, 0.0f, 1.0f, 0.0f,// p36
        0.016f, -0.459f, 0.0f, 0.0f, 1.0f, 0.0f,// p37
        // t32
        0.032f, -0.081f, 0.0f, 0.0f, 1.0f, 0.0f,// p36
        0.016f, -0.459f, 0.0f, 0.0f, 1.0f, 0.0f,// p37
        -0.160f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p39
        // t33
        0.032f, -0.081f, 0.0f, 0.0f, 1.0f, 0.0f,// p36
        -0.160f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p39
        -0.320f, -0.270f, 0.0f, 0.0f, 1.0f, 0.0f, // p42
        // t34
        0.032f, -0.081f, 0.0f, 0.0f, 1.0f, 0.0f,// p36
        -0.100f, -0.054f, 0.0f, 0.0f, 1.0f, 0.0f,// p40
        -0.320f, -0.270f, 0.0f, 0.0f, 1.0f, 0.0f, // p42
        // t35
        -0.100f, -0.054f, 0.0f, 0.0f, 1.0f, 0.0f,// p40
        -0.240f, -0.135f, 0.0f, 0.0f, 1.0f, 0.0f,// p41
        -0.320f, -0.270f, 0.0f, 0.0f, 1.0f, 0.0f, // p42
        // t36
        -0.160f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p39
        -0.320f, -0.270f, 0.0f, 0.0f, 1.0f, 0.0f, // p42
        -0.360f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p43
        // t37
        0.016f, -0.459f, 0.0f, 0.0f, 0.0f, 1.0f,// p37
        -0.104f, -0.621f, 0.0f, 0.0f, 0.0f, 1.0f,// p38
        -0.160f, -0.432f, 0.0f, 0.0f, 0.0f, 1.0f,// p39
        // t38
        -0.160f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p39
        -0.360f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p43
        -0.400f, -0.648f, 0.0f, 0.0f, 1.0f, 0.0f,// p44
        // t39
        -0.360f, -0.432f, 0.0f, 0.0f, 1.0f, 0.0f,// p43
        -0.400f, -0.648f, 0.0f, 0.0f, 1.0f, 0.0f,// p44
        -0.460f, -0.486f, 0.0f, 0.0f, 1.0f, 0.0f,// p46
        // t40
        -0.400f, -0.648f, 0.0f, 0.0f, 1.0f, 0.0f,// p44
        -0.540f, -0.648f, 0.0f, 0.0f, 1.0f, 0.0f,// p45
        -0.460f, -0.486f, 0.0f, 0.0f, 1.0f, 0.0f,// p46
        // t41
        -0.540f, -0.648f, 0.0f, 0.0f, 1.0f, 0.0f,// p45
        -0.460f, -0.486f, 0.0f, 0.0f, 1.0f, 0.0f,// p46
        -0.660f, -0.540f, 0.0f, 0.0f, 1.0f, 0.0f,// p49
        // t42
        -0.460f, -0.486f, 0.0f, 0.0f, 1.0f, 0.0f,// p46
        -0.660f, -0.540f, 0.0f, 0.0f, 1.0f, 0.0f,// p49
        -0.580f, -0.459f, 0.0f, 0.0f, 1.0f, 0.0f,// p50
        // t43
        -0.660f, -0.540f, 0.0f, 0.0f, 1.0f, 0.0f,// p49
        -0.580f, -0.459f, 0.0f, 0.0f, 1.0f, 0.0f,// p50
        -0.680f, -0.216f, 0.0f, 0.0f, 1.0f, 0.0f,// p51
        // t44
        -0.560f, 0.594f, 0.0f, 1.0f, 1.0f, 0.0f,// p52
        -0.400f, 0.432f, 0.0f, 1.0f, 1.0f, 0.0f,// p53
        0.060f, 0.270f, 0.0f, 1.0f, 1.0f, 0.0f,// p60
        // t45
        -0.400f, 0.432f, 0.0f, 1.0f, 0.9f, 0.0f,// p53
        -0.440f, 0.351f, 0.0f, 1.0f, 0.9f, 0.0f,// p54
        0.060f, 0.270f, 0.0f, 1.0f, 0.9f, 0.0f,// p60
        // t46
        -0.280f, 0.324f, 0.0f, 1.0f, 1.0f, 0.0f,// p55
        -0.300f, 0.216f, 0.0f, 1.0f, 1.0f, 0.0f,// p56
        0.060f, 0.270f, 0.0f, 1.0f, 1.0f, 0.0f,// p60
        // t47
        -0.300f, 0.216f, 0.0f, 1.0f, 0.9f, 0.0f,// p56
        -0.180f, 0.162f, 0.0f, 1.0f, 0.9f, 0.0f,// p57
        0.060f, 0.270f, 0.0f, 1.0f, 0.9f, 0.0f,// p60
        // t48
        -0.180f, 0.162f, 0.0f, 1.0f, 1.0f, 0.0f,// p57
        -0.200f, 0.054f, 0.0f, 1.0f, 1.0f, 0.0f,// p58
        0.060f, 0.270f, 0.0f, 1.0f, 1.0f, 0.0f,// p60
        // t49
        0.0f, 0.0f, 0.0f, 1.0f, 0.9f, 0.0f,// p59
        0.060f, 0.270f, 0.0f, 1.0f, 0.9f, 0.0f,// p60
        -0.120f, 0.129f, 0.0f, 1.0f, 0.9f, 0.0f,// p61
        // t50
        0.580f, 0.216f, 0.0f, 0.0f, 0.0f, 1.0f, // p11
        0.620f, 0.270f, 0.0f, 0.0f, 0.0f, 1.0f,// p62
        0.560f, 0.243f, 0.0f, 0.0f, 0.0f, 1.0f,// p63

        // dummy - sebenernya ga guna, tapi JANGAN DIHAPUS!! Nanti programnya ada bug
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

    //Declare VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);   

    //Declare VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    //Bind VAO and VBO to buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Copy vertices to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Configure buffer reading
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window)) {
        //Clear Screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 150); // Change 48 with number of vertex

        //Check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}