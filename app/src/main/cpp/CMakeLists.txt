
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("qvrholder")


include_directories(
        includes
        qvr/inc
)

add_executable(
        qvrholder

        qvrholder.cpp
)
