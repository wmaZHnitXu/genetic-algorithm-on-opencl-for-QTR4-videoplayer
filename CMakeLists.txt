cmake_minimum_required(VERSION 3.1) # 3.1 << C_STANDARD 11

project(QTR4_gpu_videoconverter LANGUAGES C)

find_package(OpenCL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(PNG REQUIRED)

add_executable(${PROJECT_NAME}
    src/image_presentation.c
    src/main.c
    src/dmatrix.c
)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SDL2_LIBRARIES}
        SDL2_image::SDL2_image
        OpenCL::OpenCL
        PNG::PNG
)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 11
                                                 C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=100)