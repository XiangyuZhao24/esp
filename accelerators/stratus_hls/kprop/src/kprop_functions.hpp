// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "kprop.hpp"

// Optional application-specific helper functions
inline void soft_max(FPDATA_WORD net_outputs[possible_outputs], FPDATA_WORD activations[possible_outputs]) {
    int i;
    FPDATA_WORD sum;
    sum = (FPDATA_WORD) 0.0;

    for(i=0; i < possible_outputs; i++) {
        sum += exp(-activations[i]);
    }
    for(i=0; i < possible_outputs; i++) {
        net_outputs[i] = exp(-activations[i])/sum;
    }
}

inline void RELU(FPDATA_WORD activations[nodes_per_layer], FPDATA_WORD dactivations[nodes_per_layer], int size) {
    int i;
    for( i = 0; i < size; i++) {
        dactivations[i] = activations[i]*(1.0-activations[i]);
        activations[i] = 1.0/(1.0+exp(-activations[i]));
    }
}

inline void add_bias_to_activations(FPDATA_WORD biases[nodes_per_layer], 
                               FPDATA_WORD activations[nodes_per_layer],
                               int size) {
    int i;
    for( i = 0; i < size; i++){
        activations[i] = activations[i] + biases[i];
    }
}

inline void matrix_vector_product_with_bias_input_layer(FPDATA_WORD biases[nodes_per_layer],
                                                 FPDATA_WORD weights[input_dimension*nodes_per_layer],
                                                 FPDATA_WORD activations[nodes_per_layer],
                                                 FPDATA_WORD input_sample[input_dimension]){
    int i,j;
    for(j = 0; j < nodes_per_layer; j++){
        activations[j] = (FPDATA_WORD)0.0;
        for (i = 0; i < input_dimension; i++){
            activations[j] += weights[j*input_dimension + i] * input_sample[i];
        }
    }
    add_bias_to_activations(biases, activations, nodes_per_layer);
}

inline void matrix_vector_product_with_bias_second_layer(FPDATA_WORD biases[nodes_per_layer],
                                                 FPDATA_WORD weights[nodes_per_layer*nodes_per_layer],
                                                 FPDATA_WORD activations[nodes_per_layer],
                                                 FPDATA_WORD input_activations[nodes_per_layer]){
    int i,j;
    for (i = 0; i < nodes_per_layer; i++){
        activations[i] = (FPDATA_WORD)0.0;
        for(j = 0; j < nodes_per_layer; j++){
            activations[i] += weights[i*nodes_per_layer + j] * input_activations[j];
        }
    }
    add_bias_to_activations(biases, activations, nodes_per_layer);
}

inline void matrix_vector_product_with_bias_output_layer(FPDATA_WORD biases[possible_outputs],
                                                 FPDATA_WORD weights[nodes_per_layer*possible_outputs],
                                                 FPDATA_WORD activations[possible_outputs],
                                                 FPDATA_WORD input_activations[nodes_per_layer]){
    int i, j;
    for(j = 0; j < possible_outputs; j++){
        activations[j] = (FPDATA_WORD)0.0;
        for (i = 0; i < nodes_per_layer; i++){
            activations[j] += weights[j*nodes_per_layer + i] * input_activations[i];
        }
    }
    add_bias_to_activations(biases, activations, possible_outputs);
}

inline void take_difference(FPDATA_WORD net_outputs[possible_outputs], 
                     FPDATA_WORD solutions[possible_outputs], 
                     FPDATA_WORD output_difference[possible_outputs],
                     FPDATA_WORD dactivations[possible_outputs]) {
    int i;
    for( i = 0; i < possible_outputs; i++){
        output_difference[i] = (((net_outputs[i]) - solutions[i]) * -1.0) * dactivations[i];
    }
}

inline void get_delta_matrix_weights3(FPDATA_WORD delta_weights3[nodes_per_layer*possible_outputs],
                               FPDATA_WORD output_difference[possible_outputs],
                               FPDATA_WORD last_activations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        for( j = 0; j < possible_outputs; j++) {
            delta_weights3[i*possible_outputs + j] = last_activations[i] * output_difference[j];
        }
    }
}

