cmake_minimum_required (VERSION 2.8.11)

include (CTest)
include ("${CMAKE_MODULE_PATH}/DownloadProject.cmake")

if (CMAKE_VERSION VERSION_LESS 3.2)
    set (UPDATE_DISCONNECTED_IF_AVAILABLE "")
else ()
    set (UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif ()

# download GoogleTest targets
download_project (
	PROJ            googletest
	GIT_REPOSITORY  https://github.com/google/googletest.git
	GIT_TAG         master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set (gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# setup unit test libraries
add_subdirectory (${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# Turn on all warnings when using gcc.
if (CMAKE_COMPILER_IS_GNUCXX)
   add_definitions ("--std=c++14 -g -W -Wall -pedantic")
endif ()

# main test suite
add_executable (micro_flyweight_test
	"micro_flyweight/basic.cpp"
	"micro_flyweight/string_view.cpp"
)
target_link_libraries (micro_flyweight_test
	micro_flyweight
	gtest
	gmock_main
)

add_test (
	NAME full_test_suite
	COMMAND micro_flyweight_test
)
add_custom_target (copy-test-files ALL
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
    DEPENDS ${MY_TARGET}
)