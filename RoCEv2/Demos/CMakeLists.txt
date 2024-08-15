cmake_minimum_required(VERSION 3.15)

# set project name
project(Demos)

find_package(CUDA REQUIRED)

# set the executable name
set(EXECUTABLE_RECV RecvDemo)
set(EXECUTABLE_SEND SendDemo)
set(EXECUTABLE_RNICINFO RNICInfo)

# set compiler
set(CMAKE_C_COMPILER "nvcc")
# set ibv_utils library path and pkt_gen library path
set(IBV_UTILS_PATH ../ibv_utils)
set(PKT_GEN_PATH pkt_gen)
set(UTILS_PATH utils)
# set source files
set(RECV_SRC RecvDemo.c)
set(SEND_SRC SendDemo.c)
set(RNICINFO_SRC RNICInfo.c)
aux_source_directory(${UTILS_PATH} UTILS_SRC)
aux_source_directory(${PKT_GEN_PATH} PKT_GEN_SRC)
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
cuda_add_executable(${EXECUTABLE_RECV} ${RECV_SRC} ${UTILS_SRC})
cuda_add_executable(${EXECUTABLE_SEND} ${SEND_SRC} ${UTILS_SRC} ${PKT_GEN_SRC})
cuda_add_executable(${EXECUTABLE_RNICINFO} ${RNICINFO_SRC})
# install executable
install(TARGETS ${EXECUTABLE_RECV} DESTINATION /usr/local/bin)
install(TARGETS ${EXECUTABLE_SEND} DESTINATION /usr/local/bin)
install(TARGETS ${EXECUTABLE_RNICINFO} DESTINATION /usr/local/bin)