// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "kprop.hpp"
#include "kprop_directives.hpp"

// Functions

#include "kprop_functions.hpp"

// Processes

void kprop::load_input()
{

    // Reset
    {
        HLS_PROTO("load-reset");
        this->reset_load_input();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_dimension;
    int32_t nodes_per_layer;
    int32_t possible_outputs;
    int32_t training_sets;
	int32_t len_weights1;
	int32_t len_weights2;
	int32_t len_weights3;
	int32_t len_biases1;
	int32_t len_biases2;
	int32_t len_biases3;
	int32_t len_training_data;
	int32_t len_training_targets;
	
	
    {
        HLS_PROTO("load-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_dimension = config.input_dimension;
        nodes_per_layer = config.nodes_per_layer;
        possible_outputs = config.possible_outputs;
        training_sets = config.training_sets;
		
		//length of each PLM
		len_weights1 = input_dimension * nodes_per_layer;
		len_weights2 = nodes_per_layer * nodes_per_layer;
		len_weights3 = nodes_per_layer * possible_outputs;
		len_biases1 = nodes_per_layer;
		len_biases2 = nodes_per_layer;	
		len_biases3 = possible_outputs;
		len_training_data = training_sets * input_dimension;
		len_training_targets = training_sets * possible_outputs;		
    }


	// New Load
    {
        HLS_PROTO("load-dma");
        wait();
        uint32_t offset = 0;
		wait();
		
		// load weights1 values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_weights1, DMA_SIZE);
			offset += len_weights1;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_weights1; i++)
		{
			HLS_BREAK_DEP(plm_weights1);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_weights1[i] = dataBv.to_int64();		
		}

		// load weights2 values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_weights2, DMA_SIZE);
			offset += len_weights2;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_weights2; i++)
		{
			HLS_BREAK_DEP(plm_weights2);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_weights2[i] = dataBv.to_int64();		
		}

		// load weights3 values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_weights3, DMA_SIZE);
			offset += len_weights3;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_weights3; i++)
		{
			HLS_BREAK_DEP(plm_weights3);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_weights3[i] = dataBv.to_int64();		
		}	

		// load biases1 values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_biases1, DMA_SIZE);
			offset += len_biases1;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_biases1; i++)
		{
			HLS_BREAK_DEP(plm_biases1);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_biases1[i] = dataBv.to_int64();			
		}	

		// load biases2 values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_biases2, DMA_SIZE);
			offset += len_biases2;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_biases2; i++)
		{
			HLS_BREAK_DEP(plm_biases2);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_biases2[i] = dataBv.to_int64();		
		}		
		
		// load biases3 values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_biases3, DMA_SIZE);
			offset += len_biases3;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_biases3; i++)
		{
			HLS_BREAK_DEP(plm_biases3);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			plm_biases3[i] = dataBv.to_int64();			
		}

		// load training data values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_training_data, DMA_SIZE);
			offset += len_training_data;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_training_data; i++)
		{
			HLS_BREAK_DEP(plm_training_data);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_training_data[i] = dataBv.to_int64();		
		}	

		// load training targets values		
		{
			HLS_PROTO("load-dma");
			
			dma_info_t dma_info(offset, len_training_targets, DMA_SIZE);
			offset += len_training_targets;
			this->dma_read_ctrl.put(dma_info);			
		}
		
		for (uint16_t i = 0; i < len_training_targets; i++)
		{
			HLS_BREAK_DEP(plm_training_targets);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			plm_training_targets[i] = dataBv.to_int64();			
		}		

		this->load_compute_handshake();
		
    }	
    // Conclude
    {
        this->process_done();
    }
}



