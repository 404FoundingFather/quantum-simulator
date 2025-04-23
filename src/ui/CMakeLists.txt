# src/ui/CMakeLists.txt
find_package(imgui CONFIG REQUIRED)

add_library(ui STATIC
    UIManager.cpp
)
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ui
    PUBLIC imgui::imgui glfw
)