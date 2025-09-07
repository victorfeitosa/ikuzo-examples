cmake_minimum_required(VERSION 3.23)

project(
	cd_load
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Example on how to load files from the CD or from your computer through PCDRV"
	HOMEPAGE_URL "https://github.com/victorfeitosa/ikuzo-examples"
)

set(CMAKE_C_STANDARD 17)

include_directories("../../include")
psn00bsdk_add_executable(cd_load GPREL main.c)
psn00bsdk_add_cd_image(
	cd_load_iso cd_load iso.xml
	DEPENDS cd_load system.cnf
)

if(PLATFORM STREQUAL EMULATOR)
	add_compile_definitions(USE_PCDRV)	
endif(PLATFORM STREQUAL EMULATOR)

install(
	FILES
		${PROJECT_BINARY_DIR}/cd_load.bin
		${PROJECT_BINARY_DIR}/cd_load.cue
	TYPE BIN
)