inline void get_oracle_activations2(FPDATA_WORD weights3[nodes_per_layer*possible_outputs], 
                             FPDATA_WORD output_differences[possible_outputs], 
                             FPDATA_WORD oracle_activations[nodes_per_layer],
                             FPDATA_WORD dactivations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        oracle_activations[i] = (FPDATA_WORD)0.0;
        for( j = 0; j < possible_outputs; j++) {
            oracle_activations[i] += output_differences[j] * weights3[i*possible_outputs + j];
        }
        oracle_activations[i] = oracle_activations[i] * dactivations[i];
    }
}

inline void get_delta_matrix_weights2(FPDATA_WORD delta_weights2[nodes_per_layer*nodes_per_layer],
                               FPDATA_WORD output_difference[nodes_per_layer],
                               FPDATA_WORD last_activations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        for( j = 0; j < nodes_per_layer; j++) {
            delta_weights2[i*nodes_per_layer + j] = last_activations[i] * output_difference[j];
        }
    }
}

inline void get_oracle_activations1(FPDATA_WORD weights2[nodes_per_layer*nodes_per_layer], 
                             FPDATA_WORD output_differences[nodes_per_layer], 
                             FPDATA_WORD oracle_activations[nodes_per_layer],
                             FPDATA_WORD dactivations[nodes_per_layer]) {
    int i, j;
    for( i = 0; i < nodes_per_layer; i++) {
        oracle_activations[i] = (FPDATA_WORD)0.0;
        for( j = 0; j < nodes_per_layer; j++) {
            oracle_activations[i] += output_differences[j] * weights2[i*nodes_per_layer + j];
        }
        oracle_activations[i] = oracle_activations[i] * dactivations[i];
    }
}

inline void get_delta_matrix_weights1(FPDATA_WORD delta_weights1[input_dimension*nodes_per_layer],
                               FPDATA_WORD output_difference[nodes_per_layer],
                               FPDATA_WORD last_activations[input_dimension]) {
    int i, j;
    for( i = 0; i < input_dimension; i++) {
        for( j = 0; j < nodes_per_layer; j++) {
            delta_weights1[i*nodes_per_layer + j] = last_activations[i] * output_difference[j];
        }
    }
}

inline void update_weights(FPDATA_WORD weights1[input_dimension*nodes_per_layer],
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
    FPDATA_WORD norm, bias_norm;
    norm = 0.0;
    bias_norm = 0.0;

    for(i=0; i < input_dimension; i++){
        for(j = 0; j < nodes_per_layer; j++){
            weights1[i*nodes_per_layer + j] -= (d_weights1[i*nodes_per_layer + j] * learning_rate);
            norm += weights1[i*nodes_per_layer + j]*weights1[i*nodes_per_layer + j];
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
            weights1[i*nodes_per_layer + j] = (weights1[i*nodes_per_layer + j]/norm);
        }
    }
    for(i=0; i < nodes_per_layer; i++){
        biases1[i] = (biases1[i]/bias_norm);
    }

    norm = (FPDATA_WORD)0.0;
    bias_norm = (FPDATA_WORD)0.0;

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < nodes_per_layer; j++){
            weights2[i*nodes_per_layer + j] -= (d_weights2[i*nodes_per_layer + j] * learning_rate);
            norm += weights2[i*nodes_per_layer + j]*weights2[i*nodes_per_layer + j];
        }
    }
    for(i=0; i < nodes_per_layer; i++){
        biases2[i] -= (d_biases2[i]*learning_rate);
        bias_norm += biases2[i]*biases2[i];
    }

    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < nodes_per_layer; j++){
            weights2[i*nodes_per_layer + j] = (weights2[i*nodes_per_layer + j]/norm);
        }
    }
    for(i=0; i < nodes_per_layer; i++){
        biases2[i] = (biases2[i]/bias_norm);
    }

    norm = (FPDATA_WORD)0.0;
    bias_norm = (FPDATA_WORD)0.0;

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < possible_outputs; j++){
            weights3[i*possible_outputs + j] -= (d_weights3[i*possible_outputs + j] * learning_rate);
            norm += weights3[i*possible_outputs + j]*weights3[i*possible_outputs + j];
        }
    }
    for(i=0; i<possible_outputs;i++){
        biases3[i] -= d_biases3[i]*learning_rate;
        bias_norm += biases3[i]*biases3[i];
    }

    norm = sqrt(norm);
    bias_norm = sqrt(bias_norm);

    for(i=0; i < nodes_per_layer; i++){
        for(j = 0; j < possible_outputs; j++){
            weights3[i*possible_outputs + j] = (weights3[i*possible_outputs + j]/norm);
        }
    }
    for(i=0; i < possible_outputs; i++){
        biases3[i] = (biases3[i]/bias_norm);
    }
}

