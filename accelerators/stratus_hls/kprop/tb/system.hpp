// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include "kprop_conf_info.hpp"
#include "kprop_debug_info.hpp"
#include "kprop.hpp"
#include "kprop_directives.hpp"

#include "esp_templates.hpp"

const size_t MEM_SIZE = 125744 / (DMA_WIDTH/8);

#include "core/systems/esp_system.hpp"

#ifdef CADENCE
#include "kprop_wrap.h"
#endif

//input file and check file
#define IN_FILE "./data/input.data"
#define CHK_FILE "./data/check.data"

class system_t : public esp_system<DMA_WIDTH, MEM_SIZE>
{
public:

    // ACC instance
#ifdef CADENCE
    kprop_wrapper *acc;
#else
    kprop *acc;
#endif

    // Constructor
    SC_HAS_PROCESS(system_t);
    system_t(sc_module_name name)
        : esp_system<DMA_WIDTH, MEM_SIZE>(name)
    {
        // ACC
#ifdef CADENCE
        acc = new kprop_wrapper("kprop_wrapper");
#else
        acc = new kprop("kprop_wrapper");
#endif
        // Binding ACC
        acc->clk(clk);
        acc->rst(acc_rst);
        acc->dma_read_ctrl(dma_read_ctrl);
        acc->dma_write_ctrl(dma_write_ctrl);
        acc->dma_read_chnl(dma_read_chnl);
        acc->dma_write_chnl(dma_write_chnl);
        acc->conf_info(conf_info);
        acc->conf_done(conf_done);
        acc->acc_done(acc_done);
        acc->debug(debug);

        /* <<--params-default-->> */
        input_dimension = 13;
        nodes_per_layer = 64;
        possible_outputs = 3;
        learning_rate = 0.01;
        training_sets = 163;
    }

    // Processes

    // Configure accelerator
    void config_proc();

    // Load internal memory
    void load_memory();

    // Dump internal memory
    void dump_memory();

    // Validate accelerator results
    int validate();

    // Accelerator-specific data
    /* <<--params-->> */
    int32_t input_dimension;
    int32_t nodes_per_layer;
    int32_t possible_outputs;
    int32_t learning_rate;
    int32_t training_sets;

    uint32_t in_words_adj;
    uint32_t out_words_adj;
    uint32_t in_size;
    uint32_t out_size;
    int64_t *in;
    int64_t *out;
    int64_t *gold;

    // Other Functions
	//Memory position
    uint32_t weights1_addr;	
    uint32_t weights2_addr;	
    uint32_t weights3_addr;	
    uint32_t biases1_addr;	
    uint32_t biases2_addr;	
    uint32_t biases3_addr;	
    uint32_t training_data_addr;	
    uint32_t training_targets_addr;	
};

#endif // __SYSTEM_HPP__
