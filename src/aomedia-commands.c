//   ____  __   __        ______        __
//  / __ \/ /__/ /__ ___ /_  __/__ ____/ /
// / /_/ / / _  / -_|_-<_ / / / -_) __/ _ \
// \____/_/\_,_/\__/___(@)_/  \__/\__/_// /
//  ~~~ oldes.huhuman at gmail.com ~~~ /_/
//
// Project: Rebol/AOMedia extension
// SPDX-License-Identifier: MIT
// =============================================================================

#include "aomedia-rebol-extension.h"
#include <stdio.h>
#include <libyuv/convert.h>
#include <stdlib.h> // malloc


#define COMMAND int

#define FRM_IS_HANDLE(n, t)     (RXA_TYPE(frm,n) == RXT_HANDLE && RXA_HANDLE_TYPE(frm, n) == t && IS_USED_HOB(RXA_HANDLE_CONTEXT(frm, n)))
#define ARG_Is_None(n)          (RXA_TYPE(frm,n) == RXT_NONE)
#define ARG_Is_Word(n)          (RXA_TYPE(frm,n) == RXT_WORD)
#define ARG_Is_Block(n)         (RXA_TYPE(frm,n) == RXT_BLOCK)
#define ARG_Is_AV1Encoder(n)    FRM_IS_HANDLE(n, Handle_AV1Encoder)
#define ARG_AV1Encoder(n)      ((AOMEncoderContext*)RXA_HANDLE_CONTEXT(frm, n)->data)
#define ARG_AV1Encoder_Series(n) RXA_HANDLE_CONTEXT(frm, n)->series



#define RETURN_HANDLE(hob)                   \
	RXA_HANDLE(frm, 1)       = hob;          \
	RXA_HANDLE_TYPE(frm, 1)  = hob->sym;     \
	RXA_HANDLE_FLAGS(frm, 1) = hob->flags;   \
	RXA_TYPE(frm, 1) = RXT_HANDLE;           \
	return RXR_VALUE

#define APPEND_STRING(str, ...) \
	len = snprintf(NULL,0,__VA_ARGS__);\
	if (len > (int)(SERIES_REST(str)-SERIES_LEN(str))) {\
		RL_EXPAND_SERIES(str, SERIES_TAIL(str), len);\
		SERIES_TAIL(str) -= len;\
	}\
	len = snprintf( \
		SERIES_TEXT(str)+SERIES_TAIL(str),\
		SERIES_REST(str)-SERIES_TAIL(str),\
		__VA_ARGS__\
	);\
	SERIES_TAIL(str) += len;

#define RETURN_ERROR(err)  do {RXA_SERIES(frm, 1) = err; return RXR_ERROR;} while(0)

static REBOOL fetch_word(REBSER *cmds, REBCNT index, u32* words, REBCNT *cmd) {
	RXIARG arg;
	REBCNT type = RL_GET_VALUE(cmds, index, &arg);
	//debug_print("fetch_word: %u type: %u\n", index, type);
	return ((RXT_WORD == type || RXT_SET_WORD == type) && (cmd[0] = RL_FIND_WORD(words, arg.int32a)));
}

int Common_mold(REBHOB *hob, REBSER *str) {
	int len;
	if (!str) return 0;
	SERIES_TAIL(str) = 0;
	APPEND_STRING(str, "0#%lx", (unsigned long)(uintptr_t)hob->data);
	return len;
}

