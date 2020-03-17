include(FetchContent)
include(CTest)
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.9.1
)
FetchContent_GetProperties(catch)
if(NOT catch_POPULATED)
  FetchContent_Populate(catch)
  add_subdirectory(${catch_SOURCE_DIR} ${catch_BINARY_DIR})
endif()

add_executable(test test.cpp)
target_link_libraries(test PRIVATE dpm_list::dpm_list Catch2::Catch2)

add_test(test COMMAND test)