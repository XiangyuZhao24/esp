// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "kprop.hpp"

// Optional application-specific helper functions
FPDATA exp_taylor(FPDATA x){          //an approximation exp(x)
	FPDATA result = 0.0;
	if(x <= 0.0){
		x *= -1;
		//result = (1 + x + x * x / 2 + x * x * x / 6 + x * x * x * x / 24 + x * x * x * x * x / 120 + x * x * x * x * x / 720 * x );
		result = (1 + x + x * x / 2 + x * x * x / 6 + x * x * x * x / 24);
		return 1.0 / result;
	}else{
		//result = (1 + x + x * x / 2 + x * x * x / 6 + x * x * x * x / 24 + x * x * x * x * x / 120 + x * x * x * x * x / 720 * x );
		result = (1 + x + x * x / 2 + x * x * x / 6 + x * x * x * x / 24);
		return result;
	}
}



void kprop::soft_max(FPDATA_WORD net_outputs[possible_outputs_size], FPDATA_WORD activations[possible_outputs_size]) {
    int i;
    FPDATA sum = 0.0;

    for(i=0; i < possible_outputs_size; i++) {
		FPDATA activation = int2fp<FPDATA, WORD_SIZE>(activations[i]);
		//sum += exp(-activation);
		sum += exp_taylor(-activation);
    }
    for(i=0; i < possible_outputs_size; i++) {
		FPDATA activation = int2fp<FPDATA, WORD_SIZE>(activations[i]);
        //FPDATA net_output = exp(-activation)/sum;
        FPDATA net_output = exp_taylor(-activation)/sum;
		net_outputs[i] = fp2int<FPDATA, WORD_SIZE>(net_output);
    }
}

void kprop::RELU(FPDATA_WORD activations[nodes_per_layer_size], FPDATA_WORD dactivations[nodes_per_layer_size], int size) {
    int i;
    for( i = 0; i < size; i++) {
		FPDATA activation = int2fp<FPDATA, WORD_SIZE>(activations[i]);
		FPDATA dactivation = activation * (1.0-activation);
        dactivations[i] = fp2int<FPDATA, WORD_SIZE>(dactivation);
		//FPDATA activation_new = (1.0/(1.0+exp(-activation)));
		FPDATA activation_new = (1.0/(1.0+exp_taylor(-activation)));
        activations[i] = fp2int<FPDATA, WORD_SIZE>(activation_new);
    }
}

void kprop::add_bias_to_activations(FPDATA_WORD biases[nodes_per_layer_size], 
                               FPDATA_WORD activations[nodes_per_layer_size],
                               int size) {
    int i;
    for( i = 0; i < size; i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases[i]);
		FPDATA activation = int2fp<FPDATA, WORD_SIZE>(activations[i]) + bias;
        activations[i] = fp2int<FPDATA, WORD_SIZE>(activation);
    }
}

void kprop::matrix_vector_product_with_bias_input_layer(FPDATA_WORD biases[nodes_per_layer_size],
                                                 FPDATA_WORD weights[input_dimension_size*nodes_per_layer_size],
                                                 FPDATA_WORD activations[nodes_per_layer_size],
                                                 FPDATA_WORD input_sample[input_dimension_size]){
    int i,j;
    for(j = 0; j < nodes_per_layer_size; j++){
		FPDATA activation = 0.0;
        for (i = 0; i < input_dimension_size; i++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights[j*input_dimension_size + i]);
			FPDATA sample = int2fp<FPDATA, WORD_SIZE>(input_sample[i]);
            activation += weight * sample;
        }
		activations[j] = fp2int<FPDATA, WORD_SIZE>(activation);
    }
    add_bias_to_activations(biases, activations, nodes_per_layer_size);
}

void kprop::matrix_vector_product_with_bias_second_layer(FPDATA_WORD biases[nodes_per_layer_size],
                                                 FPDATA_WORD weights[nodes_per_layer_size*nodes_per_layer_size],
                                                 FPDATA_WORD activations[nodes_per_layer_size],
                                                 FPDATA_WORD input_activations[nodes_per_layer_size]){
    int i,j;
    for (i = 0; i < nodes_per_layer_size; i++){
		FPDATA activation = 0.0;
        activations[i] = (FPDATA)0.0;
        for(j = 0; j < nodes_per_layer_size; j++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights[i*nodes_per_layer_size + j]);
			FPDATA input_activation = int2fp<FPDATA, WORD_SIZE>(input_activations[j]);
            activation += weight * input_activation;
        }
		activations[i] = fp2int<FPDATA, WORD_SIZE>(activation);
    }
    add_bias_to_activations(biases, activations, nodes_per_layer_size);
}

