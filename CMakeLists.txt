cmake_minimum_required(VERSION 2.8)
project( main )

SET(CMAKE_C_COMPILER g++)
add_compile_options(-std=c++11)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )

link_directories(${CMAKE_CURRENT_SOURCE_DIR}) 

find_package( OpenCV REQUIRED )
add_executable( main main.cpp )
target_link_libraries( main ${OpenCV_LIBS} )

add_executable(demo Demo.cpp)
target_link_libraries( demo 
    libdvp.so 
    libhzd.so 
    libpthread.so 
    ${OpenCV_LIBS}
)