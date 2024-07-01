//   ____  __   __        ______        __
//  / __ \/ /__/ /__ ___ /_  __/__ ____/ /
// / /_/ / / _  / -_|_-<_ / / / -_) __/ _ \
// \____/_/\_,_/\__/___(@)_/  \__/\__/_// /
//  ~~~ oldes.huhuman at gmail.com ~~~ /_/
//
// Project: Rebol/AOMedia extension
// SPDX-License-Identifier: MIT
// =============================================================================

#include <aom/aom_encoder.h>
#include <aom/aomcx.h>
#include <aom/aom_image.h>

typedef struct AOMEncoderContext {
  aom_codec_ctx_t      codec;
  aom_codec_enc_cfg_t  cfg;
  unsigned int         frame_index;
} AOMEncoderContext;
