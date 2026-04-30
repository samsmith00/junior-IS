# Phase Vocoder Pitch Shifter

A real-time audio pitch shifting plugin implemented using a phase vocoder.  
This project demonstrates frequency-domain audio processing using the Short-Time Fourier Transform (STFT).

## Overview

This project implements a pitch shifting algorithm based on the phase vocoder technique.  
The input audio signal is transformed into the frequency domain, modified by scaling frequencies, and then reconstructed back into the time domain.

## How It Works

The processing pipeline consists of three main steps:

1. **Analysis**
   - The input signal is segmented into overlapping frames
   - A Hann window is applied to each frame
   - The Short-Time Fourier Transform (STFT) is used to obtain frequency information

2. **Modification**
   - Frequencies are scaled by a pitch shift factor
   - Phase is adjusted between frames to maintain continuity and reduce artifacts

3. **Synthesis**
   - The modified signal is converted back to the time domain using the inverse STFT
   - Overlap-add is used to reconstruct the final output signal

## Implementation

This project was developed in **C++** using the **JUCE** framework.  
JUCE provides the audio processing infrastructure and plugin architecture, enabling real-time DSP and integration with digital audio workstations (DAWs).




## Feature Calendar

| **Issue** | **Due date** | |
| --------- | ------------ | -- |
| [Implement a basic project structure using JUCE and cmake](https://github.com/samsmith00/junior-IS/issues/1) | 2/15/25 | |
| [Implement dummy plugin](https://github.com/samsmith00/junior-IS/issues/4) | 2/23/25 | |
| [Implement signal visualizer](https://github.com/samsmith00/junior-IS/issues/3) | 2/27/25 | |
| [DAW Integration](https://github.com/samsmith00/junior-IS/issues/9) | 3/01/25 | |
| [Implement plugin dial](https://github.com/samsmith00/junior-IS/issues/5) | 3/05/25 | |
| [Implement visualizer axes](https://github.com/samsmith00/junior-IS/issues/6) | 3/06/25 | |
| [Implement dial lablels](https://github.com/samsmith00/junior-IS/issues/7) | 3/07/25 | |
| [Implement pitch shifting algorithm](https://github.com/samsmith00/junior-IS/issues/2) | 3/20/25 | |
| [Implement real time I/O](https://github.com/samsmith00/junior-IS/issues/10) | 3/25/25 | |
| [Add polyphonic pitch shifting support](https://github.com/samsmith00/junior-IS/issues/8) | 4/01/25 | |

| **Stretch Goals** | **Due date** | |
| --------- | ------------ | -- |
| [Implement offline feature](https://github.com/samsmith00/junior-IS/issues/11) | 4/14/25 | |
| [Implement pitch blending feature](https://github.com/samsmith00/junior-IS/issues/12) | 4/25/25 | |
