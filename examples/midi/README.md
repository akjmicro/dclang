# MIDI setup

Each of the scripts work by setting up a device using the portmidi bindings
created in lib/midi.dc

You can get an enumeration of MIDI devices on your machine by simply importing that
library in a dclang interactive shell. For example:

```
aaron@aaron-XPS-13-9350:~/programs/dclang/examples/midi$ dclang
Welcome to dclang! Aaron Krister Johnson, 2018-2024
Make sure to peruse README.md to get your bearings!
You can type 'primitives' to see a list of all the primitive (c-builtin) words.
You can type 'words' to see a list of functions defined within dclang.

There are currently 197 primitives implemented.
The following primitives are visible; invisible primitives start with '_' and are meant to be used privately by libraries:

Boolean            | null false true = <> < > <= >= assert 
Bit manipulation   | and or xor not << >> 
... # rest of shell heading cut for space....

"midi.dc" import   # <=== THIS IS WHAT YOU TYPE (AND HIT RETURN)

# BELOW IS WHAT dclang WILL OUTPUT

Here are your available MIDI devices:
0: ALSA, Midi Through Port-0 (default output)
1: ALSA, Midi Through Port-0 (default input)
2: ALSA, VirMIDI 1-0 (output)
3: ALSA, VirMIDI 1-0 (input)
4: ALSA, VirMIDI 1-1 (output)
5: ALSA, VirMIDI 1-1 (input)
6: ALSA, VirMIDI 1-2 (output)
7: ALSA, VirMIDI 1-2 (input)
8: ALSA, VirMIDI 1-3 (output)
9: ALSA, VirMIDI 1-3 (input)

PORTMIDI_IN_DEVNUM is not set in your environment!
MIDI input functionality will not work.
If you need this, please exit from dclang, set that value, and try again!
PORTMIDI_OUT_DEVNUM is not set in your environment!
MIDI output functionality will not work.
If you need this, please exit from dclang, set that value, and try again!
```

So, notice the message that we haven't selected a device. Now that we know what they are,
let's say I want to use `ALSA, VirMIDI 1-0 (output)`, a virtual port, to send data to.
Simple, that's `PORTMIDI_OUT_DEVNUM=2`, so let's export that.

```
aaron@aaron-XPS-13-9350:~/programs/dclang/examples/midi$ export PORTMIDI_OUT_DEVNUM=2
```

And, let's check again in `dclang` that it picks up:

```
aaron@aaron-XPS-13-9350:~/programs/dclang/examples/midi$ dclang
Welcome to dclang! Aaron Krister Johnson, 2018-2024
Make sure to peruse README.md to get your bearings!
You can type 'primitives' to see a list of all the primitive (c-builtin) words.
You can type 'words' to see a list of functions defined within dclang.

... # cut head again for brevity

"midi.dc" import    # <==== type this again and hit return

Here are your available MIDI devices:
0: ALSA, Midi Through Port-0 (default output)
1: ALSA, Midi Through Port-0 (default input)
2: ALSA, VirMIDI 1-0 (output)
3: ALSA, VirMIDI 1-0 (input)
4: ALSA, VirMIDI 1-1 (output)
5: ALSA, VirMIDI 1-1 (input)
6: ALSA, VirMIDI 1-2 (output)
7: ALSA, VirMIDI 1-2 (input)
8: ALSA, VirMIDI 1-3 (output)
9: ALSA, VirMIDI 1-3 (input)

PORTMIDI_IN_DEVNUM is not set in your environment!
MIDI input functionality will not work.
If you need this, please exit from dclang, set that value, and try again!
Opening device number: 2 

```

Great, it's now registered, via an environment variable, that we want to open
`PORTMIDI_OUT_DEVNUM` number `2`. (ATM, we don't have to worry about the fact
taht `PORTMIDI_IN_DEVNUM` is unset.

Now, many of these scripts use parameters set in the key/value store `redis`
to communicate variables to a live MIDI process. We can control these variables
one of two ways: either directly change them via `redis-cli`, which comes with
`redis`, and allows you to change values by typing them in, or -- start a little
`dclang` web-server that serves a page with GUI widgets in your browser, and have
AJAX messages update the same variables in `redis`, which will be picked up by the
separate `dclang` process that is running a MIDI script.

Here's how to start the server (you only type what's after the end of the shell prompt, which
ends with `$`), where all this stuff will live will depend on how and where you've installed
`dclang`:

```
aaron@aaron-XPS-13-9350:~/programs/dclang$ cd examples
aaron@aaron-XPS-13-9350:~/programs/dclang/examples$ dclang redis_control.dc
running acceptloop; serving on port 7651 
```

The message tells us the interface is being served on port `7651`, so open you browser to
`localhost:7651` in the address bar, and enjoy the widgets you'll see. Each will update
a `redis` parameter behind-the-scenes. In a separate shell, you can start one of the example
midi scripts that live in `examples/midi`. Note that this set of widgets covers most of the
variables you'll encounter over the scripts, so not every parameter is used by any one script.
