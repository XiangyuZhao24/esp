// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __KPROP_HPP__
#define __KPROP_HPP__

#include "fpdata.hpp"
#include "kprop_conf_info.hpp"
#include "kprop_debug_info.hpp"

#include "esp_templates.hpp"

#include "kprop_directives.hpp"

#define __round_mask(x, y) ((y)-1)
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)

/* <<--defines-->> */
#define DATA_WIDTH FX_WIDTH  //FX_WIDTH defined in fpdata
#if (FX_WIDTH == 64)
#define DMA_SIZE SIZE_DWORD
#elif (FX_WIDTH == 32)
#define DMA_SIZE SIZE_WORD
#endif // FX_WIDTH
#define PLM_OUT_WORD 7859
#define PLM_IN_WORD 7859


#define input_dimension_size 13
#define possible_outputs_size  3
#define training_sets_size   163
#define nodes_per_layer_size  64
#define lr 0.01

class kprop : public esp_accelerator_3P<DMA_WIDTH>
{
public:
    // Constructor
    SC_HAS_PROCESS(kprop);
    kprop(const sc_module_name& name)
    : esp_accelerator_3P<DMA_WIDTH>(name)
        , cfg("config")
    {
        // Signal binding
        cfg.bind_with(*this);

        // Map arrays to memories
        /* <<--plm-bind-->> */
		HLS_MAP_plm(plm_weights1, PLM_WEIGHTS1_NAME);
		HLS_MAP_plm(plm_weights2, PLM_WEIGHTS2_NAME);
		HLS_MAP_plm(plm_weights3, PLM_WEIGHTS3_NAME);
		HLS_MAP_plm(plm_biases1, PLM_BIASES1_NAME);
		HLS_MAP_plm(plm_biases2, PLM_BIASES2_NAME);
		HLS_MAP_plm(plm_biases3, PLM_BIASES3_NAME);
		HLS_MAP_plm(plm_training_data, PLM_TRAINING_DATA_NAME);
		HLS_MAP_plm(plm_training_targets, PLM_TRAINING_TARGETS_NAME);
		
    }

    // Processes

    // Load the input data
    void load_input();

    // Computation
    void compute_kernel();

    // Store the output data
    void store_output();

    // Configure kprop
    esp_config_proc cfg;

