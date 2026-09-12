# Specify the minimum required CMake version.
cmake_minimum_required(VERSION 3.10)

# Define the project name and language (C).
project(RayTracer C)

# Use C99 for compatibility with TIGR.
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build the executable from the main program and tigr.c.
add_executable(raytracer
    ray_tracing.c
    tigr/tigr.c
)

# Add the directory containing tigr.h to the include search path.
target_include_directories(raytracer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tigr
)

# Link the platform-specific system libraries required by TIGR and the ray tracer.
if (WIN32)
    # Windows requires OpenGL and GDI32.
    target_link_libraries(raytracer
        opengl32
        gdi32
    )
elseif (APPLE)
    # macOS requires the OpenGL and Cocoa frameworks.
    target_link_libraries(raytracer
        "-framework OpenGL"
        "-framework Cocoa"
    )
elseif (UNIX)
    # Linux and other Unix-like systems require OpenGL, X11, and libm.
    target_link_libraries(raytracer
        GLU
        GL
        X11
        m
    )
endif()
