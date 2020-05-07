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
    int32_t learning_rate;
    int32_t training_sets;
	int32_t index_weights1;
	int32_t index_weights2;
	int32_t index_weights3;
	int32_t index_biases1;
	int32_t index_biases2;
	int32_t index_biases3;
	int32_t index_training_data;
	int32_t index_training_targets;
	
	
    {
        HLS_PROTO("load-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_dimension = config.input_dimension;
        nodes_per_layer = config.nodes_per_layer;
        possible_outputs = config.possible_outputs;
        learning_rate = config.learning_rate;
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

    // Load
	/*  Default load
    {
        HLS_PROTO("load-dma");
        wait();

        bool ping = true;
        uint32_t offset = 0;

        // Batching
        for (uint16_t b = 0; b < 1; b++)
        {
            wait();
#if (DMA_WORD_PER_BEAT == 0)
            uint32_t length = input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs;
#else
            uint32_t length = round_up(input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs, DMA_WORD_PER_BEAT);
#endif
            // Chunking
            for (int rem = length; rem > 0; rem -= PLM_IN_WORD)
            {
                wait();
                // Configure DMA transaction
                uint32_t len = rem > PLM_IN_WORD ? PLM_IN_WORD : rem;
#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                dma_info_t dma_info(offset * DMA_BEAT_PER_WORD, len * DMA_BEAT_PER_WORD, DMA_SIZE);
#else
                dma_info_t dma_info(offset / DMA_WORD_PER_BEAT, len / DMA_WORD_PER_BEAT, DMA_SIZE);
#endif
                offset += len;

                this->dma_read_ctrl.put(dma_info);

#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                for (uint16_t i = 0; i < len; i++)
                {
                    sc_dt::sc_bv<DATA_WIDTH> dataBv;

                    for (uint16_t k = 0; k < DMA_BEAT_PER_WORD; k++)
                    {
                        dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH) = this->dma_read_chnl.get();
                        wait();
                    }

                    // Write to PLM
                    if (ping)
                        plm_in_ping[i] = dataBv.to_int64();
                    else
                        plm_in_pong[i] = dataBv.to_int64();
                }
#else
                for (uint16_t i = 0; i < len; i += DMA_WORD_PER_BEAT)
                {
                    HLS_BREAK_DEP(plm_in_ping);
                    HLS_BREAK_DEP(plm_in_pong);

                    sc_dt::sc_bv<DMA_WIDTH> dataBv;

                    dataBv = this->dma_read_chnl.get();
                    wait();

                    // Write to PLM (all DMA_WORD_PER_BEAT words in one cycle)
                    for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    {
                        HLS_UNROLL_SIMPLE;
                        if (ping)
                            plm_in_ping[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                        else
                            plm_in_pong[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                    }
                }
#endif
                this->load_compute_handshake();
                ping = !ping;
            }
        }
    }
	*/

	// New Load
    {
        HLS_PROTO("load-dma");
        wait();

        bool ping = true;
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
			HLS_BREAK_DEP(plm_weights1_ping);
			HLS_BREAK_DEP(plm_weights1_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_weights1_ping[i] = dataBv.to_int64();
			else
				plm_weights1_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_weights2_ping);
			HLS_BREAK_DEP(plm_weights2_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_weights2_ping[i] = dataBv.to_int64();
			else
				plm_weights2_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_weights3_ping);
			HLS_BREAK_DEP(plm_weights3_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_weights3_ping[i] = dataBv.to_int64();
			else
				plm_weights3_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_biases1_ping);
			HLS_BREAK_DEP(plm_biases1_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_biases1_ping[i] = dataBv.to_int64();
			else
				plm_biases1_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_biases2_ping);
			HLS_BREAK_DEP(plm_biases2_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_biases2_ping[i] = dataBv.to_int64();
			else
				plm_biases2_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_biases3_ping);
			HLS_BREAK_DEP(plm_biases3_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_biases3_ping[i] = dataBv.to_int64();
			else
				plm_biases3_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_training_data_ping);
			HLS_BREAK_DEP(plm_training_data_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_training_data_ping[i] = dataBv.to_int64();
			else
				plm_training_data_pong[i] = dataBv.to_int64();
			}			
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
			HLS_BREAK_DEP(plm_training_targets_ping);
			HLS_BREAK_DEP(plm_training_targets_pong);
			
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			dataBv = this->dma_read_chnl.get();
			wait();
			
			{
			if (ping)
				plm_training_targets_ping[i] = dataBv.to_int64();
			else
				plm_training_targets_pong[i] = dataBv.to_int64();
			}			
		}		

		this->load_compute_handshake();
		ping = !ping;
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
    int32_t learning_rate;
    int32_t training_sets;
    {
        HLS_PROTO("store-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_dimension = config.input_dimension;
        nodes_per_layer = config.nodes_per_layer;
        possible_outputs = config.possible_outputs;
        learning_rate = config.learning_rate;
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

    // Store
	/* Default Store
    {
        HLS_PROTO("store-dma");
        wait();

        bool ping = true;
#if (DMA_WORD_PER_BEAT == 0)
        uint32_t store_offset = (input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs) * 1;
#else
        uint32_t store_offset = round_up(input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs, DMA_WORD_PER_BEAT) * 1;
#endif
        uint32_t offset = store_offset;

        wait();
        // Batching
        for (uint16_t b = 0; b < 1; b++)
        {
            wait();
#if (DMA_WORD_PER_BEAT == 0)
            uint32_t length = input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs;
#else
            uint32_t length = round_up(input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs, DMA_WORD_PER_BEAT);
#endif
            // Chunking
            for (int rem = length; rem > 0; rem -= PLM_OUT_WORD)
            {

                this->store_compute_handshake();

                // Configure DMA transaction
                uint32_t len = rem > PLM_OUT_WORD ? PLM_OUT_WORD : rem;
#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                dma_info_t dma_info(offset * DMA_BEAT_PER_WORD, len * DMA_BEAT_PER_WORD, DMA_SIZE);
#else
                dma_info_t dma_info(offset / DMA_WORD_PER_BEAT, len / DMA_WORD_PER_BEAT, DMA_SIZE);
#endif
                offset += len;

                this->dma_write_ctrl.put(dma_info);

#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                for (uint16_t i = 0; i < len; i++)
                {
                    // Read from PLM
                    sc_dt::sc_int<DATA_WIDTH> data;
                    wait();
                    if (ping)
                        data = plm_out_ping[i];
                    else
                        data = plm_out_pong[i];
                    sc_dt::sc_bv<DATA_WIDTH> dataBv(data);

                    uint16_t k = 0;
                    for (k = 0; k < DMA_BEAT_PER_WORD - 1; k++)
                    {
                        this->dma_write_chnl.put(dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH));
                        wait();
                    }
                    // Last beat on the bus does not require wait(), which is
                    // placed before accessing the PLM
                    this->dma_write_chnl.put(dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH));
                }
#else
                for (uint16_t i = 0; i < len; i += DMA_WORD_PER_BEAT)
                {
                    sc_dt::sc_bv<DMA_WIDTH> dataBv;

                    // Read from PLM
                    wait();
                    for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    {
                        HLS_UNROLL_SIMPLE;
                        if (ping)
                            dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_ping[i + k];
                        else
                            dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_pong[i + k];
                    }
                    this->dma_write_chnl.put(dataBv);
                }
#endif
                ping = !ping;
            }
        }
    }
	*/
    // New Store
	{
        HLS_PROTO("store-dma");
        wait();

        bool ping = true;
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
			if (ping)
				dataBv = plm_weights1_ping[i];
			else
				dataBv = plm_weights1_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}

		// store weights2 values		
		this->store_compute_handshake();
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
			if (ping)
				dataBv = plm_weights2_ping[i];
			else
				dataBv = plm_weights2_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}
		
		// store weights3 values		
		this->store_compute_handshake();
		{
			HLS_PROTO("store-dma");
			
			dma_info_t dma_info(offset, len_weights3, DMA_SIZE);
			offset += len_weights3;
			this->dma_write_ctrl.put(dma_info);	
		}
		
		for (uint16_t i = 0; i < len_weights3; i++)
		{
			sc_dt::sc_bv<DMA_WIDTH> dataBv;
			// Read from PLM
			wait();
			if (ping)
				dataBv = plm_weights3_ping[i];
			else
				dataBv = plm_weights3_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}	

		// store biases1 values		
		this->store_compute_handshake();
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
			if (ping)
				dataBv = plm_biases1_ping[i];
			else
				dataBv = plm_biases1_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}	

		// store biases2 values		
		this->store_compute_handshake();
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
			if (ping)
				dataBv = plm_biases2_ping[i];
			else
				dataBv = plm_biases2_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}	

		// store biases3 values		
		this->store_compute_handshake();
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
			if (ping)
				dataBv = plm_biases3_ping[i];
			else
				dataBv = plm_biases3_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}	

		// store training data		
		this->store_compute_handshake();
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
			if (ping)
				dataBv = plm_training_data_ping[i];
			else
				dataBv = plm_training_data_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}	

		// store training targets
		this->store_compute_handshake();
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
			if (ping)
				dataBv = plm_training_targets_ping[i];
			else
				dataBv = plm_training_targets_pong[i];
			
			wait();			
			this->dma_write_chnl.put(dataBv);
		}		

		ping = !ping;
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
    int32_t learning_rate;
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
        learning_rate = config.learning_rate;
        training_sets = config.training_sets;
    }


    // Compute
    bool ping = true;
    {
        for (uint16_t b = 0; b < 1; b++)
        {
            uint32_t in_length = input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs;
            uint32_t out_length = input_dimension * nodes_per_layer + nodes_per_layer * nodes_per_layer + nodes_per_layer * possible_outputs + nodes_per_layer + nodes_per_layer + possible_outputs + training_sets * input_dimension + training_sets * possible_outputs;
            int out_rem = out_length;

            for (int in_rem = in_length; in_rem > 0; in_rem -= PLM_IN_WORD)
            {

                uint32_t in_len  = in_rem  > PLM_IN_WORD  ? PLM_IN_WORD  : in_rem;
                uint32_t out_len = out_rem > PLM_OUT_WORD ? PLM_OUT_WORD : out_rem;

                this->compute_load_handshake();

                // Computing phase implementation
                for (int i = 0; i < in_len; i++) {
                    if (ping)
                        plm_out_ping[i] = plm_in_ping[i];
                    else
                        plm_out_pong[i] = plm_in_pong[i];
                }

                out_rem -= PLM_OUT_WORD;
                this->compute_store_handshake();
                ping = !ping;
            }
        }

        // Conclude
        {
            this->process_done();
        }
    }
}
