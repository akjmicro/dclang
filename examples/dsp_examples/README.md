## DSP examples

There are 2 ways to run the examples.
  - `./run_example.sh <script_name.dc>
  - `./run_example-portaudio.sh <script_name.dc>

For both:

- Make sure that `DCLANG_LIBS` environment variable is defined in your `~/.bashrc` or equivalent, e.g:
  ```
  export DCLANG_LIBS=/usr/local/lib/dclang
  ```

For the portaudio way:

- Have `portaudio19-dev` installed on your machine:
  ```
  sudo apt-get install portaudio19-dev  # or similar
  ```

Have fun!
