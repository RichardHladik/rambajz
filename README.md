# Rambajz

A simple FFT-based tuner and spectogram.

## Compiling

Rambajz needs SDL2, SDL2\_ttf and JACK development files to compile.

To build Rambajz, simply run `make`. It is possible to customise the default
build flags either by passing them as a command line argument to `make` (e.g.,
`make C=clang`), or by putting them in `Makefile.local`.

## Usage

Before running Rambajz, ensure the JACK audio server is active. To run Rambajz,
run `build/rambajz` from the project root.

By default, Rambajz tries to connect to all physical input ports (more
specifically, all JACK ports matching the regex `system:capture.*`). You can
pass an alternative regex as the first argument to `build/rambajz`.

The top of the application window contains a tuning bar showing the detected
dominant frequency (on the right) and the tone corresponding to it (on the
left), along with its deviation from the standard tuning in cents. 

The rest of the application window displays a spectogram of the recorded data.
The vertical red line marks the found dominant frequency. The spectogram shows
the frequency response in the range (20 Hz, [jack sample rate] / 2). It uses a
logarithmical scale, meaning for example that every two notes separated by an
octave are the same distance apart.

Simple movement and zooming of the spectogram's viewport is supported: either
use the touchpad, or J/K to zoom in/out and H/L to move left/right. The
dominant frequency is only calculated from the current viewport.

The spectral analysis is done using a simple implementation of FFT which runs
in O(N log N). A custom mode in which the Fourier transform is done in a naive
way separately for each frequency of interest is also available. Pressing D
toggles between the two modes. The advantage of the latter mode is that while
the FFT samples the same set of frequency bins for each frame regardless of the
viewport, in the custom mode the frequencies of interest are chosen so that
they span the current viewport equally (as seen by the user -- i. e. accounting
for the logarithmic scale).

To exit the application, press Q, Escape, or close the window.
