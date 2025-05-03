set(CONAN_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/conan/build/${CMAKE_BUILD_TYPE}/generators/conan_toolchain.cmake")

if (EXISTS "${CONAN_TOOLCHAIN_FILE}")
    message(STATUS "Using existing Conan toolchain: ${CONAN_TOOLCHAIN_FILE}")
else ()
    message(STATUS "Running conan install...")

    execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR}
            -of=${CMAKE_BINARY_DIR}/conan
            -s build_type=${CMAKE_BUILD_TYPE}
            -s compiler=gcc
            -s compiler.version=15
            -s compiler.libcxx=libstdc++11
            -s compiler.cppstd=23
            --build=missing
            RESULT_VARIABLE _conan_result
    )

    if (NOT _conan_result EQUAL 0)
        message(FATAL_ERROR "Conan install failed")
    endif ()
endif ()

include("${CONAN_TOOLCHAIN_FILE}")