int AV1Encoder_free(void* hndl) {
	if (!hndl) return 0;
	REBHOB *hob = (REBHOB *)hndl;
	debug_print("AV1Encoder_free %lx\n", (unsigned long)(uintptr_t)hob->data);
	aom_codec_destroy((aom_codec_ctx_t*)hob->data);
	UNMARK_HOB(hob);
	if (hob->series) {
		RESET_SERIES(hob->series);
		hob->series = NULL;
	}
	return 0;
}
int AV1Encoder_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
	AOMEncoderContext* context = (AOMEncoderContext*)hob->data;
	word = RL_FIND_WORD(arg_words, word);
	if (word == W_ARG_BUFFER) {
		if (hob->series) {
			arg->series = hob->series;
			arg->index = 0;
			*type = RXT_BINARY;
		}
		else *type = RXT_NONE;
		return PE_USE;
	}
	if (word == W_ARG_HEADER) {	// obu_sequence_header
		aom_fixed_buf_t *header = aom_codec_get_global_headers(&context->codec);
		if (header) {
			REBSER *bin = RL_MAKE_STRING((REBLEN)header->sz, FALSE);
			memcpy(SERIES_DATA(bin), header->buf, header->sz);
			SERIES_TAIL(bin) = (REBLEN)header->sz;
			free(header->buf);
			free(header);
			arg->series = bin;
			arg->index = 0;
			*type = RXT_BINARY;
		}
		else *type = RXT_NONE;
		return PE_USE;
	}
	return PE_BAD_SELECT;
}
int AV1Encoder_set_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
	return PE_BAD_SET;
}





COMMAND cmd_init_words(RXIFRM *frm, void *ctx) {
	arg_words  = RL_MAP_WORDS(RXA_SERIES(frm,1));
	type_words = RL_MAP_WORDS(RXA_SERIES(frm,2));

	// custom initialization may be done here...

	return RXR_TRUE;
}


