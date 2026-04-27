## DSP examples

To run an example:
  - `./run_example.sh <script_name.dc>`


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
