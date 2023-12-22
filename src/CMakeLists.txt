
file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE data resources/*)
add_executable(sfmlgame ${sources})
target_compile_features(sfmlgame PUBLIC cxx_std_23) # or PUBLIC if you use C++14 in headers.
file(COPY ${data} DESTINATION resources)
target_link_libraries(sfmlgame PRIVATE sfml-graphics sfml-system sfml-window ImGui-SFML::ImGui-SFML)
# Copy DLLs needed for runtime on Windows
if(WIN32)
  if (BUILD_SHARED_LIBS)
    add_custom_command(TARGET sfmlgame POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
          $<TARGET_FILE:sfml-graphics>
          $<TARGET_FILE:sfml-window>
          $<TARGET_FILE:sfml-system>
          $<TARGET_FILE_DIR:sfmlgame>)
  endif()
endif()