void kprop::store_output()
{
    // Reset
    {
        HLS_PROTO("store-reset");

        this->reset_store_output();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_dimension;
    int32_t nodes_per_layer;
    int32_t possible_outputs;
    int32_t training_sets;
	
	int32_t len_weights1;
	int32_t len_weights2;
	int32_t len_weights3;
	int32_t len_biases1;
	int32_t len_biases2;
	int32_t len_biases3;
	int32_t len_training_data;
	int32_t len_training_targets;
    {
        HLS_PROTO("store-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_dimension = config.input_dimension;
        nodes_per_layer = config.nodes_per_layer;
        possible_outputs = config.possible_outputs;
        training_sets = config.training_sets;
		
		//length of each PLM
		len_weights1 = input_dimension * nodes_per_layer;
		len_weights2 = nodes_per_layer * nodes_per_layer;
		len_weights3 = nodes_per_layer * possible_outputs;
		len_biases1 = nodes_per_layer;
		len_biases2 = nodes_per_layer;	
		len_biases3 = possible_outputs;
		len_training_data = training_sets * input_dimension;
		len_training_targets = training_sets * possible_outputs;	
    }
	
    // New Store
	{
        HLS_PROTO("store-dma");
        wait();
#if (DMA_WORD_PER_BEAT == 0)
        uint32_t store_offset = (input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs) * 1;
#else
        uint32_t store_offset = round_up(input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs, DMA_WORD_PER_BEAT) * 1;
#endif
        uint32_t offset = store_offset;

        wait();
		
		// store weights1 values		
		this->store_compute_handshake();
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_weights1, DMA_SIZE);
			offset += len_weights1;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_weights1; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();	
			dataBv = plm_weights1[i];			
			this->dma_write_chnl.put(dataBv);
			wait();	
		}

		// store weights2 values		
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_weights2, DMA_SIZE);
			offset += len_weights2;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_weights2; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();	
			dataBv = plm_weights2[i];	
			this->dma_write_chnl.put(dataBv);
		}
		
		// store weights3 values		
		{
			HLS_PROTO("store-dma");
			dma_info_t dma_info(offset, len_weights3, DMA_SIZE);
			offset += len_weights3;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_weights3; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			wait();	
			dataBv = plm_weights3[i];		
			this->dma_write_chnl.put(dataBv);
		}	

		// store biases1 values		
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_biases1, DMA_SIZE);
			offset += len_biases1;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_biases1; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();
			dataBv = plm_biases1[i];			
			this->dma_write_chnl.put(dataBv);
		}	

		// store biases2 values		
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_biases2, DMA_SIZE);
			offset += len_biases2;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_biases2; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();	
			dataBv = plm_biases2[i];		
			this->dma_write_chnl.put(dataBv);
		}	

		// store biases3 values		
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_biases3, DMA_SIZE);
			offset += len_biases3;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_biases3; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();	
			dataBv = plm_biases3[i];		
			this->dma_write_chnl.put(dataBv);
		}	

		// store training data		
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_training_data, DMA_SIZE);
			offset += len_training_data;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_training_data; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();	
			dataBv = plm_training_data[i];		
			this->dma_write_chnl.put(dataBv);
		}	

		// store training targets
		{
			HLS_PROTO("store-dma");
			dma_info_t dma_info(offset, len_training_targets, DMA_SIZE);
			offset += len_training_targets;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_training_targets; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();
			dataBv = plm_training_targets[i];			
			this->dma_write_chnl.put(dataBv);
		}		
    }
    // Conclude
    {
        this->accelerator_done();
        this->process_done();
    }
}


void kprop::compute_kernel()
{
    // Reset
    {
        HLS_PROTO("compute-reset");

        this->reset_compute_kernel();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_dimension;
    int32_t nodes_per_layer;
    int32_t possible_outputs;
    int32_t training_sets;
    {
        HLS_PROTO("compute-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_dimension = config.input_dimension;
        nodes_per_layer = config.nodes_per_layer;
        possible_outputs = config.possible_outputs;
        training_sets = config.training_sets;
    }


    // Compute
	int32_t i,j;
    {
		this->compute_load_handshake();
		
		for(i=0; i<training_sets; i++)
		{
			for(j=0;j<nodes_per_layer;j++)
			{
				activations1[j] = fp2int<FPDATA, WORD_SIZE>(0.0);
				activations2[j] = fp2int<FPDATA, WORD_SIZE>(0.0);
				if(j<possible_outputs)
				{
				   activations3[j] = fp2int<FPDATA, WORD_SIZE>(0.0);
				}
			}
			// only read from PLMs and arrays
			matrix_vector_product_with_bias_input_layer(plm_biases1, plm_weights1, activations1, &plm_training_data[i*input_dimension]);
			RELU(activations1, dactivations1, nodes_per_layer);	
			matrix_vector_product_with_bias_second_layer(plm_biases2, plm_weights2, activations2, activations1);
			RELU(activations2, dactivations2, nodes_per_layer);
			matrix_vector_product_with_bias_output_layer(plm_biases3, plm_weights3, activations3, activations2);
			RELU(activations3, dactivations3, possible_outputs);
			soft_max(net_outputs, activations3);			
			take_difference(net_outputs, &plm_training_targets[i*possible_outputs], output_difference, dactivations3);
			get_delta_matrix_weights3(delta_weights3, output_difference, activations2);
			get_oracle_activations2(plm_weights3, output_difference, oracle_activations2, dactivations2);		
			get_delta_matrix_weights2(delta_weights2, oracle_activations2, activations1);
			get_oracle_activations1(plm_weights2, oracle_activations2, oracle_activations1, dactivations1);	
			get_delta_matrix_weights1(delta_weights1, oracle_activations1, &plm_training_data[i*input_dimension]);		
			//update parameters (write to PLMs)
			update_weights(plm_weights1, plm_weights2, plm_weights3, delta_weights1, delta_weights2, delta_weights3, 
                       plm_biases1, plm_biases2, plm_biases3, oracle_activations1, oracle_activations2, output_difference);	
		}
		this->compute_store_handshake();
		/*
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights1[0]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights1[1]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights1[2]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights2[0]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights2[1]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights2[2]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights3[0]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights3[1]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_weights2[2]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_training_data[0]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_training_data[1]) << std::endl;
		std::cout<< int2fp<FPDATA,WORD_SIZE>(plm_training_data[2]) << std::endl;
		*/

        // Conclude
        {
            this->process_done();
        }
    }
}
