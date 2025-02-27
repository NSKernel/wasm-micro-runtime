# Copyright (C) 2019 Intel Corporation. All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required (VERSION 2.8...3.16)

project(bwa_wasm)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/preparation.cmake)

#######################################
include(ExternalProject)

################ HEADERS ################
set(EMSDK_SYSTEM_HEADERS "$ENV{EMSDK}/upstream/emscripten/system/include")
set(EMSDK_SSE_HEADERS "${EMSDK_SYSTEM_HEADERS}/SSE")
set(EMSDK_LIBC_HEADERS "${EMSDK_SYSTEM_HEADERS}/libc")
ExternalProject_Add(headers_from_emcc
   PREFIX headers
   SOURCE_DIR ${EMSDK_SYSTEM_HEADERS}
   CONFIGURE_COMMAND ""
   BUILD_COMMAND     ""
   INSTALL_COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/include/SSE
                     && ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/include/pthread/sys
                     && ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/include/emscripten
                     # copy emscripten SSE header files
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_SYSTEM_HEADERS}/wasm_simd128.h ${CMAKE_CURRENT_SOURCE_DIR}/include/SSE/
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_SSE_HEADERS}/immintrin.h ${CMAKE_CURRENT_SOURCE_DIR}/include/SSE/
                     # SSE
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_SSE_HEADERS}/xmmintrin.h ${CMAKE_CURRENT_SOURCE_DIR}/include/SSE/
                     # SSE2
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_SSE_HEADERS}/emmintrin.h ${CMAKE_CURRENT_SOURCE_DIR}/include/SSE/
                     # SSE4.1
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_SSE_HEADERS}/smmintrin.h ${CMAKE_CURRENT_SOURCE_DIR}/include/SSE/
                     # a fake empty header to aovid further depenency
                     && ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/include/emscripten/emscripten.h
                     # copy emscripten pthread related header files
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_LIBC_HEADERS}/pthread.h ${CMAKE_CURRENT_SOURCE_DIR}/include/pthread/
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_LIBC_HEADERS}/signal.h ${CMAKE_CURRENT_SOURCE_DIR}/include/pthread/
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_LIBC_HEADERS}/netdb.h ${CMAKE_CURRENT_SOURCE_DIR}/include/pthread/
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_LIBC_HEADERS}/sys/wait.h ${CMAKE_CURRENT_SOURCE_DIR}/include/pthread/sys/
                     && ${CMAKE_COMMAND} -E copy ${EMSDK_LIBC_HEADERS}/sys/socket.h ${CMAKE_CURRENT_SOURCE_DIR}/include/pthread/sys/
)

################ libz ################
ExternalProject_Add(libz_src
  PREFIX libz
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        master
  GIT_PROGRESS   ON
  GIT_SHALLOW    ON
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/libz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND  ""
  INSTALL_COMMAND ""
)

################ bwa ################
ExternalProject_Add(bwa
  PREFIX bwa
  GIT_REPOSITORY https://github.com/lh3/bwa.git
  GIT_TAG        master
  GIT_PROGRESS   ON
  GIT_SHALLOW    ON
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/bwa
  DEPENDS        libz_src headers_from_emcc
  UPDATE_COMMAND git clean -fd && git checkout -- *
                  && ${CMAKE_COMMAND} -E echo "Copying pre-installed CMakeLists.txt"
                  && ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.bwa_wasm.txt CMakeLists.txt
                  && git apply ../bwa.patch
  CONFIGURE_COMMAND  ${CMAKE_COMMAND}
                         -DWASI_SDK_PREFIX=${WASI_SDK_HOME}/wasi-sdk
                         -DCMAKE_TOOLCHAIN_FILE=${WASI_SDK_HOME}/wasi-sdk/share/cmake/wasi-sdk.cmake
                         ${CMAKE_CURRENT_SOURCE_DIR}/bwa
  BUILD_COMMAND  make bwa_wasm_opt
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ./bwa.opt.wasm ${CMAKE_CURRENT_SOURCE_DIR}/build/bwa.wasm
)
