cmake_minimum_required(VERSION 3.21)

project(
	fps_indep_move
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Example on how to perform fps independent movement"
	HOMEPAGE_URL "https://github.com/victorfeitosa/ikuzo-examples"
)

include_directories("../../include")
psn00bsdk_add_executable(fps_independent_movement GPREL main.c)
