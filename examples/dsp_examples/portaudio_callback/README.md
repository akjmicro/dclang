## DSP examples

To run each of these examples:

- Make sure that `DCLANG_LIBS` environment variable is defined in your `~/.bashrc` or equivalent, e.g:
  ```
  export DCLANG_LIBS=/usr/local/lib/dclang
  ```
- Have `portaudio19-dev` installed on your machine:
  ```
  sudo apt-get install portaudio19-dev
  ```
- Execute the helper script:
  ```
  run-example allpass_reverb.dc
  ```

Have fun!
