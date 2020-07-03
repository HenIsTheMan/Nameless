#pragma once
#include "../Core.h"
#include "Global.h"

inline void FramebufferSizeCallback(GLFWwindow*, int width, int height){ //Resize callback
    glViewport(0, 0, width, height); //For viewport transform
} //Aspect ratio??

inline void CursorPosCallback(GLFWwindow*, double xPos, double yPos){
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

inline void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods){ //For mouse buttons
    leftRightMB = float(glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) - glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT));
}

inline void ScrollCallback(GLFWwindow*, double xOffset, double yOffset){
    angularFOV -= float(xOffset) + float(yOffset);
    angularFOV = std::max(1.f, std::min(75.f, angularFOV));
}