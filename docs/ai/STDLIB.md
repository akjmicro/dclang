# dclang Standard Library Reference (draft v0.6)

> **Status:** Source inventory reconciled with repository commit `3b333a8`. Historical runtime observations remain partial and are not exhaustive semantic coverage.

**Coverage:** 20 library modules, 334 definition occurrences, 333 distinct names.


## Additional interpreter observations (v0.5)

The accompanying `stdlib_additional_results.json` contains 16 additional independent interpreter invocations: **13 passed and 3 did not match the initial hypotheses**. The three mismatches are documented rather than concealed. These observations are not exhaustive contracts.

| Module | Word | Input | Observed | Interpretation |
|---|---|---|---|---|
| `math.dc` | `isneg` | `-4 isneg` | `-1` | Not a normalized Boolean; implementation is `1 swap / 0 <` |
| `math.dc` | `isneg` | `4 isneg` | `0` | Observed nonnegative case |
| `math.dc` | `factorial` | `0 factorial` | `0` | Does not produce mathematical `0! = 1`; investigate loop behavior |
| `math.dc` | `factorial` | `4 factorial` | `24` | Positive case passes |
| `math.dc` | `bounce` | `5 0 4 bounce` | `3` | Observed reflection result |
| `string.dc` | `str+` | `"abc" "def" str+ print` | Empty output | **Invalid test invocation**: expects a writable destination buffer; no stack effect established |

**Testing discipline:** a failed hypothesis is not necessarily a failing library implementation. In particular, `str+` must be retested with a properly allocated and initialized destination buffer, and `isneg` should be tested with negative magnitudes other than `-4` before assigning it a general Boolean signature.

## Conventions

- `( a b -- c )` describes the data stack, bottom to top from left to right.
- An empty stack-effect field means **unknown**, not zero arguments.
- Names beginning with `_` are treated as internal by convention, not necessarily enforced private access.
- Imports may define words and perform initialization. Import order and collisions matter.
- Memory allocation, I/O, and external state changes are not fully described by stack effects.

## Module index

| Module | Words | Literal imports |
|---|---:|---|
| `clock_sleep.dc` | 1 | — |
| `csvlib.dc` | 6 | — |
| `deprecated_stack.dc` | 7 | — |
| `dsp.dc` | 65 | — |
| `fractions.dc` | 6 | `math.dc` |
| `http_server.dc` | 23 | `string.dc` |
| `linked_list.dc` | 8 | — |
| `logging.dc` | 3 | `string.dc` |
| `math.dc` | 31 | — |
| `midi.dc` | 27 | `string.dc` |
| `music.dc` | 69 | `math.dc`, `midi.dc`, `osc.dc`, `linked_list.dc`, `music_keyval.dc` |
| `music_keyval.dc` | 40 | `string.dc`, `clock_sleep.dc` |
| `osc.dc` | 27 | — |
| `permutations.dc` | 20 | `math.dc` |
| `redis.dc` | 10 | `string.dc` |
| `regex.dc` | 4 | `string.dc` |
| `sequencer.dc` | 5 | — |
| `shuffle.dc` | 2 | — |
| `sqlite3.dc` | 7 | — |
| `string.dc` | 16 | `deprecated_stack.dc` |

## Name collisions


## Module definitions

### clock_sleep.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `sleep_until` | 4 | — | `( epoch-seconds nanoseconds -- )` (runtime-verified) |

### csvlib.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `csv_open_input_file` | 19 | — | **Unverified** |
| `_csv_init_col_buf` | 27 | — | **Unverified** |
| `csv_next_input_line` | 40 | — | **Unverified** |
| `csv_column_loop` | 55 | — | **Unverified** |
| `csv_find_col_x` | 83 | — | **Unverified** |
| `csv_cleanup` | 89 | — | **Unverified** |

### deprecated_stack.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `nip` | 16 | — | `( a b -- b )` (source_reviewed_and_runtime_sampled_v1.7) |
| `tuck` | 17 | — | `( a b -- b a b )` (source_reviewed_and_runtime_sampled_v1.7) |
| `rot` | 21 | — | `( a b c -- b c a )` (source_reviewed_and_runtime_sampled_v1.7) |
| `-rot` | 26 | — | `( a b c -- c a b )` (source_reviewed_and_runtime_sampled_v1.7) |
| `2swap` | 34 | — | `( a b c d -- c d a b )` (source_reviewed_and_runtime_sampled_v1.7) |
| `2rot` | 39 | — | `( a b c d e f -- c d e f a b )` (source_reviewed_and_runtime_sampled_v1.7) |
| `-2rot` | 45 | — | `( a b c d e f -- e f a b c d )` (source_reviewed_and_runtime_sampled_v1.7) |

