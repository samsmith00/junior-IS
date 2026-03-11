function(course_add_example_app)
  get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  juce_add_gui_app(${target}
    PRODUCT_NAME "${target}Example"
    VERSION "1.0.0"
  )
  target_compile_definitions(${target}
      PRIVATE
          JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
          JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
          JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${target},JUCE_PRODUCT_NAME>"
          JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${target},JUCE_VERSION>")

  target_sources(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Main.cpp)

  target_link_libraries(${target} PRIVATE juce::juce_gui_basics juce::juce_audio_utils)

  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

if(WIN32)
  set(CMAKE_OBJECT_PATH_MAX 280)
endif()

add_subdirectory(Coordinates)
add_subdirectory(GuiThreadIdPrinting)
add_subdirectory(GuiAndAudioThreadIdPrinting)
add_subdirectory(Justifications)
add_subdirectory(LongRunningTask)
add_subdirectory(UserEventHandlers)
