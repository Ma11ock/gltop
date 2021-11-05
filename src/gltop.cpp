
extern "C" {
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <proc/procps.h>
#include <proc/readproc.h>
}

#include <string>
#include <iostream>
#include <cmath>
#include <limits>
#include <vector>
#include <array>
#include <random>
#include <cstdint>
#include <algorithm>
#include <glm/glm.hpp>
#include <cctype>

// Global constants.
constexpr char WINDOW_TITLE[] = "gltop";
constexpr GLfloat BACKCOLOR[] = { 0.f, 0.f, 0.f, 1.f };
constexpr char ESCAPE = 0x1b;
constexpr GLfloat PI = 3.14159265359f;

// Utility functions:
inline GLfloat deg2rad(GLfloat x)
{
    return x * PI / 180;
}

template<class T = std::mt19937,
         std::size_t N = T::state_size * sizeof(typename T::result_type)>
inline auto seededMT () -> typename std::enable_if<N, T>::type
{
    std::random_device source;
    std::array<std::random_device::result_type,
               (N - 1) / sizeof(source()) + 1> randomData;
    std::generate(std::begin(randomData), std::end(randomData), std::ref(source));
    std::seed_seq seeds(std::begin(randomData), std::end(randomData));
    return T(seeds);
}

// Global variables.
static std::mt19937 rng = seededMT();       // RNG. 
static int mainWindow = 0;                  // Glut window.

// Callback functions:

// Handle displaying to the screen.
static void handleDisplay()
{
    glutSetWindow(mainWindow);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Swap the double buffers.
    glutSwapBuffers();

    // Be sure the graphics buffer has been send:
    glFlush();
}

// Handle animations.
static void handleAnimate()
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
}

// Handle special keyboard codes (non alphanumeric).
static void handleSpecialKeyboard(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_F1:
        // TODO help
        break;
    case GLUT_KEY_F2:
        // TODO setup
        break;
    case GLUT_KEY_F3:
        // TODO search
        break;
    case GLUT_KEY_F4:
        // TODO Filter
        break;
    case GLUT_KEY_F5:
        // TODO Tree
        break;
    case GLUT_KEY_F6:
        // TODO Sort by
        break;
    case GLUT_KEY_F7:
        // TODO Nice -
        break;
    case GLUT_KEY_F8:
        // TODO Nice +
        break;
    case GLUT_KEY_F9:
        // TODO Kill
        break;
    case GLUT_KEY_F10:
        // TODO Quit
        break;
    case GLUT_KEY_DOWN:
        // TODO Select parent process.
        break;
    case GLUT_KEY_UP:
        // TODO Select a child process.
        break;
    case GLUT_KEY_RIGHT:
        // TODO Select a sister processes.
        break;
    case GLUT_KEY_LEFT:
        // TODO Select a sister processes.
        break;
    }
}

// Handle alphanumeric keyboard codes.
static void handleKeyboard(unsigned char key, int x, int y)
{
    key = static_cast<unsigned char>(std::tolower(static_cast<int>(key)));
    switch(key)
    {
    case 'k':
        // TODO kill process
        break;
    case 'w':
        // TODO move down.
        break;
    case 'a':
        // TODO move left
        break;
    case 's':
        // TODO move down
        break;
    case 'd':
        // TODO move right
        break;
    default:
        break;
    }
}

// Handle mouse button presses.
static void handleMouseButton(int button, int state, int x, int y)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        // TODO select process
        break;
    case GLUT_MIDDLE_BUTTON:
        // TODO 
        break;
    case GLUT_RIGHT_BUTTON:
        // TODO 
        break;
    default:
        break;
    }
}

// Handle mouse motion events.
static void handleMouseMotion(int x, int y)
{
    
}

// Handle passive mouse motion events.
static void handleMouseMotionPassive(int x, int y)
{
    
}

// Handle mouse wheel action.
static void handleMouseWheel(int wheel, int direction, int x, int y)
{
    switch(direction)
    {
    case -1: // Down.
        break;
    case 1: // Up.
        break;
    default:
        std::cerr << "Invalid mouse wheel direction.\n";
        break;
    }
}

// Handle reset.
static void handleReset()
{
    
}

static void handleResize(int width, int height)
{
    
}

static void handleVisibility(int state)
{
    
}

// Main.
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // Initialize graphics.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);

    mainWindow = glutCreateWindow(WINDOW_TITLE);
    glutSetWindowTitle(WINDOW_TITLE);
    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

    // Set up callback functions.
    glutSetWindow(mainWindow);
    glutDisplayFunc(handleDisplay);
    glutReshapeFunc(handleResize);
    glutKeyboardFunc(handleKeyboard);
    glutMouseFunc(handleMouseButton);
    glutMotionFunc(handleMouseMotion);
    glutPassiveMotionFunc(handleMouseMotionPassive);
    glutVisibilityFunc(handleVisibility);
    glutIdleFunc(handleAnimate);
    glutMouseWheelFunc(handleMouseWheel);
    glutSpecialFunc(handleSpecialKeyboard);
    // Unused callbacks.
    glutEntryFunc(nullptr);
    glutSpecialFunc(nullptr);
    glutSpaceballMotionFunc(nullptr);
	glutSpaceballRotateFunc(nullptr);
	glutSpaceballButtonFunc(nullptr);
	glutButtonBoxFunc(nullptr);
	glutDialsFunc(nullptr);
	glutTabletMotionFunc(nullptr);
	glutTabletButtonFunc(nullptr);
	glutMenuStateFunc(nullptr);
	glutTimerFunc(-1, nullptr, 0);
    
	// Draw the scene once and wait for some interaction:
	// (this will never return).
    glutSetWindow(mainWindow);
    glutMainLoop();

	// glutMainLoop( ) never returns.
	// This line is here to make the compiler happy:
    return 0;
}
