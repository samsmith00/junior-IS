# Mandatory line, sets the minimum version of CMake that should be used with this repository.
# 3.22 is the minimum CMake version required by JUCE.
# 3.25 is required for the MSVC_DEBUG_INFORMATION_FORMAT feature to work.
# To verify your version run
# $ cmake --version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Enable setting CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
set(CMAKE_POLICY_DEFAULT_CMP0141        NEW      CACHE STRING "" FORCE)

# Sets the minimum deployment target
# macOS 10.11 is the minimum required by JUCE
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "" FORCE)
# Builds a binary that works on Apple Silicon (arm64) and Intel-based Macs
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)

# Sets a few variables, like PROJECT_NAME.
project(JucePluginDevelopmentCourseTremoloPlugin)

# Set the C++ standard only if this project is standalone;
# otherwise, let clients decide.
if(PROJECT_IS_TOP_LEVEL)
  if (NOT CMAKE_CXX_STANDARD)
    # Always use the newest C++ standard on green-field projects if possible.
    set(CMAKE_CXX_STANDARD 23)
  endif()

  # Links to C++ runtime statically
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  # Embeds debug information
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>" CACHE STRING "" FORCE)
endif()

# I like to download the dependencies to the same folder as the project.
# If you want to install them system-wide, set CPM_SOURCE_CACHE with a path to the dependencies
# either as an environment variable or pass it with -DCPM_SOURCE_CACHE=<path> when invoking cmake.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
cpmaddpackage(
  NAME
    JUCE
  GIT_TAG
    8.0.12
  GITHUB_REPOSITORY
    juce-framework/JUCE
  SOURCE_DIR
    ${LIB_DIR}/juce
)

# Adds compiler warning utilities & other.
include(cmake/CompilerWarnings.cmake)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Sets the Windows VST3 installation location to one
  # that doesn't require elevated permissions
  set_directory_properties(PROPERTIES JUCE_VST3_COPY_DIR "$ENV{LOCALAPPDATA}/Programs/Common/VST3")
endif()

# Adds a plugin target.
set(TREMOLO_PLUGIN_NAME "Tremolo")
juce_add_plugin(
  TremoloCoursePlugin
  PRODUCT_NAME
    ${TREMOLO_PLUGIN_NAME}
  VERSION
    0.0.0 # 0 forces AU hosts to always refresh
  COMPANY_NAME
    "WolfSound"
  PLUGIN_MANUFACTURER_CODE
    Wfsd
  PLUGIN_CODE
    Trcp
  FORMATS
    Standalone
    AU
    VST3
  IS_SYNTH
    FALSE
  NEEDS_MIDI_INPUT
    FALSE
  NEEDS_MIDI_OUTPUT
    FALSE
  VST3_AUTO_MANIFEST
    FALSE
  COPY_PLUGIN_AFTER_BUILD
    FALSE
)

# Configures the JUCE sources included in the plugin target.
target_compile_definitions(TremoloCoursePlugin
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_add_binary_data(tremolo_plugin_assets
  HEADER_NAME
    "TremoloPluginAssets.h"
  NAMESPACE
    tremolo::assets
  SOURCES
    assets/Background.png
    assets/Logo.png
    assets/Inter-Bold.ttf
    assets/Inter-Medium.ttf
)

add_library(tremolo::tremolo_plugin_assets ALIAS tremolo_plugin_assets)

juce_add_module(tremolo_plugin ALIAS_NAMESPACE tremolo)

# Additional definitions required by all users of this module.
target_compile_definitions(tremolo_plugin
  INTERFACE
    TREMOLO_PLUGIN_NAME="${TREMOLO_PLUGIN_NAME}"
)

# Enables strict C++ warnings.
target_compile_options(tremolo_plugin INTERFACE "${PROJECT_WARNINGS_CXX}")

target_link_libraries(tremolo_plugin INTERFACE tremolo::tremolo_plugin_assets)

target_link_libraries(
  TremoloCoursePlugin
  PRIVATE
    tremolo::tremolo_plugin
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Zip the .vst3 bundle with the README.txt file
get_target_property(VST3_BUNDLE_PATH TremoloCoursePlugin_VST3 JUCE_PLUGIN_ARTEFACT_FILE)
cmake_path(GET VST3_BUNDLE_PATH PARENT_PATH VST3_BUNDLE_OUTPUT_DIR)
add_custom_command(
  TARGET TremoloCoursePlugin_VST3
  POST_BUILD
  WORKING_DIRECTORY ${VST3_BUNDLE_OUTPUT_DIR}
  # For some reason, if the README.txt is in a different directory, zipping it fails, thus, we need to copy it first
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/README.txt" ${VST3_BUNDLE_OUTPUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar cvf "${VST3_BUNDLE_OUTPUT_DIR}/Tremolo.zip" --format=zip -- "${VST3_BUNDLE_PATH}" README.txt
)

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)

option(BUILD_EXAMPLES "When on, the supplied example apps will be added as targets and built" OFF)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

option(BUILD_TESTS "When on, will download the GoogleTest framework and add the automated tests project" OFF)
if(BUILD_TESTS)
  # This command allows running tests from the "build" folder (the one where CMake generates the project to).
  enable_testing()

  # Adds all the targets configured in the "test" folder.
  add_subdirectory(test)
endif()