static void show_stream_config(aom_codec_enc_cfg_t *cfg) {
#define SHOW(field) \
  fprintf(stderr, "    %-28s = %d\n", #field, cfg->field)

  fprintf(stderr, "Encoder parameters:\n");

  SHOW(g_usage);
  SHOW(g_threads);
  SHOW(g_profile);
  SHOW(g_w);
  SHOW(g_h);
  SHOW(g_bit_depth);
  SHOW(g_input_bit_depth);
  SHOW(g_timebase.num);
  SHOW(g_timebase.den);
  SHOW(g_error_resilient);
  SHOW(g_pass);
  SHOW(g_lag_in_frames);
  SHOW(rc_dropframe_thresh);
  SHOW(rc_resize_mode);
  SHOW(rc_resize_denominator);
  SHOW(rc_resize_kf_denominator);
  SHOW(rc_superres_mode);
  SHOW(rc_superres_denominator);
  SHOW(rc_superres_kf_denominator);
  SHOW(rc_superres_qthresh);
  SHOW(rc_superres_kf_qthresh);
  SHOW(rc_end_usage);
  SHOW(rc_target_bitrate);
  SHOW(rc_min_quantizer);
  SHOW(rc_max_quantizer);
  SHOW(rc_undershoot_pct);
  SHOW(rc_overshoot_pct);
  SHOW(rc_buf_sz);
  SHOW(rc_buf_initial_sz);
  SHOW(rc_buf_optimal_sz);
  SHOW(rc_2pass_vbr_bias_pct);
  SHOW(rc_2pass_vbr_minsection_pct);
  SHOW(rc_2pass_vbr_maxsection_pct);
  SHOW(fwd_kf_enabled);
  SHOW(kf_mode);
  SHOW(kf_min_dist);
  SHOW(kf_max_dist);
  SHOW(sframe_dist);
  SHOW(sframe_mode);
  SHOW(large_scale_tile);
}

COMMAND cmd_test(RXIFRM *frm, void *ctx) {

//    frame.use_argb = 1;
//	frame.argb   = (uint32_t*)SERIES_DATA((REBSER *)RXA_ARG(frm,3).image);
//	frame.width  = RXA_IMAGE_WIDTH(frm, 3);
//	frame.height = RXA_IMAGE_HEIGHT(frm, 3);
//	frame.argb_stride = frame.width;




	return RXR_TRUE;
}


COMMAND cmd_make_encoder(RXIFRM *frm, void *ctx) {
	REBHOB* hob = NULL;
	AOMEncoderContext* context;
	aom_codec_err_t res;
	aom_codec_ctx_t *codec;
	aom_codec_enc_cfg_t cfg;
	aom_codec_iface_t *const iface = aom_codec_av1_cx();

	trace("cmd_anim_encoder");

	hob = RL_MAKE_HANDLE_CONTEXT(Handle_AV1Encoder);
	if (hob == NULL) return RXR_NONE;
	context = (AOMEncoderContext*)hob->data;
	cfg = context->cfg;
	codec = &context->codec;

	//debug_print("context->frame_index: %u\n", context->frame_index);

	trace("aom_codec_enc_config_default");
	res = aom_codec_enc_config_default(iface, &cfg, 0);
	if (res != AOM_CODEC_OK) goto Error;

	int width  = (int)RXA_PAIR(frm,1).x;
	int height = (int)RXA_PAIR(frm,1).y;

	cfg.g_threads = 8;
	cfg.g_w = width;
	cfg.g_h = height;
	cfg.g_forced_max_frame_width = width;
	cfg.g_forced_max_frame_height = height;
	cfg.g_timebase.num = 1;
	cfg.g_timebase.den = 1;
	cfg.g_pass = AOM_RC_ONE_PASS;
	cfg.g_lag_in_frames = 4;
	cfg.g_usage = AOM_USAGE_REALTIME;
	cfg.rc_end_usage = AOM_VBR;
	cfg.rc_min_quantizer = 2;
	cfg.rc_max_quantizer = 58;
	

	if (ARG_Is_Block(3)) {
		// block...
		REBSER *args = RXA_SERIES(frm, 3);
		REBCNT index = RXA_INDEX(frm, 3);
		REBCNT type, option;
		RXIARG arg;

		while (index < args->tail) {
			if (!fetch_word(args, index++, arg_words, &option)) {
				debug_print(" [AOM] Unknown option!");
				continue;
			}
			type = RL_GET_VALUE_RESOLVED(args, index++, &arg);
			debug_print(" [AOM] option: %u type: %u\n", option, type);
			if (type != RXT_INTEGER || arg.int64 < 0 || arg.int64 > 0xffffffffL) goto Invalid_Option;
			switch(option) {
			case W_ARG_USAGE:
				cfg.g_usage = (unsigned int)arg.int64;
				continue;
			case W_ARG_CONSTANT_QUALITY:
					cfg.rc_end_usage = AOM_Q;
					cfg.rc_min_quantizer = (unsigned int)arg.int64;
					cfg.rc_max_quantizer = (unsigned int)arg.int64;
					cfg.rc_target_bitrate = 0;
					break;
			case W_ARG_G_USAGE:
			    cfg.g_usage = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_THREADS:
			    cfg.g_threads = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_PROFILE:
			    cfg.g_profile = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_BIT_DEPTH:
			    cfg.g_bit_depth = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_INPUT_BIT_DEPTH:
			    cfg.g_input_bit_depth = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_TIMEBASE_NUM:
			    cfg.g_timebase.num = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_TIMEBASE_DEN:
			    cfg.g_timebase.den = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_ERROR_RESILIENT:
			    cfg.g_error_resilient = (unsigned int)arg.uint64;
			    break;
			case W_ARG_G_PASS:
			    cfg.g_pass = (int)arg.int64;
			    break;
			case W_ARG_G_LAG_IN_FRAMES:
			    cfg.g_lag_in_frames = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_DROPFRAME_THRESH:
			    cfg.rc_dropframe_thresh = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_RESIZE_MODE:
			    cfg.rc_resize_mode = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_RESIZE_DENOMINATOR:
			    cfg.rc_resize_denominator = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_RESIZE_KF_DENOMINATOR:
			    cfg.rc_resize_kf_denominator = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_SUPERRES_MODE:
			    cfg.rc_superres_mode = (int)arg.int64;
			    break;
			case W_ARG_RC_SUPERRES_DENOMINATOR:
			    cfg.rc_superres_denominator = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_SUPERRES_KF_DENOMINATOR:
			    cfg.rc_superres_kf_denominator = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_SUPERRES_QTHRESH:
			    cfg.rc_superres_qthresh = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_SUPERRES_KF_QTHRESH:
			    cfg.rc_superres_kf_qthresh = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_END_USAGE:
			    cfg.rc_end_usage = (int)arg.int64;
			    break;
			case W_ARG_RC_TARGET_BITRATE:
			    cfg.rc_target_bitrate = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_MIN_QUANTIZER:
			    cfg.rc_min_quantizer = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_MAX_QUANTIZER:
			    cfg.rc_max_quantizer = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_UNDERSHOOT_PCT:
			    cfg.rc_undershoot_pct = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_OVERSHOOT_PCT:
			    cfg.rc_overshoot_pct = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_BUF_SZ:
			    cfg.rc_buf_sz = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_BUF_INITIAL_SZ:
			    cfg.rc_buf_initial_sz = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_BUF_OPTIMAL_SZ:
			    cfg.rc_buf_optimal_sz = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_2PASS_VBR_BIAS_PCT:
			    cfg.rc_2pass_vbr_bias_pct = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_2PASS_VBR_MINSECTION_PCT:
			    cfg.rc_2pass_vbr_minsection_pct = (unsigned int)arg.uint64;
			    break;
			case W_ARG_RC_2PASS_VBR_MAXSECTION_PCT:
			    cfg.rc_2pass_vbr_maxsection_pct = (unsigned int)arg.uint64;
			    break;
			case W_ARG_FWD_KF_ENABLED:
			    cfg.fwd_kf_enabled = (int)arg.int64;
			    break;
			case W_ARG_KF_MODE:
			    cfg.kf_mode = arg.int64;
			    break;
			case W_ARG_KF_MIN_DIST:
			    cfg.kf_min_dist = (unsigned int)arg.uint64;
			    break;
			case W_ARG_KF_MAX_DIST:
			    cfg.kf_max_dist = (unsigned int)arg.uint64;
			    break;

			case W_ARG_SFRAME_DIST:
			    cfg.sframe_dist = (unsigned int)arg.uint64;
			    break;
			case W_ARG_SFRAME_MODE:
			    cfg.sframe_mode = (unsigned int)arg.uint64;
			    break;
			case W_ARG_LARGE_SCALE_TILE:
			    cfg.large_scale_tile = (unsigned int)arg.uint64;
			    break;

			case W_ARG_MONOCHROME:
			    cfg.monochrome = (unsigned int)arg.uint64;
			    break;
			}
			continue;
			Invalid_Option:
			debug_print(" [AOM] Invalid option value!\n");
			continue;
		}
	}

	show_stream_config(&cfg);

	trace("aom_codec_enc_init");
	res = aom_codec_enc_init(codec, iface, &cfg, 0);
	if (res != AOM_CODEC_OK) goto Error;

	debug_print("AV1Encoder new: %lx %lx\n", (unsigned long)(uintptr_t)context, (unsigned long)(uintptr_t)codec);

//	res = aom_codec_set_option(codec, "passes", "1");
//	if (res != AOM_CODEC_OK) goto Error;
//	AOM_CODEC_CONTROL_TYPECHECKED(codec, AV1E_SET_COLOR_RANGE, 0);

	hob->series = RL_MAKE_STRING(1000, FALSE);

	RXA_HANDLE(frm, 1)       = hob;
	RXA_HANDLE_TYPE(frm, 1)  = hob->sym;
	RXA_HANDLE_FLAGS(frm, 1) = hob->flags;
	RXA_TYPE(frm, 1) = RXT_HANDLE;


	return RXR_VALUE;

Error:
	RXA_SERIES(frm, 1) = (void*)aom_codec_err_to_string(res);
	return RXR_ERROR;
}


// Function to convert BGRA to YUV420
void bgra_to_yuv420(const unsigned char *argb, aom_image_t *img, int width, int height) {
	unsigned char *y_plane = img->planes[AOM_PLANE_Y];
	unsigned char *u_plane = img->planes[AOM_PLANE_U];
	unsigned char *v_plane = img->planes[AOM_PLANE_V];

	int y_stride = img->stride[AOM_PLANE_Y];
	int u_stride = img->stride[AOM_PLANE_U];
	int v_stride = img->stride[AOM_PLANE_V];

	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int b = argb[(j * width + i) * 4 + 0];  // Blue channel
			int g = argb[(j * width + i) * 4 + 1];  // Green channel
			int r = argb[(j * width + i) * 4 + 2];  // Red channel

			int y = (66 * r + 129 * g + 25 * b + 128) >> 8;
			int u = (-38 * r - 74 * g + 112 * b + 128) >> 8;
			int v = (112 * r - 94 * g - 18 * b + 128) >> 8;

			y_plane[j * y_stride + i] = y + 16;
			if (j % 2 == 0 && i % 2 == 0) {
				u_plane[(j / 2) * u_stride + (i / 2)] = u + 128;
				v_plane[(j / 2) * v_stride + (i / 2)] = v + 128;
			}
		}
	}
}

