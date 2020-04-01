#ifndef __ESP_CFG_000_H__
#define __ESP_CFG_000_H__

#include "libesp.h"

typedef int64_t token_t;

/* <<--params-def-->> */
#define INPUT_DIMENSION 13
#define NODES_PER_LAYER 64
#define POSSIBLE_OUTPUTS 3
#define LEARNING_RATE 0.01
#define TRAINING_SETS 163

/* <<--params-->> */
const int32_t input_dimension = INPUT_DIMENSION;
const int32_t nodes_per_layer = NODES_PER_LAYER;
const int32_t possible_outputs = POSSIBLE_OUTPUTS;
const int32_t learning_rate = LEARNING_RATE;
const int32_t training_sets = TRAINING_SETS;

#define NACC 1

esp_thread_info_t cfg_000[] = {
	{
		.run = true,
		.devname = "kprop.0",
		.type = kprop,
		/* <<--descriptor-->> */
		.desc.kprop_desc.input_dimension = INPUT_DIMENSION,
		.desc.kprop_desc.nodes_per_layer = NODES_PER_LAYER,
		.desc.kprop_desc.possible_outputs = POSSIBLE_OUTPUTS,
		.desc.kprop_desc.learning_rate = LEARNING_RATE,
		.desc.kprop_desc.training_sets = TRAINING_SETS,
		.desc.kprop_desc.src_offset = 0,
		.desc.kprop_desc.dst_offset = 0,
		.desc.kprop_desc.esp.coherence = ACC_COH_NONE,
		.desc.kprop_desc.esp.p2p_store = 0,
		.desc.kprop_desc.esp.p2p_nsrcs = 0,
		.desc.kprop_desc.esp.p2p_srcs = {"", "", "", ""},
	}
};

#endif /* __ESP_CFG_000_H__ */
