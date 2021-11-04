
extern "C" {
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
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

constexpr char ESCAPE = 0x1b;

constexpr float PI = 3.14159265359f;
inline float deg2rad(float x)
{
    return x * PI / 180;
}

/**
 * @brief Create a fully-seeded mt19937.
 *
 * @return A fully-seeded mt19937.
 */
template<class T = std::mt19937, std::size_t N = T::state_size * sizeof(typename T::result_type)>
inline auto seededMT () -> typename std::enable_if<N, T>::type
{
    std::random_device source;
    std::array<std::random_device::result_type, (N - 1) / sizeof(source()) + 1> randomData;
    std::generate(std::begin(randomData), std::end(randomData), std::ref(source));
    std::seed_seq seeds(std::begin(randomData), std::end(randomData));
    return T(seeds);
}

// Global constants.
constexpr char WINDOW_TITLE[] = "gltop";
constexpr GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// Global variables.
static std::mt19937 rng = seededMT();       // RNG. 
static int mainWindow = 0;                  // Glut window.

// Functions:
static void handleDisplay()
{

}

static void handleAnimate()
{
    
}

static void handleKeyboard(unsigned char key, int x, int y)
{
    
}

static void handleMouseButton(int button, int state, int x, int y)
{
    
}

static void handleMouseMotion(int x, int y)
{
    
}

static void handleMouseMotionPassive(int x, int y)
{
    
}

static void handleReset()
{
    
}

static void handleResize(int width, int height)
{
    
}

static void handleVisibility(int state)
{
    
}

int main(int argc, const char * const argv[])
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
    

    glutMainLoop();
    return 0;
}
