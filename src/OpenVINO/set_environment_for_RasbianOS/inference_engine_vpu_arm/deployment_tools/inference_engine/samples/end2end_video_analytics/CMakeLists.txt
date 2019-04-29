# Copyright (C) 2018-2019 Intel Corporation
# SPDX-License-Identifier: Apache-2.0
#

project (end2end_video_analytics)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/common)

# collect all samples subdirectories
file(GLOB samples_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
# skip building of unnecessary subdirectoriefs
list(REMOVE_ITEM samples_dirs common test_content)
add_samples_to_build(${samples_dirs})