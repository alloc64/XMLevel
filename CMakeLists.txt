cmake_minimum_required(VERSION 3.22)
project(XMLevel)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_compile_options(-fpermissive)

add_executable(XMLevel
        main.cpp
        main.h
        xmlevel.h
        xmlevel.cpp
        math/vector.cpp
        math/vector.h
        collset.cpp
        collset.h
        set.h
        TexturePacker.h
        TexturePacker.cpp
        cmrenderer.cpp
        cmrenderer.h
        textures.h
        textures.cpp
        skybox.cpp
        skybox.h
        frustum.cpp
        frustum.h
        )

target_link_libraries(XMLevel ${OPENGL_LIBRARIES})