cmake_minimum_required(VERSION 3.21)

project(
	model_loading
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Example on how load a 3d model. Using MD2 for simplicity and nostalgia"
	HOMEPAGE_URL "https://github.com/victorfeitosa/ikuzo-examples"
)

include_directories("../../include")
file(GLOB _sources *.c)
psn00bsdk_add_executable(model_loading GPREL ${_sources})
