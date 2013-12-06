project(TrafficSignRecognition)
cmake_minimum_required(VERSION 2.8)

# CMake Modiles
#SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
else(OpenCV_FOUND)
    message(FATEL_ERROR "OpenCV library not found!")
endif(OpenCV_FOUND)

aux_source_directory(src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
