<div align="center">

# Tremolo Audio Plugin

[![License](https://img.shields.io/badge/license-Unlicense-blue.svg)](LICENSE.md)
![Build Status](https://github.com/juce-framework/tremolo-juce-course/actions/workflows/cmake.yml/badge.svg)
![CMake](https://img.shields.io/badge/CMake-3.25+-064F8C.svg?logo=cmake)
![C++](https://img.shields.io/badge/C++-23-00599C.svg?logo=cplusplus)
![JUCE](https://img.shields.io/badge/JUCE-8.0.10-orange.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows%20%7C%20Linux-lightgrey.svg)

<img alt="Tremolo user interface" src="docs/UI.png" width="512px">

Tremolo audio plugin project used for the [Official JUCE Audio Plugin Development Course](https://www.wolfsoundacademy.com/juce) 🎓

![JUCE audio plugin development course logo](docs/JUCE_Course_logo.svg)

</div>

## ✨ Features

* Preconfigured plugin formats:
  * AU
  * VST3
  * Standalone
* Tremolo audio effect: tremolo is amplitude modulation at a rate below the human hearing range. The result is a "pulsing" sound.
* Parameters
  * modulation rate
  * LFO waveform
  * bypass
* Real-time LFO visualization
* Custom UI
* State serialization
* Template *README.txt* for distribution
* Configured GitHub Actions CI pipeline
* Unit tests using [googletest](https://github.com/google/googletest)
* CMake presets, including "release"
* Automatic dependency downloading using [CPM](https://github.com/cpm-cmake/CPM.cmake)
* Helper *.clang-format*, *.clang-tidy*, and *.pre-commit-config.yaml* configuration files

## 📋 Requirements

You need to have the following software installed your machine:

* Git version control system
* CMake 3.25 or higher (the one bundled with CLion 2025.1.1 or higher should work)
* C++ compiler and build system. Tested on:
  * macOS: Xcode 15.4 (Apple Clang 15.0.0.15000309), 16.4 (Apple Clang 17.0.0.17000013)
  * Windows: Visual Studio 2022 17.14.13 (MSVC 19.44.35215)
  * Ubuntu, Debian: gcc 12.2.0, 13.3.0 and make 4.3

## 🚀 Getting Started

**Detailed instructions on the setup process [are described in the course.](https://www.wolfsoundacademy.com/juce)**

1. Click “Use this template” on GitHub to create your own copy and begin your project.
2. Clone **your repository** locally.
3. Build the *complete/* plugin
   1. Open the *complete/* folder in CLion, or
   2. In the main repo directory execute

      ```bash
      cd complete
      cmake --preset default
      cmake --build --preset default
      ```

The first build will take the most time because the dependencies (CPM and JUCE) need to be downloaded.

Build artifacts will be located at *complete/cmake-build/TremoloCoursePlugin_artefacts/Debug*.

## 📂 Structure

This repository is a template. Click “Use this template” on GitHub to create your own copy and begin your project.

The plugin project is present in two folders:

* *complete/* contains the completed tremolo plugin project. It compiles and runs out of the box.
* *todo/* contains skeleton code of the same plugin. This is the code you will work on throughout the course to gradually bring it to the same state as the *complete/* folder. In its default state, it does not compile.

## 🤝 Contributing

* If you find a bug, please, open an issue and/or provide a pull request.
* Feature requests won't be merged because the goal of the repository is to be educational not feature-full.

## 📜 License

We use the incredibly liberal ["Unlicense" license](LICENSE.md). You can basically do whatever you want with the code. Remember that the commercial use of products built with JUCE requires a JUCE license. Refer to the JUCE license for details.
