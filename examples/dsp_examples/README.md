## DSP examples

To run each of these examples:

- Set up a `DCLANG_LIBS` environment variable, e.g. `DCLANG_LIBS=/usr/local/lib/dclang`
- If desired, make that variable symlink to your local repo copy:
  `sudo ln -s /home/yourname/Downloads/dclang/lib /usr/local/lib/dclang`
- Create an alias command for piping through `aplay`:
  `alias dcplayer=aplay -f S32_LE -r 48000 -c 2`
- Pipe the example by running dclang on it:
  ```
  cd examples/dsp_examples
  dclang biquad_lp_example.dc | dcplayer
  ```

Have fun!
