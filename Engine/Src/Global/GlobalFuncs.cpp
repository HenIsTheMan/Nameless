#include "GlobalFuncs.h"

extern bool firstCall;
extern float leftRightMB;
extern float pitch;
extern float yaw;
extern float lastX;
extern float lastY;
extern float SENS;
extern float angularFOV;
extern int winWidth;
extern int winHeight;

bool Key(const char& key){
    return GetAsyncKeyState((unsigned short)key) & 0x8000;
}

bool InitAPI(GLFWwindow*& win){
    glfwInit();
    //glfwWindowHint(GLFW_SAMPLES, 4); //4 subsamples in a general pattern per set of screen coords of a pixel to determine pixel coverage //Better pixel coverage precision but more performance reduction with more sample pts as they cause size of... buffers to rise by...
    //Super Sample Anti-Aliasing (SSAA, draw more frags, sample pt in the center of each pixel determines if each pixel is influenced by any frag shader or not) temporarily uses a much higher resolution render buffer to render to and the resolution is downsampled back to normal after the scene is rendered
    //Result of Multisample Anti-Aliasing (MSAA) is a framebuffer with higher resolution depth or stencil buffer where primitive edges are smoother
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac OS X
    #endif

    //RECT rect;
    //if(GetWindowRect(hwnd, &rect)){
    //    winWidth = rect.right - rect.left;
    //    winHeight = rect.bottom - rect.top;
    //} else{
    //    puts("Failed to set winWidth and winHeight\n");
    //}
    //HWND hwnd = ::GetActiveWindow();
    //ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    SetWindowPos(GetConsoleWindow(), 0,
        int((GetSystemMetrics(SM_CXFULLSCREEN) - float(mode->width - GetSystemMetrics(SM_CXFULLSCREEN)) / 2.f) * 5.f / 6.f),
        0,
        int((GetSystemMetrics(SM_CXFULLSCREEN) - float(mode->width - GetSystemMetrics(SM_CXFULLSCREEN)) / 2.f) / 6.f),
        GetSystemMetrics(SM_CYFULLSCREEN),
        0);
    winWidth = int((GetSystemMetrics(SM_CXFULLSCREEN) - float(mode->width - GetSystemMetrics(SM_CXFULLSCREEN)) / 2.f) * 5.f / 6.f);
    winHeight = GetSystemMetrics(SM_CYFULLSCREEN) - int(float(mode->height - GetSystemMetrics(SM_CYFULLSCREEN)) / 2.f);
    win = glfwCreateWindow(winWidth, winHeight, "Nameless Engine", nullptr, nullptr);
    glfwSetWindowPos(win, int(float(mode->width - GetSystemMetrics(SM_CXFULLSCREEN)) / 2.f), int(float(mode->height - GetSystemMetrics(SM_CYFULLSCREEN)) / 2.f));

    if(win == 0){ //Get a handle to the created window obj
        puts("Failed to create GLFW win\n");
        return false;
    }
    glfwMakeContextCurrent(win); //Make context of the window the main context on the curr thread
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        puts("Failed to init GLAD\n");
        return false;
    }
    glfwSetFramebufferSizeCallback(win, &FramebufferSizeCallback);
    glfwSetCursorPosCallback(win, CursorPosCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide and capture mouse cursor
    glfwSetMouseButtonCallback(win, MouseButtonCallback);
    glfwSetScrollCallback(win, ScrollCallback);

    return true;
}

bool InitConsole(){
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    SetConsoleTitleA("Nameless Console");
    system("Color 0A");

    HANDLE StdHandle = GetStdHandle(DWORD(-11));
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(StdHandle, &cursorInfo);
    cursorInfo.bVisible = 0;
    SetConsoleCursorInfo(StdHandle, &cursorInfo);

    auto ConsoleEventHandler = [](const DWORD event){
        LPCWSTR msg;
        switch(event){
            case CTRL_C_EVENT: msg = L"Ctrl + C"; break;
            case CTRL_BREAK_EVENT: msg = L"Ctrl + BREAK"; break;
            case CTRL_CLOSE_EVENT: msg = L"Closing prog..."; break;
            case CTRL_LOGOFF_EVENT: case CTRL_SHUTDOWN_EVENT: msg = L"User is logging off..."; break;
            default: msg = L"???";
        }
        MessageBox(NULL, msg, L"Nameless", MB_OK);
        return TRUE;
    };

    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleEventHandler, TRUE)){
        puts("Failed to install console event handler!\n");
        return false;
    }
    return true;
}

static void FramebufferSizeCallback(GLFWwindow*, int width, int height){ //Resize callback
    glViewport(0, 0, width, height); //For viewport transform
    winWidth = width;
    winHeight = height;
}

static void CursorPosCallback(GLFWwindow*, double xPos, double yPos){
    if(firstCall){
        firstCall = 0;
    } else{ //Add mouse movement offset between last frame and curr frame
        yaw -= (float(xPos) - lastX) * SENS;
        pitch -= (float(yPos) - lastY) * SENS;
    }
    lastX = float(xPos);
    lastY = float(yPos);
}

static void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods){ //For mouse buttons
    leftRightMB = float(glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) - glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT));
}

static void ScrollCallback(GLFWwindow*, double xOffset, double yOffset){
    angularFOV -= float(xOffset) + float(yOffset);
    angularFOV = std::max(1.f, std::min(75.f, angularFOV));
}