int frame_index = 0;
COMMAND cmd_encode_frame(RXIFRM *frm, void *ctx) {
	aom_codec_err_t res;
	aom_codec_ctx_t *codec;
	aom_image_t raw;
	AOMEncoderContext *context;
	const aom_codec_cx_pkt_t *pkt;
	aom_codec_iter_t iter = NULL;
	size_t encoded_data_size;

	if (!ARG_Is_AV1Encoder(1))
		RETURN_ERROR("Expected handle of type: AV1mEncoder");

	context = ARG_AV1Encoder(1);
	codec = &context->codec;
	//debug_print("AV1Encoder: %lx %lx\n", (unsigned long)(uintptr_t)context, (unsigned long)(uintptr_t)codec);

	if (ARG_Is_None(2)) {
		res = aom_codec_encode(codec, NULL, 0, 0, 0);

	} else {
		int width = (int)RXA_IMAGE_WIDTH(frm, 2);
		int height = (int)RXA_IMAGE_HEIGHT(frm, 2);

		if(!aom_img_alloc(&raw, AOM_IMG_FMT_I420, width, height, 1))
			RETURN_ERROR("aom_img_alloc failed!");

		bgra_to_yuv420(SERIES_DATA((REBSER *)RXA_ARG(frm, 2).image), &raw, width, height);

	//	int time = (int)(RXA_TIME(frm,2) / 1000000L);
	//	debug_print("aom_codec_encode time: %3i ", time);
		
		int flags = context->frame_index == 0 ? AOM_EFLAG_FORCE_KF : 0;
		res = aom_codec_encode(codec, &raw, context->frame_index++, 1, flags);
		if (res != AOM_CODEC_OK) goto Error;
	}

	REBSER* ser = ARG_AV1Encoder_Series(1);
	encoded_data_size = SERIES_TAIL(ser);

	// Write the encoded data to the output buffer
	while ((pkt = aom_codec_get_cx_data(codec, &iter)) != NULL) {
		//debug_print("pkt->kind: %i\n", pkt->kind);
		if (pkt->kind == AOM_CODEC_CX_FRAME_PKT) {
			size_t len = pkt->data.frame.sz;
			debug_print("[%4u] encoded_data_size: %7zu frame size: %7zu flags: %4i     \r",context->frame_index, encoded_data_size, len, pkt->data.frame.flags);
			RL_EXPAND_SERIES(ser, -1, (REBLEN)pkt->data.frame.sz);
			memcpy(SERIES_SKIP(ser, encoded_data_size), pkt->data.frame.buf, len);
			encoded_data_size += len;
		}
	}

	// Output the encoded data size
	//printf("Encoded data size: %zu bytes            \r", encoded_data_size);
	SERIES_TAIL(ser) = (REBLEN)encoded_data_size;

	if (ARG_Is_None(2)) {
		printf("                                                                 \r");
		printf("Encoded data size: %zu\n", encoded_data_size);
		RXA_TYPE(frm, 1) = RXT_BINARY;
		RXA_ARG(frm, 1).series  = ser;
		RXA_ARG(frm, 1).index = 0;
		return RXR_VALUE;
	}

	return RXR_TRUE;
Error:
	debug_print("Error code: %i\n", (int)res);
	RXA_SERIES(frm, 1) = (void*)aom_codec_err_to_string(res);
	return RXR_ERROR;
}

COMMAND cmd_image_to_yuv420(RXIFRM *frm, void *ctx) {
	return RXR_UNSET;
}
