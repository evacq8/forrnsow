# forrnsow: (wip) basic synth audio plugin

a basic synth which uses the **cl**ever **a**udio **p**lugin standard.

the code is really messy and bad and horrible and i'm not a professional by any stretch of the imagination...

## features...

...if you can call them that:

* midi parsing
* polyphony
* linear adsr envelopes
* formant filters (w/ biquad)
* basic gui structure
    * uses dearimgui
    * only works on x11 (xwayland also works if you're using wayland)
    * plan on adding microsoft windows support in the future (hopefully)

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

- [ ] seperate note logic into its own file
- [x] adsr envelopes
- [ ] non-linear adsr envelopes
- [x] oscillator class
- [ ] get sample rate from daw to adjust calculations (assumes 44100 hz right now)
