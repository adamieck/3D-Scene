#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

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
        camera.ProcessMouseMovement(0, 1);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessMouseMovement(0, -1);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(-1, 0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessMouseMovement(1, 0);
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

        camera.ProcessMouseMovement(xoffset, yoffset);
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

    float rotationAngle = timeElapsed * speed;
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

    Shader modelShader;
    modelShader.AddShader("res/shaders/model.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/model.frag", ShaderType::FRAGMENT);
    modelShader.Build();
    
    Shader shader;
    shader.AddShader("res/shaders/texture.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/texture.frag", ShaderType::FRAGMENT)
		.AddShader("res/shaders/texture.tcs", ShaderType::TESS_CTRL)
        .AddShader("res/shaders/texture.tes", ShaderType::TESS_EVAL);
    shader.Build();
    shader.Bind();



    Texture tex("res/textures/carpet.jpg", 0);
    tex.Bind(0);
    shader.SetUniform1i("_Tex", 0); // (name , tex_slot)

    Texture normalmap("res/normalmaps/stone_floor.jpg", 1);
    normalmap.Bind(1);
	//shader.SetUniform1i("_NormalMap", 1);

    //glm::vec4 col = glm::vec4(0.70f, 0.745f, 0.99f, 1.0f);
    //shader.SetUniform4fv("_Color", col);

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


    float Kd = 0.5f;
    float Ks = 0.5f;
    float m = 1.0f;
    float zLight = 0.3f;
    bool paused = false;

    float fogIntensity = 0.5;
    glm::vec3 fogColor = glm::vec3(0.286f, 0.902f, 0.902f);

    glm::vec3 lightColor(1.0, 1.0, 1.0);
    //float reflectorAlpha = 0.1f;
    //shader.SetUniform1f("Kd", Kd);
    //shader.SetUniform1f("Ks", Ks);
    //shader.SetUniform1f("m", m);
    //shader.SetUniform3f("LightPosition", 0.5f, 0.5f, zLight);
    //shader.SetUniform3fv("LightColor", lightColor);
    //stbi_set_flip_vertically_on_load(0);
    Model garfield = Model("res/models/Garfield/garfield.obj");
    Model desert = Model("res/models/Desert/desert2.obj");
    //Model backpack = Model("res/models/Backpack/backpack.obj");
    Light light{ glm::vec3(6.9,-2.5,11.9), glm::vec3(0.5,0.5,0.5),
    glm::vec3(0.8,0.8,0.8), glm::vec3(0.2,0.2,0.2) };

    float timeElapsed = 0.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 0.0, 0.0));
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Process input
        processInput(window);

        // Render
        renderer.Clear();

        timeElapsed += deltaTime;


        model = circularMotion(timeElapsed, deltaTime);
        view = camera.GetViewMatrix();
        proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        MVP = proj * view * model;
        glm::vec3 viewPos = glm::inverse(view)[3];



        // Main Draw Call

        // Desert draw
        modelShader.Bind();
        glm::mat4 modelDesert = glm::mat4(1.0f);
        modelDesert = glm::scale(modelDesert, glm::vec3(0.5, 0.5, 0.5));
        modelDesert = glm::translate(modelDesert, glm::vec3(20.0f, -20.0f, 0.0));
        modelShader.SetUniformMatrix4f("model", modelDesert);
        modelShader.SetUniformMatrix4f("view", view);
		modelShader.SetUniformMatrix4f("proj", proj);
        modelShader.SetUniform3fv("light.position", light.position);
        modelShader.SetUniform3fv("light.ambient", light.ambient);
        modelShader.SetUniform3fv("light.diffuse", light.diffuse);
        modelShader.SetUniform3fv("light.specular", light.specular);
        modelShader.SetUniform1f("fogIntensity", fogIntensity);
        modelShader.SetUniform3fv("fogColor", fogColor);
        modelShader.SetUniform3fv("viewPos", viewPos);

        //modelShader.SetUniformMatrix4f("MVP", MVP);
        desert.Draw(modelShader);
        garfield.Draw(modelShader);
        modelShader.Unbind();

        // Bezier draw
        for(int i = 0; i < 16; i++)
        {
            vertices[i * 5 + 2] = (float)sin(timeElapsed + (i / 4)) * 0.3;
        }
        vb.Bind();
        vb.Update(vertices, sizeof(vertices));
        
        shader.Bind();
        shader.SetUniformMatrix4f("MVP", MVP);
        shader.SetUniform1f("TessLevel", float(tessLevel));
        renderer.Draw(va, shader, sizeof(vertices) / (sizeof(float) * 5));
        vb.Unbind();

    	// ImGui here //
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        FPSCounter(deltaTime);
        ImGui::Text(glm::to_string(viewPos).c_str());
        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &isWireframe);
        ImGui::Checkbox("Fill", &isFill);


        // Light
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light"))
        {
			ImGui::Text("Parameters:");
            ImGui::SliderFloat("Z-Position", &zLight, 0.0f, 2.0f);
            ImGui::SliderFloat("Kd", &Kd, 0.0f, 1.0f);

            ImGui::SliderFloat("Ks", &Ks, 0.0f, 1.0f);
            ImGui::SliderFloat("Fog Intensity", &fogIntensity, 0.0f, 1.0f);

            ImGui::ColorEdit3("Fog Color", (float*)&fogColor);
            ImGui::Checkbox("Light?", &hasLight);
            ImGui::Checkbox("Reflectors?", &hasReflectors);


            if (ImGui::Button("Toggle Animation")) {
                paused = !paused;
            }
            ImGui::TreePop();
        }


        if(ImGui::TreeNode("Surface"))
        {
            ImGui::SliderInt("Tesselation Level", &tessLevel , 4, 64);



            for (int i = 0; i < 16; i++)
            {
                ImGui::SliderFloat(
                    ("Ctrl Point " + std::to_string(i)).c_str(), & vertices[i * 5 + 2],
                    0.0f, 1.0f                              
                );
            }
            vb.Bind();
        	vb.Update(vertices, sizeof(vertices));
            vb.Unbind();


            ImGui::TreePop();
        }

        //ImGui::PopFont();
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



