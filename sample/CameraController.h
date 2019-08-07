#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <hairgl/Math.h>
#include <GLFW/glfw3.h>

enum class DragMode
{
    Rotation,
    Translation
};

class CameraController
{
public:
    CameraController(GLFWwindow* window);

    HairGL::Matrix4 GetViewMatrix() const;
    HairGL::Vector3 GetPosition() const;

    void Update();
    void SetDistance(float distance);
    void SetPitch(float pitch);
    void SetYaw(float yaw);
    void SetSensitivity(float sensitivity);
    void SetZoomSensitivity(float zoomSensitivity);

    ~CameraController();

private:
    GLFWwindow* window;
    HairGL::Vector3 target;
    float distance, pitch, yaw, sensitivity, zoomSensitivity;
    bool isDragging;
    HairGL::Vector3 dragPrevious;
    DragMode dragMode;

    void OnDrag(const HairGL::Vector3& offset, DragMode dragMode);
    void Pan(const HairGL::Vector3& offset);
    void OnScroll(double xoffset, double yoffset);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif