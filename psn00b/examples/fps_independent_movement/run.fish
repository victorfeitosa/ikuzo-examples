#!/usr/local/bin/fish
echo Building $argv[1]
cmake --preset $argv[1] && \
cmake --build ./$argv[1] && \
open /Applications/PCSX-Redux.app --args -exe /Users/victorfeitosa/Workspace/Dev/Projects/ikuzo-examples/psn00b/examples/fps_independent_movement/$argv[1]/fps_independent_movement.exe -run
