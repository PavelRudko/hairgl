#include "CameraController.h"
#include <cmath>

CameraController::CameraController(GLFWwindow* window) :
    window(window),
    target(HairGL::Vector3()),
    distance(0.5f),
    pitch(0),
    yaw(0),
    zoomSensitivity(0.05f),
    sensitivity(0.005f),
    isDragging(false),
    dragPrevious(HairGL::Vector3()),
    dragMode(DragMode::Rotation)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);
}

HairGL::Matrix4 CameraController::GetViewMatrix() const
{
    auto position = GetPosition();
    auto viewMatrix = HairGL::Matrix4::LookAt(position, target, HairGL::Vector3(0, 1, 0));
    return viewMatrix;
}

HairGL::Vector3 CameraController::GetPosition() const
{
    HairGL::Vector4 offset(0, 0, distance, 1.0f);
    auto verticalRotation = HairGL::Matrix4::RotateX(pitch);
    auto horizontalRotation = HairGL::Matrix4::RotateY(yaw);
    offset = verticalRotation * HairGL::Vector4(offset.x, offset.y, offset.z, 1);
    offset = horizontalRotation * HairGL::Vector4(offset.x, offset.y, offset.z, 1);

    return target + offset.XYZ();
}

void CameraController::Update()
{
    SetDistance(distance);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_RELEASE || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) != GLFW_RELEASE) {
        double currentMousePositionX;
        double currentMousePositionY;
        glfwGetCursorPos(window, &currentMousePositionX, &currentMousePositionY);
        HairGL::Vector3 currentMousePosition(currentMousePositionX, currentMousePositionY, 0);

        if (!isDragging) {
            isDragging = true;
            dragPrevious = currentMousePosition;
            dragMode = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_RELEASE ? DragMode::Rotation : DragMode::Translation;
        }
        
        OnDrag(currentMousePosition - dragPrevious, dragMode);
        dragPrevious = currentMousePosition;
    }
    else {
        isDragging = false;
    }
}

void CameraController::SetDistance(float distance)
{
    this->distance = std::fmax(distance, 0.01f);
}

void CameraController::SetPitch(float pitch)
{
    if (pitch >= HairGL::PI / 2) {
        pitch = HairGL::PI / 2 - 0.00001f;
    }

    if (pitch <= -HairGL::PI / 2) {
        pitch = -HairGL::PI / 2 + 0.00001f;
    }

    this->pitch = pitch;
}

void CameraController::SetYaw(float yaw)
{
    if (yaw > HairGL::PI * 2) {
        yaw = 0;
    }

    this->yaw = yaw;
}

void CameraController::SetSensitivity(float sensitivity)
{
    this->sensitivity = std::fmax(sensitivity, 0.0f);
}

void CameraController::SetZoomSensitivity(float zoomSensitivity)
{
    this->zoomSensitivity = std::fmax(zoomSensitivity, 0.0f);
}

void CameraController::Pan(const HairGL::Vector3& offset)
{
    auto forward = (target - GetPosition()).Normalized();
    auto right = HairGL::Vector3::Cross(forward, HairGL::Vector3(0, 1, 0)).Normalized();
    auto up = HairGL::Vector3::Cross(right, forward);
    target += up * offset.y * sensitivity - right * offset.x * sensitivity;
}

void CameraController::OnScroll(double xoffset, double yoffset)
{
    SetDistance(distance - yoffset * zoomSensitivity);
}

void CameraController::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    controller->OnScroll(xoffset, yoffset);
}

void CameraController::OnDrag(const HairGL::Vector3& offset, DragMode dragMode)
{
    if (dragMode == DragMode::Rotation) {
        SetPitch(pitch + offset.y * sensitivity);
        SetYaw(yaw + offset.x * sensitivity);
    }
    else if (dragMode == DragMode::Translation) {
        Pan(offset);
    }
}

CameraController::~CameraController()
{
    glfwSetWindowUserPointer(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
}
