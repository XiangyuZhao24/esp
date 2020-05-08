// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "kprop.hpp"

// Optional application-specific helper functions
void kprop::soft_max(FPDATA_WORD net_outputs[possible_outputs], FPDATA_WORD activations[possible_outputs]) {
    int i;
    FPDATA sum;
    sum = (FPDATA) 0.0;

    for(i=0; i < possible_outputs; i++) {
        sum += exp(-activations[i]);
    }
    for(i=0; i < possible_outputs; i++) {
        net_outputs[i] = exp(-activations[i])/sum;
    }
}

void kprop::RELU(FPDATA_WORD activations[nodes_per_layer], FPDATA_WORD dactivations[nodes_per_layer], int size) {
    int i;
    for( i = 0; i < size; i++) {
        dactivations[i] = activations[i]*(1.0-activations[i]);
        activations[i] = 1.0/(1.0+exp(-activations[i]));
    }
}

void kprop::add_bias_to_activations(FPDATA_WORD biases[nodes_per_layer], 
                               FPDATA_WORD activations[nodes_per_layer],
                               int size) {
    int i;
    for( i = 0; i < size; i++){
		bias = int2fp<FPDATA, WORD_SIZE>(biases[i]);
        activations[i] = activations[i] + bias;
    }
}

void kprop::matrix_vector_product_with_bias_input_layer(FPDATA_WORD biases[nodes_per_layer],
                                                 FPDATA_WORD weights[input_dimension*nodes_per_layer],
                                                 FPDATA_WORD activations[nodes_per_layer],
                                                 FPDATA_WORD input_sample[input_dimension]){
    int i,j;
    for(j = 0; j < nodes_per_layer; j++){
        activations[j] = (FPDATA)0.0;
        for (i = 0; i < input_dimension; i++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights[j*input_dimension + i]);
			sample = int2fp<FPDATA, WORD_SIZE>(input_sample[i]);
            activations[j] += weight * sample;
        }
    }
    add_bias_to_activations(biases, activations, nodes_per_layer);
}

void kprop::matrix_vector_product_with_bias_second_layer(FPDATA_WORD biases[nodes_per_layer],
                                                 FPDATA_WORD weights[nodes_per_layer*nodes_per_layer],
                                                 FPDATA_WORD activations[nodes_per_layer],
                                                 FPDATA_WORD input_activations[nodes_per_layer]){
    int i,j;
    for (i = 0; i < nodes_per_layer; i++){
        activations[i] = (FPDATA)0.0;
        for(j = 0; j < nodes_per_layer; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights[i*nodes_per_layer + j])
			activation = int2fp<FPDATA, WORD_SIZE>(input_activations[j]);
            activations[i] += weight * activation;
        }
    }
    add_bias_to_activations(biases, activations, nodes_per_layer);
}

void kprop::matrix_vector_product_with_bias_output_layer(FPDATA_WORD biases[possible_outputs],
                                                 FPDATA_WORD weights[nodes_per_layer*possible_outputs],
                                                 FPDATA_WORD activations[possible_outputs],
                                                 FPDATA_WORD input_activations[nodes_per_layer]){
    int i, j;
    for(j = 0; j < possible_outputs; j++){
        activations[j] = (FPDATA)0.0;
        for (i = 0; i < nodes_per_layer; i++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights[j*nodes_per_layer + i])
			activation = int2fp<FPDATA, WORD_SIZE>(input_activations[i]);
            activations[j] += weight * activation;
        }
    }
    add_bias_to_activations(biases, activations, possible_outputs);
}

void kprop::take_difference(FPDATA_WORD net_outputs[possible_outputs], 
                     FPDATA_WORD solutions[possible_outputs], 
                     FPDATA_WORD output_difference[possible_outputs],
                     FPDATA_WORD dactivations[possible_outputs]) {
    int i;
    for( i = 0; i < possible_outputs; i++){
        output_difference[i] = (((net_outputs[i]) - solutions[i]) * -1.0) * dactivations[i];
    }
}

void kprop::get_delta_matrix_weights3(FPDATA_WORD delta_weights3[nodes_per_layer*possible_outputs],
                               FPDATA_WORD output_difference[possible_outputs],
                               FPDATA_WORD last_activations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        for( j = 0; j < possible_outputs; j++) {
            delta_weights3[i*possible_outputs + j] = last_activations[i] * output_difference[j];
        }
    }
}

void kprop::get_oracle_activations2(FPDATA_WORD weights3[nodes_per_layer*possible_outputs], 
                             FPDATA_WORD output_differences[possible_outputs], 
                             FPDATA_WORD oracle_activations[nodes_per_layer],
                             FPDATA_WORD dactivations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        oracle_activations[i] = (FPDATA)0.0;
        for( j = 0; j < possible_outputs; j++) {
			weight = int2fp<FPDATA, WORD_SIZE>(weights3[i*possible_outputs + j]);
            oracle_activations[i] += output_differences[j] * weight;
        }
        oracle_activations[i] = oracle_activations[i] * dactivations[i];
    }
}

void kprop::get_delta_matrix_weights2(FPDATA_WORD delta_weights2[nodes_per_layer*nodes_per_layer],
                               FPDATA_WORD output_difference[nodes_per_layer],
                               FPDATA_WORD last_activations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        for( j = 0; j < nodes_per_layer; j++) {
            delta_weights2[i*nodes_per_layer + j] = last_activations[i] * output_difference[j];
        }
    }
}

