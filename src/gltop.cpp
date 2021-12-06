
extern "C" {
#include <GL/glew.h>
#include <GL/freeglut.h>
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
#include <cstdlib>
#include <chrono>

#include "gltop.hpp"
#include "util.hpp"
#include "loadobj.hpp"

using namespace std::chrono_literals;
using namespace std::string_literals;
namespace chron = std::chrono;

constexpr bool VERBOSE = false;
constexpr int BMP_MAGIC_NUMBER = 0x4d42;

// Global constants.
constexpr char WINDOW_TITLE[] = "gltop";      // Name of window.
constexpr GLfloat BACKCOLOR[] = { 0.f, 0.f, 0.f, 1.f }; // Reset color.
constexpr char ESCAPE = 0x1b;                 // ESCAPE key code.
constexpr GLfloat PI = 3.14159265359f;        // PI.
constexpr int DEFAULT_WINDOW_WIDTH = 1200;    // Initial window width.
constexpr int DEFAULT_WINDOW_HEIGHT = 800;    // Initial window height.
constexpr float MINSCALE = 0.05f;

// Utility functions:

// Convert degrees to radians.
inline GLfloat deg2rad(GLfloat x)
{
    return x * PI / 180;
}

// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;

int ReadInt(FILE *fp)
{
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	const unsigned char b2 = fgetc(fp);
	const unsigned char b3 = fgetc(fp);
	return (b3 << 24)  |  (b2 << 16)  |  (b1 << 8)  |  b0;
}

short ReadShort(FILE *fp)
{
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	return (b1 << 8)  |  b0;
}


std::uint8_t *BmpToTexture(const char *filename, int *width, int *height)
{
	FILE *fp;
#ifdef _WIN32
        errno_t err = fopen_s( &fp, filename, "rb" );
        if( err != 0 )
        {
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
        }
#else
	fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if( VERBOSE ) fprintf( stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
			FileHeader.bfType, FileHeader.bfType&0xff, (FileHeader.bfType>>8)&0xff );
	if( FileHeader.bfType != BMP_MAGIC_NUMBER )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}


	FileHeader.bfSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize );

	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );

	FileHeader.bfOffBytes = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfOffBytes = %d\n", FileHeader.bfOffBytes );


	InfoHeader.biSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSize = %d\n", InfoHeader.biSize );
	InfoHeader.biWidth = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biWidth = %d\n", InfoHeader.biWidth );
	InfoHeader.biHeight = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biHeight = %d\n", InfoHeader.biHeight );

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biPlanes = %d\n", InfoHeader.biPlanes );

	InfoHeader.biBitCount = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount );

	InfoHeader.biCompression = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression );

	InfoHeader.biSizeImage = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage );

	InfoHeader.biXPixelsPerMeter = ReadInt( fp );
	InfoHeader.biYPixelsPerMeter = ReadInt( fp );

	InfoHeader.biClrUsed = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed );

	InfoHeader.biClrImportant = ReadInt( fp );


	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );


	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char *texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\n" );
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ( ( InfoHeader.biBitCount*InfoHeader.biWidth + 31 ) / 32 );
	if( VERBOSE )	fprintf( stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes );

	int myRowSizeInBytes = ( InfoHeader.biBitCount*InfoHeader.biWidth + 7 ) / 8;
	if( VERBOSE )	fprintf( stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes );

	int oldNumExtra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;
	if( VERBOSE )	fprintf( stderr, "Old NumExtra padding = %d\n", oldNumExtra );

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if( VERBOSE )	fprintf( stderr, "New NumExtra padding = %d\n", numExtra );


	// this function does not support compression:

	if( InfoHeader.biCompression != 0 )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}
	

	// we can handle 24 bits of direct color:
	if( InfoHeader.biBitCount == 24 )
	{
		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if( InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256 )
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32 *colorTable = new struct rgba32[ InfoHeader.biClrUsed ];

		rewind( fp );
		fseek( fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET );
		for( int c = 0; c < InfoHeader.biClrUsed; c++ )
		{
			colorTable[c].r = fgetc( fp );
			colorTable[c].g = fgetc( fp );
			colorTable[c].b = fgetc( fp );
			colorTable[c].a = fgetc( fp );
			if( VERBOSE )	fprintf( stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a );
		}

		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				int index = fgetc( fp );
				*(tp+0) = colorTable[index].r;	// r
				*(tp+1) = colorTable[index].g;	// g
				*(tp+2) = colorTable[index].b;	// b
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}

		delete[ ] colorTable;
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}
// Create a mt19937 random number generator with a good seed.
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
static std::vector<std::string> args;       // Argument vector.
static gltop::Timer animTimer(1000ms);
static gltop::Timer procTimer(500ms, [](float f)
{
    gltop::Proctab allTab;
    for(auto proc = allTab.getNextProcess(); proc;
        proc = allTab.getNextProcess())
    {
        auto argv = proc.getArgv();
        std::string nameStr = argv.size() ? argv[0] : "";
    }
});

