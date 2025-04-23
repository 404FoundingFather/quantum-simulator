# src/config/CMakeLists.txt
add_library(config STATIC
    ConfigLoader.cpp
)
target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(config PRIVATE nlohmann_json::nlohmann_json HDF5::HDF5)
