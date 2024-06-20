cmake_minimum_required(VERSION 3.21)

project(
	texture_distortion
	LANGUAGES    C ASM
	VERSION      1.0.0
	DESCRIPTION  "Testing how much textures can distort when using different primitives and subdivision levels"
	HOMEPAGE_URL "https://github.com/victorfeitosa/ikuzo-examples"
)

include_directories("../../include")
psn00bsdk_add_executable(texture_distortion GPREL main.c)
psn00bsdk_target_incbin(texture_distortion PRIVATE checker_tex checker.tim)
psn00bsdk_target_incbin(texture_distortion PRIVATE fence_tex fence.tim)
psn00bsdk_target_incbin(texture_distortion PRIVATE wall_tex wall.tim)
