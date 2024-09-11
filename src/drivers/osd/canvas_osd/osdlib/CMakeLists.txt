############################################################################
#
#   Copyright (c) 2022 PX4 Development Team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name PX4 nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################



# execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.a OUTPUT_VARIABLE LIBSTDCCXX_LOCATION)
# string(STRIP ${LIBSTDCCXX_LOCATION} LIBSTDCCXX_LOCATION)
# message(FATAL_ERROR "libstdc++.a: ${LIBSTDCCXX_LOCATION}")


# find_library(STDCPP_LIBRARY
#   NAMES stdc++ libstdc++.a
# #   PATHS /usr/lib /usr/local/lib
# #   PATH_SUFFIXES gcc/x86_64-redhat-linux/10/32
# #   NO_DEFAULT_PATH
# )

# if(STDCPP_LIBRARY)
#   message(STATUS "Found libstdc++.a: ${STDCPP_LIBRARY}")
# else()
#   message(FATAL_ERROR "libstdc++.a not found")
# endif()





# set(CMAKE_CXX_STANDARD 17)
remove_definitions(-DCONFIG_ARCH_BOARD_HOLYBRO_KAKUTEH7 -D_SYS_CDEFS_H_ -D_SYS_REENT_H_ -D__CUSTOM_FILE_IO__ -D__PX4_NUTTX -D__STDC_FORMAT_MACROS)
set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "")
set_directory_properties(PROPERTIES COMPILE_FLAGS "")
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")

# set_directory_properties(PROPERTIES LINK_OPTIONS "-static -static-libgcc -static-libstdc++")
# set(CMAKE_STATIC_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

# px4_add_library(osdlib
add_library(osdlib STATIC
	osd/elements/base.cpp
	osd/elements/objects.cpp
	osd/layout.cpp
	osd/utils.cpp
	lib.cpp
	msp.cpp
	utils.cpp
)

add_library(libstdc++ STATIC IMPORTED)
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.a OUTPUT_VARIABLE LIBSTDCCXX_LOCATION)
# set(cpu_flags "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")
# separate_arguments(cpu_flags_list UNIX_COMMAND ${cpu_flags})
# execute_process(COMMAND /usr/lib/ccache/arm-none-eabi-g++ ${cpu_flags_list} -print-file-name=libstdc++.a OUTPUT_VARIABLE LIBSTDCCXX_LOCATION COMMAND_ECHO STDOUT)
string(STRIP ${LIBSTDCCXX_LOCATION} LIBSTDCCXX_LOCATION)
# message(OSD_COMPILE_FLAGS_1="${cpu_flags_list}")
# message(OSD_COMPILE_FLAGS_2="${LIBSTDCCXX_LOCATION}")
set_target_properties(libstdc++ PROPERTIES IMPORTED_LOCATION ${LIBSTDCCXX_LOCATION})

add_library(stdosdlib STATIC
	$<TARGET_OBJECTS:osdlib>
	$<TARGET_OBJECTS:libstdc++>
)

# set_target_properties(osdlib PROPERTIES LINK_SEARCH_START_STATIC ON)
# set_target_properties(osdlib PROPERTIES LINK_SEARCH_END_STATIC ON)
# target_link_options(osdlib PRIVATE -static-libgcc -static-libstdc++ -static)

# target_link_options(osdlib PRIVATE -lstdc++)
# target_link_options(osdlib PRIVATE -static-libgcc -static-libstdc++)
# set(CMAKE_EXE_LINKER_FLAGS " -static")
# target_link_libraries(osdlib -static-libgcc -static-libstdc++)

# target_compile_options(osdlib PUBLIC -std=c++17)
# target_include_directories(osdlib PRIVATE "/opt/gcc/arm-none-eabi/include/c++/9.3.1" /tmp)

# get_target_property(OSD_COMPILE_FLAGS osdlib COMPILE_OPTIONS)
# get_directory_property(OSD_COMPILE_DEFINITIONS osdlib DEFINITIONS)
# get_target_property(OSD_INCLUDE_DIRECTORIES osdlib INCLUDE_DIRECTORIES)

# list(JOIN OSD_COMPILE_DEFINITIONS ", " OSD_INCLUDE_DIRECTORIES_1)
# message(OSD_INCLUDE_DIRECTORIES_1="${OSD_INCLUDE_DIRECTORIES_1}")

# list(REMOVE_ITEM OSD_COMPILE_FLAGS "$<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>" "-include" "visibility.h")
# list(APPEND OSD_COMPILE_FLAGS "$<$<COMPILE_LANGUAGE:CXX>:-D_STDLIB_H_>")
# set(OSD_COMPILE_FLAGS "")
# set(OSD_INCLUDE_DIRECTORIES "")
# set(OSD_INCLUDE_DIRECTORIES
# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/nuttx/include
# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/nuttx/include/cxx
# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/include/cxx

# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/nuttx/arch/${CONFIG_ARCH}/src/${CONFIG_ARCH_FAMILY}
# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/nuttx/arch/${CONFIG_ARCH}/src/chip
# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/nuttx/arch/${CONFIG_ARCH}/src/common

# 	${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/apps/include
# )

# list(JOIN OSD_COMPILE_FLAGS ", " OSD_INCLUDE_DIRECTORIES_2)
# message(OSD_INCLUDE_DIRECTORIES_2="${OSD_INCLUDE_DIRECTORIES_2}")

# set_target_properties(osdlib PROPERTIES COMPILE_OPTIONS "${OSD_COMPILE_FLAGS}")
# set_target_properties(osdlib PROPERTIES INCLUDE_DIRECTORIES "${OSD_INCLUDE_DIRECTORIES}")

# set_target_properties(osdlib PROPERTIES COMPILE_DEFINITIONS "")
# set_target_properties(osdlib PROPERTIES COMPILE_FEATURES "")
# set_target_properties(osdlib PROPERTIES COMPILE_FLAGS "")
# set_target_properties(osdlib PROPERTIES COMPILE_OPTIONS "")
# set_target_properties(osdlib PROPERTIES INCLUDE_DIRECTORIES "")

# set_target_properties(osdlib PROPERTIES INTERFACE_COMPILE_DEFINITIONS "")
# set_target_properties(osdlib PROPERTIES INTERFACE_COMPILE_FEATURES "")
# set_target_properties(osdlib PROPERTIES INTERFACE_COMPILE_OPTIONS "")

# set_target_properties(osdlib PROPERTIES STATIC_LIBRARY_FLAGS "")
# set_target_properties(osdlib PROPERTIES STATIC_LIBRARY_OPTIONS "")

# message(MSG1_________________="${CMAKE_CXX_FLAGS}")
# message(MSG2_________________="${CMAKE_CXX_STANDARD}")
# remove_definitions(-DCONFIG_ARCH_BOARD_HOLYBRO_KAKUTEH7 -D_SYS_CDEFS_H_ -D_SYS_REENT_H_ -D__CUSTOM_FILE_IO__ -D__PX4_NUTTX -D__STDC_FORMAT_MACROS)
