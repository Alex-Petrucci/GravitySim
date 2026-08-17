cmake_minimum_required(VERSION 4.3)
project(GravitySim)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(dep/SFML-3.1.0)

add_executable(GravitySim
        src/Main.cpp
        src/Application.cpp
        src/Application.hpp
        src/config.hpp
        src/Objects.cpp
        src/Objects.hpp)

target_link_libraries(GravitySim PRIVATE SFML::Graphics)

add_custom_command(
        TARGET GravitySim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:GravitySim>/assets
)