cmake_minimum_required(VERSION 3.22)
project(synthetizer)

include(FetchContent)

function(downloadFile url name_)

    FetchContent_Declare(${name_}
        URL ${url}
    )

    FetchContent_Populate(${name_})

endfunction(downloadFile)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

downloadFile("https://github.com/ocornut/imgui/archive/refs/heads/docking.zip" IMGUI)

FetchContent_MakeAvailable(IMGUI)

file(GLOB SOURCE
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/node/*.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp

    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_*glfw*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_*opengl3*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        GL
        GLEW
        glfw
        soundio
        m
)

target_compile_options(${PROJECT_NAME}
    PUBLIC
        -ggdb
        -g3
)
