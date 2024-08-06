cmake_minimum_required(VERSION 3.0.0)

# create project
project(ibv_utils)

# set libname
set(LIBNAME ibv_utils)
# add compile options
add_compile_options(-Wno-address-of-packed-member)
# add source files
aux_source_directory(src UTILS_SRC)
# add include directories
include_directories(include)
# add library
link_libraries(ibverbs)
# set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# create shared library
add_library(${LIBNAME} SHARED ${UTILS_SRC})
# install library
install(TARGETS ${LIBNAME} DESTINATION /usr/local/lib)




