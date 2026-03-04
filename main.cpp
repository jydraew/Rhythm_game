#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "MenuScene.h"
#include "GameScene.h" // 이전에 만든 파일

enum SceneType { MENU, GAME };

int main() {
    if (!glfwInit()) return 1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Rhythm Game", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    MenuScene menu;
    GameScene game;
    SceneType currentScene = MENU;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (currentScene == MENU) {
            menu.Update();
            if (menu.startRequested) {
                game.Start(menu.selectedSong.title, menu.selectedSong.bpm);
                currentScene = GAME;
            }
        } 
        else if (currentScene == GAME) {
            game.Update();
            if (game.shouldExit) currentScene = MENU;
        }

        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    // Cleanup 생략...
    return 0;
}