cmake_minimum_required(VERSION 3.23)

project(
	load_from_cd
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Example on how to load files from the CD or from your computer through PCDRV"
	HOMEPAGE_URL "https://github.com/victorfeitosa/ikuzo-examples"
)

include_directories("../../include")
psn00bsdk_add_executable(load_from_cd GPREL main.c)

if(PLATFORM_EMULATOR)
	add_compile_definitions(USE_PCDRV)	
endif(PLATFORM_EMULATOR)