constexpr GLfloat FOGCOLOR[4] = {0.f, 0.f, 0.f, 1.f};
constexpr GLenum FOGMODE = GL_LINEAR;
constexpr GLfloat FOGDENSITY = 0.3f;
constexpr GLfloat FOGSTART = 1.5f;
constexpr GLfloat FOGEND = 4.f;

struct thing
{
    std::uint8_t *texture = nullptr;
    int width = 0;
    int height = 0;
    GLuint texID = 0;
    GLuint list = 0;

    void create(const std::string &objPath, const std::string &textPath)
    {
        list = glGenLists(1);
        if(list == 0)
            throw std::runtime_error("Could not create list");
        glNewList(list, GL_COMPILE);
        if(LoadObjFile(objPath.c_str(), 1.f) != 0)
            throw std::invalid_argument("No obj at "s + objPath);
        glEndList();

        texture = BmpToTexture(textPath.c_str(), &width, &height);
        if(!texture)
            throw std::invalid_argument("No texture at "s + objPath);

        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, texture);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void draw()
    {
        glBindTexture(GL_TEXTURE_2D, texID);
        glCallList(list);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

static thing cuckoo;
static int activeButton = 0;			// current button that is down
static int Xmouse = 0;
static int Ymouse = 0;			// mouse values
static float xRot = 0.f;
static float yRot = 0.f;				// rotation angles in degrees
static float scale = 0.f;					// scaling factor

// scroll wheel button values:

constexpr float ANGFACT = 1.;
constexpr float SCLFACT = 0.005f;
constexpr int SCROLL_WHEEL_UP   =  3;
constexpr int SCROLL_WHEEL_DOWN =  4;
constexpr float SCROLL_WHEEL_CLICK_FACTOR =  5.f;
constexpr int LEFT = 4;
constexpr int MIDDLE = 2;
constexpr int RIGHT  = 1;
// General Functions:

// Callback functions:

// Handle displaying to the screen.
static void handleDisplay()
{
    glutSetWindow(mainWindow);

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat tGreen[] = {0.f, 1.f, 0.f, 1.f};
    GLfloat tWhite[] = {1.f, 1.f, 1.f, 1.f};
    GLfloat tBlack[] = {0.f, 0.f, 0.f, 1.f};
    GLfloat tShine = 60.f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, tWhite);
    glMaterialfv(GL_FRONT, GL_AMBIENT, tWhite);
    glMaterialfv(GL_FRONT, GL_SPECULAR, tWhite);
    glMaterialf(GL_FRONT, GL_SHININESS, tShine);
    

    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_FLAT);

    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy;
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 0.1f, 1000.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.f, 100.f, 50.f, 0.f, 80.f, 0.f, 0.f, 1.f, 0.f);

    // Lights


    // Draw
	glRotatef(yRot, 0., 1., 0.);
	glRotatef(xRot, 1., 0., 0.);
	if(scale < MINSCALE)
		scale = MINSCALE;
	glScalef(scale, scale, scale);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    cuckoo.draw();


    // End draw

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.f, 100.f, 0.f, 100.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.f, 1.f, 1.f);
    // Swap the double buffers.
    glutSwapBuffers();

    // Be sure the graphics buffer has been send:
    glFlush();
}

// Handle animations, physics, etc..
static void handleIdle()
{
    animTimer.elapseAnimateNormalized();
    procTimer.elapseAnimateNormalized();
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
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (scale < MINSCALE)
				scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (scale < MINSCALE)
				scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		activeButton |= b;		// set the proper bit
	}
	else
	{
		activeButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(mainWindow);
	glutPostRedisplay();
}

// Handle mouse motion events.
static void handleMouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( activeButton & LEFT ) != 0 )
	{
		xRot += (ANGFACT*dy);
		yRot += (ANGFACT*dx);
	}


	if( ( activeButton & MIDDLE ) != 0 )
	{
		scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( scale < MINSCALE )
			scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( mainWindow );
	glutPostRedisplay( );
}

// Handle passive mouse motion events.
static void handleMouseMotionPassive(int x, int y)
{
    handleMouseMotion(x, y);
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

static void init()
{
    cuckoo.create("chicken.obj", "chicken.bmp");
}

// Main.
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    args = std::vector<std::string>(argv + 1, argv + argc);
    // Initialize graphics.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

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
    glutIdleFunc(handleIdle);
    glutMouseWheelFunc(handleMouseWheel);
    glutSpecialFunc(handleSpecialKeyboard);
    // Unused callbacks.
    glutEntryFunc(nullptr);
    glutSpaceballMotionFunc(nullptr);
	glutSpaceballRotateFunc(nullptr);
	glutSpaceballButtonFunc(nullptr);
	glutButtonBoxFunc(nullptr);
	glutDialsFunc(nullptr);
	glutTabletMotionFunc(nullptr);
	glutTabletButtonFunc(nullptr);
	glutMenuStateFunc(nullptr);
	glutTimerFunc(-1, nullptr, 0);

    init();

	// Draw the scene once and wait for some interaction:
	// (this will never return).
    glutSetWindow(mainWindow);
    glutMainLoop();

	// glutMainLoop( ) never returns.
	// This line is here to make the compiler happy:
    return 0;
}

