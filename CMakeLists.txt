cmake_minimum_required(VERSION 3.22)
project(XMLevel)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions("HAVE_BOOLEAN=1")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/glew-2.1.0")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/freeglut")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/libjpeg")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(JPEG REQUIRED)

add_compile_options(-fpermissive)

include_directories(${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

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
        directx/ddraw.h)

target_link_libraries(XMLevel JPEG::JPEG  ${OPENGL_LIBRARIES} GLUT::GLUT GLEW::GLEW)