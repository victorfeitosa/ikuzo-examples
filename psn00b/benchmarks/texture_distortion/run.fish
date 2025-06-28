#!/usr/local/bin/fish
echo Building $argv[1]
cmake --preset $argv[1] && \
cmake --build ./$argv[1] && \
open /Applications/PCSX-Redux.app --args -exe /Users/victorfeitosa/Workspace/Dev/Projects/ikuzo-examples/psn00b/benchmarks/texture_distortion/$argv[1]/texture_distortion.exe -run
