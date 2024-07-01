//   ____  __   __        ______        __
//  / __ \/ /__/ /__ ___ /_  __/__ ____/ /
// / /_/ / / _  / -_|_-<_ / / / -_) __/ _ \
// \____/_/\_,_/\__/___(@)_/  \__/\__/_// /
//  ~~~ oldes.huhuman at gmail.com ~~~ /_/
//
// Project: Rebol/AOMedia extension
// SPDX-License-Identifier: MIT
// =============================================================================
// NOTE: auto-generated file, do not modify!

#include "rebol-extension.h"
#include "aomedia-common.h"

#define SERIES_TEXT(s)   ((char*)SERIES_DATA(s))

#define MIN_REBOL_VER 3
#define MIN_REBOL_REV 14
#define MIN_REBOL_UPD 1
#define VERSION(a, b, c) (a << 16) + (b << 8) + c
#define MIN_REBOL_VERSION VERSION(MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD)

extern REBCNT Handle_AV1Encoder;

extern u32* arg_words;
extern u32* type_words;

enum ext_commands {
	CMD_MINIAUDIO_INIT_WORDS,
	CMD_MINIAUDIO_TEST,
	CMD_MINIAUDIO_AOM_ENCODER,
	CMD_MINIAUDIO_ENCODE_FRAME,
};


int cmd_init_words(RXIFRM *frm, void *ctx);
int cmd_test(RXIFRM *frm, void *ctx);
int cmd_aom_encoder(RXIFRM *frm, void *ctx);
int cmd_encode_frame(RXIFRM *frm, void *ctx);

enum ma_arg_words {W_ARG_0,
	W_ARG_USAGE,
	W_ARG_RATE_CONTROL,
	W_ARG_CONSTANT_QUALITY,
	W_ARG_G_USAGE,
	W_ARG_G_THREADS,
	W_ARG_G_PROFILE,
	W_ARG_G_BIT_DEPTH,
	W_ARG_G_INPUT_BIT_DEPTH,
	W_ARG_G_TIMEBASE_NUM,
	W_ARG_G_TIMEBASE_DEN,
	W_ARG_G_ERROR_RESILIENT,
	W_ARG_G_PASS,
	W_ARG_G_LAG_IN_FRAMES,
	W_ARG_RC_DROPFRAME_THRESH,
	W_ARG_RC_RESIZE_MODE,
	W_ARG_RC_RESIZE_DENOMINATOR,
	W_ARG_RC_RESIZE_KF_DENOMINATOR,
	W_ARG_RC_SUPERRES_MODE,
	W_ARG_RC_SUPERRES_DENOMINATOR,
	W_ARG_RC_SUPERRES_KF_DENOMINATOR,
	W_ARG_RC_SUPERRES_QTHRESH,
	W_ARG_RC_SUPERRES_KF_QTHRESH,
	W_ARG_RC_END_USAGE,
	W_ARG_RC_TARGET_BITRATE,
	W_ARG_RC_MIN_QUANTIZER,
	W_ARG_RC_MAX_QUANTIZER,
	W_ARG_RC_UNDERSHOOT_PCT,
	W_ARG_RC_OVERSHOOT_PCT,
	W_ARG_RC_BUF_SZ,
	W_ARG_RC_BUF_INITIAL_SZ,
	W_ARG_RC_BUF_OPTIMAL_SZ,
	W_ARG_RC_2PASS_VBR_BIAS_PCT,
	W_ARG_RC_2PASS_VBR_MINSECTION_PCT,
	W_ARG_RC_2PASS_VBR_MAXSECTION_PCT,
	W_ARG_FWD_KF_ENABLED,
	W_ARG_KF_MODE,
	W_ARG_KF_MIN_DIST,
	W_ARG_KF_MAX_DIST,
	W_ARG_SFRAME_DIST,
	W_ARG_SFRAME_MODE,
	W_ARG_LARGE_SCALE_TILE,
	W_ARG_MONOCHROME,
	W_ARG_BUFFER,
	W_ARG_HEADER
};
enum ma_type_words {W_TYPE_0
};

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);

