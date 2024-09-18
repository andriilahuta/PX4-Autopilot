set(CMAKE_CXX_STANDARD 17)

add_subdirectory(osdlib)
add_subdirectory(stubslib)

add_library(libstdc++ STATIC IMPORTED)
separate_arguments(cpu_flags_list UNIX_COMMAND ${cpu_flags})
execute_process(
	COMMAND ${CMAKE_CXX_COMPILER} ${cpu_flags_list} -print-file-name=libstdc++_nano.a
	OUTPUT_VARIABLE LIBSTDCCXX_LOCATION
)
string(STRIP ${LIBSTDCCXX_LOCATION} LIBSTDCCXX_LOCATION)
set_target_properties(libstdc++ PROPERTIES IMPORTED_LOCATION ${LIBSTDCCXX_LOCATION})

px4_add_module(
	MODULE drivers__osd__canvas_osd
	MAIN canvas_osd
	SRCS
		msp_osd.cpp
		MspV1.cpp
		uorb_to_msp.cpp
	MODULE_CONFIG
		module.yaml
	DEPENDS
		px4_work_queue
		osdlib
		libstdc++
		stubslib
)
