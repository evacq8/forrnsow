# forrnsow: (wip) basic synth audio plugin

a basic synth which uses the **cl**ever **a**udio **p**lugin standard.

the code is really messy and bad and horrible and i'm not a professional by any stretch of the imagination...

i dream of making it more modular and adding a gui (if i don't give up which is a lot to ask of me).

## features...

...if you can call them that:

* midi parsing
* polyphony

## building from source

```bash
# clone the repo and dependencies (clap and clap-helpers)
git clone --recursive https://github.com/evacq8/forrnsow.git
cd forrnsow

# build
make
mkdir -p ~/.clap
cp forrnsow.clap ~/.clap/forrnsow.clap
```

## todo

- [ ] seperate note logic into its own file
- [ ] envelopes
- [ ] get sample rate from daw to adjust increment calculations (assumes 44100 hz right now)

### how it works (for reference in-case i forgot)

- voice management - fixed-size pool of note objects where inactive notes are recycled to avoid malloc/new in the audio thread.
- midi - midi1.0 note events are parsed and converted to a struct which is queued to toggle notes in the notes pool every buffer.
