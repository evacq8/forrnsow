# forrnsow: (wip) basic synth audio plugin

a basic synth which uses the **cl**ever **a**udio **p**lugin standard.

the code is really messy and bad and horrible and i'm not a professional by any stretch of the imagination...


## features...

...if you can call them that:

* midi parsing
* polyphony
* linear adsr envelopes
* formant filters (w/ biquad)
* lfo vibrato
* basic gui with oscilloscope and spectrum chart
    * uses dearimgui
    * only works on x11 (xwayland also works if you're using wayland)
    * plan on adding microsoft windows support in the future (hopefully)

## current (known) issues

note: all my testing is done on linux wayland on the reaper daw. i'm not sure if things might differ in your environment.

* gui currently doesn't work on microsoft windows
* faint clicking sound when reactivating a note as it is fading (release)
* currently uses the "slow" discrete fourier transform rather than the fast fourier transform.
* opening multiple guis will cause one of them to be blank (x11 implementation issue)
* expects a sample-rate of 44100 hz because this is currently hardcoded into all the calculations


## building from source

```bash
# clone the repo and dependencies (clap, clap-helpers, imgui)
git clone --recursive https://github.com/evacq8/forrnsow.git
cd forrnsow

# build
make
mkdir -p ~/.clap
cp forrnsow.clap ~/.clap/forrnsow.clap
```

## todo

- [ ] incremental builds
- [ ] ability to draw custom waveforms in the gui and save/load them.
- [ ] more robust pitch modulation (move it into oscillator.cpp)
    - [ ] ..and pitchwheel functionality!
- [ ] replace normal dft with fast fourier transform for better performance at higher input sizes
- [ ] seperate note logic into its own file
- [ ] non-linear adsr envelopes
- [ ] get sample rate from daw to adjust calculations (assumes 44100 hz right now)
