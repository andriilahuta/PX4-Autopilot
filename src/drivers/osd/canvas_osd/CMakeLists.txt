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
# add_compile_options(-std=c++17)
# include_directories(BEFORE SYSTEM "/opt/gcc/arm-none-eabi/include/c++/9.3.1")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(osdlib)

add_library(libstdc++ STATIC IMPORTED)
separate_arguments(cpu_flags_list UNIX_COMMAND ${cpu_flags})
execute_process(COMMAND ${CMAKE_CXX_COMPILER} ${cpu_flags_list} -print-file-name=libstdc++_nano.a OUTPUT_VARIABLE LIBSTDCCXX_LOCATION COMMAND_ECHO STDOUT)
string(STRIP ${LIBSTDCCXX_LOCATION} LIBSTDCCXX_LOCATION)
# message(OSD_COMPILE_FLAGS_1="${LIBSTDCCXX_LOCATION}")
# message(OSD_COMPILE_FLAGS_2="${CMAKE_CXX_COMPILER} ${cpu_flags_list}")
set_target_properties(libstdc++ PROPERTIES IMPORTED_LOCATION ${LIBSTDCCXX_LOCATION})

px4_add_module(
	MODULE drivers__osd__canvas_osd
	MAIN canvas_osd
	SRCS
		msp_osd.cpp
		MspV1.cpp
		uorb_to_msp.cpp
		# osd/layout.cpp osd/utils.cpp osd/elements/base.cpp osd/elements/objects.cpp
	# INCLUDES
		# /opt/gcc/arm-none-eabi/include/c++/9.3.1/
	# COMPILE_FLAGS
		# -std=c++17
		# -luClibc++
		# -static-libgcc
		# -static-libstdc++
	# LINK_FLAGS
	# 	-Wl,--gc-sections
	# 	-static -static-libgcc -static-libstdc++
	MODULE_CONFIG
		module.yaml
	DEPENDS
		px4_work_queue
		osdlib
		libstdc++
	)
# target_link_options(drivers__osd__canvas_osd PRIVATE -static-libgcc -static-libstdc++)
# get_target_property(OSD_COMPILE_FLAGS drivers__osd__canvas_osd COMPILE_OPTIONS)

# list(JOIN OSD_COMPILE_FLAGS ", " OSD_COMPILE_FLAGS_1)
# message(OSD_COMPILE_FLAGS_1="${OSD_COMPILE_FLAGS_1}")

# list(REMOVE_ITEM OSD_COMPILE_FLAGS "$<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>")

# list(JOIN OSD_COMPILE_FLAGS ", " OSD_COMPILE_FLAGS_2)
# message(OSD_COMPILE_FLAGS_2="${OSD_COMPILE_FLAGS_2}")

# set_target_properties(drivers__osd__canvas_osd PROPERTIES COMPILE_OPTIONS "${OSD_COMPILE_FLAGS}")
