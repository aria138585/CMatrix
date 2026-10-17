# test_obj subdir

cmake_minimum_required(VERSION 3.20)

project(multiplication LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_executable(multiplication main.c)
# target_link_libraries(multiplication ${CMAKE_SOURCE_DIR}/build/libCMatrix.dll)
target_link_libraries(multiplication PRIVATE CMatrix)

add_custom_command(TARGET multiplication POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:CMatrix>
        $<TARGET_FILE_DIR:multiplication>
)
