#include "GlobalFuncs.h"

extern bool firstCall;
extern float leftRightMB;
extern float pitch;
extern float yaw;
extern float lastX;
extern float lastY;
extern float SENS;
extern float angularFOV;

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

    win = glfwCreateWindow(800, 600, "Nameless Game Engine", 0, 0);
    if(win == 0){ //Get a handle to the created window obj
        printf("Failed to create GLFW win\n");
        return false;
    }
    glfwMakeContextCurrent(win); //Make context of the window the main context on the curr thread
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to init GLAD\n");
        return false;
    }

    glfwSetFramebufferSizeCallback(win, &FramebufferSizeCallback);
    glfwSetCursorPosCallback(win, CursorPosCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide and capture mouse cursor
    glfwSetMouseButtonCallback(win, MouseButtonCallback);
    glfwSetScrollCallback(win, ScrollCallback);

    return true;
}

static void FramebufferSizeCallback(GLFWwindow*, int width, int height){ //Resize callback
    glViewport(0, 0, width, height); //For viewport transform
} //Aspect ratio??

static void CursorPosCallback(GLFWwindow*, double xPos, double yPos){
    if(firstCall){
        firstCall = 0;
    } else{ //Add mouse movement offset between last frame and curr frame
        yaw -= (float(xPos) - lastX) * SENS;
        pitch -= (float(yPos) - lastY) * SENS; //??
    }
    lastX = float(xPos);
    lastY = float(yPos);

    ///For geometric intersection testing by ray casting
    //float x = (2.f * float(xPos)) / 800.f - 1.f, y = 1.f - (2.f * float(yPos)) / 600.f; //Viewport coords (viewportWidth to viewportHeight) to NDC (-1 to 1)
    //glm::vec3 rayNDC(x, y, -1.f); //-1.f so ray points forward
    //glm::vec4 rayClipSpace(rayNDC, 1.f); //1.f to form 4D vec //No need to reverse perspective division here as the ray has no intrinsic depth (only need in the special case of pts, for certain effects)
    //glm::vec4 rayViewSpace(glm::inverse(glm::perspective(glm::radians(angularFOV), 800.f / 600.f, .1f, 9999.f)) * rayClipSpace);
    //rayViewSpace = glm::vec4(rayViewSpace.x, rayViewSpace.y, -1.f, 0.f); //-1.f so... //0.f to indicate that the ray is not a pt (similar to directional light rays)
    //rayWorldSpace = glm::inverse(cam->LookAt()) * rayViewSpace;
    //rayWorldSpace = glm::normalize(rayWorldSpace);
}

static void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods){ //For mouse buttons
    leftRightMB = float(glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) - glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT));
}

static void ScrollCallback(GLFWwindow*, double xOffset, double yOffset){
    angularFOV -= float(xOffset) + float(yOffset);
    angularFOV = std::max(1.f, std::min(75.f, angularFOV));
}