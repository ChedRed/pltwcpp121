# Basic project requirements
cmake_minimum_required(VERSION 3.29.6)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_SYSROOT macosx)
endif()
project(pltwcpp)


# Set CMake variables
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Main executable
add_executable(pltwcpp WIN32 main.cpp)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # set_target_properties(pltwcpp PROPERTIES WIN32_EXECUTABLE TRUE)
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Set macos executable rpath and result path
    set_target_properties(pltwcpp PROPERTIES INSTALL_RPATH "@executable_path/../Resources" BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(pltwcpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/Contents/MacOS)


    # Link libraries and include directories
    target_link_libraries(pltwcpp PRIVATE ${CMAKE_SOURCE_DIR}/../Cached/libSDL3.0.dylib)
    target_link_libraries(pltwcpp PRIVATE ${CMAKE_SOURCE_DIR}/../Cached/libSDL3_ttf.0.0.0.dylib)
    # target_link_libraries(pltwcpp PRIVATE ${CMAKE_SOURCE_DIR}/../Cached/libgtk-4.1.dylib)
    target_include_directories(pltwcpp PRIVATE /usr/local/include)


    # Make directories for macos app
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/Contents/MacOS)
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/Contents/Resources)


    # Add basic files to macos app
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/info.plist DESTINATION ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/Contents)
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/icon.icns DESTINATION ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/Contents/Resources)


    # Add resources to macos app
    file(COPY ${CMAKE_SOURCE_DIR}/../Cached/libSDL3.0.dylib DESTINATION ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/contents/Resources)
    configure_file(${CMAKE_SOURCE_DIR}/../Cached/libSDL3_ttf.0.0.0.dylib ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/contents/Resources/libSDL3_ttf.0.dylib COPYONLY)
    # configure_file(${CMAKE_SOURCE_DIR}/../Cached/libgtk-4.1.dylib ${CMAKE_SOURCE_DIR}/nogit/PLTWcpp.app/contents/Resources/libgtk-4.1.dylib COPYONLY)


else(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Set windows result path
    set_target_properties(pltwcpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/nogit/PLTWcpp")


    # Link libraries and include directories
    target_link_libraries(pltwcpp PRIVATE C:/usr/local/lib/SDL3.lib)
    target_link_libraries(pltwcpp PRIVATE C:/usr/local/lib/SDL3_ttf.lib)
    target_include_directories(pltwcpp PRIVATE C:/usr/local/include) # "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/cppwinrt"


    # Add basic files to windows app


    # Add resources to windows app
    file(COPY C:/usr/local/lib/SDL3.dll DESTINATION "${CMAKE_SOURCE_DIR}/nogit/PLTWcpp")
    file(COPY C:/usr/local/lib/SDL3_ttf.dll DESTINATION "${CMAKE_SOURCE_DIR}/nogit/PLTWcpp")
endif()
