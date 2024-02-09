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
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "stb/stb_image.h"


int WIDTH = 800;
int HEIGHT = 600;

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
    
    0.0f, 2.0/ 3.0, 1.0f,       0.0f, 2.0 / 3.0,
    1.0 / 3.0, 2.0 / 3.0, 0.0f, 1.0 / 3.0, 2.0 / 3.0,
    2.0 / 3.0, 2.0 / 3.0, -1.0f, 2.0 / 3.0, 2.0 / 3.0,
    1.0f, 2.0 / 3.0, 0.0f,      1.0f, 2.0 / 3.0,
    
    0.0f, 1.0 / 3.0, 0.0f,      0.0f, 1.0 / 3.0,
    1.0 / 3.0, 1.0 / 3.0, 0.0f, 1.0 / 3.0, 1.0 / 3.0,
    2.0 / 3.0, 1.0 / 3.0, 0.0f, 2.0 / 3.0, 1.0 / 3.0,
    1.0f, 1.0 / 3.0, 0.0f,      1.0f, 1.0 / 3.0,
    
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,
    1.0 / 3.0, 0.0f, 0.0f, 1.0 / 3.0, 0.0f,
    2.0 / 3.0, 0.0f, 0.1f, 2.0 / 3.0, 0.0f,
    1.0f, 0.0f, 0.0f,      1.0f, 0.0f,

    };

    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));
    VBLayout layout;

    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 2);
    va.AddBuffer(vb, layout);
    
    Shader shader;
    shader.AddShader("res/shaders/texture.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/texture.frag", ShaderType::FRAGMENT)
		.AddShader("res/shaders/texture.tcs", ShaderType::TESS_CTRL)
        .AddShader("res/shaders/texture.tes", ShaderType::TESS_EVAL);
    shader.Build();
    shader.Bind();


    // Wireframe shader
    Shader shaderWire;
    shaderWire.AddShader("res/shaders/texture.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/basic.frag", ShaderType::FRAGMENT)
        .AddShader("res/shaders/texture.tcs", ShaderType::TESS_CTRL)
        .AddShader("res/shaders/texture.tes", ShaderType::TESS_EVAL);
    shaderWire.Build();

    Shader modelShader;
    modelShader.AddShader("res/shaders/model.vert", ShaderType::VERTEX)
        .AddShader("res/shaders/model.frag", ShaderType::FRAGMENT);
    modelShader.Build();

    Texture tex("res/textures/stone_floor.jpg", 0);
    tex.Bind(0);
    shader.SetUniform1i("_Tex", 0); // (name , tex_slot)

    Texture normalmap("res/normalmaps/stone_floor.jpg", 1);
    normalmap.Bind(1);
	shader.SetUniform1i("_NormalMap", 1);

    glm::vec4 col = glm::vec4(0.70f, 0.745f, 0.99f, 1.0f);
    shader.SetUniform4fv("_Color", col);

    Renderer renderer;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGuiSetCatppuccinTheme();


    glm::mat4 model;
    glm::mat4 modelY;
    glm::mat4 modelZ;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 MVP;

    int tessLevel = 16;
	glPatchParameteri(GL_PATCH_VERTICES, tessLevel);
    shader.SetUniform1f("TessLevel", float(tessLevel));
    shaderWire.Bind();
    shaderWire.SetUniform1f("TessLevel", float(tessLevel));
    shader.Bind();


    float Kd = 0.5f;
    float Ks = 0.5f;
    float m = 1.0f;
    float zLight = 0.3f;
    bool paused = false;
    float maxArg = 25.0f;
    float minArg = 0.1f;
    float direction = 1.0f;
    glm::vec3 lightColor(1.0, 1.0, 1.0);
    float reflectorAlpha = 0.1f;
    shader.SetUniform1f("Kd", Kd);
    shader.SetUniform1f("Ks", Ks);
    shader.SetUniform1f("m", m);
    shader.SetUniform3f("LightPosition", 0.5f, 0.5f, zLight);
    shader.SetUniform3fv("LightColor", lightColor);
    float arg = 0;
    float rotateTime = 0;
    //stbi_set_flip_vertically_on_load(1);
    Model garfield = Model("res/models/Garfield/garfield.obj");
    Model desert = Model("res/models/Desert/desert.obj");
    //Model backpack = Model("res/models/Backpack/backpack.obj");
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // Process input
        processInput(window);

        // Render
        renderer.Clear();
        float rotateSpeed = 50.0f;
        rotateTime = (rotateTime + deltaTime * rotateSpeed) < 360 ? (rotateTime + deltaTime * rotateSpeed) : 0;

        // X ROTATION
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0));
        model = glm::rotate(model,  glm::radians(rotateTime), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(-0.5, -0.5, 0.0));
        model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));

        // Y ROTATION
        modelY = glm::mat4(1.0f);
        modelY = glm::translate(modelY, glm::vec3(0.5f, 0.5f, 0.0));
        modelY = glm::rotate(modelY, glm::radians(rotateTime), glm::vec3(0.0f, 1.0f, 0.0f));
        modelY = glm::translate(modelY, glm::vec3(-0.5, -0.5, 0.0));

        // Z ROTATION
        modelZ = glm::mat4(1.0f);
        modelZ = glm::translate(modelZ, glm::vec3(0.5f, 0.5f, 0.0));
        modelZ = glm::rotate(modelZ, glm::radians(rotateTime), glm::vec3(0.0f, 0.0f, 1.0f));
        modelZ = glm::translate(modelZ, glm::vec3(-0.5, -0.5, 0.0));

        view = camera.GetViewMatrix();
        proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        MVP = proj * view * model;

        // shader Uniforms
        shader.Bind();
        shader.SetUniformMatrix4f("MVP", MVP);
        shader.SetUniform1f("TessLevel", float(tessLevel));

        shader.SetUniform1i("hasTexture", (int)hasTexture);
        shader.SetUniform1i("hasNormals", (int)hasNormals);
        shader.SetUniform1i("hasReflectors", (int)hasReflectors);
    	shader.SetUniform1i("hasLight", (int)hasLight);
        shader.SetUniform1f("Kd", Kd);
        shader.SetUniform1f("Ks", Ks);
        shader.SetUniform1f("m", m);
        shader.SetUniform3fv("LightColor", lightColor);

        // ShaderWire 
        shaderWire.Bind();
        shaderWire.SetUniformMatrix4f("MVP", MVP);
        shaderWire.SetUniform1f("TessLevel", (float)tessLevel);

        shader.Bind();

        // LIGHT ANIMATION //
        if (!paused)
        {
            float speed = 4.0f;
            arg += static_cast<float>(deltaTime) * speed * direction;

            if (arg > maxArg || arg < minArg) {
                direction = -direction; 
            }
            float startPos = 0.5f;
            float offset = 0.5;
            float x = arg / 40 * cos(arg);
            float y = arg / 40 * sin(arg);
            float z = zLight;
            shader.SetUniform3f("LightPosition", x + startPos, y + startPos, z);
        }

        // Main Draw Call

        //glDisable(GL_DEPTH_TEST);
        //shader.Unbind();
        //shaderWire.Unbind();
        modelShader.Bind();
        glm::mat4 modelGarf = glm::mat4(1.0f);
        modelGarf = glm::translate(modelGarf, glm::vec3(0.0f, 0.0f, 0.0));
        //modelShader.SetUniformMatrix4f("modelGarf", modelGarf);
        //modelShader.SetUniformMatrix4f("viewGarf", view);
		//modelShader.SetUniformMatrix4f("projGarf", proj);
        modelShader.SetUniformMatrix4f("MVP", MVP);
        garfield.Draw(modelShader);
        //backpack.Draw(modelShader);
        //desert.Draw(modelShader);

        if (isFill)
        {
            renderer.Draw(va, shader, sizeof(vertices) / (sizeof(float) * 5));
            glm::mat4 MVPY = proj * view * modelY;
            shader.SetUniformMatrix4f("MVP", MVPY);
            //renderer.Draw(va, shader, sizeof(vertices) / (sizeof(float) * 5));
            glm::mat4 MVPZ = proj * view * modelZ;
            shader.SetUniformMatrix4f("MVP", MVPZ);
            //renderer.Draw(va, shader, sizeof(vertices) / (sizeof(float) * 5));

        }
        if (isWireframe)
        {
            renderer.SetPolygonMode(GL_LINE);
            renderer.Draw(va, shaderWire, sizeof(vertices) / (sizeof(float) * 5));
            glm::mat4 MVPY = proj * view * modelY;
            shaderWire.SetUniformMatrix4f("MVP", MVPY);
            renderer.Draw(va, shaderWire, sizeof(vertices) / (sizeof(float) * 5));
            glm::mat4 MVPZ = proj * view * modelZ;
            shaderWire.SetUniformMatrix4f("MVP", MVPZ);
            renderer.Draw(va, shaderWire, sizeof(vertices) / (sizeof(float) * 5));
        }
        renderer.SetPolygonMode(GL_FILL);
        glEnable(GL_DEPTH_TEST);
        shader.Bind();



    	// ImGui here //
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        


        FPSCounter(deltaTime);
        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &isWireframe);
        ImGui::Checkbox("Fill", &isFill);


        // Light
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light"))
        {
			ImGui::Text("Parameters:");
            ImGui::SliderFloat("Z-Position", &zLight, 0.0, 2.0);
            ImGui::SliderFloat("Kd", &Kd, 0.0, 1.0);

            ImGui::SliderFloat("Ks", &Ks, 0.0, 1.0);
            ImGui::SliderFloat("m", &m, 1.0, 100.0);

            ImGui::ColorEdit3("Color", (float*)&lightColor);
            ImGui::Checkbox("Light?", &hasLight);
            ImGui::Checkbox("Reflectors?", &hasReflectors);
            //ImGui::SliderFloat("reflectorAlpha", &reflectorAlpha, 0.0, 20.0);
            //shader.SetUniform1f("reflectorAlpha", reflectorAlpha);

            if (ImGui::Button("Toggle Animation")) {
                paused = !paused;
            }
            ImGui::TreePop();
        }

        //if (ImGui::TreeNode("Textures"))
        //{
        //    ImGui::Checkbox("Texture?", &hasTexture);

        //    if (hasTexture)
        //    {
        //        ImGui::Image(reinterpret_cast<void*>(static_cast <intptr_t>(tex.GetID())), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
        //        DisplayFileDialog(&tex, "Change texture...", "Choose texture", 0);
        //    }
        //    else
        //    {
        //        ImGui::ColorEdit4("Solid Color", (float*)&col);
        //        shader.SetUniform4fv("_Color", col);
        //    }

        //    ImGui::Checkbox("NormalMap?", &hasNormals);
        //    if (hasNormals)
        //    {
        //        ImGui::Image(reinterpret_cast<void*>(static_cast <intptr_t>(normalmap.GetID())), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
        //        DisplayFileDialog(&normalmap, "Change normal...", "Choose normalmap", 1);
        //    }
        //    ImGui::TreePop();
        //}
        //
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
        	vb.Update(vertices, sizeof(vertices));


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


