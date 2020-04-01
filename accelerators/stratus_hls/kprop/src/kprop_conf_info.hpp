// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#ifndef __KPROP_CONF_INFO_HPP__
#define __KPROP_CONF_INFO_HPP__

#include <systemc.h>

//
// Configuration parameters for the accelerator.
//
class conf_info_t
{
public:

    //
    // constructors
    //
    conf_info_t()
    {
        /* <<--ctor-->> */
        this->input_dimension = 13;
        this->nodes_per_layer = 64;
        this->possible_outputs = 3;
        this->learning_rate = 0.01;
        this->training_sets = 163;
    }

    conf_info_t(
        /* <<--ctor-args-->> */
        int32_t input_dimension, 
        int32_t nodes_per_layer, 
        int32_t possible_outputs, 
        int32_t learning_rate, 
        int32_t training_sets
        )
    {
        /* <<--ctor-custom-->> */
        this->input_dimension = input_dimension;
        this->nodes_per_layer = nodes_per_layer;
        this->possible_outputs = possible_outputs;
        this->learning_rate = learning_rate;
        this->training_sets = training_sets;
    }

    // equals operator
    inline bool operator==(const conf_info_t &rhs) const
    {
        /* <<--eq-->> */
        if (input_dimension != rhs.input_dimension) return false;
        if (nodes_per_layer != rhs.nodes_per_layer) return false;
        if (possible_outputs != rhs.possible_outputs) return false;
        if (learning_rate != rhs.learning_rate) return false;
        if (training_sets != rhs.training_sets) return false;
        return true;
    }

    // assignment operator
    inline conf_info_t& operator=(const conf_info_t& other)
    {
        /* <<--assign-->> */
        input_dimension = other.input_dimension;
        nodes_per_layer = other.nodes_per_layer;
        possible_outputs = other.possible_outputs;
        learning_rate = other.learning_rate;
        training_sets = other.training_sets;
        return *this;
    }

    // VCD dumping function
    friend void sc_trace(sc_trace_file *tf, const conf_info_t &v, const std::string &NAME)
    {}

    // redirection operator
    friend ostream& operator << (ostream& os, conf_info_t const &conf_info)
    {
        os << "{";
        /* <<--print-->> */
        os << "input_dimension = " << conf_info.input_dimension << ", ";
        os << "nodes_per_layer = " << conf_info.nodes_per_layer << ", ";
        os << "possible_outputs = " << conf_info.possible_outputs << ", ";
        os << "learning_rate = " << conf_info.learning_rate << ", ";
        os << "training_sets = " << conf_info.training_sets << "";
        os << "}";
        return os;
    }

        /* <<--params-->> */
        int32_t input_dimension;
        int32_t nodes_per_layer;
        int32_t possible_outputs;
        int32_t learning_rate;
        int32_t training_sets;
};

#endif // __KPROP_CONF_INFO_HPP__
