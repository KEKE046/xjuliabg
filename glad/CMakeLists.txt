add_library(glad src/glad.c)
target_link_libraries(glad ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})