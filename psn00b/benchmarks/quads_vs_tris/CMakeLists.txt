cmake_minimum_required(VERSION 3.21)

project(
	tris_vs_quads
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "Benchmark performance of rendering quads vs tris"
	HOMEPAGE_URL "https://github.com/victorfeitosa/ikuzo-examples"
)

psn00bsdk_add_executable(tris_vs_quads GPREL main.c)
