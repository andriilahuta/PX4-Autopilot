get_directory_property(DIR_COMPILE_OPTIONS COMPILE_OPTIONS)
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>")
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "-Werror")

get_directory_property(DIR_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
list(REMOVE_ITEM DIR_INCLUDE_DIRECTORIES "${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/include/cxx")
list(REMOVE_ITEM DIR_INCLUDE_DIRECTORIES "${PX4_SOURCE_DIR}/platforms/nuttx/NuttX/nuttx/include/cxx")

set_directory_properties(
	PROPERTIES
	# INCLUDE_DIRECTORIES ""
	# COMPILE_OPTIONS ""
	INCLUDE_DIRECTORIES "${DIR_INCLUDE_DIRECTORIES}"
	COMPILE_OPTIONS "${DIR_COMPILE_OPTIONS}"
)
remove_definitions(
	# -D_SYS_CDEFS_H_
	# -D_SYS_REENT_H_
	# -D__CUSTOM_FILE_IO__
)
add_definitions(-D_STDLIB_H_)


px4_add_library(compatlib
# add_library(
# 	compatlib STATIC
	fmt/format.cc
)