void kprop::matrix_vector_product_with_bias_output_layer(FPDATA_WORD biases[possible_outputs_size],
                                                 FPDATA_WORD weights[nodes_per_layer_size*possible_outputs_size],
                                                 FPDATA_WORD activations[possible_outputs_size],
                                                 FPDATA_WORD input_activations[nodes_per_layer_size]){
    int i, j;
    for(j = 0; j < possible_outputs_size; j++){
		FPDATA activation = 0.0;
        for (i = 0; i < nodes_per_layer_size; i++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights[j*nodes_per_layer_size + i]);
			FPDATA input_activation = int2fp<FPDATA, WORD_SIZE>(input_activations[i]);
            activation += weight * input_activation;
        }
		activations[j] = fp2int<FPDATA, WORD_SIZE>(activation);
    }
    add_bias_to_activations(biases, activations, possible_outputs_size);
}

void kprop::take_difference(FPDATA_WORD net_outputs[possible_outputs_size], 
                     FPDATA_WORD solutions[possible_outputs_size], 
                     FPDATA_WORD output_difference[possible_outputs_size],
                     FPDATA_WORD dactivations[possible_outputs_size]) {
    int i;
    for( i = 0; i < possible_outputs_size; i++){
		FPDATA net_output = int2fp<FPDATA, WORD_SIZE>(net_outputs[i]);
		FPDATA solution = int2fp<FPDATA, WORD_SIZE>(solutions[i]);		
		FPDATA dactivation = int2fp<FPDATA, WORD_SIZE>(dactivations[i]);	
        FPDATA difference = ((net_output - solution) * -1.0) * dactivation;
		output_difference[i] = fp2int<FPDATA, WORD_SIZE>(difference);
    }
}

void kprop::get_delta_matrix_weights3(FPDATA_WORD delta_weights3[nodes_per_layer_size*possible_outputs_size],
                               FPDATA_WORD output_difference[possible_outputs_size],
                               FPDATA_WORD last_activations[nodes_per_layer_size]) {
    int i, j;
    for( i = 0; i < nodes_per_layer_size; i++) {
        for( j = 0; j < possible_outputs_size; j++) {
			FPDATA activation = int2fp<FPDATA, WORD_SIZE>(last_activations[i]);
			FPDATA difference = int2fp<FPDATA, WORD_SIZE>(output_difference[j]);
			FPDATA d_weight = activation * difference;
            delta_weights3[i*possible_outputs_size + j] = fp2int<FPDATA, WORD_SIZE>(d_weight);
        }
    }
}

void kprop::get_oracle_activations2(FPDATA_WORD weights3[nodes_per_layer_size*possible_outputs_size], 
                             FPDATA_WORD output_differences[possible_outputs_size], 
                             FPDATA_WORD oracle_activations[nodes_per_layer_size],
                             FPDATA_WORD dactivations[nodes_per_layer_size]) {
    int i, j;
    for( i = 0; i < nodes_per_layer_size; i++) {
		FPDATA oracle_activation = 0.0;
        for( j = 0; j < possible_outputs_size; j++) {
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights3[i*possible_outputs_size + j]);
			FPDATA difference = int2fp<FPDATA, WORD_SIZE>(output_differences[j]);
            oracle_activation += difference * weight;
        }
		FPDATA dactivation = int2fp<FPDATA, WORD_SIZE>(dactivations[i]);
        oracle_activation *= dactivation;
		oracle_activations[i] = fp2int<FPDATA, WORD_SIZE>(oracle_activation);
    }
}

void kprop::get_delta_matrix_weights2(FPDATA_WORD delta_weights2[nodes_per_layer_size*nodes_per_layer_size],
                               FPDATA_WORD output_difference[nodes_per_layer_size],
                               FPDATA_WORD last_activations[nodes_per_layer_size]) {
    int i, j;
    for( i = 0; i < nodes_per_layer_size; i++) {
        for( j = 0; j < nodes_per_layer_size; j++) {
			FPDATA activation = int2fp<FPDATA, WORD_SIZE>(last_activations[i]);
			FPDATA difference = int2fp<FPDATA, WORD_SIZE>(output_difference[j]);
			FPDATA d_weight = activation * difference;
            delta_weights2[i*nodes_per_layer_size + j] = fp2int<FPDATA, WORD_SIZE>(d_weight);
        }
    }
}

void kprop::get_oracle_activations1(FPDATA_WORD weights2[nodes_per_layer_size*nodes_per_layer_size], 
                             FPDATA_WORD output_difference[nodes_per_layer_size], 
                             FPDATA_WORD oracle_activations[nodes_per_layer_size],
                             FPDATA_WORD dactivations[nodes_per_layer_size]) {
    int i, j;
    for( i = 0; i < nodes_per_layer_size; i++) {
		FPDATA oracle_activation = 0.0;
        oracle_activations[i] = (FPDATA)0.0;
        for( j = 0; j < nodes_per_layer_size; j++) {
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights2[i*nodes_per_layer_size + j]);
			FPDATA difference = int2fp<FPDATA, WORD_SIZE>(output_difference[j]);
            oracle_activation += difference * weight;
        }
		FPDATA dactivation = int2fp<FPDATA, WORD_SIZE>(dactivations[i]);
        oracle_activation *= dactivation;
        oracle_activations[i] = fp2int<FPDATA, WORD_SIZE>(oracle_activation);
    }
}

