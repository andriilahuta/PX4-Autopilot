set_directory_properties(PROPERTIES COMPILE_OPTIONS "")

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