inline void backprop(FPDATA_WORD weights1[input_dimension*nodes_per_layer], 
                FPDATA_WORD weights2[nodes_per_layer*nodes_per_layer],
                FPDATA_WORD weights3[nodes_per_layer*possible_outputs],
                FPDATA_WORD biases1[nodes_per_layer], 
                FPDATA_WORD biases2[nodes_per_layer],
                FPDATA_WORD biases3[possible_outputs],
                FPDATA_WORD training_data[training_sets*input_dimension],
                FPDATA_WORD training_targets[training_sets*possible_outputs]) {
    int i,j;
    //Forward and training structures
    FPDATA_WORD activations1[nodes_per_layer];
    FPDATA_WORD activations2[nodes_per_layer];
    FPDATA_WORD activations3[possible_outputs];
    FPDATA_WORD dactivations1[nodes_per_layer];
    FPDATA_WORD dactivations2[nodes_per_layer];
    FPDATA_WORD dactivations3[possible_outputs];
    FPDATA_WORD net_outputs[possible_outputs];
    //Training structure
    FPDATA_WORD output_difference[possible_outputs];
    FPDATA_WORD delta_weights1[input_dimension*nodes_per_layer]; 
    FPDATA_WORD delta_weights2[nodes_per_layer*nodes_per_layer];
    FPDATA_WORD delta_weights3[nodes_per_layer*possible_outputs];
    FPDATA_WORD oracle_activations1[nodes_per_layer];
    FPDATA_WORD oracle_activations2[nodes_per_layer];

    for(i=0; i<training_sets; i++){
        for(j=0;j<nodes_per_layer;j++){
            activations1[j] = (FPDATA_WORD)0.0;
            activations2[j] = (FPDATA_WORD)0.0;
            if(j<possible_outputs){
                activations3[j] = (FPDATA_WORD)0.0;
            }
        }
        matrix_vector_product_with_bias_input_layer(biases1, weights1, activations1, &training_data[i*input_dimension]);
        RELU(activations1, dactivations1, nodes_per_layer);
        matrix_vector_product_with_bias_second_layer(biases2, weights2, activations2, activations1);
        RELU(activations2, dactivations2, nodes_per_layer);
        matrix_vector_product_with_bias_output_layer(biases3, weights3, activations3, activations2);
        RELU(activations3, dactivations3, possible_outputs);
        soft_max(net_outputs, activations3);
        take_difference(net_outputs, &training_targets[i*possible_outputs], output_difference, dactivations3);
        get_delta_matrix_weights3(delta_weights3, output_difference, activations2);
        get_oracle_activations2(weights3, output_difference, oracle_activations2, dactivations2);
        get_delta_matrix_weights2(delta_weights2, oracle_activations2, activations1);
        get_oracle_activations1(weights2, oracle_activations2, oracle_activations1, dactivations1);
        get_delta_matrix_weights1(delta_weights1, oracle_activations1, &training_data[i*input_dimension]);
        update_weights(weights1, weights2, weights3, delta_weights1, delta_weights2, delta_weights3, 
                       biases1, biases2, biases3, oracle_activations1, oracle_activations2, output_difference);
    }
}
