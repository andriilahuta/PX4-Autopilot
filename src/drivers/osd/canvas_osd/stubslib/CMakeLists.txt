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

if(${PX4_PLATFORM} STREQUAL "nuttx")
	px4_add_library(
		stubslib
		lib_libfflush.c
		lib_libfread_unlocked.c
		lib_rdflush_unlocked.c
		lib_wrflush_unlocked.c
		lib_libfilelock.c
		lib_maxlocale.c
		lib_libfwrite.c
		lib_ctype.c
		lib_fgetc.c
		lib_getc.c
		lib_fgetwc.c
		lib_getwc.c
		lib_ungetwc.c
		lib_fputwc.c
		lib_fputc.c
		lib_putc.c
		lib_putwc.c
	)
else()
	add_library(stubslib INTERFACE)
endif()
