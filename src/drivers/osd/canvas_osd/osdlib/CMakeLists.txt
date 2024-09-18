remove_definitions(
	-DCONFIG_ARCH_BOARD_HOLYBRO_KAKUTEH7 -D_SYS_CDEFS_H_ -D_SYS_REENT_H_
	-D__CUSTOM_FILE_IO__ -D__PX4_NUTTX -D__STDC_FORMAT_MACROS
)
set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "")
set_directory_properties(PROPERTIES COMPILE_FLAGS "")
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")

add_library(
	osdlib STATIC
	osd/elements/base.cpp
	osd/elements/objects.cpp
	osd/layout.cpp
	osd/utils.cpp
	lib.cpp
	msp.cpp
	utils.cpp
)
