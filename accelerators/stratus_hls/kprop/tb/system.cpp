// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include <fstream>
#include <iostream>
#include <sstream>
#include "system.hpp"

// Process
void system_t::config_proc()
{

    // Reset
    {
        conf_done.write(false);
        conf_info.write(conf_info_t());
        wait();
    }

    ESP_REPORT_INFO("reset done");

    // Config
    load_memory();
    {
        conf_info_t config;
        // Custom configuration
        /* <<--params-->> */
        config.input_dimension = input_dimension;
        config.nodes_per_layer = nodes_per_layer;
        config.possible_outputs = possible_outputs;
        config.learning_rate = learning_rate;
        config.training_sets = training_sets;

        wait(); conf_info.write(config);
        conf_done.write(true);
    }

    ESP_REPORT_INFO("config done");

    // Compute
    {
        // Print information about begin time
        sc_time begin_time = sc_time_stamp();
        ESP_REPORT_TIME(begin_time, "BEGIN - kprop");

        // Wait the termination of the accelerator
        do { wait(); } while (!acc_done.read());
        debug_info_t debug_code = debug.read();

        // Print information about end time
        sc_time end_time = sc_time_stamp();
        ESP_REPORT_TIME(end_time, "END - kprop");

        esc_log_latency(sc_object::basename(), clock_cycle(end_time - begin_time));
        wait(); conf_done.write(false);
    }

    // Validate
    {
        dump_memory(); // store the output in more suitable data structure if needed
        // check the results with the golden model
        if (validate())
        {
            ESP_REPORT_ERROR("validation failed!");
        } else
        {
            ESP_REPORT_INFO("validation passed!");
        }
    }

    // Conclude
    {
        sc_stop();
    }
}

// Functions
void system_t::load_memory()
{
    // Optional usage check
#ifdef CADENCE
    if (esc_argc() != 1)
    {
        ESP_REPORT_INFO("usage: %s\n", esc_argv()[0]);
        sc_stop();
    }
#endif

    // Memory allocation

    //  ===========================  ^
    //  |  weights1     (double)  |  | input_dimension * nodes_per_layer 
    //  ===========================  -
    //  |  weights2     (double)  |  | nodes_per_layer * nodes_per_layer
    //  ===========================  -
    //  |  weights3     (double)  |  | nodes_per_layer * possible_outputs
    //  ===========================  -
    //  |  biases1      (double)  |  | nodes_per_layer
    //  ===========================  -
    //  |  biases2      (double)  |  | nodes_per_layer
    //  ===========================  -
    //  |  biases3      (double)  |  | possible_outputs
    //  ===========================  -
    //  |  training_data(double)  |  | training_sets * input_dimension
    //  ===========================  -
    //  | training_targets(double)|  | training_sets * possible_outputs	
    //  ===========================  v
	
    // Input data and golden output (aligned to DMA_WIDTH makes your life easier)
	fp = fopen(IN_FILE, "r")
	fp_gold = fopen(CHK_FILE, "r")
    if (!fp) {
    	ESP_REPORT_INFO("cannot open input file.");
    	return;
    }
    if (!fp_gold) {
    	ESP_REPORT_INFO("cannot open check file.");
    	return;
    }	
    
    //Memory position
    weights1_addr = 0;	
    weights2_addr = input_dimension * nodes_per_layer;	
    weights3_addr = weights2_addr + nodes_per_layer * nodes_per_layer;	
    biases1_addr = weights3_addr + nodes_per_layer * possible_outputs;	
    biases2_addr = biases1_addr + nodes_per_layer;	
    biases3_addr = biases2_addr + nodes_per_layer;	
    training_data_addr = biases3_addr + possible_outputs;	
    training_targets_addr = training_data_addr + training_sets * possible_outputs;		
	
	
	
#if (DMA_WORD_PER_BEAT == 0)
    in_words_adj = input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs;
    out_words_adj = input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs;
#else
    in_words_adj = round_up(input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs, DMA_WORD_PER_BEAT);
    out_words_adj = round_up(input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs, DMA_WORD_PER_BEAT);
#endif

    in_size = in_words_adj * (1);
    out_size = out_words_adj * (1);
	
    //Load input_data (in) and golden output (gold)
    in = new float[in_size];
    gold = new float[out_size];
    //weight1
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (i = 0; i < weights2_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
        in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }
    //weight2
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < weights3_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }
    //weight3
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < biases1_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }
    //biases1
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < biases1_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }	
    //biases2
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < biases2_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }
    //biases3
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < training_data_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }
    //training_data
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < training_targets_addr; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }	
    //training_targets
    fscanf(fp, "%s\n", str_tmp); // Read separator line: %%
    fscanf(fp_gold, "%s\n", str_tmp);
    for (; i < in_size; i++) {
    	float val;
    	fscanf(fp, "%f\n", &val);
	in[i] = val;
	mem[i] = fp2bv<FPDATA, WORD_SIZE>((FPDATA) val); // FPDATA -> sc_bv and store it
	fscanf(fp_gold, "%f\n", &val);
    	gold[i] = val;    	
    }
