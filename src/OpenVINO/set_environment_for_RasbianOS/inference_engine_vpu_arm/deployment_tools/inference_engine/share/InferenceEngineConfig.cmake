# Copyright (C) 2018-2019 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0
#
#
# FindIE
# ------
#
#   You can specify the path to Inference Engine files in IE_ROOT_DIR
#
# This will define the following variables:
#
#   InferenceEngine_FOUND        - True if the system has the Inference Engine library
#   InferenceEngine_INCLUDE_DIRS - Inference Engine include directories
#   InferenceEngine_LIBRARIES    - Inference Engine libraries
#
# and the following imported targets:
#
#   IE::inference_engine    - The Inference Engine library
#

macro(ext_message TRACE_LEVEL)
    if (${TRACE_LEVEL} STREQUAL FATAL_ERROR)
        if(InferenceEngine_FIND_REQUIRED)
            message(FATAL_ERROR "${ARGN}")
        elseif(NOT InferenceEngine_FIND_QUIETLY)
            message(WARNING "${ARGN}")
        endif()
        return()
    elseif(NOT InferenceEngine_FIND_QUIETLY)
        message(${TRACE_LEVEL} "${ARGN}")
    endif ()
endmacro()

set(InferenceEngine_FOUND FALSE)

if(TARGET IE::inference_engine)
    set(InferenceEngine_FOUND TRUE)
    get_target_property(InferenceEngine_INCLUDE_DIRS IE::inference_engine INTERFACE_INCLUDE_DIRECTORIES)
    set(InferenceEngine_LIBRARIES IE::inference_engine)
else()
    if (WIN32)
        set(_ARCH intel64)
    else()
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
            set(_ARCH armv7l)
        elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
            set(_ARCH intel64)
        elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
            set(_ARCH ia32)
        endif()
    endif()

    set(THREADING "TBB")

    # check whether setvars.sh is sourced
    if(NOT IE_ROOT_DIR AND (DEFINED ENV{InferenceEngine_DIR} OR InferenceEngine_DIR OR DEFINED ENV{INTEL_OPENVINO_DIR}))
        if (EXISTS "${InferenceEngine_DIR}")
            # InferenceEngine_DIR manually set via command line params
            set(IE_ROOT_DIR "${InferenceEngine_DIR}/..")
        elseif (EXISTS "$ENV{InferenceEngine_DIR}")
            # InferenceEngine_DIR manually set via env
            set(IE_ROOT_DIR "$ENV{InferenceEngine_DIR}/..")
        elseif (EXISTS "$ENV{INTEL_OPENVINO_DIR}/inference_engine")
            # if we installed DL SDK
            set(IE_ROOT_DIR "$ENV{INTEL_OPENVINO_DIR}/inference_engine")
        elseif (EXISTS "$ENV{INTEL_OPENVINO_DIR}/deployment_tools/inference_engine")
            # CV SDK is installed
            set(IE_ROOT_DIR "$ENV{INTEL_OPENVINO_DIR}/deployment_tools/inference_engine")
        endif()
    endif()

    if(NOT IE_ROOT_DIR)  
        ext_message(FATAL_ERROR "inference_engine directory is not found")
    endif()


    if(IE_INCLUDE_DIR AND NOT "${IE_ROOT_DIR}/include" EQUAL "${IE_INCLUDE_DIR}")
        unset(IE_INCLUDE_DIR CACHE)
    endif()

    if(IE_SRC_DIR AND NOT "${IE_ROOT_DIR}/src" EQUAL "${IE_SRC_DIR}")
        unset(IE_SRC_DIR CACHE)
    endif()

    if(IE_LIBRARY AND NOT "${IE_ROOT_DIR}/lib/${_ARCH}" EQUAL "${IE_LIBRARY}")
        unset(IE_LIBRARY CACHE)
    endif()

    set(_IE_ROOT_INCLUDE_DIR "${IE_ROOT_DIR}/include")
    set(_IE_ROOT_SRC_DIR "${IE_ROOT_DIR}/src")
    set(_IE_ROOT_LIBRARY "${IE_ROOT_DIR}/lib/${_ARCH}")

    find_path(IE_INCLUDE_DIR inference_engine.hpp "${_IE_ROOT_INCLUDE_DIR}")
    find_path(IE_SRC_DIR extension "${_IE_ROOT_SRC_DIR}")

    set(IE_LIB_DIR "${_IE_ROOT_LIBRARY}")
    set(IE_LIB_REL_DIR "${IE_LIB_DIR}/Release")
    set(IE_LIB_DBG_DIR "${IE_LIB_DIR}/Debug")
    set(IE_EXTERNAL_DIR "${IE_ROOT_DIR}/external")

    include(FindPackageHandleStandardArgs)

    if (WIN32)
        find_library(IE_RELEASE_LIBRARY inference_engine "${IE_LIB_REL_DIR}")
        find_library(IE_DEBUG_LIBRARY inference_engined "${IE_LIB_DBG_DIR}")
        find_package_handle_standard_args(  InferenceEngine
                                            FOUND_VAR INFERENCEENGINE_FOUND
                                            REQUIRED_VARS IE_RELEASE_LIBRARY IE_DEBUG_LIBRARY IE_INCLUDE_DIR
                                            FAIL_MESSAGE "Inference Engine cannot be found at ${_IE_ROOT_LIBRARY}. Please consult InferenceEgnineConfig.cmake module's help page.")
    else()
        find_library(IE_LIBRARY inference_engine "${IE_LIB_DIR}")
        find_package_handle_standard_args(  InferenceEngine
                                            FOUND_VAR INFERENCEENGINE_FOUND
                                            REQUIRED_VARS IE_LIBRARY IE_INCLUDE_DIR
                                            FAIL_MESSAGE "Inference Engine cannot be found at ${_IE_ROOT_LIBRARY}. Please consult InferenceEgnineConfig.cmake module's help page.")
    endif()
    if(INFERENCEENGINE_FOUND)
        # to keep this line for successful execution in CMake 2.8
        set(InferenceEngine_FOUND TRUE)

        add_library(IE::inference_engine SHARED IMPORTED GLOBAL)

        if (WIN32)
            set_property(TARGET IE::inference_engine APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
            set_property(TARGET IE::inference_engine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

            set_target_properties(IE::inference_engine PROPERTIES
                    IMPORTED_IMPLIB_RELEASE    "${IE_RELEASE_LIBRARY}"
                    IMPORTED_IMPLIB_DEBUG      "${IE_DEBUG_LIBRARY}"
                    MAP_IMPORTED_CONFIG_DEBUG Debug
                    MAP_IMPORTED_CONFIG_RELEASE Release
                    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
                    INTERFACE_INCLUDE_DIRECTORIES "${IE_INCLUDE_DIR}")
        else()
            set_target_properties(IE::inference_engine PROPERTIES
                    IMPORTED_LOCATION "${IE_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${IE_INCLUDE_DIR}")
            target_link_libraries(IE::inference_engine INTERFACE ${CMAKE_DL_LIBS})
        endif()

        set(InferenceEngine_INCLUDE_DIRS ${IE_INCLUDE_DIR})
        set(InferenceEngine_LIBRARIES IE::inference_engine)

        include("${IE_ROOT_DIR}/share/ie_parallel.cmake")

        add_subdirectory(${IE_SRC_DIR}/extension EXCLUDE_FROM_ALL ie_cpu_extension)
        add_library(IE::ie_cpu_extension ALIAS ie_cpu_extension)
    endif()
endif()
