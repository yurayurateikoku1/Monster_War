cmake_minimum_required(VERSION 3.16)
project(MonsterWar VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})
list(PREPEND CMAKE_PREFIX_PATH 
"D:/GameDevelopmentLibrary/SDL-3.3.3/install/cmake"
"D:/GameDevelopmentLibrary/SDL3_image-3.2.4/cmake"
"D:/GameDevelopmentLibrary/SDL_mixer-3.0.0/install/cmake"
"D:/GameDevelopmentLibrary/SDL3_ttf-3.2.2/cmake"
"D:/GameDevelopmentLibrary/glm-1.0.2/share/glm"
"D:/GameDevelopmentLibrary/json-3.12.0/build/install/share/cmake/nlohmann_json"
"D:/GameDevelopmentLibrary/spdlog-1.16.0/build/install/lib/cmake/spdlog"
"D:/GameDevelopmentLibrary/entt-3.16.0/build/install/lib/EnTT/cmake"
)
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(EnTT REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extrernal/imgui-1.92.5)
set(IMGUI_SOURCES 
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp)

include_directories(
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

set(TARGET ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/**/*.cpp
    src/*.h
    src/**/*.h
)

add_executable(${TARGET} ${SOURCES} ${IMGUI_SOURCES})

target_link_libraries(${TARGET}
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
    glm::glm
    EnTT::EnTT
)

if(MSVC)
    target_link_options(${TARGET} PRIVATE "/SUBSYSTEM:CONSOLE")
endif()