void kprop::get_delta_matrix_weights1(FPDATA_WORD delta_weights1[input_dimension_size*nodes_per_layer_size],
                               FPDATA_WORD output_difference[nodes_per_layer_size],
                               FPDATA_WORD last_activations[input_dimension_size]) {
    int i, j;
    for( i = 0; i < input_dimension_size; i++) {
        for( j = 0; j < nodes_per_layer_size; j++) {
			FPDATA activation = int2fp<FPDATA, WORD_SIZE>(last_activations[i]);
			FPDATA difference = int2fp<FPDATA, WORD_SIZE>(output_difference[j]);
			FPDATA d_weight = activation * difference;
            delta_weights1[i*nodes_per_layer_size + j] = fp2int<FPDATA, WORD_SIZE>(d_weight);
        }
    }
}

void kprop::update_weights(FPDATA_WORD weights1[input_dimension_size*nodes_per_layer_size],
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
                    FPDATA_WORD d_biases3[possible_outputs_size]) {
    int i, j;
    FPDATA norm, bias_norm;
    norm = 0.0;
    bias_norm = 0.0;
    for(i=0; i < input_dimension_size; i++){
        for(j = 0; j < nodes_per_layer_size; j++){
			
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights1[i*nodes_per_layer_size + j]);
			FPDATA d_weight = int2fp<FPDATA, WORD_SIZE>(d_weights1[i*nodes_per_layer_size + j]);
            weight -= (d_weight * lr);
            norm += weight*weight;     //will overflow here
			weights1[i*nodes_per_layer_size + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
	
    for(i=0; i < nodes_per_layer_size; i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases1[i]);
		FPDATA d_bias = int2fp<FPDATA, WORD_SIZE>(d_biases1[i]);
        bias -= (d_bias*lr);
        bias_norm += bias*bias;
		biases1[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }
    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);
	
    for(i=0; i < input_dimension_size; i++){
        for(j = 0; j < nodes_per_layer_size; j++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights1[i*nodes_per_layer_size + j]);
            weight = (weight / norm);
			weights1[i*nodes_per_layer_size + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
	
    for(i=0; i < nodes_per_layer_size; i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases1[i]);
        bias = (bias / bias_norm);
		biases1[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }


    norm = (FPDATA)0.0;
    bias_norm = (FPDATA)0.0;

    for(i=0; i < nodes_per_layer_size; i++){
        for(j = 0; j < nodes_per_layer_size; j++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights2[i*nodes_per_layer_size + j]);
			FPDATA d_weight = int2fp<FPDATA, WORD_SIZE>(d_weights2[i*nodes_per_layer_size + j]);
            weight -= (d_weight * lr);
            norm += weight*weight;
			weights2[i*nodes_per_layer_size + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i < nodes_per_layer_size; i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases2[i]);
		FPDATA d_bias = int2fp<FPDATA, WORD_SIZE>(d_biases2[i]);
        bias -= (d_bias*lr);
        bias_norm += bias*bias;
		biases2[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < nodes_per_layer_size; i++){
        for(j = 0; j < nodes_per_layer_size; j++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights2[i*nodes_per_layer_size + j]);
            weight = (weight / norm);
            weights2[i*nodes_per_layer_size + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i < nodes_per_layer_size; i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases2[i]);
        bias = (bias / bias_norm);
        biases2[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = (FPDATA)0.0;
    bias_norm = (FPDATA)0.0;

    for(i=0; i < nodes_per_layer_size; i++){
        for(j = 0; j < possible_outputs_size; j++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights3[i*possible_outputs_size + j]);
			FPDATA d_weight = int2fp<FPDATA, WORD_SIZE>(d_weights3[i*possible_outputs_size + j]);
            weight -= (d_weight * lr);
            norm += weight*weight;
			weights3[i*possible_outputs_size + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i<possible_outputs_size;i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases3[i]);
		FPDATA d_bias = int2fp<FPDATA, WORD_SIZE>(d_biases3[i]);
        bias -= d_bias*lr;
        bias_norm += bias*bias;
		biases3[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < nodes_per_layer_size; i++){
        for(j = 0; j < possible_outputs_size; j++){
			FPDATA weight = int2fp<FPDATA, WORD_SIZE>(weights3[i*possible_outputs_size + j]);
            weight = (weight / norm);
			weights3[i*possible_outputs_size + j] = fp2int<FPDATA, WORD_SIZE>(weight);
		}
    }
    for(i=0; i < possible_outputs_size; i++){
		FPDATA bias = int2fp<FPDATA, WORD_SIZE>(biases3[i]);
        bias = (bias / bias_norm);
		biases3[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }
}