#define AOMEDIA_EXT_INIT_CODE \
	"REBOL [Title: {Rebol AOMedia Extension} Type: module Version: 0.0.0.1 Needs: 3.14.1 Home:  https://github.com/Oldes/Rebol-AOMedia ]\n"\
	"init-words: command [args [block!] type [block!]]\n"\
	"test: command [\"Just an example\"]\n"\
	"aom-encoder: command [\"Initialize a new AV1 Encoder\" size [pair!] \"Size of the output\" /with config [block!]]\n"\
	"encode-frame: command [\"Encode an image into a AV1Encoder object\" encoder [handle!] {AV1Encoder object to which the frame is to be added} image [image! none!] {Rebol image to be added. Use `none` to finish the stream.}]\n"\
	"init-words [usage: rate-control: constant-quality: g_usage: g_threads: g_profile: g_bit_depth: g_input_bit_depth: g_timebase_num: g_timebase_den: g_error_resilient: g_pass: g_lag_in_frames: rc_dropframe_thresh: rc_resize_mode: rc_resize_denominator: rc_resize_kf_denominator: rc_superres_mode: rc_superres_denominator: rc_superres_kf_denominator: rc_superres_qthresh: rc_superres_kf_qthresh: rc_end_usage: rc_target_bitrate: rc_min_quantizer: rc_max_quantizer: rc_undershoot_pct: rc_overshoot_pct: rc_buf_sz: rc_buf_initial_sz: rc_buf_optimal_sz: rc_2pass_vbr_bias_pct: rc_2pass_vbr_minsection_pct: rc_2pass_vbr_maxsection_pct: fwd_kf_enabled: kf_mode: kf_min_dist: kf_max_dist: sframe_dist: sframe_mode: large_scale_tile: monochrome: buffer header][]\n"\
	"protect/hide 'init-words\n"\
	"\n"\
	"AOM_USAGE_GOOD_QUALITY: 0 ;; usage parameter analogous to AV1 GOOD QUALITY mode\n"\
	"AOM_USAGE_REALTIME:     1 ;; usage parameter analogous to AV1 REALTIME mode\n"\
	"AOM_USAGE_ALL_INTRA:    2 ;; usage parameter analogous to AV1 all intra mode\n"\
	"\n"\
	";; Rate control mode\n"\
	"AOM_VBR: 0  ;; Variable Bit Rate\n"\
	"AOM_CBR: 1  ;; Constant Bit Rate\n"\
	"AOM_CQ:  2  ;; Constrained Quality\n"\
	"AOM_Q:   3  ;; Constant Quality\n"\
	"\n"\
	";; Frame super-resolution mode\n"\
	"AOM_SUPERRES_NONE:    0 ;; Frame super-resolution is disabled for all frames\n"\
	"AOM_SUPERRES_FIXED:   1 ;; All frames are coded at the specified scale and super-resolved\n"\
	"AOM_SUPERRES_RANDOM:  2 ;; All frames are coded at a random scale and super-resolved\n"\
	"AOM_SUPERRES_QTHRESH: 3 ;; Super-resolution scale for each frame is determined based on the q index of that frame\n"\
	"AOM_SUPERRES_AUTO:    4 ;; Full-resolution or super-resolution and the scale (in case of super-resolution) are automatically selected for each frame\n"\
	"\n"\
	"config-options: [\n"\
	"	;- Encoder options\n"\
	"	usage: integer!   \"AOM_USAGE_GOOD_QUALITY, AOM_USAGE_REALTIME or AOM_USAGE_ALL_INTRA\"\n"\
	"	rate-control: integer! \"0-3\"\n"\
	"	constant-quality:            integer! \"Ensure that every frame gets the number of bits it deserves to achieve a certain (perceptual) quality level, rather than encoding each frame to meet a bit rate target\"\n"\
	"	\n"\
	"	;- Generic settings\n"\
	"	g_usage:                     integer! \"Algorithm specific `usage` value\"\n"\
	"	g_threads:                   integer! \"Maximum number of threads to use\"\n"\
	"	g_profile:                   integer! \"Bitstream profile to use\"\n"\
	"	g_bit_depth:                 integer! \"Bit-depth of the codec\"\n"\
	"	g_input_bit_depth:           integer! \"Bit-depth of the input frames\"\n"\
	"	g_timebase_num:              integer! \"The smallest interval of time, in seconds (numerator part)\"\n"\
	"	g_timebase_den:              integer! \"The smallest interval of time, in seconds (denominator part)\"\n"\
	"	g_error_resilient:           integer! \"Which features the encoder should enable to take measures for streaming over lossy or noisy links\"\n"\
	"	g_pass:                      integer! \"Multi-pass Encoding Mode\"\n"\
	"	g_lag_in_frames:             integer! \"If set, this value allows the encoder to consume a number of input frames before producing output frames\"\n"\
	"\n"\
	"	;- Rate control settings (rc)\n"\
	"	rc_dropframe_thresh:         integer! \"Temporal resampling allows the codec to `drop` frames as a strategy to meet its target data rate\"\n"\
	"	rc_resize_mode:              integer! \"Spatial resampling allows the codec to compress a lower resolution version of the frame, which is then upscaled by the decoder to the correct presentation resolution\"\n"\
	"	rc_resize_denominator:       integer! \"The denominator for resize (frame) to use, assuming 8 as the numerator. (8-16)\"\n"\
	"	rc_resize_kf_denominator:    integer! \"The denominator for resize (key frame) to use, assuming 8 as the numerator. (8-16)\"\n"\
	"	rc_superres_mode:            integer! \"Frame super-resolution scaling mode (AOM_SUPERRES_*)\"\n"\
	"	rc_superres_denominator:     integer! \"Frame super-resolution denominator (8-16)\"\n"\
	"	rc_superres_kf_denominator:  integer! \"Keyframe super-resolution denominator (8-16)\"\n"\
	"	rc_superres_qthresh:         integer! \"The q level threshold after which superres is used (1-63) Used only by AOM_SUPERRES_QTHRESH\"\n"\
	"	rc_superres_kf_qthresh:      integer! \"The q level threshold after which superres is used for keyframes(1-63) Used only by AOM_SUPERRES_QTHRESH\"\n"\
	"	rc_end_usage:                integer! \"Rate control algorithm to use (0-3)\"\n"\
	"	rc_target_bitrate:           integer! \"Target bitrate to use for this stream, in kilobits per second (max 2000000)\"\n"\
	"\n"\
	"	;- Quantizer settings\n"\
	"	rc_min_quantizer:            integer! \"Minimum (Best Quality) Quantizer\"\n"\
	"	rc_max_quantizer:            integer! \"Maximum (Worst Quality) Quantizer\"\n"\
	"\n"\
	"	;- Bitrate tolerance\n"\
	"	rc_undershoot_pct:           integer! \"Rate control adaptation undershoot control (0-100)\"\n"\
	"	rc_overshoot_pct:            integer! \"Rate control adaptation overshoot control (0-100)\"\n"\
	"\n"\
	"	;- Decoder buffer model parameters\n"\
	"	rc_buf_sz:                   integer! \"The amount of data that may be buffered by the decoding application in milliseconds\"\n"\
	"	rc_buf_initial_sz:           integer! \"The amount of data that will be buffered by the decoding application prior to beginning playback\"\n"\
	"	rc_buf_optimal_sz:           integer! \"The amount of data that the encoder should try to maintain in the decoder's buffer\"\n"\
	"\n"\
	"	;- 2 pass rate control parameters\n"\
	"	rc_2pass_vbr_bias_pct:       integer! \"CBR/VBR bias, expressed on a scale of 0 to 100, for determining target size for the current frame\"\n"\
	"	rc_2pass_vbr_minsection_pct: integer! \"This value, expressed as a percentage of the target bitrate, indicates the minimum bitrate to be used for a single GOP (aka `section`)\"\n"\
	"	rc_2pass_vbr_maxsection_pct: integer! \"This value, expressed as a percentage of the target bitrate, indicates the maximum bitrate to be used for a single GOP\"\n"\
	"	\n"\
	"	;- keyframing settings (kf)\n"\
	"	fwd_kf_enabled:              integer! \"Option to enable forward reference key frame\"\n"\
	"	kf_mode:                     integer! \"This value indicates whether the encoder should place keyframes at a fixed interval (0), or determine the optimal placement automatically (1)\"\n"\
	"	kf_min_dist:                 integer! \"This value, expressed as a number of frames, prevents the encoder from placing a keyframe nearer than kf_min_dist to the previous keyframe\"\n"\
	"	kf_max_dist:                 integer! \"This value, expressed as a number of frames, forces the encoder to code a keyframe if one has not been coded in the last kf_max_dist frames\"\n"\
	"	sframe_dist:                 integer! \"This value, expressed as a number of frames, forces the encoder to code an S-Frame every sframe_dist frames\"\n"\
	"	sframe_mode:                 integer! \"1 = the considered frame will be made into an S-Frame only if it is an altref frame; 2 = the next altref frame will be made into an S-Frame\"\n"\
	"	large_scale_tile:            integer! \"A value of 0 implies a normal non-large-scale tile coding. A value of 1 implies a large-scale tile coding.\"\n"\
	"	monochrome:                  integer! \"If this is nonzero, the encoder will generate a monochrome stream with no chroma planes\"\n"\
	"]\n"\
	"\n"

#ifdef  USE_TRACES
#include <stdio.h>
#define debug_print(fmt, ...) do { printf(fmt, __VA_ARGS__); } while (0)
#define trace(str) puts(str)
#else
#define debug_print(fmt, ...)
#define trace(str) 
#endif

