## DSP examples

To run an example:
  - `./run-example.sh <script_name.dc>`

- Make sure that `DCLANG_LIBS` environment variable is defined in your `~/.bashrc` or equivalent, e.g:
  ```
  export DCLANG_LIBS=/usr/local/lib/dclang
  ```

- If you are on Linux with `pipewire`, the script will attempt to create a node
  and attach to the soundcard.

- If you don't have `pipewire`, you should build the `pa_play.c` executable:

# Mac
  ```
  brew install portaudio
  gcc pa_play.c \
      -I /opt/homebrew/include \
      -L /opt/homebrew/lib \
      -o pa_play \
      -lportaudio
  ```

## Linux
  ```
  xbps-install -Su portaudio-devel  # void-linux
  apt-get install portaudio-dev     # Debian/Ubuntu
  ...

  gcc pa_play.c -o pa_play -lportaudio
  ```

Have fun!

## dclang2c and Bela

`render_frame` words return `( left right )`; the named playback word owns the
interpreter loop and passes those samples to `_stereo_out32`. The files listed
below have bounded frame words and have compiled with `dclang2c`.

## `dclang2c` assessment

These currently compile with `dclang2c` after extracting `render_frame`:

- `ambient_cloud_lite.dc` (five-voice fast-tanh patch)
- `bells_of_doom_example.dc`
- `delay_example.dc`
- `ebow_example.dc`
- `fm_example.dc`
- `biquad_bp_example.dc`
- `biquad_hp_example.dc`
- `biquad_lp_example.dc`
- `metronome_example.dc`
- `noise_example.dc`
- `hc_bandpass_example.dc`
- `hc_lowpass_example.dc`
- `otonal_bells.dc`
- `panning_example.dc`
- `ping_pong_delay.dc`
- `pw_mod_example.dc`
- `randtrig_example.dc`
- `reverb_example.dc`
- `simple_delay_example.dc`

Known compiler blockers or workflow exclusions:

- `sineloop_example.dc` and `simple_sineloop.dc`: audio frame depends on MIDI
  control words, which are outside the current compiler subset.
- `midi_beating.dc` and `midi_trigger.dc`: depend on MIDI control or output.
- `standalone_reverb.dc`: reads from `stereo_in`; the compiler entry currently
  generates audio rather than consuming an input stream.
- `port_bench.dc`: benchmark loops and printing, not an audio render entry.
- `portamento_lead.dc`: reads live key-value controls and generates MIDI/keyed
  rhythmic state; this path needs a separate control-input design.
- `sync.dc`: startup helper; keep its `synchronized_start` call and do not
  treat it as a standalone DSP patch.

Not targeted for Bela in this pass:

- `ambient_cloud.dc`: known to exceed the practical Bela CPU budget.
- `drum_machine.dc`: uses sequencing and control behavior not yet assessed for
  `dclang2c`.
- `bassline.dc`: imports the music and sequencer libraries; not assessed.

All other examples without an explicit result above remain untested with
`dclang2c`; do not infer compatibility from their source shape alone.
