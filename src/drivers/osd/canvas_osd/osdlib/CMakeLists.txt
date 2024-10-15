message("1---------------------${PX4_SOURCE_DIR}/src/include")

get_directory_property(DIR_COMPILE_OPTIONS COMPILE_OPTIONS)
# message("1---------------------${DIR_COMPILE_OPTIONS}")
# list(REMOVE_ITEM DIR_COMPILE_OPTIONS "-include visibility.h")
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>")
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>")
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "-Werror")
message("2---------------------${DIR_COMPILE_OPTIONS}")

# get_directory_property(DIR_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
# list(REMOVE_ITEM DIR_INCLUDE_DIRECTORIES "${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/include/cxx")
# message("3---------------------${DIR_INCLUDE_DIRECTORIES}")



set_directory_properties(
	PROPERTIES
	# INCLUDE_DIRECTORIES ""
	# COMPILE_OPTIONS ""
	# INCLUDE_DIRECTORIES "${DIR_INCLUDE_DIRECTORIES}"
	COMPILE_OPTIONS "${DIR_COMPILE_OPTIONS}"
)
remove_definitions(
	# -D_SYS_CDEFS_H_
	# -D_SYS_REENT_H_
	# -D__CUSTOM_FILE_IO__
)
add_definitions(-DPX4_NUTTX_NEW)



# list(JOIN OSD_COMPILE_FLAGS ", " OSD_COMPILE_FLAGS_1)
# message(OSD_COMPILE_FLAGS_1="${OSD_COMPILE_FLAGS_1}")

# list(REMOVE_ITEM OSD_COMPILE_FLAGS "$<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>")

# list(JOIN OSD_COMPILE_FLAGS ", " OSD_COMPILE_FLAGS_2)
# message(OSD_COMPILE_FLAGS_2="${OSD_COMPILE_FLAGS_2}")

# set_target_properties(drivers__osd__canvas_osd PROPERTIES COMPILE_OPTIONS "${OSD_COMPILE_FLAGS}")

px4_add_library(osdlib
# add_library(
# 	osdlib STATIC
	osd/elements/base.cpp
	osd/elements/objects.cpp
	osd/layout.cpp
	osd/utils.cpp
	lib.cpp
	msp.cpp
	utils.cpp
)
