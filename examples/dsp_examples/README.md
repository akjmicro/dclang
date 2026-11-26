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

## dclang2c-dsp and Bela

`render_frame` words return `( left right )`; the named playback word owns the
interpreter loop and passes those samples to `stereo_out`. The files listed
below have bounded frame words and have compiled with `dclang2c-dsp`.

### Local JACK build

From the repository root, generate and compile an instrument as a native JACK
client with:

```sh
make -f Makefile.linux local-dsp \
  DSP=panning_example.dc
/tmp/dclang-panning_example
```

Use `Makefile.mac` on macOS. The build requires JACK development files and a
`pkg-config` entry for `jack`.

`local-dsp` uses the host cell width, removes the interpreter's static-link
flag, links against the system JACK library and `libm`, and generates the patch
for `SR` (44100 by default). The executable creates `out_1` and `out_2` JACK
ports and performs no allocation in the process callback. It refuses to start
when JACK's sample rate differs from the generation rate. An optional argument
sets the JACK client name:

```sh
/tmp/dclang-panning_example my-panning-instrument
```

Override `LOCAL_DSP`, `LOCAL_CFLAGS`, `LOCAL_CPPFLAGS`, or `LOCAL_LDFLAGS` when
the host needs a different output path or toolchain flags.

### Local OSC controls

If `<instrument>_osc.conf` exists beside the source patch, `local-dsp` embeds
its path as the executable's default receiving-side control map. Each entry
maps an OSC endpoint to a generated dclang variable:

```text
# UDP-port OSC-path generated-variable minimum maximum
7562 /ambient_cloud_lite/master_volume master_volume 0.0 1.0
7562 /ambient_cloud_lite/ms_width      ms_width      0.0 1.0
```

Following the Faust convention, an instrument should use its source basename
as the first OSC path component: `ambient_cloud_lite.dc` therefore owns the
`/ambient_cloud_lite/...` namespace.

All entries currently share one UDP port. Incoming `,f` values are clamped to
the declared range and copied atomically into the generated patch at the start
of each JACK callback. A `,s "get"` request receives the Faust-compatible
`path ,fff current minimum maximum` response on `port + 1`, which gives
`midi_to_osc.dc` its normal discovery and soft-takeover behavior. Parsing,
variable lookup, and allocation happen before JACK activation.

`ambient_cloud_lite.dc` supplies such a map for normalized master-volume and
mid/side-width controls. Its matching `midi_to_osc.conf` entries use
LaunchControl channel 10, CC 47 for width and CC 48 for master volume. Width
0.0 is mono; width 1.0 preserves the patch's original stereo image. The
controller map uses `channel controller host port path`, so the MIDI bridge
can route each synth or effect to a local or remote machine independently.

Pass a different map as the executable's second argument after the optional
JACK client name, or override `LOCAL_OSC_CONFIG` while building.

## `dclang2c-dsp` assessment

These currently compile with `dclang2c-dsp` after extracting `render_frame`:

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
- `drum_machine.dc` (generic OSC control wrapper and companion controller)

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
- `bassline.dc`: imports the music and sequencer libraries; not assessed.

An instrument with a companion `<instrument>_controller.dc` automatically uses
the generic OSC Bela wrapper in `run-bela-example.sh`. Controllers send
`/dclang/set` messages containing a generated variable name and an integer or
float value; the wrapper stays silent until its first valid update.

All other examples without an explicit result above remain untested with
`dclang2c-dsp`; do not infer compatibility from their source shape alone.
