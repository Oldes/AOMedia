[![Rebol-AOMedia CI](https://github.com/Oldes/Rebol-AOMedia/actions/workflows/master.yml/badge.svg)](https://github.com/Oldes/Rebol-AOMedia/actions/workflows/master.yml)

# Rebol/AOMedia

AOMedia extension for [Rebol3](https://github.com/Oldes/Rebol3) (version 3.14.1 and higher)

## Usage
```rebol
aomedia: import aomedia

```

## Extension commands:


#### `test`
Just an example

#### `aom-encoder` `:size`
Initialize a new AV1 Encoder
* `size` `[pair!]` Size of the output
* `/with`
* `config` `[block!]`

#### `encode-frame` `:encoder` `:image`
Encode an image into a AV1Encoder object
* `encoder` `[handle!]` AV1Encoder object to which the frame is to be added
* `image` `[image! none!]` Rebol image to be added. Use `none` to finish the stream.


## Used handles and its getters / setters

#### __AOMEDIA-HANDLE__ - AOMedia handle

```rebol
;Refinement       Gets                Sets                          Description
/buffer           binary!             none                          "Output buffer with encoded data"
/header           binary!             none                          "obu_sequence_header"
```


## Other extension values:
```rebol
AOM_USAGE_GOOD_QUALITY: 0 ;; usage parameter analogous to AV1 GOOD QUALITY mode
AOM_USAGE_REALTIME:     1 ;; usage parameter analogous to AV1 REALTIME mode
AOM_USAGE_ALL_INTRA:    2 ;; usage parameter analogous to AV1 all intra mode

;; Rate control mode
AOM_VBR: 0  ;; Variable Bit Rate
AOM_CBR: 1  ;; Constant Bit Rate
AOM_CQ:  2  ;; Constrained Quality
AOM_Q:   3  ;; Constant Quality

;; Frame super-resolution mode
AOM_SUPERRES_NONE:    0 ;; Frame super-resolution is disabled for all frames
AOM_SUPERRES_FIXED:   1 ;; All frames are coded at the specified scale and super-resolved
AOM_SUPERRES_RANDOM:  2 ;; All frames are coded at a random scale and super-resolved
AOM_SUPERRES_QTHRESH: 3 ;; Super-resolution scale for each frame is determined based on the q index of that frame
AOM_SUPERRES_AUTO:    4 ;; Full-resolution or super-resolution and the scale (in case of super-resolution) are automatically selected for each frame

config-options: [
	;- Encoder options
	usage: integer!   "AOM_USAGE_GOOD_QUALITY, AOM_USAGE_REALTIME or AOM_USAGE_ALL_INTRA"
	rate-control: integer! "0-3"
	constant-quality:            integer! "Ensure that every frame gets the number of bits it deserves to achieve a certain (perceptual) quality level, rather than encoding each frame to meet a bit rate target"
	
	;- Generic settings
	g_usage:                     integer! "Algorithm specific `usage` value"
	g_threads:                   integer! "Maximum number of threads to use"
	g_profile:                   integer! "Bitstream profile to use"
	g_bit_depth:                 integer! "Bit-depth of the codec"
	g_input_bit_depth:           integer! "Bit-depth of the input frames"
	g_timebase_num:              integer! "The smallest interval of time, in seconds (numerator part)"
	g_timebase_den:              integer! "The smallest interval of time, in seconds (denominator part)"
	g_error_resilient:           integer! "Which features the encoder should enable to take measures for streaming over lossy or noisy links"
	g_pass:                      integer! "Multi-pass Encoding Mode"
	g_lag_in_frames:             integer! "If set, this value allows the encoder to consume a number of input frames before producing output frames"

	;- Rate control settings (rc)
	rc_dropframe_thresh:         integer! "Temporal resampling allows the codec to `drop` frames as a strategy to meet its target data rate"
	rc_resize_mode:              integer! "Spatial resampling allows the codec to compress a lower resolution version of the frame, which is then upscaled by the decoder to the correct presentation resolution"
	rc_resize_denominator:       integer! "The denominator for resize (frame) to use, assuming 8 as the numerator. (8-16)"
	rc_resize_kf_denominator:    integer! "The denominator for resize (key frame) to use, assuming 8 as the numerator. (8-16)"
	rc_superres_mode:            integer! "Frame super-resolution scaling mode (AOM_SUPERRES_*)"
	rc_superres_denominator:     integer! "Frame super-resolution denominator (8-16)"
	rc_superres_kf_denominator:  integer! "Keyframe super-resolution denominator (8-16)"
	rc_superres_qthresh:         integer! "The q level threshold after which superres is used (1-63) Used only by AOM_SUPERRES_QTHRESH"
	rc_superres_kf_qthresh:      integer! "The q level threshold after which superres is used for keyframes(1-63) Used only by AOM_SUPERRES_QTHRESH"
	rc_end_usage:                integer! "Rate control algorithm to use (0-3)"
	rc_target_bitrate:           integer! "Target bitrate to use for this stream, in kilobits per second (max 2000000)"

	;- Quantizer settings
	rc_min_quantizer:            integer! "Minimum (Best Quality) Quantizer"
	rc_max_quantizer:            integer! "Maximum (Worst Quality) Quantizer"

	;- Bitrate tolerance
	rc_undershoot_pct:           integer! "Rate control adaptation undershoot control (0-100)"
	rc_overshoot_pct:            integer! "Rate control adaptation overshoot control (0-100)"

	;- Decoder buffer model parameters
	rc_buf_sz:                   integer! "The amount of data that may be buffered by the decoding application in milliseconds"
	rc_buf_initial_sz:           integer! "The amount of data that will be buffered by the decoding application prior to beginning playback"
	rc_buf_optimal_sz:           integer! "The amount of data that the encoder should try to maintain in the decoder's buffer"

	;- 2 pass rate control parameters
	rc_2pass_vbr_bias_pct:       integer! "CBR/VBR bias, expressed on a scale of 0 to 100, for determining target size for the current frame"
	rc_2pass_vbr_minsection_pct: integer! "This value, expressed as a percentage of the target bitrate, indicates the minimum bitrate to be used for a single GOP (aka `section`)"
	rc_2pass_vbr_maxsection_pct: integer! "This value, expressed as a percentage of the target bitrate, indicates the maximum bitrate to be used for a single GOP"
	
	;- keyframing settings (kf)
	fwd_kf_enabled:              integer! "Option to enable forward reference key frame"
	kf_mode:                     integer! "This value indicates whether the encoder should place keyframes at a fixed interval (0), or determine the optimal placement automatically (1)"
	kf_min_dist:                 integer! "This value, expressed as a number of frames, prevents the encoder from placing a keyframe nearer than kf_min_dist to the previous keyframe"
	kf_max_dist:                 integer! "This value, expressed as a number of frames, forces the encoder to code a keyframe if one has not been coded in the last kf_max_dist frames"
	sframe_dist:                 integer! "This value, expressed as a number of frames, forces the encoder to code an S-Frame every sframe_dist frames"
	sframe_mode:                 integer! "1 = the considered frame will be made into an S-Frame only if it is an altref frame; 2 = the next altref frame will be made into an S-Frame"
	large_scale_tile:            integer! "A value of 0 implies a normal non-large-scale tile coding. A value of 1 implies a large-scale tile coding."
	monochrome:                  integer! "If this is nonzero, the encoder will generate a monochrome stream with no chroma planes"
]
```
