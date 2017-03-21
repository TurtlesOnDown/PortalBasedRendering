#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <cmath>

// Other includes
#include "XPlane.h"
#include "Shader.h"
#include "RenderStructure.h"
#include "Camera.h"

#include "GeometryLoader.h"


const double PI = 3.14159265359;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLFWwindow* openGLinit();
GLuint newTexture(const char *path);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start the application and run the game loop
int main()
{
  GeometryLoader loader;
  if (loader.loadFromFile("sectors.json")) {
    cout << "loaded sectors.json successfuly!" << endl;
  }
  else {
    cout << loader.getLastError() << endl;
  }
    
  GLFWwindow* window = openGLinit();

    // Build and compile our shader program
    Shader ourShader("default.vs", "default.frag");

    Vertex one{ {-.25,.25,0} };

    vector<Vertex> testPlaneCoords{ 
      { { 10.0f,10.0f,0 }, {1,1}},
      { { 10.0f,-10.0f,0 }, {1,0}},
      { { -10.0f,-10.0f,0 }, {0,0}},
      { { -10.0f,10.0f,0 }, {0,1}},
    };


    XVertex v0 = { { 20, 20,-20 },{ .0f, .0f } };
    XVertex v1 = { {-20, 20,-20 },{ .0f, .0f } };
    XVertex v2 = { {-20,-20,-20 },{ .0f, .0f } };
    XVertex v3 = { { 20,-20,-20 },{ .0f, .0f } };
    XVertex v4 = { { 20, 20, 20 },{ .0f, .0f } };
    XVertex v5 = { {-20, 20, 20 },{ .0f, .0f } };
    XVertex v6 = { {-20,-20, 20 },{ .0f, .0f } };
    XVertex v7 = { { 20,-20, 20 },{ .0f, .0f } };
    XVertex v8 = { { 20, 20, 60 },{ .0f, .0f } };
    XVertex v9 = { {-20, 20, 60 },{ .0f, .0f } };
    XVertex v10= { {-20,-20, 60 },{ .0f, .0f } };
    XVertex v11= { { 20,-20, 60 },{ .0f, .0f } };

    vector<XVertex> s1b{ v0, v1, v2 };
    vector<XVertex> s1f{ v4, v5, v6 };
    vector<XVertex> s1l{ v1, v5, v6 };
    vector<XVertex> s1r{ v0, v4, v7 };
    vector<XVertex> s1u{ v0, v1, v5 };
    vector<XVertex> s1d{ v3, v2, v6 };

    vector<XVertex> s2b{ v4, v5, v6 };
    vector<XVertex> s2f{ v8, v9, v10 };
    vector<XVertex> s2l{ v5, v9, v10 };
    vector<XVertex> s2r{ v4, v8, v11 };
    vector<XVertex> s2u{ v4, v5, v9 };
    vector<XVertex> s2d{ v7, v6, v10 };

    GLuint texture = newTexture("container.jpg");

    XPlane s1B(s1b, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s1F(s1f, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s1L(s1l, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s1R(s1r, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s1U(s1u, {0, 0, 1}, texture, nullptr, nullptr);
    XPlane s1D(s1d, {0, 0, 1}, texture, nullptr, nullptr);

    XPlane s2B(s2b, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s2F(s2f, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s2L(s2l, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s2R(s2r, {0, 1, 0}, texture, nullptr, nullptr);
    XPlane s2U(s2u, {0, 0, 1}, texture, nullptr, nullptr);
    XPlane s2D(s2d, {0, 0, 1}, texture, nullptr, nullptr);

    vector<XPlane> cube1{s1B, s1F, s1L, s1R, s1U, s1D};
    vector<XPlane> cube2{s2B, s2F, s2L, s2R, s2U, s2D};

    Sector sector1(cube1);
    Sector sector2(cube2);
    World testWorld{sector1, sector2};

    glm::mat4 tempTf;
    tempTf = glm::translate(tempTf, glm::vec3(-1, 0, 0));

    //cout << "This Garbage :" << testingShit << endl;

    /*Texture testTexture{ {texture} };
    Transformation testTransform1{ {0,0,0},{5, 5, 5},{90},{0},{0} };
    Transformation testTransform2{ { 0,0,5 },{ 5, 5, 5 },{ 0 },{0},{0} };
    Transformation testTransform3{ { 0,-5,0 },{ 5, 5, 5 },{ 45 },{ 45 },{ 0 } };
    Transformation testTransform4{ { 0,5,0 },{ 5, 5, 5 },{ 45 },{ -45 },{ 0 } };

    Plane testPlane(testPlaneCoords, texture, tempTf);
    Plane testPlane2(testPlaneCoords, texture, tempTf);
    Plane testPlane3(testPlaneCoords, texture, tempTf);
    Plane testPlane4(testPlaneCoords, texture, tempTf);*/

    glm::mat4 projection;
    projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
    Renderer TestingRenderer(projection, ourShader, testWorld);
    //TestingRenderer.pushPlane(testingShit);
    //TestingRenderer.pushPlane(testPlane2);
    //TestingRenderer.pushPlane(testPlane3);
    //TestingRenderer.pushPlane(testPlane4);
    //TestingRenderer.pushPlane(testingShit);



    // Game loop
    while (!glfwWindowShouldClose(window))
    {
      // Set frame time
      GLfloat currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        Do_Movement();

        TestingRenderer.draw(camera);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

GLFWwindow* openGLinit() {
  // Init GLFW
  glfwInit();
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PortalBasedRendering", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  glewInit();

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  glEnable(GL_DEPTH_TEST);

  return window;
}

GLuint newTexture(const char *path) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
                                         // Set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Load image, create texture and generate mipmaps
  int width, height;
  unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

  return texture;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
  // Camera controls
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  //cout << key << endl;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if (key >= 0 && key < 1024)
  {
    if (action == GLFW_PRESS)
      keys[key] = true;
    else if (action == GLFW_RELEASE)
      keys[key] = false;
  }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  GLfloat xoffset = xpos - lastX;
  GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}
