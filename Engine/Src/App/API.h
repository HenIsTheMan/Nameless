#pragma once
#include "Callback.h"

inline bool InitAPI(GLFWwindow*& win){
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
        printf("Failed to create GLFW window\n");
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