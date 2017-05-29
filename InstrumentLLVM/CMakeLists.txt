
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include(CheckCXXCompilerFlag)
macro(add_cxx_flag flag name)
    CHECK_CXX_COMPILER_FLAG(${flag} "${name}_SUPPORTED")
    if("${name}_SUPPORTED")
        add_definitions(${flag})
    else()
        message(FATAL_ERROR "${flag} flag is not supported by ${CMAKE_CXX_COMPILER}")
    endif()
endmacro()

# FIXME: Setting flags this way isn't very portable
if (NOT LLVM_ENABLE_RTTI)
    message(STATUS "LLVM was built without RTTI, so we must disable it too for linking to work properly")
    add_cxx_flag(-fno-rtti RTTI) # Can't use LLVMSupport properly if we have rtti
endif()
add_cxx_flag(-std=c++11 CXX11)




include_directories(${LLVM_INCLUDE_DIRS})


add_subdirectory(FunctionInstr)
