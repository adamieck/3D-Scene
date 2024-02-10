#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/gl.h>
#include <GL/gl.h>

#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Model.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "stb/stb_image.h"


int WIDTH = 800;
int HEIGHT = 600;

struct Light {
	glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Pointlight{
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

bool isWireframe = false;
bool isFill = true;
bool _enableCameraControls = true;
bool _leftMouseButtonPressed = false;
bool lightTree = false;

bool hasTexture = true;
bool hasNormals = true;
bool hasReflectors = true;
bool hasLight = true;
float turnSpeed = 1.0;
float movementSpeed = 5.0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessMouseMovement(0, 1 * turnSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessMouseMovement(0, -1 * turnSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(-1 * turnSpeed, 0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessMouseMovement(1 * turnSpeed, 0);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (_enableCameraControls && ImGui::GetIO().WantCaptureMouse)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        //camera.ProcessMouseMovement(xoffset, yoffset); // disabled mouse control for now
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }


}

void FPSCounter(float deltaTime)
{
    float fps = 1.0f / deltaTime;

    ImGui::Text("FPS: %.1f", fps);
}

void DisplayFileDialog(Texture* texture, const char* buttonTitle, const char* dialogTitle, unsigned slot)
{
    if (ImGui::Button(buttonTitle))
    {
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", dialogTitle, ".png,.jpg", ".");
    }
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action

            texture->ChangeTexture(filePathName);
            texture->Bind(slot);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}
glm::mat4 circularMotion(float& timeElapsed, float deltaTime) {
    float radius = 20.0f;
    float speed = 0.1f;  

    float centerX = 11.0f;
    float x = centerX + radius * cos(timeElapsed * speed);
    float z = radius * sin(timeElapsed * speed);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, 5.0f, z));

    float rotationAngle = atan2(x - centerX, z);
    model = glm::rotate(model, glm::degrees(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void ImGuiSetCatppuccinTheme()
{
    ImGuiIO& io = ImGui::GetIO();

    // Font
    ImFont* jetMono = io.Fonts->AddFontFromFileTTF("res/fonts/JetBrainsMono-Bold.ttf", 16);
    ImGui_ImplOpenGL3_CreateFontsTexture();

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.117, 0.117, 0.180, 1.0f }; // mocha base
    colors[ImGuiCol_ChildBg] = ImVec4{ 0.094, 0.094, 0.145, 1.0f }; // mocha mantle
    colors[ImGuiCol_Border] = ImVec4{ 0.117, 0.117, 0.180, 1.0f }; // mocha base

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.537, 0.862, 0.921, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.537, 0.862, 0.921, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.537, 0.862, 0.921, 1.0f };

    colors[ImGuiCol_Text] = ImVec4{ 0.803, 0.839, 0.956, 1.f }; // mocha sapphire

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.405, 0.573, 0.838, 1.0f }; // mocha blue modified #6792d6
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.537, 0.705, 0.980, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.537, 0.705, 0.980, 1.0f };

