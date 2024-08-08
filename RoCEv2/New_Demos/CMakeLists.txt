cmake_minimum_required(VERSION 3.15)

# set project name
project(Demos)

find_package(CUDA REQUIRED)

# set the executable name
set(EXECUTABLE_NAME RecvDemo)

# set compiler
set(CMAKE_C_COMPILER "nvcc")
# set ibv_utils library path and pkt_gen library path
set(IBV_UTILS_PATH ../ibv_utils)
set(PKT_GEN_PATH ../pkt_gen)
set(UTILS_PATH utils)
# set source files
aux_source_directory(. SRC)
aux_source_directory(${UTILS_PATH} UTILS_SRC)
# include ibv_utils header files
include_directories(${IBV_UTILS_PATH}/include ${PKT_GEN_PATH})
include_directories(${UTILS_PATH})
# link ibv_utils library
#link_directories(${IBV_UTILS_PATH}/lib)
link_directories(/usr/local/lib)
link_libraries(ibv_utils)
link_libraries(cuda)
link_libraries(ibverbs)
# add compile options
add_compile_options(-arch=sm_86)

# add executable
cuda_add_executable(${EXECUTABLE_NAME} ${SRC} ${UTILS_SRC})

# install executable
install(TARGETS ${EXECUTABLE_NAME} DESTINATION /usr/local/bin)