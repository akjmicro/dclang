# dclang Standard Library Reference (draft v0.5)

> **Status:** Source inventory from the uploaded `dclang.tgz`. 16 interpreter examples passed across 14 words; this is partial runtime verification, not exhaustive semantic coverage.

**Coverage:** 20 library modules, 329 definition occurrences, 328 distinct names.


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

| Module | Definitions | Imports |
|---|---:|---|
| [`clock_sleep.dc`](#clock_sleepdc) | 2 | — |
| [`csvlib.dc`](#csvlibdc) | 6 | — |
| [`deprecated_stack.dc`](#deprecated_stackdc) | 7 | — |
| [`dsp.dc`](#dspdc) | 64 | — |
| [`fractions.dc`](#fractionsdc) | 7 | — |
| [`http_server.dc`](#http_serverdc) | 23 | `string.dc` |
| [`linked_list.dc`](#linked_listdc) | 8 | — |
| [`logging.dc`](#loggingdc) | 3 | `string.dc` |
| [`math.dc`](#mathdc) | 32 | — |
| [`midi.dc`](#mididc) | 25 | `string.dc` |
| [`music.dc`](#musicdc) | 50 | `math.dc`, `midi.dc`, `osc.dc`, `linked_list.dc`, `music_keyval.dc` |
| [`music_keyval.dc`](#music_keyvaldc) | 29 | `string.dc`, `clock_sleep.dc` |
| [`osc.dc`](#oscdc) | 10 | — |
| [`permutations.dc`](#permutationsdc) | 20 | `math.dc` |
| [`redis.dc`](#redisdc) | 10 | `string.dc` |
| [`regex.dc`](#regexdc) | 4 | `string.dc` |
| [`sequencer.dc`](#sequencerdc) | 5 | — |
| [`shuffle.dc`](#shuffledc) | 2 | — |
| [`sqlite3.dc`](#sqlite3dc) | 7 | — |
| [`string.dc`](#stringdc) | 15 | `deprecated_stack.dc` |

## Name collisions

These names are defined in more than one module; verify import order and redefinition semantics before using them.

- `gcd`: `fractions.dc`, `math.dc`

## Module definitions

### clock_sleep.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_tight_watch` | 9 | `clock_target` | **Unverified** |
| `sleep_until` | 21 | `clock_target` | **Unverified** |

### csvlib.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `csv_open_input_file` | 21 | — | **Unverified** |
| `_csv_init_col_buf` | 28 | — | **Unverified** |
| `csv_next_input_line` | 41 | — | **Unverified** |
| `csv_column_loop` | 56 | — | **Unverified** |
| `csv_find_col_x` | 84 | — | **Unverified** |
| `csv_cleanup` | 90 | — | **Unverified** |

### deprecated_stack.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `nip` | 16 | — | **Unverified** |
| `tuck` | 18 | — | **Unverified** |
| `rot` | 21 | — | **Unverified** |
| `-rot` | 27 | — | **Unverified** |
| `2swap` | 34 | — | **Unverified** |
| `2rot` | 40 | — | **Unverified** |
| `-2rot` | 46 | — | **Unverified** |

### dsp.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `seconds2samples` | 38 | — | **Unverified** |
| `t` | 44 | — | **Unverified** |
| `lcget` | 45 | — | **Unverified** |
| `lcset` | 46 | — | **Unverified** |
| `rcget` | 47 | — | **Unverified** |
| `rcset` | 48 | — | **Unverified** |
| `_clear_channels` | 49 | — | **Unverified** |
| `stereo_in` | 55 | — | **Unverified** |
| `_reset_phasor_count` | 67 | — | **Unverified** |
| `phasor_pointer` | 69 | — | **Unverified** |
| `phasor` | 71 | `freq` | **Unverified** |
| `sawtooth` | 99 | — | **Unverified** |
| `metro` | 107 | — | **Unverified** |
| `gate` | 111 | — | **Unverified** |
| `pulse` | 115 | — | **Unverified** |
| `pulse_tanh` | 117 | — | **Unverified** |
| `tri` | 122 | — | **Unverified** |
| `sine` | 129 | — | **Unverified** |
| `sine_phase` | 132 | `freq`, `phase` | **Unverified** |
| `fm` | 141 | `carrier_freq`, `mod_ratio`, `index` | **Unverified** |
| `sineloop` | 153 | `mem`, `freq`, `fb` | **Unverified** |
| `noise` | 169 | — | **Unverified** |
| `randint` | 177 | `range` | **Unverified** |
| `randtrig` | 184 | `compare_val`, `hold_var` | **Unverified** |
| `make_delay_line` | 204 | `seconds` | **Unverified** |
| `%abs` | 218 | `n`, `d` | **Unverified** |
| `_set_delay_value` | 220 | — | **Unverified** |
| `_get_floor_val` | 227 | — | **Unverified** |
| `_get_ceil_val` | 237 | — | **Unverified** |
| `_interpolate` | 247 | `real_val`, `floor_val`, `ceil_val` | **Unverified** |
| `delay_read` | 255 | `delay_struct`, `time` | **Unverified** |
| `delay_write` | 269 | `sig`, `delay_struct` | **Unverified** |
| `tau_to_alpha` | 286 | `tau` | **Unverified** |
| `port` | 290 | `sig`, `mem`, `rise_alpha`, `fall_alpha` | **Unverified** |
| `lowpass` | 303 | `sig`, `memvar`, `pole` | **Unverified** |
| `make_dcblock` | 315 | — | **Unverified** |
| `dcblock` | 317 | `sig`, `dc_struct` | **Unverified** |
| `make_allpass` | 341 | — | **Unverified** |
| `allpass` | 354 | `sig`, `ap_struct`, `dltm`, `coef` | **Unverified** |
| `_set_bq_type` | 405 | `struct`, `type` | **Unverified** |
| `_set_cf_res_reinit` | 426 | `struct`, `reinit`, `cutoff`, `res` | **Unverified** |
| `_calc_theta` | 432 | `struct` | **Unverified** |
| `_calc_beta` | 439 | `struct`, `tmp` | **Unverified** |
| `_calc_gamma` | 452 | `struct` | **Unverified** |
| `_calc_alpha` | 459 | `struct` | **Unverified** |
| `_reinit_biquad` | 468 | `struct` | **Unverified** |
| `_init_biquad` | 478 | `struct`, `reinit`, `cutoff`, `res`, `type` | **Unverified** |
| `biquad_compute` | 491 | `sig`, `struct` | **Unverified** |
| `make_biquad` | 535 | — | **Unverified** |
| `biquad_lp` | 540 | `sig`, `struct`, `reinit`, `cutoff`, `res` | **Unverified** |
| `biquad_hp` | 549 | `sig`, `struct`, `reinit`, `cutoff`, `res` | **Unverified** |
| `biquad_bp` | 558 | `sig`, `struct`, `reinit`, `cutoff`, `res` | **Unverified** |
| `make_hc_filter` | 581 | — | **Unverified** |
| `_hc_common` | 583 | `sig`, `struct`, `fc`, `res` | **Unverified** |
| `hc_lowpass` | 621 | `sig`, `struct`, `fc`, `res` | **Unverified** |
| `hc_highpass` | 626 | `sig`, `struct`, `fc`, `res` | **Unverified** |
| `hc_bandpass` | 631 | `sig`, `struct`, `fc`, `res` | **Unverified** |
| `hc_notch` | 636 | `sig`, `struct`, `fc`, `res` | **Unverified** |
| `_fix` | 658 | — | **Unverified** |
| `make_reverb_struct` | 660 | — | **Unverified** |
| `reverb` | 694 | `sigL`, `sigR`, `reverb_struct`, `cutoff`, `feedback` | **Unverified** |
| `panmix` | 898 | — | **Unverified** |
| `stereo_out` | 907 | — | **Unverified** |
| `t+` | 914 | — | **Unverified** |

### fractions.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `gcd` | 7 | — | **Unverified** |
| `simplify` | 9 | `n`, `d` | `( n d -- n_reduced d_reduced )` (source-reviewed) |
| `fracadd` | 13 | `n1`, `d1`, `n2`, `d2` | `( n1 d1 n2 d2 -- n d )` (source-reviewed) |
| `fracsub` | 17 | — | **Unverified** |
| `fracmul` | 19 | `n1`, `d1`, `n2`, `d2` | `( n1 d1 n2 d2 -- n d )` (source-reviewed) |
| `fracdiv` | 23 | — | **Unverified** |
| `fracmediant` | 25 | `n1`, `d1`, `n2`, `d2` | `( n1 d1 n2 d2 -- n d )` (source-reviewed) |

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
| `read_request` | 54 | — | **Unverified** |
| `split_request_topline` | 63 | — | **Unverified** |
| `split_path_and_query` | 69 | — | **Unverified** |
| `hash_assign` | 74 | — | **Unverified** |
| `_split_query_string_helper` | 86 | — | **Unverified** |
| `split_query_string` | 96 | — | **Unverified** |
| `process_request` | 117 | — | **Unverified** |
| `_print_hash_table_helper` | 123 | — | **Unverified** |
| `print_hash_table` | 142 | — | **Unverified** |
| `copy_header_to_buffer` | 149 | — | **Unverified** |
| `copy_footer_to_buffer` | 155 | — | **Unverified** |
| `write_response` | 159 | — | **Unverified** |
| `_acceptloop` | 170 | — | **Unverified** |
| `acceptloop` | 177 | — | **Unverified** |

### linked_list.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_list_init_helper` | 4 | `list_obj`, `token_method` | **Unverified** |
| `_lmap_helper` | 16 | — | **Unverified** |
| `_lshow_helper` | 30 | — | **Unverified** |
| `linit` | 45 | — | **Unverified** |
| `lmap` | 67 | — | **Unverified** |
| `lshow` | 82 | — | **Unverified** |
| `make_lgen_struct` | 92 | — | **Unverified** |
| `lgen` | 99 | `lgen_struct`, `infinite?` | **Unverified** |

### logging.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `open_log` | 11 | — | **Unverified** |
| `to_log` | 17 | — | **Unverified** |
| `close_log` | 31 | — | **Unverified** |

### math.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `isneg` | 3 | — | `( x -- flag )` (source-reviewed) |
| `absmod` | 5 | — | `( a b -- remainder )` (source-reviewed) |
| `divmod` | 16 | — | `( dividend divisor -- quotient remainder )` (source-reviewed) |
| `bounce` | 21 | `n`, `lower`, `upper` | **Unverified** |
| `?dup` | 36 | — | `( x -- x [x if nonzero] )` (source-reviewed) |
| `gcd` | 41 | — | **Unverified** |
| `factorial` | 51 | `x` | **Unverified** |
| `_digit_sum_helper` | 58 | — | **Unverified** |
| `digit_sum` | 74 | — | **Unverified** |
| `make_digit_expansion_struct` | 90 | — | **Unverified** |
| `_get_base` | 92 | — | **Unverified** |
| `_set_base` | 93 | — | **Unverified** |
| `_get_div` | 94 | — | **Unverified** |
| `_set_div` | 95 | — | **Unverified** |
| `_get_old_base` | 96 | — | **Unverified** |
| `_set_old_base` | 97 | — | **Unverified** |
| `_get_old_div` | 98 | — | **Unverified** |
| `_set_old_div` | 99 | — | **Unverified** |
| `_get_remainder` | 100 | — | **Unverified** |
| `_set_remainder` | 101 | — | **Unverified** |
| `_base_changed?` | 102 | — | **Unverified** |
| `_reset_old_base` | 103 | — | **Unverified** |
| `_div_changed?` | 104 | — | **Unverified** |
| `_reset_old_div` | 105 | — | **Unverified** |
| `gen_digit_expansion` | 107 | `exp_struct` | **Unverified** |
| `is-2-3-5-7?` | 131 | — | **Unverified** |
| `check-higher-than-49` | 143 | — | **Unverified** |
| `is-not-div-otherwise?` | 156 | — | **Unverified** |
| `is-not-div-by-2-3-5-7?` | 165 | — | **Unverified** |
| `is-prime` | 182 | — | **Unverified** |
| `find-next-prime` | 194 | `direction`, `x` | **Unverified** |
| `closest-prime` | 203 | `x` | **Unverified** |

### midi.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `populate_midi_channel` | 11 | — | **Unverified** |
| `make_channel_string` | 33 | `prefix` | **Unverified** |
| `midi_shutdown` | 48 | — | **Unverified** |
| `check_mididev` | 53 | — | **Unverified** |
| `refresh_midictl` | 92 | — | **Unverified** |
| `midictl` | 104 | — | **Unverified** |
| `send_midi` | 107 | — | **Unverified** |
| `send_midi_reverse` | 109 | — | **Unverified** |
| `panic` | 112 | — | **Unverified** |
| `_bend_to_MSB_LSB_bytes` | 131 | — | **Unverified** |
| `_read_midi_byte` | 170 | `mfp` | **Unverified** |
| `_read_var_length` | 176 | — | **Unverified** |
| `read_var_length` | 193 | `mfp` | **Unverified** |
| `_read_chunk_length` | 197 | — | **Unverified** |
| `read_midi_header` | 204 | `mfp` | **Unverified** |
| `_output_as_hex` | 241 | — | **Unverified** |
| `_handle_sysex` | 245 | `mfp` | **Unverified** |
| `_handle_track_name` | 254 | `mfp` | **Unverified** |
| `_handle_tempo` | 263 | `mfp` | **Unverified** |
| `_handle_timesig` | 274 | `mfp` | **Unverified** |
| `_handle_meta` | 284 | `meta_byte`, `mfp` | **Unverified** |
| `_handle_status` | 309 | `status_byte`, `mfp` | **Unverified** |
| `_track_loop` | 353 | `mfp` | **Unverified** |
| `read_midi_track` | 407 | `mfp` | **Unverified** |
| `read_midi_file` | 427 | `midi_filename` | **Unverified** |

### music.dc

**Imports:** `math.dc`, `midi.dc`, `osc.dc`, `linked_list.dc`, `music_keyval.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `set_bend_depth` | 19 | — | **Unverified** |
| `O-` | 31 | — | **Unverified** |
| `O+` | 32 | — | **Unverified** |
| `_reset_gamut_idx` | 231 | — | **Unverified** |
| `_update_gamut_idx` | 232 | — | **Unverified** |
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
| `euclidean_rhythm` | 422 | — | **Unverified** |
| `euclidean_test_loop` | 448 | — | **Unverified** |
| `_string_to_lgen` | 467 | `str_to_check`, `cur_str`, `lgen_struct`, `method` | **Unverified** |
| `_token_has_backslash?` | 517 | — | **Unverified** |
| `_process_rhythm` | 519 | `tok` | **Unverified** |
| `_do_rhythm_string` | 535 | — | **Unverified** |
| `_do_gate_string` | 544 | — | **Unverified** |
| `rhythm_generator` | 554 | — | **Unverified** |
| `data_player` | 575 | — | **Unverified** |
| `fraction_music` | 608 | `expansion_struct` | **Unverified** |
| `digit_sum_music` | 619 | — | **Unverified** |
| `primes_music` | 639 | — | **Unverified** |
| `do_attack_string` | 662 | — | **Unverified** |
| `do_gain_string` | 676 | — | **Unverified** |
| `_get_edo_3_2` | 720 | `local_edo` | **Unverified** |
| `_update_note_symbol_tree` | 724 | `local_edo` | **Unverified** |
| `symbol_to_edo_int` | 752 | `pitch_token` | **Unverified** |
| `_reset_sym_str_ptr` | 777 | — | **Unverified** |
| `_process_next_symbol` | 781 | — | **Unverified** |
| `do_symbol_string` | 811 | — | **Unverified** |
| `_reset_seq_str_ptr` | 863 | — | **Unverified** |
| `_ratio_to_edo_delta` | 867 | `num`, `den` | **Unverified** |
| `_process_keyval_token` | 877 | `tok` | **Unverified** |
| `_process_duration_token` | 883 | `tok` | **Unverified** |
| `_parse_abs_ratio_token` | 892 | `tok` | **Unverified** |
| `_parse_rel_ratio_token` | 898 | `tok` | **Unverified** |
| `_parse_rel_ratio_token_silent` | 907 | `tok` | **Unverified** |
| `_token_has_equals?` | 914 | — | **Unverified** |
| `_token_has_semicolon?` | 915 | — | **Unverified** |
| `_token_has_colon?` | 916 | — | **Unverified** |
| `_token_has_slash?` | 917 | — | **Unverified** |
| `_token_has_letter?` | 918 | — | **Unverified** |
| `_copy_next_token` | 920 | — | **Unverified** |
| `_process_next_event` | 956 | — | **Unverified** |
| `do_seq_string` | 1034 | — | **Unverified** |

### music_keyval.dc

**Imports:** `string.dc`, `clock_sleep.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `keyval_get` | 21 | `key` | **Unverified** |
| `keyval_set` | 33 | `val`, `key` | **Unverified** |
| `tonum_free` | 49 | — | **Unverified** |
| `_get_duration` | 106 | — | **Unverified** |
| `synchronized_start` | 114 | — | **Unverified** |
| `get_x_tempo` | 128 | `which_str` | **Unverified** |
| `get_drum_vol` | 136 | — | **Unverified** |
| `get_bass_vol` | 140 | — | **Unverified** |
| `get_lead_vol` | 144 | — | **Unverified** |
| `set_clock_start` | 161 | — | **Unverified** |
| `_iterate_clock` | 165 | `length`, `gate` | **Unverified** |
| `get_on_gate` | 178 | `length`, `gate` | **Unverified** |
| `get_off_gate` | 183 | — | **Unverified** |
| `get_mode` | 191 | — | **Unverified** |
| `get_transpose` | 195 | — | **Unverified** |
| `get_base_pitch` | 203 | — | **Unverified** |
| `get_edo` | 214 | — | **Unverified** |
| `get_scale` | 225 | — | **Unverified** |
| `get_gamut` | 237 | — | **Unverified** |
| `get_rhythm_type` | 249 | — | **Unverified** |
| `get_remainder` | 267 | — | **Unverified** |
| `set_remainder` | 268 | — | **Unverified** |
| `get_base` | 270 | — | **Unverified** |
| `get_div` | 275 | — | **Unverified** |
| `get_mul` | 280 | — | **Unverified** |
| `get_chaos` | 285 | — | **Unverified** |
| `get_wrap` | 289 | — | **Unverified** |
| `get_euclid_n` | 293 | — | **Unverified** |
| `get_euclid_d` | 298 | — | **Unverified** |

### osc.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `osc_connect` | 16 | `host`, `port` | **Unverified** |
| `osc_message` | 21 | `path`, `types` | **Unverified** |
| `osc_s` | 27 | `strval` | **Unverified** |
| `osc_i` | 31 | `val` | **Unverified** |
| `osc_f` | 35 | `val` | **Unverified** |
| `osc_send` | 39 | — | **Unverified** |
| `osc_close` | 43 | — | **Unverified** |
| `_make_path` | 57 | `path_prefix`, `path_suffix` | **Unverified** |
| `osc_synth_note_on` | 69 | `path_prefix`, `freq`, `gain` | **Unverified** |
| `osc_synth_note_off` | 81 | `path_prefix` | **Unverified** |

### permutations.dc

**Imports:** `math.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `setup_permutation` | 28 | — | **Unverified** |
| `show_permutation_data` | 35 | — | **Unverified** |
| `show_permutation` | 41 | — | **Unverified** |
| `_return_current_cell` | 48 | — | **Unverified** |
| `_advance_output_index` | 52 | — | **Unverified** |
| `_needs_permuting?` | 56 | — | **Unverified** |
| `_get_perm_val_i` | 57 | — | **Unverified** |
| `_set_perm_val_i` | 58 | — | **Unverified** |
| `_get_curr_high_val` | 59 | — | **Unverified** |
| `_set_curr_high_val` | 60 | — | **Unverified** |
| `_get_curr_high_slot` | 61 | — | **Unverified** |
| `_set_curr_high_slot` | 62 | — | **Unverified** |
| `_common_compare` | 64 | — | **Unverified** |
| `_set_largest_mobile_cell` | 78 | — | **Unverified** |
| `_setup_swap` | 109 | — | **Unverified** |
| `_pmswap` | 123 | — | **Unverified** |
| `_swap_neighbor_cells` | 125 | — | **Unverified** |
| `_reverse_direction_of_greater_values` | 130 | — | **Unverified** |
| `next_permutation_item` | 148 | — | **Unverified** |
| `print_next_permutation` | 162 | — | **Unverified** |

### redis.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_zero_redis_buffer` | 44 | — | **Unverified** |
| `_write_redis_buffer` | 48 | — | **Unverified** |
| `_read_redis_buffer` | 57 | — | **Unverified** |
| `_redis_write_read` | 66 | — | **Unverified** |
| `_redis_auth` | 71 | — | **Unverified** |
| `redis_connect` | 94 | — | **Unverified** |
| `redis_set` | 116 | — | **Unverified** |
| `redis_get` | 139 | — | **Unverified** |
| `redis_keys` | 160 | — | **Unverified** |
| `redis_del` | 179 | — | **Unverified** |

### regex.dc

**Imports:** `string.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_match` | 51 | — | **Unverified** |
| `_derive_real_needle_and_flags` | 78 | `needle_with_options` | **Unverified** |
| `match` | 98 | — | **Unverified** |
| `matches?` | 112 | — | **Unverified** |

### sequencer.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_seq_ptr_incr` | 9 | `seq` | **Unverified** |
| `_seq_cur_val` | 17 | `seq` | **Unverified** |
| `seq_cnt_incr` | 21 | `seq` | **Unverified** |
| `seq_signal` | 43 | `seq` | **Unverified** |
| `iterator` | 64 | `iter_array` | **Unverified** |

### shuffle.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `_cell_swap` | 1 | — | **Unverified** |
| `shuffle_cells` | 5 | — | **Unverified** |

### sqlite3.dc

**Imports:** none detected via literal imports.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `sqlite3_open` | 1 | — | **Unverified** |
| `sqlite3_prepare` | 10 | — | **Unverified** |
| `sqlite3_step` | 19 | — | **Unverified** |
| `sqlite3_column` | 28 | — | **Unverified** |
| `sqlite3_finalize` | 37 | — | **Unverified** |
| `sqlite3_exec` | 46 | — | **Unverified** |
| `sqlite3_close` | 55 | — | **Unverified** |

### string.dc

**Imports:** `deprecated_stack.dc`.

| Word | Source line | Locals | Stack effect / status |
|---|---:|---|---|
| `strdup` | 37 | — | `( string_addr -- copied_string_addr )` (source-reviewed) |
| `str+` | 46 | — | **Unverified** |
| `strslice` | 53 | — | **Unverified** |
| `str_between` | 97 | — | **Unverified** |
| `_strreplace_helper` | 110 | `buf`, `s`, `find`, `repl` | **Unverified** |
| `strreplace` | 127 | `buf`, `s`, `find`, `repl` | **Unverified** |
| `_get_next_num_from_line` | 138 | — | **Unverified** |
| `next_num_from_file` | 159 | — | **Unverified** |
| `uuid` | 194 | — | **Unverified** |
| `hex8token` | 238 | — | **Unverified** |
| `_str_split_real` | 259 | `in_str`, `split_token` | **Unverified** |
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

