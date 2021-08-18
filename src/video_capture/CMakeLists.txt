CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(video_capture)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

find_package(ZED 2 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)
find_package(Boost COMPONENTS
    filesystem)

include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)

SET(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ZED_LIBS} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})