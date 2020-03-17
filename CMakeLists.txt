cmake_minimum_required(VERSION 3.14)
project(dpm_list CXX)

option(DPM_LIST_INSTALL "Builds the install target" OFF)
option(DPM_LIST_TEST "Builds the tests" ON)

add_library(dpm_list INTERFACE)
add_library(dpm_list::dpm_list ALIAS dpm_list)
target_include_directories(dpm_list INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(dpm_list INTERFACE cxx_std_20)

if(DPM_LIST_TEST)
    add_subdirectory(test)
endif()
if(DPM_LIST_INSTALL)
    #install
endif()