### dsp.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_get_sr` | 7 | — | `( -- sample-rate )` (source-reviewed through commit 3b333a8) |
| `seconds2samples` | 48 | — | `( seconds -- samples )` (source-reviewed through commit 3b333a8) |
| `t` | 55 | — | `( -- sample-index )` (source-reviewed through commit 3b333a8) |
| `lcget` | 56 | — | `( -- left )` (source-reviewed through commit 3b333a8) |
| `lcset` | 57 | — | `( left -- )` (source-reviewed through commit 3b333a8) |
| `rcget` | 58 | — | `( -- right )` (source-reviewed through commit 3b333a8) |
| `rcset` | 59 | — | `( right -- )` (source-reviewed through commit 3b333a8) |
| `_clear_channels` | 60 | — | `( -- )` (source-reviewed through commit 3b333a8) |
| `stereo_in` | 65 | — | `( -- left right )` (source-reviewed through commit 3b333a8) |
| `_reset_phasor_count` | 77 | — | `( -- )` (source-reviewed through commit 3b333a8) |
| `phasor_pointer` | 79 | — | `( -- phase-slot )` (source-reviewed through commit 3b333a8) |
| `phasor` | 81 | — | `( frequency -- phase )` (source-reviewed through commit 3b333a8) |
| `sawtooth` | 85 | — | `( frequency -- signal )` (source-reviewed through commit 3b333a8) |
| `metro` | 92 | — | `( frequency -- trigger )` (source-reviewed through commit 3b333a8) |
| `gate` | 99 | — | `( width frequency -- gate )` (source-reviewed through commit 3b333a8) |
| `pulse` | 106 | — | `( width frequency -- signal )` (source-reviewed through commit 3b333a8) |
| `pulse_tanh` | 110 | — | `( width frequency -- signal )` (source-reviewed through commit 3b333a8) |
| `tri` | 115 | — | `( frequency -- signal )` (source-reviewed through commit 3b333a8) |
| `wavetable_phase` | 128 | `freq`, `phase`, `table`, `length` | `( frequency phase-offset table table-length -- signal )` (source-reviewed) |
| `wavetable` | 132 | `freq`, `table`, `length` | `( frequency table table-length -- signal )` (source-reviewed) |
| `sine` | 121 | — | `( frequency -- signal )` (source-reviewed through commit 3b333a8) |
| `fast_sin` | 128 | — | `( radians -- approximate-sine )` (source-reviewed through commit 3b333a8) |
| `fast_tanh` | 131 | — | `( value -- approximate-tanh )` (source-reviewed through commit 3b333a8) |
| `fast_sine` | 135 | — | `( frequency -- approximate-sine )` (source-reviewed through commit 3b333a8) |
| `sine_phase` | 142 | `freq`, `phase` | `( frequency phase-offset -- signal )` (source-reviewed through commit 3b333a8) |
| `fm_init` | 151 | `carrier_freq`, `mod_ratio` | `( carrier-frequency mod-ratio -- carrier-frequency modulator-frequency )` (source-reviewed) |
| `fm_steps` | 155 | — | `( carrier-frequency modulator-frequency index -- signal )` (source-reviewed) |
| `fm` | 159 | `carrier_freq`, `mod_ratio`, `index` | `( carrier-frequency mod-ratio index -- signal )` (source-reviewed through commit 3b333a8) |
| `sineloop` | 165 | `mem`, `freq`, `fb` | `( state-slot frequency feedback -- signal )` (source-reviewed through commit 3b333a8) |
| `noise` | 175 | — | `( -- signal )` (source-reviewed through commit 3b333a8) |
| `randint` | 183 | `range` | `( range -- integer )` (source-reviewed through commit 3b333a8) |
| `randtrig` | 190 | `compare_val`, `hold_var` | **Unverified** |
| `make_delay_line` | 210 | `seconds` | `( seconds <name-token> -- )` (source-reviewed through commit 3b333a8) |
| `_set_delay_value` | 225 | — | `( signal delay-struct -- )` (source-reviewed through commit 3b333a8) |
| `delay_read` | 232 | — | `( delay-struct seconds -- signal )` (source-reviewed through commit 3b333a8) |
| `delay_write` | 236 | `sig`, `delay_struct` | `( signal delay-struct -- )` (source-reviewed through commit 3b333a8) |
| `tau_to_alpha` | 252 | `tau` | `( tau -- alpha )` (source-reviewed through commit 3b333a8) |
| `port` | 256 | — | `( signal state-slot rise-alpha fall-alpha -- output )` (source-reviewed through commit 3b333a8) |
| `lowpass` | 264 | `sig`, `memvar`, `pole` | `( signal state-slot pole -- output )` (source-reviewed through commit 3b333a8) |
| `make_dcblock` | 275 | — | `( <name-token> -- )` (source-reviewed through commit 3b333a8) |
| `dcblock` | 277 | `sig`, `dc_struct` | `( signal dc-struct -- output )` (source-reviewed through commit 3b333a8) |
| `make_allpass` | 301 | — | **Unverified** |
| `allpass` | 314 | `sig`, `ap_struct`, `dltm`, `coef` | `( signal allpass-struct delay-time coefficient -- output )` (source-reviewed through commit 3b333a8) |
| `_set_bq_type` | 365 | `struct`, `type` | **Unverified** |
| `_set_cf_res_reinit` | 386 | `struct`, `reinit`, `cutoff`, `res` | **Unverified** |
| `_calc_theta` | 392 | `struct` | **Unverified** |
| `_calc_beta` | 398 | `struct`, `tmp` | **Unverified** |
| `_calc_gamma` | 411 | `struct` | **Unverified** |
| `_calc_alpha` | 417 | `struct` | **Unverified** |
| `_reinit_biquad` | 425 | `struct` | **Unverified** |
| `_init_biquad` | 436 | `struct`, `reinit`, `cutoff`, `res`, `type` | **Unverified** |
| `biquad_compute` | 448 | `sig`, `struct` | `( signal biquad-struct -- output )` (source-reviewed through commit 3b333a8) |
| `make_biquad` | 488 | — | **Unverified** |
| `biquad_lp` | 494 | `sig`, `struct`, `reinit`, `cutoff`, `res` | `( signal struct reinit cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `biquad_hp` | 503 | `sig`, `struct`, `reinit`, `cutoff`, `res` | `( signal struct reinit cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `biquad_bp` | 512 | `sig`, `struct`, `reinit`, `cutoff`, `res` | `( signal struct reinit cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `make_hc_filter` | 535 | — | **Unverified** |
| `_hc_common` | 537 | `sig`, `struct`, `fc`, `res` | `( signal struct cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `hc_lowpass` | 575 | `sig`, `struct`, `fc`, `res` | `( signal struct cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `hc_highpass` | 580 | `sig`, `struct`, `fc`, `res` | `( signal struct cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `hc_bandpass` | 585 | `sig`, `struct`, `fc`, `res` | `( signal struct cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `hc_notch` | 590 | `sig`, `struct`, `fc`, `res` | `( signal struct cutoff resonance -- output )` (source-reviewed through commit 3b333a8) |
| `make_reverb_struct` | 603 | — | **Unverified** |
| `reverb` | 608 | — | `( left right struct cutoff feedback -- wet-left wet-right )` (source-reviewed through commit 3b333a8) |
| `panmix` | 618 | — | `( signal pan -- )` (source-reviewed through commit 3b333a8) |
| `stereo_out` | 627 | — | `( -- )` (source-reviewed through commit 3b333a8) |
| `t+` | 634 | — | `( -- )` (source-reviewed through commit 3b333a8) |

### fractions.dc

**Imports:** `math.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `simplify` | 8 | `n`, `d` | `( n d -- reduced_n reduced_d )` (runtime_observed_source_reviewed) |
| `fracadd` | 12 | `n1`, `d1`, `n2`, `d2` | `( n1 d1 n2 d2 -- n d )` (runtime_observed_source_reviewed) |
| `fracsub` | 16 | — | `( n1 d1 n2 d2 -- n d )` (runtime_observed_source_reviewed) |
| `fracmul` | 18 | `n1`, `d1`, `n2`, `d2` | `( n1 d1 n2 d2 -- n d )` (runtime_observed_source_reviewed) |
| `fracdiv` | 22 | — | `( n1 d1 n2 d2 -- n d )` (runtime_observed_source_reviewed) |
| `fracmediant` | 24 | `n1`, `d1`, `n2`, `d2` | `( n1 d1 n2 d2 -- n d )` (runtime_observed_source_reviewed) |

### http_server.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `get_accepted_conn` | 40 | — | **Unverified** |
| `get_connbuf` | 41 | — | **Unverified** |
| `get_received` | 42 | — | **Unverified** |
| `zerobuf` | 43 | — | **Unverified** |
| `get_method` | 44 | — | **Unverified** |
| `get_path_and_query` | 45 | — | **Unverified** |
| `get_httpversion` | 46 | — | **Unverified** |
| `get_path` | 47 | — | **Unverified** |
| `get_query` | 48 | — | **Unverified** |
| `read_request` | 53 | — | **Unverified** |
| `split_request_topline` | 63 | — | **Unverified** |
| `split_path_and_query` | 68 | — | **Unverified** |
| `hash_assign` | 73 | — | **Unverified** |
| `_split_query_string_helper` | 85 | — | **Unverified** |
| `split_query_string` | 95 | — | **Unverified** |
| `process_request` | 117 | — | **Unverified** |
| `_print_hash_table_helper` | 122 | — | **Unverified** |
| `print_hash_table` | 141 | — | **Unverified** |
| `copy_header_to_buffer` | 148 | — | **Unverified** |
| `copy_footer_to_buffer` | 154 | — | **Unverified** |
| `write_response` | 158 | — | **Unverified** |
| `_acceptloop` | 170 | — | **Unverified** |
| `acceptloop` | 176 | — | **Unverified** |

### linked_list.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_list_init_helper` | 3 | `list_obj`, `token_method` | **Unverified** |
| `_lmap_helper` | 15 | — | **Unverified** |
| `_lshow_helper` | 29 | — | **Unverified** |
| `linit` | 44 | — | `( mutable_token_buffer token_word_name -- list )` (source_reviewed_and_runtime_sampled_v1.8) |
| `lmap` | 66 | — | `( list word_name -- new_list )` (source_reviewed_and_runtime_sampled_v1.8) |
| `lshow` | 81 | — | **Unverified** |
| `make_lgen_struct` | 91 | — | **Unverified** |
| `lgen` | 98 | `lgen_struct`, `infinite?` | **Unverified** |

### logging.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `open_log` | 10 | — | **Unverified** |
| `to_log` | 16 | — | **Unverified** |
| `close_log` | 30 | — | **Unverified** |

### math.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `isneg` | 2 | — | `( x -- flag )` (source_reviewed_not_executed) |
| `absmod` | 4 | — | `( a modulus -- nonnegative_remainder )` (runtime_observed_source_reviewed) |
| `divmod` | 15 | — | `( dividend divisor -- quotient remainder )` (runtime_observed_source_reviewed) |
| `bounce` | 20 | `n`, `lower`, `upper` | **Unverified** |
| `gcd` | 35 | — | `( a b -- gcd )` (runtime_observed_source_reviewed) |
| `factorial` | 47 | `x` | `( n -- factorial )` (runtime_observed_source_reviewed) |
| `_digit_sum_helper` | 54 | — | **Unverified** |
| `digit_sum` | 70 | — | `( n base -- sum )` (runtime_observed_source_reviewed) |
| `make_digit_expansion_struct` | 86 | — | **Unverified** |
| `_get_base` | 88 | — | **Unverified** |
| `_set_base` | 90 | — | **Unverified** |
| `_get_div` | 91 | — | **Unverified** |
| `_set_div` | 92 | — | **Unverified** |
| `_get_old_base` | 93 | — | **Unverified** |
| `_set_old_base` | 94 | — | **Unverified** |
| `_get_old_div` | 95 | — | **Unverified** |
| `_set_old_div` | 96 | — | **Unverified** |
| `_get_remainder` | 97 | — | **Unverified** |
| `_set_remainder` | 98 | — | **Unverified** |
| `_base_changed?` | 99 | — | **Unverified** |
| `_reset_old_base` | 100 | — | **Unverified** |
| `_div_changed?` | 101 | — | **Unverified** |
| `_reset_old_div` | 102 | — | **Unverified** |
| `gen_digit_expansion` | 103 | `exp_struct` | **Unverified** |
| `is-2-3-5-7?` | 127 | — | **Unverified** |
| `check-higher-than-49` | 139 | — | **Unverified** |
| `is-not-div-otherwise?` | 152 | — | **Unverified** |
| `is-not-div-by-2-3-5-7?` | 161 | — | **Unverified** |
| `is-prime` | 178 | — | **Unverified** |
| `find-next-prime` | 190 | `direction`, `x` | **Unverified** |
| `closest-prime` | 198 | `x` | **Unverified** |

### midi.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `populate_midi_channel` | 10 | — | **Unverified** |
| `make_channel_string` | 32 | `prefix` | **Unverified** |
| `midi_shutdown` | 47 | — | **Unverified** |
| `check_mididev` | 52 | — | **Unverified** |
| `_maybe_check_mididev` | 88 | — | **Unverified** |
| `refresh_midictl` | 98 | — | **Unverified** |
| `midi_cc_event` | 112 | — | **Unverified** |
| `midictl` | 129 | — | **Unverified** |
| `send_midi` | 132 | — | **Unverified** |
| `send_midi_reverse` | 133 | — | **Unverified** |
| `panic` | 137 | — | **Unverified** |
| `_bend_to_MSB_LSB_bytes` | 155 | — | **Unverified** |
| `_read_midi_byte` | 194 | `mfp` | **Unverified** |
| `_read_var_length` | 200 | — | **Unverified** |
| `read_var_length` | 217 | `mfp` | **Unverified** |
| `_read_chunk_length` | 221 | — | **Unverified** |
| `read_midi_header` | 228 | `mfp` | **Unverified** |
| `_output_as_hex` | 265 | — | **Unverified** |
| `_handle_sysex` | 269 | `mfp` | **Unverified** |
| `_handle_track_name` | 278 | `mfp` | **Unverified** |
| `_handle_tempo` | 287 | `mfp` | **Unverified** |
| `_handle_timesig` | 298 | `mfp` | **Unverified** |
| `_handle_meta` | 308 | `meta_byte`, `mfp` | **Unverified** |
| `_handle_status` | 333 | `status_byte`, `mfp` | **Unverified** |
| `_track_loop` | 377 | `mfp` | **Unverified** |
| `read_midi_track` | 431 | `mfp` | **Unverified** |
| `read_midi_file` | 451 | `midi_filename` | **Unverified** |

### music.dc

**Imports:** `math.dc`, `midi.dc`, `osc.dc`, `linked_list.dc`, `music_keyval.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `set_bend_depth` | 18 | — | **Unverified** |
| `O-` | 30 | — | **Unverified** |
| `O+` | 32 | — | **Unverified** |
| `_reset_gamut_idx` | 231 | — | **Unverified** |
| `_update_gamut_idx` | 233 | — | **Unverified** |
| `_toggle_gamuts` | 234 | — | **Unverified** |
| `_protect_value` | 246 | — | **Unverified** |
| `frac_to_oct_gamut` | 248 | `frac_str` | **Unverified** |
| `_process_fraction_list` | 267 | — | **Unverified** |
| `process_fraction_list` | 277 | `frac_str_list` | **Unverified** |
| `process_custom_scale` | 291 | — | **Unverified** |
| `set_scale` | 317 | — | **Unverified** |
| `show_scale` | 352 | — | **Unverified** |
| `edo_degree_to_midi` | 366 | — | **Unverified** |
| `edo_degree_to_hz` | 406 | `degree` | **Unverified** |
| `make_euc_space` | 419 | — | **Unverified** |
| `_get_euc_n` | 424 | — | **Unverified** |
| `_get_euc_k` | 426 | — | **Unverified** |
| `_get_euc_usr_p` | 427 | — | **Unverified** |
| `_get_euc_l` | 428 | — | **Unverified** |
| `_get_euc_s` | 429 | — | **Unverified** |
| `_get_euc_q` | 430 | — | **Unverified** |
| `_get_euc_r` | 431 | — | **Unverified** |
| `_get_euc_p` | 432 | — | **Unverified** |
| `_set_euc_n` | 433 | — | **Unverified** |
| `_set_euc_k` | 435 | — | **Unverified** |
| `_set_euc_usr_p` | 436 | — | **Unverified** |
| `_set_euc_l` | 437 | — | **Unverified** |
| `_set_euc_s` | 438 | — | **Unverified** |
| `_set_euc_q` | 439 | — | **Unverified** |
| `_set_euc_r` | 440 | — | **Unverified** |
| `_set_euc_p` | 441 | — | **Unverified** |
| `euc_init` | 442 | — | **Unverified** |
| `_euc_conf_check_init` | 455 | — | **Unverified** |
| `_euc` | 467 | — | **Unverified** |
| `euclidean_rhythm` | 495 | — | **Unverified** |
| `_string_to_lgen` | 510 | `str_to_check`, `cur_str`, `lgen_struct`, `method` | **Unverified** |
| `_token_has_backslash?` | 560 | — | **Unverified** |
| `_process_rhythm` | 562 | `tok` | **Unverified** |
| `_do_rhythm_string` | 578 | — | **Unverified** |
| `_do_gate_string` | 587 | — | **Unverified** |
| `rhythm_generator` | 597 | — | **Unverified** |
| `data_player` | 618 | — | **Unverified** |
| `fraction_music` | 651 | `expansion_struct` | **Unverified** |
| `digit_sum_music` | 662 | — | **Unverified** |
| `primes_music` | 683 | — | **Unverified** |
| `do_attack_string` | 705 | — | **Unverified** |
| `do_gain_string` | 719 | — | **Unverified** |
| `_get_edo_3_2` | 763 | `local_edo` | **Unverified** |
| `_update_note_symbol_tree` | 767 | `local_edo` | **Unverified** |
| `symbol_to_edo_int` | 795 | `pitch_token` | **Unverified** |
| `_reset_sym_str_ptr` | 820 | — | **Unverified** |
| `_process_next_symbol` | 824 | — | **Unverified** |
| `do_symbol_string` | 854 | — | **Unverified** |
| `_reset_seq_str_ptr` | 906 | — | **Unverified** |
| `_ratio_to_edo_delta` | 910 | `num`, `den` | **Unverified** |
| `_process_keyval_token` | 920 | `tok` | **Unverified** |
| `_process_duration_token` | 926 | `tok` | **Unverified** |
| `_parse_abs_ratio_token` | 934 | `tok` | **Unverified** |
| `_parse_rel_ratio_token` | 940 | `tok` | **Unverified** |
| `_parse_rel_ratio_token_silent` | 949 | `tok` | **Unverified** |
| `_token_has_equals?` | 956 | — | **Unverified** |
| `_token_has_semicolon?` | 958 | — | **Unverified** |
| `_token_has_colon?` | 959 | — | **Unverified** |
| `_token_has_slash?` | 960 | — | **Unverified** |
| `_token_has_letter?` | 961 | — | **Unverified** |
| `_copy_next_token` | 962 | — | **Unverified** |
| `_process_next_event` | 998 | — | **Unverified** |
| `do_seq_string` | 1076 | — | **Unverified** |

### music_keyval.dc

**Imports:** `string.dc`, `clock_sleep.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `keyval_get` | 20 | `key` | **Unverified** |
| `keyval_set` | 32 | `val`, `key` | **Unverified** |
| `tonum_free` | 49 | — | **Unverified** |
| `_numeric_string_is_hex` | 54 | — | **Unverified** |
| `_numeric_string_is_float` | 58 | — | **Unverified** |
| `tonum_float_free` | 66 | — | **Unverified** |
| `tonum_int_free` | 72 | — | **Unverified** |
| `_get_duration` | 139 | — | **Unverified** |
| `synchronized_start` | 149 | `period`, `mod_period`, `seconds`, `nanoseconds` | `( -- )` (runtime-verified) |
| `get_x_tempo` | 162 | `which_str` | **Unverified** |
| `get_drum_vol` | 169 | — | **Unverified** |
| `get_bass_vol` | 173 | — | **Unverified** |
| `get_lead_vol` | 177 | — | **Unverified** |
| `_next_on!` | 201 | — | `( epoch-seconds nanoseconds -- )` (source-reviewed) |
| `_next_on@` | 202 | — | `( -- epoch-seconds nanoseconds )` (source-reviewed) |
| `_next_off!` | 203 | — | `( epoch-seconds nanoseconds -- )` (source-reviewed) |
| `_next_off@` | 204 | — | `( -- epoch-seconds nanoseconds )` (source-reviewed) |
| `set_clock_start` | 206 | — | `( -- )` (source-reviewed) |
| `_iterate_clock` | 210 | `length`, `gate` | `( length gate -- )` (runtime-verified) |
| `get_on_gate` | 220 | `length`, `gate` | `( length gate -- )` (source-reviewed) |
| `get_off_gate` | 225 | — | `( -- )` (source-reviewed) |
| `get_mode` | 224 | — | **Unverified** |
| `get_transpose` | 228 | — | **Unverified** |
| `get_base_pitch` | 236 | — | **Unverified** |
| `get_edo` | 247 | — | **Unverified** |
| `get_scale` | 257 | — | **Unverified** |
| `get_gamut` | 269 | — | **Unverified** |
| `get_rhythm_type` | 281 | — | **Unverified** |
| `get_remainder` | 298 | — | **Unverified** |
| `set_remainder` | 300 | — | **Unverified** |
| `get_base` | 301 | — | **Unverified** |
| `get_div` | 306 | — | **Unverified** |
| `get_mul` | 311 | — | **Unverified** |
| `get_chaos` | 316 | — | **Unverified** |
| `get_wrap` | 320 | — | **Unverified** |
| `get_euclid_n` | 324 | — | **Unverified** |
| `get_euclid_k` | 330 | — | **Unverified** |
| `get_euclid_p` | 336 | — | **Unverified** |
| `get_euclid_l` | 342 | — | **Unverified** |
| `get_euclid_s` | 348 | — | **Unverified** |

### osc.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `osc_connect` | 15 | `host`, `port` | **Unverified** |
| `osc_message` | 20 | `path`, `types` | **Unverified** |
| `osc_s` | 26 | `strval` | **Unverified** |
| `osc_i` | 30 | `val` | **Unverified** |
| `osc_f` | 34 | `val` | **Unverified** |
| `osc_send` | 38 | — | **Unverified** |
| `osc_close` | 42 | — | **Unverified** |
| `osc_listen` | 69 | — | **Unverified** |
| `osc_recv` | 77 | — | **Unverified** |
| `osc_parse` | 83 | — | **Unverified** |
| `osc_parse_ok?` | 90 | — | **Unverified** |
| `osc_get_s` | 99 | — | **Unverified** |
| `osc_get_i` | 108 | — | **Unverified** |
| `osc_get_f` | 117 | — | **Unverified** |
| `osc_rx_close` | 125 | — | **Unverified** |
| `_make_path` | 139 | `path_prefix`, `path_suffix` | **Unverified** |
| `osc_synth_note_on` | 151 | `path_prefix`, `freq`, `gain` | **Unverified** |
| `osc_synth_note_off` | 163 | `path_prefix` | **Unverified** |
| `osc_synth_get` | 203 | — | **Unverified** |
| `osc_synth_floats` | 211 | — | **Unverified** |
| `osc_synth_typed` | 228 | — | **Unverified** |
| `osc_synth_parse` | 239 | — | **Unverified** |
| `osc_synth_recv` | 266 | — | **Unverified** |
| `dclang_control_f` | 287 | `value`, `name` | `( value name -- )` (source-reviewed) |
| `dclang_control_i` | 294 | `value`, `name` | `( value name -- )` (source-reviewed) |
| `dclang_control_f_changed` | 301 | `value`, `name`, `previous` | `( value name previous -- )` (source-reviewed) |
| `dclang_control_i_changed` | 309 | `value`, `name`, `previous` | `( value name previous -- )` (source-reviewed) |

### permutations.dc

**Imports:** `math.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `setup_permutation` | 27 | — | `( count -- permutation_object )` (source_reviewed_and_runtime_sampled_v1.8) |
| `show_permutation_data` | 34 | — | **Unverified** |
| `show_permutation` | 40 | — | **Unverified** |
| `_return_current_cell` | 47 | — | **Unverified** |
| `_advance_output_index` | 51 | — | **Unverified** |
| `_needs_permuting?` | 55 | — | **Unverified** |
| `_get_perm_val_i` | 57 | — | **Unverified** |
| `_set_perm_val_i` | 58 | — | **Unverified** |
| `_get_curr_high_val` | 59 | — | **Unverified** |
| `_set_curr_high_val` | 60 | — | **Unverified** |
| `_get_curr_high_slot` | 61 | — | **Unverified** |
| `_set_curr_high_slot` | 62 | — | **Unverified** |
| `_common_compare` | 63 | — | **Unverified** |
| `_set_largest_mobile_cell` | 77 | — | **Unverified** |
| `_setup_swap` | 108 | — | **Unverified** |
| `_pmswap` | 122 | — | **Unverified** |
| `_swap_neighbor_cells` | 124 | — | **Unverified** |
| `_reverse_direction_of_greater_values` | 129 | — | **Unverified** |
| `next_permutation_item` | 147 | — | **Unverified** |
| `print_next_permutation` | 161 | — | **Unverified** |

### redis.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_zero_redis_buffer` | 43 | — | **Unverified** |
| `_write_redis_buffer` | 47 | — | **Unverified** |
| `_read_redis_buffer` | 56 | — | **Unverified** |
| `_redis_write_read` | 65 | — | **Unverified** |
| `_redis_auth` | 70 | — | **Unverified** |
| `redis_connect` | 93 | — | **Unverified** |
| `redis_set` | 115 | — | **Unverified** |
| `redis_get` | 138 | — | **Unverified** |
| `redis_keys` | 159 | — | **Unverified** |
| `redis_del` | 178 | — | **Unverified** |

### regex.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_match` | 50 | — | **Unverified** |
| `_derive_real_needle_and_flags` | 77 | `needle_with_options` | **Unverified** |
| `match` | 97 | — | **Unverified** |
| `matches?` | 111 | — | **Unverified** |

### sequencer.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_seq_ptr_incr` | 8 | `seq` | **Unverified** |
| `_seq_cur_val` | 16 | `seq` | **Unverified** |
| `seq_cnt_incr` | 20 | `seq` | **Unverified** |
| `seq_signal` | 42 | `seq` | **Unverified** |
| `iterator` | 63 | `iter_array` | **Unverified** |

### shuffle.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_cell_swap` | 1 | — | **Unverified** |
| `shuffle_cells` | 4 | — | **Unverified** |

### sqlite3.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `sqlite3_open` | 1 | — | **Unverified** |
| `sqlite3_prepare` | 9 | — | **Unverified** |
| `sqlite3_step` | 18 | — | **Unverified** |
| `sqlite3_column` | 27 | — | **Unverified** |
| `sqlite3_finalize` | 36 | — | **Unverified** |
| `sqlite3_exec` | 45 | — | **Unverified** |
| `sqlite3_close` | 54 | — | **Unverified** |

### string.dc

**Imports:** `deprecated_stack.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `strdup` | 36 | — | `( string -- copy_ptr )` (source_reviewed_and_runtime_sampled_v1.8) |
| `str+` | 45 | — | **Unverified** |
| `strslice` | 52 | — | `( string start_idx end_idx -- slice_ptr )` (source_reviewed_and_runtime_sampled_v1.8) |
| `str_between` | 96 | — | `( string lower_bound upper_bound -- flag )` (source_reviewed_and_runtime_sampled_v1.8) |
| `_strreplace_helper` | 109 | `buf`, `s`, `find`, `repl` | **Unverified** |
| `strreplace` | 126 | `buf`, `s`, `find`, `repl` | `( dest_buf haystack needle replacement -- dest_buf )` (source_reviewed_and_runtime_sampled_v1.8) |
| `_get_next_num_from_line` | 137 | — | **Unverified** |
| `next_num_from_file` | 158 | — | **Unverified** |
| `uuid` | 193 | — | **Unverified** |
| `hex8token` | 237 | — | **Unverified** |
| `_str_split_next` | 258 | — | **Unverified** |
| `_str_split_real` | 269 | `in_str`, `split_token` | **Unverified** |
| `str_split` | 281 | — | **Unverified** |
| `chop_head` | 290 | — | **Unverified** |
| `chop_tail` | 315 | — | **Unverified** |
| `chop_between` | 338 | — | **Unverified** |

## Runtime verification protocol

1. Pin an image by digest, rather than relying on the mutable `latest` tag.
2. Run without network access, read-only filesystem, and with CPU/memory limits; execute only vetted pure test cases.
3. For each word, check argument order, output order, stack depth, conditional branches, and failure behavior.
4. Capture the interpreter version, image digest, input program, output, and exit status in a test manifest.
5. Mark a signature `runtime_verified` only after its test passes; preserve `source_reviewed_not_executed` otherwise.

## Source limitations

The index detects top-level `: word` definitions and literal `"file.dc" import` statements. Dynamically constructed definitions, imports, native extension functions, and runtime side effects require separate review.


## Runtime validation supplement (v0.4)

The following signatures were source-reviewed and successfully exercised in the distributed musl-linked interpreter. They are **runtime-observed**, not exhaustive proofs (particularly for edge cases).

| Module | Word | Observed stack effect | Passing cases |
|---|---|---|---:|
| `math.dc` | `divmod` | `( dividend divisor -- quotient remainder )` | 2 |
| `math.dc` | `absmod` | `( a modulus -- nonnegative_remainder )` | 1 |
| `math.dc` | `?dup` | `( x -- x [x if nonzero] )` | 2 |
| `math.dc` | `gcd` | `( a b -- gcd )` | 1 |
| `math.dc` | `factorial` | `( n -- factorial )` | 1 |
| `math.dc` | `digit_sum` | `( n base -- sum )` | 1 |
| `fractions.dc` | `gcd` | `( a b -- gcd )` | 1 |
| `fractions.dc` | `simplify` | `( n d -- reduced_n reduced_d )` | 1 |
| `fractions.dc` | `fracadd` | `( n1 d1 n2 d2 -- n d )` | 1 |
| `fractions.dc` | `fracsub` | `( n1 d1 n2 d2 -- n d )` | 1 |
| `fractions.dc` | `fracmul` | `( n1 d1 n2 d2 -- n d )` | 1 |
| `fractions.dc` | `fracdiv` | `( n1 d1 n2 d2 -- n d )` | 1 |
| `fractions.dc` | `fracmediant` | `( n1 d1 n2 d2 -- n d )` | 1 |
| `string.dc` | `strslice` | `( str start end -- slice_buffer_address )` | 1 |

`strslice` uses a reusable library buffer; the observed string was printed, rather than comparing its raw pointer. `?dup` has a conditional stack effect. The fraction routines were tested with positive inputs.

Reproduce with `docs/ai/verify_stdlib.py` and a compatible runtime.


## Runtime verification addendum (v0.6)

The accompanying `stdlib_boolean_and_edge_results.json` records 24 independent interpreter invocations. These tests are observations against the uploaded x86_64 musl runtime, not exhaustive API contracts. Some deliberately used hypotheses of `1` for true and therefore show `mismatch`; the observed `-1` is **not a test failure of dclang**.

| Module | Word / case | Observed | Notes |
|---|---|---|---|
| core | `true`, `2 3 <`, `2 2 =` | `-1` | Canonical all-bits-set true |
| core | `false`, `2 3 >` | `0` | Canonical false |
| core | `0 not` | `-1` | Bitwise complement |
| core | `1 not` | `-2` | Not logical negation |
| core | `-1 1 and` | `1` | Bitwise AND; result is not Boolean-normalized |
| `math.dc` | `isneg` with `-1`, `-2`, `-10` | `-1` | Three tested negative magnitudes |
| `math.dc` | `isneg` with `0`, `8` | `0` | Nonnegative cases |
| `math.dc` | `factorial` with `0`, `1`, `5` | `0`, `1`, `120` | `0!` is an implementation edge case |
| `math.dc` | `-17 5 divmod` | `-4 -2` | Negative division/remainder convention, one tested case |
| `string.dc` | `"abcdef" 1 4 strslice print` | `bcd` | End index exclusive in this example |
| `string.dc` | `"hello" strdup print` | `hello` | Duplicated string contents |

**Guidance for AI code generation:** Treat `true` as `-1`, `false` as `0`; use truthiness for conditions and distinguish bitwise `not` from a logical negation. Do not automatically replace bitwise results with Boolean values. Avoid claiming a single example establishes all edge cases.


## Focused runtime validation (v1.7)

See `STDLIB_RUNTIME_V17.md` and `stdlib_runtime_results_v1.7.json` for exact per-case results. Seven traditional stack-helper signatures are now source-reviewed and runtime-sampled in `stdlib.json`. This does not imply exhaustive library verification.


## Strings, linked lists, permutations (v1.8)

See `STDLIB_RUNTIME_V18.md` and `stdlib_runtime_results_v1.8.json` for exact test cases, outputs, and buffer-lifetime warnings.


---

## Consolidated technical notes and audit findings

These sections preserve the original source-review and runtime findings; milestone labels below identify their provenance, not separate releases.


### Notes originally in `STDLIB_RUNTIME_V17.md`

### Standard-library runtime verification — v1.7

**Scope:** Focused runtime examples for `deprecated_stack.dc`, `math.dc`, and `fractions.dc`, executed against the bundled musl-linked dclang runtime. Tests use `.s` and compare exact stack output. These are **examples**, not exhaustive proofs, and do not cover errors, memory safety, or all edge cases.

- Passed: **27/27**
- Newly promoted machine-readable stack effects: **7** (`nip, tuck, rot, -rot, 2swap, 2rot, -2rot`)
- Word definitions inventoried: **329** across **20** modules.

#### Observed results

| Module | Word | Expression | Expected | Observed | Status |
|---|---|---|---|---|---|
| `deprecated_stack.dc` | `nip` | `10 20 nip` | `20` | `20` | PASS |
| `deprecated_stack.dc` | `tuck` | `10 20 tuck` | `20 10 20` | `20 10 20` | PASS |
| `deprecated_stack.dc` | `rot` | `10 20 30 rot` | `20 30 10` | `20 30 10` | PASS |
| `deprecated_stack.dc` | `-rot` | `10 20 30 -rot` | `30 10 20` | `30 10 20` | PASS |
| `deprecated_stack.dc` | `2swap` | `1 2 3 4 2swap` | `3 4 1 2` | `3 4 1 2` | PASS |
| `deprecated_stack.dc` | `2rot` | `1 2 3 4 5 6 2rot` | `3 4 5 6 1 2` | `3 4 5 6 1 2` | PASS |
| `deprecated_stack.dc` | `-2rot` | `1 2 3 4 5 6 -2rot` | `5 6 1 2 3 4` | `5 6 1 2 3 4` | PASS |
| `math.dc` | `isneg` | `-1 isneg` | `-1` | `-1` | PASS |
| `math.dc` | `isneg` | `-2 isneg` | `-1` | `-1` | PASS |
| `math.dc` | `isneg` | `0 isneg` | `0` | `0` | PASS |
| `math.dc` | `isneg` | `2 isneg` | `0` | `0` | PASS |
| `math.dc` | `absmod` | `-17 5 absmod` | `3` | `3` | PASS |
| `math.dc` | `divmod` | `17 5 divmod` | `3 2` | `3 2` | PASS |
| `math.dc` | `?dup` | `0 ?dup` | `0` | `0` | PASS |
| `math.dc` | `?dup` | `4 ?dup` | `4 4` | `4 4` | PASS |
| `math.dc` | `gcd` | `54 24 gcd` | `6` | `6` | PASS |
| `math.dc` | `factorial` | `1 factorial` | `1` | `1` | PASS |
| `math.dc` | `factorial` | `5 factorial` | `120` | `120` | PASS |
| `math.dc` | `digit_sum` | `123 10 digit_sum` | `6` | `6` | PASS |
| `math.dc` | `bounce` | `5 0 4 bounce` | `3` | `3` | PASS |
| `fractions.dc` | `gcd` | `54 24 gcd` | `6` | `6` | PASS |
| `fractions.dc` | `simplify` | `8 12 simplify` | `2 3` | `2 3` | PASS |
| `fractions.dc` | `fracadd` | `1 2 1 3 fracadd` | `5 6` | `5 6` | PASS |
| `fractions.dc` | `fracsub` | `3 4 1 4 fracsub` | `1 2` | `1 2` | PASS |
| `fractions.dc` | `fracmul` | `2 3 3 4 fracmul` | `1 2` | `1 2` | PASS |
| `fractions.dc` | `fracdiv` | `1 2 3 4 fracdiv` | `2 3` | `2 3` | PASS |
| `fractions.dc` | `fracmediant` | `1 2 1 3 fracmediant` | `2 5` | `2 5` | PASS |

#### Cautions

- `deprecated_stack.dc` explicitly discourages these traditional stack-dancing helpers; their signatures are documented for compatibility, not endorsement.
- `math.dc` and `fractions.dc` both define `gcd`; import collisions remain a concern.
- Tests execute modules in separate interpreter processes to avoid cross-module state and name collisions.
- The `isneg` implementation uses reciprocal arithmetic; observed cases do not establish behavior for every numeric magnitude or floating-point representation.
- `factorial` with zero previously yielded `0`, so this pass intentionally does not promote a general mathematical contract.
- Existing unverified `stdlib.json` entries remain unverified rather than guessed.



### Notes originally in `STDLIB_RUNTIME_V18.md`

### Standard-library verification — v1.8

This pass executes isolated tests against the bundled dclang runtime, after inspecting the library definitions. Only passing cases promote signatures to runtime-sampled. These are **not** exhaustive tests.

#### Results

Passed **13/14**; promoted **7** signatures.

| Module | Word | Expression | Result |
|---|---|---|---|
| `string.dc` | `strdup` | `"hello" strdup dup strlen swap print cr` | PASS |
| `string.dc` | `strslice` | `"abcdef" 1 4 strslice print cr` | PASS |
| `string.dc` | `strslice` | `"abcdef" 0 0 strslice print cr` | PASS |
| `string.dc` | `strslice` | `"abcdef" 2 6 strslice print cr` | PASS |
| `string.dc` | `strreplace` | `64 mkbuf "banana" "na" "XY" strreplace print cr` | PASS |
| `string.dc` | `strreplace` | `64 mkbuf "abc" "z" "Q" strreplace print cr` | PASS |
| `string.dc` | `strreplace` | `64 mkbuf "abc" "" "Q" strreplace print cr` | PASS |
| `string.dc` | `str_between` | `"m" "a" "z" str_between .s cr` | PASS |
| `string.dc` | `str_between` | `"z" "a" "m" str_between .s cr` | PASS |
| `linked_list.dc` | `linit` | `"1 2 3" "tonum" linit lsize .s cr` | PASS |
| `linked_list.dc` | `lmap` | ` : inc 1 + ; "1 2 3" "tonum" linit "inc" lmap lsize .s cr` | PASS |
| `linked_list.dc` | `lmap` | ` : inc 1 + ; "1 2 3" "tonum" linit "inc" lmap 0 l@ .s cr` | PASS |
| `permutations.dc` | `setup_permutation` | `3 setup_permutation dup @ .s cr` | PASS |
| `shuffle.dc` | `shuffle_cells` | `10 20 30 3 shuffle_cells .s cr` | INVESTIGATE |

#### Important API cautions

- `strslice` uses a **shared 1024-byte module buffer**. Later slices overwrite earlier results. The end index is exclusive in the passing examples. Do not assume bounds checking against input length or slice-buffer capacity.
- `strreplace` requires a caller-allocated destination buffer sized for the result, including its NUL terminator. Its empty-needle branch copies the original string.
- `strdup` allocates a separate string buffer; caller should consider the allocator’s release discipline.
- `linit` uses `strtok` and therefore modifies the input string. Prefer writable memory rather than immutable string literals; the tests are observational and do not prove this safe for all runtimes.
- `lmap` calls `exec` with a word name, so the named word must accept each element and leave one result.
- `setup_permutation` stores a multi-cell object in the language’s data memory; do not confuse this with a heap pointer.
- Failures remain visible in the JSON results and are not promoted to verified signatures.