    //// Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.423, 0.439, 0.525, 1.0f }; // mocha overlay0
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.498, 0.517, 0.611, 1.0f }; // mocha overlay1
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.498, 0.517, 0.611, 1.0f }; // mocha overlay1

    colors[ImGuiCol_CheckMark] = ImVec4{ 0.454, 0.780, 0.925, 1.f }; // mocha sapphire
    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.980, 0.701, 0.529, 1.f }; // mocha peach
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.976, 0.886, 0.686, 1.f }; // mocha yellow

    //// Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.498, 0.517, 0.611, 1.0f }; // mocha overlay1
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.498, 0.517, 0.611, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.498, 0.517, 0.611, 1.0f };
}
void setCommonShaderUniforms(Shader& shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj,
    const Light& light, float fogIntensity, const glm::vec3& fogColor,
    const Pointlight& carpetLight)
{
    shader.SetUniformMatrix4f("model", model);
    shader.SetUniformMatrix4f("view", view);
    shader.SetUniformMatrix4f("proj", proj);
    shader.SetUniform3fv("light.position", light.position);
    shader.SetUniform3fv("light.ambient", light.ambient);
    shader.SetUniform3fv("light.diffuse", light.diffuse);
    shader.SetUniform3fv("light.specular", light.specular);
    shader.SetUniform1f("fogIntensity", fogIntensity);
    shader.SetUniform3fv("fogColor", fogColor);
    shader.SetUniform3fv("carpetLight.position", carpetLight.position);
    shader.SetUniform3fv("carpetLight.ambient", carpetLight.ambient);
    shader.SetUniform3fv("carpetLight.diffuse", carpetLight.diffuse);
    shader.SetUniform3fv("carpetLight.specular", carpetLight.specular);
    shader.SetUniform1f("carpetLight.constant", carpetLight.constant);
    shader.SetUniform1f("carpetLight.linear", carpetLight.linear);
    shader.SetUniform1f("carpetLight.quadratic", carpetLight.quadratic);
}
int main(void) 
{
    /* INITIALIZATION */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bezier Surface", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window!\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);

    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD!\n";
    }
    std::cout << glGetString(GL_VERSION) << '\n';

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);


    /* CODE STARTS HERE */

    float vertices[] = {                // tex coords
    0.0f, 1.0f, 0.0f,         0.0f, 1.0f,
    1.0 /3.0, 1.0f, 0.0f,     1.0 / 3.0, 1.0f,
    2.0 / 3.0, 1.0f, 0.0f,    2.0 / 3.0, 1.0f,
    1.0f, 1.0f, 0.0f,         1.0f, 1.0f,
    
    0.0f, 2.0/ 3.0, 0.0f,       0.0f, 2.0 / 3.0,
    1.0 / 3.0, 2.0 / 3.0, 0.0f, 1.0 / 3.0, 2.0 / 3.0,
    2.0 / 3.0, 2.0 / 3.0, 0.0f, 2.0 / 3.0, 2.0 / 3.0,
    1.0f, 2.0 / 3.0, 0.0f,      1.0f, 2.0 / 3.0,
    
    0.0f, 1.0 / 3.0, 0.0f,      0.0f, 1.0 / 3.0,
    1.0 / 3.0, 1.0 / 3.0, 0.0f, 1.0 / 3.0, 1.0 / 3.0,
    2.0 / 3.0, 1.0 / 3.0, 0.0f, 2.0 / 3.0, 1.0 / 3.0,
    1.0f, 1.0 / 3.0, 0.0f,      1.0f, 1.0 / 3.0,
    
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,
    1.0 / 3.0, 0.0f, 0.0f, 1.0 / 3.0, 0.0f,
    2.0 / 3.0, 0.0f, 0.0f, 2.0 / 3.0, 0.0f,
    1.0f, 0.0f, 0.0f,      1.0f, 0.0f,

    };

    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));
    VBLayout layout;

    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 2);
    va.AddBuffer(vb, layout);
    va.Unbind();

    Shader shader;
    shader.AddShader("res/shaders/texture.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/texture.frag", ShaderType::FRAGMENT)
        .AddShader("res/shaders/texture.tcs", ShaderType::TESS_CTRL)
        .AddShader("res/shaders/texture.tes", ShaderType::TESS_EVAL);
    shader.Build();
    shader.Bind();

    Texture tex("res/textures/carpet.jpg", 1);
    tex.Bind(1);
    std::cout << "Carpet text bound in slot " << tex.GetID();

    Shader phongShader;
    phongShader.AddShader("res/shaders/viewPhong.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/viewPhong.frag", ShaderType::FRAGMENT);
    phongShader.Build();

    Shader gouraudShader;
    gouraudShader.AddShader("res/shaders/viewGouraud.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/viewGouraud.frag", ShaderType::FRAGMENT);
    gouraudShader.Build();

    Shader flatShader;
    flatShader.AddShader("res/shaders/flat.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/flat.frag", ShaderType::FRAGMENT);
    flatShader.Build();

    Renderer renderer;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGuiSetCatppuccinTheme();


    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 MVP;

    shader.Bind();
    int tessLevel = 16;
	glPatchParameteri(GL_PATCH_VERTICES, tessLevel);
    shader.SetUniform1f("TessLevel", float(tessLevel));

    float fogIntensity = 0.5;
    glm::vec3 fogColor = glm::vec3(0.286f, 0.902f, 0.902f);
    Model desert = Model("res/models/Desert/oasis.obj");

    va.Unbind();

    Light light{ glm::vec3(5.0,5.44,12.08), glm::vec3(0.5,0.5,0.5),
    glm::vec3(0.8,0.8,0.8), glm::vec3(0.4,0.4,0.4) };

    Pointlight carpetLight{ glm::vec3(5.0,5.44,12.08), glm::vec3(0.5,0.5,0.5),
glm::vec3(0.8,0.8,0.8), glm::vec3(0.4,0.4,0.4),
    1.0, 0.07f, 0.017f};

    float timeElapsed = 0.0f;
    const char* cameras[] = { "Free", "Fixed", "Tracking", "TPP" };
    static int currentCamera = 0;

    const char* shaders[] = { "Flat", "Phong", "Gouraud"};
    static int currentShader = 0;

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        timeElapsed += deltaTime;

        // Process input
        processInput(window);
        camera.ChangeMovementSpeed(movementSpeed);

        // Render 
        renderer.Clear();



        model = circularMotion(timeElapsed, deltaTime);
        glm::vec3 targetPosition = glm::vec3(model[3]);
        float distFromTarget = 2.0f;
        switch (currentCamera) {
        case 0:
            view = camera.GetViewMatrix();
            break;
        case 1:
            view = glm::lookAt(glm::vec3(-25.0f, 15.7f, -17.6f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0, 1, 0));
            break;
        case 2:
            view = glm::lookAt(glm::vec3(9.5f, 20.0f, 0.0f), targetPosition, glm::vec3(0, 1, 0));
            break;
        case 3:
            glm::vec3 targetForward = glm::normalize(glm::vec3(model[1]));
            glm::vec3 viewDirection = -targetForward;
            glm::vec3 cameraPosition = targetPosition + (distFromTarget * viewDirection);
            view = glm::lookAt(cameraPosition + glm::vec3(0.0, 0.5, 0.0), targetPosition, glm::vec3(0.0, 1.0, 0.0));
            break;
        default:
            camera.GetViewMatrix();
            break;
        }

        carpetLight.position = targetPosition;

        proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        MVP = proj * view * model;
        glm::vec3 viewPos = glm::inverse(view)[3]; // 3rd row of inversed view matrix is the world position of camera


        // Draw Calls

        // Desert draw
        glm::mat4 modelDesert = glm::mat4(1.0f);
        modelDesert = glm::scale(modelDesert, glm::vec3(0.5, 0.5, 0.5));
        modelDesert = glm::translate(modelDesert, glm::vec3(20.0f, -20.0f, 0.0));

        switch (currentShader) {
        case 0:
            flatShader.Bind();
            setCommonShaderUniforms(flatShader, modelDesert, view, proj, light, fogIntensity, fogColor, carpetLight);
            desert.Draw(flatShader);
            flatShader.Unbind();
            break;
        case 1:
            phongShader.Bind();
            setCommonShaderUniforms(phongShader, modelDesert, view, proj, light, fogIntensity, fogColor, carpetLight);
            desert.Draw(phongShader);
            phongShader.Unbind();
            break;
        case 2:
            gouraudShader.Bind();
            setCommonShaderUniforms(gouraudShader, modelDesert, view, proj, light, fogIntensity, fogColor, carpetLight);
            desert.Draw(gouraudShader);
            gouraudShader.Unbind();
            break;
        default:
            break;
        }

        // Bezier draw
        for(int i = 15; i >= 0; i--)
        {
            vertices[i * 5 + 2] = (float)sin(timeElapsed + (i % 4)) * 0.3; // "flying" animation
        }
        vb.Bind();
        vb.Update(vertices, sizeof(vertices));
        
        shader.Bind();
        tex.Bind(1);
        shader.SetUniformMatrix4f("MVP", MVP);
        shader.SetUniform1f("TessLevel", float(tessLevel));
        renderer.Draw(va, shader, sizeof(vertices) / (sizeof(float) * 5));
        tex.Unbind();
        vb.Unbind();

    	// ImGui here //
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        FPSCounter(deltaTime);
        ImGui::Text(glm::to_string(viewPos).c_str());
        ImGui::Separator();
        // Light
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light"))
        {
			ImGui::Text("Parameters:");
            ImGui::SliderFloat("Fog Intensity", &fogIntensity, 0.0f, 1.0f);

            ImGui::ColorEdit3("Fog Color", (float*)&fogColor);

            if (ImGui::Button("Day!")) {
                renderer.ChangeBackground(0.286f, 0.902f, 0.902f);
                fogColor = glm::vec3(0.286f, 0.902f, 0.902f);
                light.ambient = glm::vec3(0.5, 0.5, 0.5);
                light.diffuse = glm::vec3(0.8, 0.8, 0.8);
            }
            if (ImGui::Button("Night!")) {
                renderer.ChangeBackground(0.141f, 0.129f, 0.2f);
                fogColor = glm::vec3(0.141f, 0.129f, 0.2f);
                light.ambient = glm::vec3(0.00, 0.00, 0.0);
                light.diffuse = glm::vec3(0.00, 0.00, 0.0);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera"))
        {
            ImGui::Combo("Camera", &currentCamera, cameras, IM_ARRAYSIZE(cameras));
            ImGui::SliderFloat("Free camera movement speed", &movementSpeed, 0.0f, 10.0f);
            ImGui::SliderFloat("Free camera turning speed", &turnSpeed, 0.0f, 10.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Shaders"))
        {
            ImGui::Combo("Shaders", &currentShader, shaders, IM_ARRAYSIZE(shaders));
            ImGui::TreePop();
        }


        if(ImGui::TreeNode("Surface"))
        {
            ImGui::SliderInt("Tesselation Level", &tessLevel , 4, 64);



            for (int i = 0; i < 16; i++)
            {
                ImGui::SliderFloat(
                    ("Ctrl Point " + std::to_string(i)).c_str(), & vertices[i * 5 + 2],
                    -1.0f, 1.0f                              
                );
            }
            vb.Bind();
        	vb.Update(vertices, sizeof(vertices));
            vb.Unbind();


            ImGui::TreePop();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //ImGui end //

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwTerminate();
    return 0;
}