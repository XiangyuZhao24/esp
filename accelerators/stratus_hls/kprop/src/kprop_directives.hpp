// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __KPROP_DIRECTIVES_HPP__
#define __KPROP_DIRECTIVES_HPP__


#if (DMA_WIDTH == 32)
#define DMA_BEAT_PER_WORD 2
#define DMA_WORD_PER_BEAT 0
#define PLM_IN_NAME "kprop_plm_block_in_dma32"
#define PLM_OUT_NAME "kprop_plm_block_out_dma32"
#elif (DMA_WIDTH == 64)
#define DMA_BEAT_PER_WORD 1
#define DMA_WORD_PER_BEAT 1
#define PLM_WEIGHTS1_NAME "kprop_plm_block_weights1_dma64"
#define PLM_WEIGHTS2_NAME "kprop_plm_block_weights2_dma64"
#define PLM_WEIGHTS3_NAME "kprop_plm_block_weights3_dma64"
#define PLM_BIASES1_NAME "kprop_plm_block_biases1_dma64"
#define PLM_BIASES2_NAME "kprop_plm_block_biases2_dma64"
#define PLM_BIASES3_NAME "kprop_plm_block_biases3_dma64"
#define PLM_TRAINING_DATA_NAME "kprop_plm_block_training_data_dma64"
#define PLM_TRAINING_TARGETS_NAME "kprop_plm_block_training_targets_dma64"
#endif

#if defined(STRATUS_HLS)	
	
#define HLS_MAP_plm(_mem, _plm_block_name)      \
    HLS_MAP_TO_MEMORY(_mem, _plm_block_name)

#define HLS_PROTO(_s)                           \
    HLS_DEFINE_PROTOCOL(_s)

#define HLS_FLAT(_a)                            \
    HLS_FLATTEN_ARRAY(_a);

#define HLS_BREAK_DEP(_a)                       \
    HLS_BREAK_ARRAY_DEPENDENCY(_a)

#define HLS_UNROLL_SIMPLE                       \
    HLS_UNROLL_LOOP(ON)

#if defined(HLS_DIRECTIVES_BASIC)

#else

#error Unsupported or undefined HLS configuration

#endif /* HLS_DIRECTIVES_* */

#else /* !STRATUS_HLS */

#define HLS_MAP_plm(_mem, _plm_block_name)
#define HLS_PROTO(_s)
#define HLS_FLAT(_a)
#define HLS_BREAK_DEP(_a)
#define HLS_UNROLL_SIMPLE

#endif /* STRATUS_HLS */

#endif /* __KPROP_DIRECTIVES_HPP_ */