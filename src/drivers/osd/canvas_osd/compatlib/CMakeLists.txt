get_directory_property(DIR_COMPILE_OPTIONS COMPILE_OPTIONS)
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "$<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>")
list(REMOVE_ITEM DIR_COMPILE_OPTIONS "-Werror")

set_directory_properties(
	PROPERTIES
	# INCLUDE_DIRECTORIES ""
	# COMPILE_OPTIONS ""
	# INCLUDE_DIRECTORIES "${PX4_SOURCE_DIR}/src/include"
	COMPILE_OPTIONS "${DIR_COMPILE_OPTIONS}"
)
remove_definitions(
	# -D_SYS_CDEFS_H_
	# -D_SYS_REENT_H_
	# -D__CUSTOM_FILE_IO__
)


px4_add_library(compatlib
# add_library(
# 	compatlib STATIC
	fmt/format.cc
)
