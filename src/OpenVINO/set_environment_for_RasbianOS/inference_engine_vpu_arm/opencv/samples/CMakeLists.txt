cmake_minimum_required(VERSION 3.5.1)

project(openvino_opencv_samples)
set (CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED PATHS "..")
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

file(GLOB_RECURSE files "*.cpp")
foreach(file ${files})
  get_filename_component(name "${file}" NAME_WE)
  add_executable(openvino_sample_${name} ${file})
  target_link_libraries(openvino_sample_${name} ${OpenCV_LIBS})
endforeach()
