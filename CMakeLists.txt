cmake_minimum_required(VERSION 3.16)

project(juliabg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

list(APPEND CMAKE_CXX_FLAGS "-O3")
list(APPEND CMAKE_C_FLAGS   "-O3")

add_subdirectory(glfw EXCLUDE_FROM_ALL)

add_subdirectory(bin2c)

macro(target_embed_file target variable file)
    set(embed_file_name ${target}-${variable}-${file}.c)
    add_custom_command(
        OUTPUT ${embed_file_name}
        COMMAND bin2c ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${embed_file_name} ${variable}
        DEPENDS ${file}
        DEPENDS bin2c
    )
    target_sources(${target} PRIVATE ${embed_file_name})
endmacro()

include_directories(glad/include)
add_subdirectory(glad)

add_executable(juliabg main.cpp)
target_link_libraries(juliabg glfw glad)
target_embed_file(juliabg vert_shader embed.vert)
target_embed_file(juliabg frag_shader embed.frag)

install(TARGETS juliabg DESTINATION bin)