/*
    // Memory initialization:
#if (DMA_WORD_PER_BEAT == 0)
    for (int i = 0; i < in_size; i++)  {
        sc_dt::sc_bv<DATA_WIDTH> data_bv(in[i]);
        for (int j = 0; j < DMA_BEAT_PER_WORD; j++)
            mem[DMA_BEAT_PER_WORD * i + j] = data_bv.range((j + 1) * DMA_WIDTH - 1, j * DMA_WIDTH);
    }
#else
    for (int i = 0; i < in_size / DMA_WORD_PER_BEAT; i++)  {
        sc_dt::sc_bv<DMA_WIDTH> data_bv(in[i]);
        for (int j = 0; j < DMA_WORD_PER_BEAT; j++)
            data_bv.range((j+1) * DATA_WIDTH - 1, j * DATA_WIDTH) = in[i * DMA_WORD_PER_BEAT + j];
        mem[i] = data_bv;
    }
#endif
*/

    ESP_REPORT_INFO("load memory completed");
}

void system_t::dump_memory()
{
    // Get results from memory
    out = new float[out_size];
    uint32_t offset = in_size;

#if (DMA_WORD_PER_BEAT == 0)
    offset = offset * DMA_BEAT_PER_WORD;
    for (int i = 0; i < out_size; i++)  {
        sc_dt::sc_bv<DATA_WIDTH> data_bv;

        for (int j = 0; j < DMA_BEAT_PER_WORD; j++)
            data_bv.range((j + 1) * DMA_WIDTH - 1, j * DMA_WIDTH) = mem[offset + DMA_BEAT_PER_WORD * i + j];
		FPDATA out_fx = bv2fp<FPDATA, WORD_SIZE>(data_bv);
        out[i] = (float) out_fx;
    }
#else
    offset = offset / DMA_WORD_PER_BEAT;
    for (int i = 0; i < out_size / DMA_WORD_PER_BEAT; i++)
        for (int j = 0; j < DMA_WORD_PER_BEAT; j++)
            FPDATA out_fx = bv2fp<FPDATA, WORD_SIZE>(mem[offset + i].range((j + 1) * DATA_WIDTH - 1, j * DATA_WIDTH));
			out[i * DMA_WORD_PER_BEAT + j] = (float) out_fx;
#endif

    ESP_REPORT_INFO("dump memory completed");
}

int system_t::validate()
{
    // Check for mismatches
    uint32_t errors = 0;

    for (int i = 0; i < 1; i++)
        for (int j = 0; j < input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs; j++)
            if (gold[i * out_words_adj + j] != out[i * out_words_adj + j])
                errors++;

    delete [] in;
    delete [] out;
    delete [] gold;

    return errors;
}