void kprop::get_oracle_activations1(FPDATA_WORD weights2[nodes_per_layer*nodes_per_layer], 
                             FPDATA_WORD output_differences[nodes_per_layer], 
                             FPDATA_WORD oracle_activations[nodes_per_layer],
                             FPDATA_WORD dactivations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        oracle_activations[i] = (FPDATA)0.0;
        for( j = 0; j < nodes_per_layer; j++) {
			weight = int2fp<FPDATA, WORD_SIZE>(weights2[i*nodes_per_layer + j]);
            oracle_activations[i] += output_differences[j] * weight;
        }
        oracle_activations[i] = oracle_activations[i] * dactivations[i];
    }
}

void kprop::get_delta_matrix_weights1(FPDATA_WORD delta_weights1[input_dimension*nodes_per_layer],
                               FPDATA_WORD output_difference[nodes_per_layer],
                               FPDATA_WORD last_activations[input_dimension]) {
    int i, j;
    for( i = 0; i < input_dimension; i++) {
        for( j = 0; j < nodes_per_layer; j++) {
            delta_weights1[i*nodes_per_layer + j] = last_activations[i] * output_difference[j];
        }
    }
}

void kprop::update_weights(FPDATA_WORD weights1[input_dimension*nodes_per_layer],
                    FPDATA_WORD weights2[nodes_per_layer*nodes_per_layer],
                    FPDATA_WORD weights3[nodes_per_layer*possible_outputs],
                    FPDATA_WORD d_weights1[input_dimension*nodes_per_layer],
                    FPDATA_WORD d_weights2[nodes_per_layer*nodes_per_layer],
                    FPDATA_WORD d_weights3[nodes_per_layer*possible_outputs],
                    FPDATA_WORD biases1[nodes_per_layer],
                    FPDATA_WORD biases2[nodes_per_layer],
                    FPDATA_WORD biases3[possible_outputs],
                    FPDATA_WORD d_biases1[nodes_per_layer],
                    FPDATA_WORD d_biases2[nodes_per_layer],
                    FPDATA_WORD d_biases3[possible_outputs]) {
    int i, j;
    FPDATA norm, bias_norm;
    norm = 0.0;
    bias_norm = 0.0;

    for(i=0; i < input_dimension; i++){
        for(j = 0; j < nodes_per_layer; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights1[i*nodes_per_layer + j]);
            weight -= (d_weights1[i*nodes_per_layer + j] * learning_rate);
            norm += weight*weight;
			weights1[i*nodes_per_layer + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i < nodes_per_layer; i++){
        biases1[i] -= (d_biases1[i]*learning_rate);
        bias_norm += biases1[i]*biases1[i];
    }
    
    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < input_dimension; i++){
        for(j = 0; j < nodes_per_layer; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights1[i*nodes_per_layer + j]);
            weight = (weight / norm);
			weights1[i*nodes_per_layer + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i < nodes_per_layer; i++){
		bias = int2fp<FPDATA, WORD_SIZE>(biases1[i]);
        bias = (bias / bias_norm);
		biases1[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = (FPDATA)0.0;
    bias_norm = (FPDATA)0.0;

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < nodes_per_layer; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights2[i*nodes_per_layer + j]);
            weight -= (d_weights2[i*nodes_per_layer + j] * learning_rate);
            norm += weight*weights;
			weights2[i*nodes_per_layer + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i < nodes_per_layer; i++){
		bias = int2fp<FPDATA, WORD_SIZE>(biases2[i]);
        bias -= (d_biases2[i]*learning_rate);
        bias_norm += bias*bias;
		biases2[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < nodes_per_layer; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights2[i*nodes_per_layer + j]);
            weight = (weight / norm);
            weights2[i*nodes_per_layer + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i < nodes_per_layer; i++){
		bias = int2fp<FPDATA, WORD_SIZE>(biases2[i]);
        bias = (bias / bias_norm);
        biases2[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = (FPDATA)0.0;
    bias_norm = (FPDATA)0.0;

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < possible_outputs; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights3[i*possible_outputs + j]);
            weight -= (d_weights3[i*possible_outputs + j] * learning_rate);
            norm += weights*weight;
			weights3[i*possible_outputs + j] = fp2int<FPDATA, WORD_SIZE>(weight);
        }
    }
    for(i=0; i<possible_outputs;i++){
		bias = int2fp<FPDATA, WORD_SIZE>(biases3[i]);
        bias -= d_biases3[i]*learning_rate;
        bias_norm += bias*bias;
		biases3[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }

    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < possible_outputs; j++){
			weight = int2fp<FPDATA, WORD_SIZE>(weights3[i*possible_outputs + j]);
            weight = (weight / norm);
			weights3[i*possible_outputs + j] = fp2int<FPDATA, WORD_SIZE>(weight);
		}
    }
    for(i=0; i < possible_outputs; i++){
		bias = int2fp<FPDATA, WORD_SIZE>(biases3[i]);
        bias = (bias / bias_norm);
		biases3[i] = fp2int<FPDATA, WORD_SIZE>(bias);
    }
}