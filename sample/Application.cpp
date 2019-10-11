#include "Application.h"
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Application::Application(const char* windowTitle, int initialScreenWidth, int initialScreenHeight) :
	hairSystem(nullptr),
	hairAsset(nullptr),
	hairInstance(nullptr),
	cameraController(nullptr),
	windMagnitude(0.0f)
{
    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Cannot initialize GLFW.");
    }

    window = glfwCreateWindow(initialScreenWidth, initialScreenHeight, windowTitle, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    hairSystem = new HairGL::HairSystem();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    hairAsset = hairSystem->LoadAsset("data/hair.hgl");
    hairInstance = hairSystem->CreateInstance(hairAsset);

    hairSettings.visualizeGuides = true;
    hairSettings.visualizeGrowthMesh = true;
    hairSettings.modelMatrix.SetIdentity();
    hairSettings.tipWidth = 0.0001f;
    hairSettings.color = { 0.8f, 0.5f, 0.3f, 1.0f };

    cameraController = new CameraController(window);
}

void Application::Run()
{
    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Update(1.0f / 60.0f);
        Render();
    }
}

void Application::Update(float timeStep)
{
    cameraController->Update();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 settingsWindowPosition;
    settingsWindowPosition.x = 10;
    settingsWindowPosition.y = 10;

    ImVec2 settingsWindowSize;
    settingsWindowSize.x = 400;
    settingsWindowSize.y = 420;

    ImGui::SetNextWindowPos(settingsWindowPosition);
    ImGui::SetNextWindowSizeConstraints(settingsWindowSize, settingsWindowSize);
    ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Checkbox("Visualize Hair Guides", &hairSettings.visualizeGuides);
    ImGui::Checkbox("Visualize Growth Mesh", &hairSettings.visualizeGrowthMesh);
    ImGui::Checkbox("Render Hair", &hairSettings.renderHair);
    ImGui::SliderFloat("Density", &hairSettings.density, 3.0f, 64.0f);
    ImGui::SliderFloat("Tesselation Factor", &hairSettings.tesselationFactor, 1.0f, 4.0f);
    ImGui::SliderFloat("Tip Width", &hairSettings.tipWidth, 0.0001f, 0.001f);
    ImGui::SliderFloat("Root Width", &hairSettings.rootWidth, 0.001f, 0.005f);
    ImGui::SliderFloat("Thinning Start", &hairSettings.thinningStart, 0.0f, 1.0f);
    ImGui::SliderFloat("Ambient", &hairSettings.ambient, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse", &hairSettings.diffuse, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular", &hairSettings.specular, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Power", &hairSettings.specularPower, 20.0f, 100.0f);
    ImGui::ColorEdit3("Color", hairSettings.color.m);
    ImGui::SliderFloat("Global Stiffness", &hairSettings.globalStiffness, 0.0f, 1.0f);
	ImGui::SliderFloat("Local Stiffness", &hairSettings.localStiffness, 0.0f, 1.0f);
    ImGui::SliderFloat("Damping", &hairSettings.damping, 0.0f, 0.5f);
	ImGui::SliderFloat("Wind Magnitude", &windMagnitude, 0.0f, 100.0f);
    ImGui::End();
    ImGui::Render();

	hairSettings.wind = HairGL::Vector3(1.0f, 0.3f, 1.0f).Normalized() * windMagnitude;

    hairSystem->UpdateInstanceSettings(hairInstance, hairSettings);
    hairSystem->Simulate(hairInstance, timeStep);
}

void Application::Render()
{
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    auto viewMatrix = cameraController->GetViewMatrix();
    auto projectionMatrix = HairGL::Matrix4::Perspective(HairGL::DegToRad * 100.0f, (float)screenWidth / (float)screenHeight, 0.01f, 100.0f);

    glClearColor(0.9f, 0.9f, 0.9f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    hairSystem->Render(hairInstance, viewMatrix, projectionMatrix);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    hairSystem->DestroyInstance(hairInstance);
    hairSystem->DestroyAsset(hairAsset);
    delete hairSystem;

    delete cameraController;

    glfwDestroyWindow(window);
    glfwTerminate();
}