    // Functions
	void soft_max(FPDATA_WORD net_outputs[possible_outputs_size], FPDATA_WORD activations[possible_outputs_size]);
	void RELU(FPDATA_WORD activations[nodes_per_layer_size], FPDATA_WORD dactivations[nodes_per_layer_size], int size);
	void add_bias_to_activations(FPDATA_WORD biases[nodes_per_layer_size], 
                               FPDATA_WORD activations[nodes_per_layer_size],
                               int size);
	void matrix_vector_product_with_bias_input_layer(FPDATA_WORD biases[nodes_per_layer_size],
                                                 FPDATA_WORD weights[input_dimension_size*nodes_per_layer_size],
                                                 FPDATA_WORD activations[nodes_per_layer_size],
                                                 FPDATA_WORD input_sample[input_dimension_size]);
	void matrix_vector_product_with_bias_second_layer(FPDATA_WORD biases[nodes_per_layer_size],
                                                 FPDATA_WORD weights[nodes_per_layer_size*nodes_per_layer_size],
                                                 FPDATA_WORD activations[nodes_per_layer_size],
                                                 FPDATA_WORD input_activations[nodes_per_layer_size]);
	void matrix_vector_product_with_bias_output_layer(FPDATA_WORD biases[possible_outputs_size],
                                                 FPDATA_WORD weights[nodes_per_layer_size*possible_outputs_size],
                                                 FPDATA_WORD activations[possible_outputs_size],
                                                 FPDATA_WORD input_activations[nodes_per_layer_size]);
	void take_difference(FPDATA_WORD net_outputs[possible_outputs_size], 
                     FPDATA_WORD solutions[possible_outputs_size], 
                     FPDATA_WORD output_difference[possible_outputs_size],
                     FPDATA_WORD dactivations[possible_outputs_size]);
	void get_delta_matrix_weights3(FPDATA_WORD delta_weights3[nodes_per_layer_size*possible_outputs_size],
                               FPDATA_WORD output_difference[possible_outputs_size],
                               FPDATA_WORD last_activations[nodes_per_layer_size]);
	void get_oracle_activations2(FPDATA_WORD weights3[nodes_per_layer_size*possible_outputs_size], 
                             FPDATA_WORD output_differences[possible_outputs_size], 
                             FPDATA_WORD oracle_activations[nodes_per_layer_size],
                             FPDATA_WORD dactivations[nodes_per_layer_size]);
	void get_delta_matrix_weights2(FPDATA_WORD delta_weights2[nodes_per_layer_size*nodes_per_layer_size],
                               FPDATA_WORD output_difference[nodes_per_layer_size],
                               FPDATA_WORD last_activations[nodes_per_layer_size]);
	void get_oracle_activations1(FPDATA_WORD weights2[nodes_per_layer_size*nodes_per_layer_size], 
                             FPDATA_WORD output_differences[nodes_per_layer_size], 
                             FPDATA_WORD oracle_activations[nodes_per_layer_size],
                             FPDATA_WORD dactivations[nodes_per_layer_size]);
	void get_delta_matrix_weights1(FPDATA_WORD delta_weights1[input_dimension_size*nodes_per_layer_size],
                               FPDATA_WORD output_difference[nodes_per_layer_size],
                               FPDATA_WORD last_activations[input_dimension_size]);
	void update_weights(FPDATA_WORD weights1[input_dimension_size*nodes_per_layer_size],
                    FPDATA_WORD weights2[nodes_per_layer_size*nodes_per_layer_size],
                    FPDATA_WORD weights3[nodes_per_layer_size*possible_outputs_size],
                    FPDATA_WORD d_weights1[input_dimension_size*nodes_per_layer_size],
                    FPDATA_WORD d_weights2[nodes_per_layer_size*nodes_per_layer_size],
                    FPDATA_WORD d_weights3[nodes_per_layer_size*possible_outputs_size],
                    FPDATA_WORD biases1[nodes_per_layer_size],
                    FPDATA_WORD biases2[nodes_per_layer_size],
                    FPDATA_WORD biases3[possible_outputs_size],
                    FPDATA_WORD d_biases1[nodes_per_layer_size],
                    FPDATA_WORD d_biases2[nodes_per_layer_size],
                    FPDATA_WORD d_biases3[possible_outputs_size]);

    // Private local memories
	/* Generated by Default
    sc_dt::sc_int<DATA_WIDTH> plm_in_ping[PLM_IN_WORD];
    sc_dt::sc_int<DATA_WIDTH> plm_in_pong[PLM_IN_WORD];
    sc_dt::sc_int<DATA_WIDTH> plm_out_ping[PLM_OUT_WORD];
    sc_dt::sc_int<DATA_WIDTH> plm_out_pong[PLM_OUT_WORD];
	*/
	
	FPDATA_WORD plm_weights1[input_dimension_size*nodes_per_layer_size];
	FPDATA_WORD plm_weights2[nodes_per_layer_size*nodes_per_layer_size];
	FPDATA_WORD plm_weights3[nodes_per_layer_size*possible_outputs_size];
	FPDATA_WORD plm_biases1[nodes_per_layer_size];
	FPDATA_WORD plm_biases2[nodes_per_layer_size];
	FPDATA_WORD plm_biases3[possible_outputs_size];
	FPDATA_WORD plm_training_data[training_sets_size*input_dimension_size];
	FPDATA_WORD plm_training_targets[training_sets_size*possible_outputs_size];
	
	//Intermediate results
	//Forward and training structures
    FPDATA_WORD activations1[nodes_per_layer_size];
    FPDATA_WORD activations2[nodes_per_layer_size];
    FPDATA_WORD activations3[possible_outputs_size];
    FPDATA_WORD dactivations1[nodes_per_layer_size];
    FPDATA_WORD dactivations2[nodes_per_layer_size];
    FPDATA_WORD dactivations3[possible_outputs_size];
    FPDATA_WORD net_outputs[possible_outputs_size];
    //Training structure
    FPDATA_WORD output_difference[possible_outputs_size];
    FPDATA_WORD delta_weights1[input_dimension_size*nodes_per_layer_size]; 
    FPDATA_WORD delta_weights2[nodes_per_layer_size*nodes_per_layer_size];
    FPDATA_WORD delta_weights3[nodes_per_layer_size*possible_outputs_size];
    FPDATA_WORD oracle_activations1[nodes_per_layer_size];
    FPDATA_WORD oracle_activations2[nodes_per_layer_size];

};


#endif /* __KPROP_HPP__ */
