cmake_minimum_required(VERSION 3.20)

project(row-echelon LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_executable(row-echelon main.c)
# target_link_libraries(row-echelon ${CMAKE_SOURCE_DIR}/build/libCMatrix.dll)
target_link_libraries(row-echelon PRIVATE CMatrix)

add_custom_command(TARGET row-echelon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:CMatrix>
        $<TARGET_FILE_DIR:row-echelon>
)
