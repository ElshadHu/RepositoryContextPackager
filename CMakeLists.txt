cmake_minimum_required(VERSION 3.20)
project(repoctx VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG v3.4.0 
)
FetchContent_MakeAvailable(tomlplusplus)


FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)



# if does not open try   link_directories("<Path-to-vcpkg>/installed/x64-windows/lib")
add_library(repoctx_lib STATIC
   src/cli.cpp 
   src/fs_travel.cpp 
   src/git_info.cpp 
   src/renderer.cpp 
   src/utils.cpp 
   src/filter.cpp 
   src/config_manager.cpp 
)

target_include_directories(repoctx_lib PUBLIC src include)
target_link_libraries(repoctx_lib PUBLIC tomlplusplus::tomlplusplus)


find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBGIT2 REQUIRED libgit2)
    if(LIBGIT2_FOUND)
       
        target_include_directories(repoctx_lib PUBLIC ${LIBGIT2_INCLUDE_DIRS})
        target_link_directories(repoctx_lib PUBLIC ${LIBGIT2_LIBRARY_DIRS})
        target_link_libraries(repoctx_lib PUBLIC ${LIBGIT2_LIBRARIES})
        target_compile_options(repoctx_lib PUBLIC ${LIBGIT2_CFLAGS_OTHER})
        message(STATUS "Using system libgit2")
    else()
        message(FATAL_ERROR "libgit2 not found with pkg-config")
    endif()
else()
    # Go back to vcpkg libgit2 
    find_package(libgit2 CONFIG REQUIRED)
    target_link_libraries(repoctx_lib PUBLIC libgit2::libgit2package)
    message(STATUS "Using vcpkg libgit2")
endif()

if(WIN32)
    target_compile_definitions(repoctx_lib PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(repoctx src/main.cpp)
target_link_libraries(repoctx PRIVATE repoctx_lib)

add_custom_command(TARGET repoctx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.toml
        $<TARGET_FILE_DIR:repoctx>/config.toml
)

add_subdirectory(tests)

# install(TARGETS repoctx RUNTIME DESTINATION bin)
install(TARGETS repoctx DESTINATION bin)