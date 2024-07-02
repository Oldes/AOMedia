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

RL_LIB *RL; // Link back to reb-lib from embedded extensions

//==== Globals ===============================================================//
extern MyCommandPointer Command[];


u32* arg_words;
u32* type_words;

REBCNT Handle_AV1Encoder;
//============================================================================//

static const char* init_block = AOMEDIA_EXT_INIT_CODE;

int Common_mold(REBHOB *hob, REBSER *ser);

int AV1Encoder_free(void* hndl);
int AV1Encoder_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg);
int AV1Encoder_set_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg);


RXIEXT const char *RX_Init(int opts, RL_LIB *lib) {
	RL = lib;
	REBYTE ver[8];
	RL_VERSION(ver);
	debug_print(
		"RXinit aomedia-extension; Rebol v%i.%i.%i\n",
		ver[1], ver[2], ver[3]);

	if (MIN_REBOL_VERSION > VERSION(ver[1], ver[2], ver[3])) {
		debug_print(
			"Needs at least Rebol v%i.%i.%i!\n",
			 MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD);
		return 0;
	}
	if (!CHECK_STRUCT_ALIGN) {
		trace("CHECK_STRUCT_ALIGN failed!");
		return 0;
	}

	REBHSP spec;
	spec.mold = Common_mold;

	spec.size      = sizeof(AOMEncoderContext);
	spec.flags     = HANDLE_REQUIRES_HOB_ON_FREE;
	spec.free      = AV1Encoder_free;
	spec.get_path  = AV1Encoder_get_path;
	spec.set_path  = AV1Encoder_set_path;
	Handle_AV1Encoder = RL_REGISTER_HANDLE_SPEC((REBYTE*)"AV1Encoder", &spec);

	//debug_print("AOMEncoderContext size: %u\n", spec.size);

	return init_block;
}

RXIEXT int RX_Call(int cmd, RXIFRM *frm, void *ctx) {
	return Command[cmd](frm, ctx);
}
