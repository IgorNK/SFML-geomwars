
file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE data resources/*)
add_executable(sfmlgame ${sources})
file(COPY ${data} DESTINATION resources)
target_link_libraries(sfmlgame PRIVATE sfml-graphics sfml-system sfml-window ImGui-SFML::ImGui-SFML)
