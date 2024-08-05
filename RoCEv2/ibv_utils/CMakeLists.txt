cmake_minimum_required(VERSION 3.0.0)

project(ibv_utils)

set(LIBNAME ibv_utils)
add_compile_options(-Wno-address-of-packed-member)
aux_source_directory(src UTILS_SRC)
include_directories(include)
link_libraries(ibverbs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
add_library(${LIBNAME} SHARED ${UTILS_SRC})




