#define UNICODE
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>

// Link necessary libraries for the compiler
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// --- Global Variables ---
// State Management (same as before)
float lightPos[] = { 0.0f, 2.0f, 2.0f, 1.0f };
float rotationAngle = 0.0f;
bool isLightOn = true;
int currentShape = 1; // 0 for Pyramid, 1 for Sphere

// Win32 Handles
HDC   hDC = NULL;   // Private GDI Device Context
HGLRC hRC = NULL;   // Permanent Rendering Context
HWND  hWnd = NULL;  // Holds Our Window Handle

// --- Function Prototypes ---
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void init();
void drawPyramid();
void drawSphere(float radius, int sectors, int stacks);
void renderScene();

// --- WinMain: The Application's Entry Point ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"OpenGLWindowClass";
    wc.style = CS_OWNDC; // Use one Device Context for the entire lifetime of the window

    RegisterClass(&wc);

    // Create the window
    hWnd = CreateWindowEx(
        0,                              // Optional window styles.
        L"OpenGLWindowClass",           // Window class
        L"OpenGL with Win32 API",       // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // Size and position
        NULL, NULL, hInstance, NULL
    );

    if (hWnd == NULL) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


// --- WindowProc: Handles Messages for the Window ---
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // --- OpenGL Context Creation ---
        hDC = GetDC(hwnd);
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR), 1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
        };
        int pixelFormat = ChoosePixelFormat(hDC, &pfd);
        SetPixelFormat(hDC, pixelFormat, &pfd);
        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);
        init(); // Our OpenGL setup
        return 0;
    }

    case WM_CLOSE: {
        DestroyWindow(hwnd);
        return 0;
    }

    case WM_DESTROY: {
        // Cleanup
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC);
        ReleaseDC(hwnd, hDC);
        PostQuitMessage(0);
        return 0;
    }

    case WM_SIZE: { // Handle window resizing
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
        glMatrixMode(GL_MODELVIEW);
        return 0;
    }

    case WM_PAINT: { // Handle redrawing the window
        renderScene();
        SwapBuffers(hDC); // Swap buffers for double-buffering
        ValidateRect(hwnd, NULL); // Tell Windows we've handled the paint message
        return 0;
    }

    case WM_KEYDOWN: { // Handle key presses
        float step = 0.5f;
        float angleStep = 5.0f;
        switch (wParam) {
        case 'W': lightPos[1] += step; break;
        case 'S': lightPos[1] -= step; break;
        case 'A': lightPos[0] -= step; break;
        case 'D': lightPos[0] += step; break;
        case 'E': lightPos[2] -= step; break;
        case 'Q': lightPos[2] += step; break;
        case 'P': currentShape = 0; break;
        case 'O': currentShape = 1; break;
        case VK_SPACE: isLightOn = !isLightOn; break;
        case VK_UP: rotationAngle += angleStep; break;
        case VK_DOWN: rotationAngle -= angleStep; break;
        case VK_ESCAPE: DestroyWindow(hwnd); break;
        }
        InvalidateRect(hwnd, NULL, FALSE); // Request a redraw
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// --- OpenGL Functions ---

// The scene rendering logic
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 3.0, 7.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    if (isLightOn) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);

    glRotatef(rotationAngle, 1.0f, 1.0f, 1.0f);

    if (currentShape == 0) drawPyramid();
    else drawSphere(1.5f, 30, 30);
}

// init(), drawPyramid(), and drawSphere() are exactly the same as the GLFW example.
// They contain only standard OpenGL calls and are independent of the windowing system.

void init() {
    glClearColor(1.0f, 0.85f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    float lightDiffuse[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    float materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
}

void drawPyramid() {
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.447f, 0.894f); glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glNormal3f(0.894f, 0.447f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glNormal3f(0.0f, 0.447f, -0.894f);
    glVertex3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glNormal3f(-0.894f, 0.447f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
    glEnd();
}

void drawSphere(float radius, int sectors, int stacks) {
    glColor3f(1.0f, 1.0f, 1.0f);
    const float M_PI = 3.1415926535f;
    for (int i = 0; i < stacks; ++i) {
        float lat0 = M_PI * (-0.5f + (float)i / stacks);
        float z0 = radius * sin(lat0);
        float zr0 = radius * cos(lat0);
        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z1 = radius * sin(lat1);
        float zr1 = radius * cos(lat1);
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= sectors; ++j) {
            float lng = 2 * M_PI * (float)j / sectors;
            float x = cos(lng); float y = sin(lng);
            glNormal3f(x * zr0, y * zr0, z0); glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1); glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}
