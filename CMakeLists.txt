# 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和语言（C）
project(RayTracer C)

# 设置 C 语言标准（建议至少 C99，TIGR 兼容）
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加可执行文件目标，包含你的主程序以及 tigr.c
add_executable(raytracer
    ray_tracing.c
    tigr/tigr.c
)

# 让编译器能搜索到 tigr.h 所在的目录
target_include_directories(raytracer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tigr
)

# 根据操作系统，链接不同的系统图形库
if (WIN32)
    # Windows 下需要 OpenGL 和 GDI32
    target_link_libraries(raytracer
        opengl32
        gdi32
    )
elseif (APPLE)
    # macOS 下需要 OpenGL 和 Cocoa 框架
    target_link_libraries(raytracer
        "-framework OpenGL"
        "-framework Cocoa"
    )
elseif (UNIX)
    # Linux 及其他类 Unix 系统
    target_link_libraries(raytracer
        GLU
        GL
        X11
    )
endif()