# src/visualization/CMakeLists.txt
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)

add_library(visualization STATIC
    VisualizationEngine.cpp
)
target_include_directories(visualization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(visualization PUBLIC OpenGL::GL glfw glad::glad)