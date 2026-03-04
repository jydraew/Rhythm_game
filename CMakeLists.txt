cmake_minimum_required(VERSION 3.25)

# [중요] 프로젝트 이름 정의 전에 vcpkg 경로를 수동으로 지정합니다.
set(VCPKG_ROOT "D:/game/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# 라이브러리가 설치된 실제 폴더를 직접 힌트로 줍니다.
set(CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-windows")

project(ImGuiDemo)

# 다시 패키지를 찾습니다.
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(MyApp 
    main.cpp 
    MenuScene.cpp 
    MenuScene.h 
    GameScene.cpp 
    GameScene.h
)
# 링킹 설정
target_link_libraries(MyApp PRIVATE imgui::imgui glfw opengl32)