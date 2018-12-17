cmake_minimum_required(VERSION 2.8)
project( Synthetic )

set (CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )
FIND_PACKAGE( Boost  REQUIRED COMPONENTS system filesystem )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable( extractColor extractColor.cpp )
target_link_libraries( extractColor ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable( checkColor checkColor.cpp )
target_link_libraries( checkColor ${OpenCV_LIBS})

add_executable( makeSynthethic makeSynthethic.cpp )
target_link_libraries( makeSynthethic ${OpenCV_LIBS} ${Boost_LIBRARIES})
