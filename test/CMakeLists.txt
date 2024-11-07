cmake_minimum_required(VERSION 3.1)
project(MahjongTest)

# 设置Windows子系统
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# 静态链接设置
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

# ImGui 源文件
set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

# GLFW 设置
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)

# 查找 OpenGL 和 GLFW
find_package(OpenGL REQUIRED)

# 直接指定 GLFW 静态库路径
set(GLFW_LIBRARY "D:/msys2/ucrt64/lib/libglfw3.a")

# 添加源文件
set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Network.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UIManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/MainMenuPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/AboutPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/HelpPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/JoinRoomPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/CreateRoomPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pages/ConsolePage.cpp
    ${IMGUI_SOURCES}
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 添加包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../Mahjong
    ${CMAKE_CURRENT_SOURCE_DIR}/pages
    "D:/msys2/ucrt64/include"
)

# 链接库
target_link_libraries(${PROJECT_NAME} 
    MahjongCore 
    fmt 
    tenhou_shuffle 
    Ws2_32
    ${GLFW_LIBRARY}
    OpenGL::GL
    gdi32
    -static
    -Wl,-Bstatic -lpthread
)
