# Copyright (c) 2011-2019 Columbia University, System Level Design Group
# SPDX-License-Identifier: MIT

############################################################
# Design Parameters
############################################################

#
# Source the common configurations
#
source ../../common/stratus/project.tcl


#
# Set the private memory library
#
use_hls_lib "./memlib"


#
# Local synthesis attributes
#
if {$TECH eq "virtex7"} {
    # Library is in ns, but simulation uses ps!
    set CLOCK_PERIOD 10.0
    set SIM_CLOCK_PERIOD 10000.0
    set_attr default_input_delay      0.1
}
if {$TECH eq "zynq7000"} {
    # Library is in ns, but simulation uses ps!
    set CLOCK_PERIOD 10.0
    set SIM_CLOCK_PERIOD 10000.0
    set_attr default_input_delay      0.1
}
if {$TECH eq "cmos32soi"} {
    set CLOCK_PERIOD 1000.0
    set SIM_CLOCK_PERIOD 1000.0
    set_attr default_input_delay      100.0
}
set_attr clock_period $CLOCK_PERIOD

#
# DSE configuration parameters
#
# TODO REMOVE small image
# set PLM_IMG_SIZE "307200"
set PLM_IMG_SIZE "256"
set PLM_HIST_SIZE "65536"
set IMAGES "lena"

#
# System level modules to be synthesized
#
define_hls_module visionchip ../src/visionchip.cpp


#
# Testbench or system level modules
#
define_system_module tb ../tb/system.cpp ../tb/sc_main.cpp

######################################################################
# HLS and Simulation configurations
######################################################################
set DEFAULT_ARGV ""

foreach dma [list 32] {
    define_io_config * IOCFG_DMA$dma -DDMA_WIDTH=$dma \
	-DPLM_IMG_SIZE=$PLM_IMG_SIZE \
	-DPLM_HIST_SIZE=$PLM_HIST_SIZE

    define_system_config tb TESTBENCH_DMA$dma -io_config IOCFG_DMA$dma

    define_sim_config "BEHAV_DMA$dma" "visionchip BEH" "tb TESTBENCH_DMA$dma" -io_config IOCFG_DMA$dma -argv $DEFAULT_ARGV

    foreach cfg [list BASIC] {
	set cname $cfg\_DMA$dma
	define_hls_config visionchip $cname -io_config IOCFG_DMA$dma --clock_period=$CLOCK_PERIOD $COMMON_HLS_FLAGS -DHLS_DIRECTIVES_$cfg
	if {$TECH_IS_XILINX == 1} {
	    define_sim_config "$cname\_V" "visionchip RTL_V $cname" "tb TESTBENCH_DMA$dma" -io_config IOCFG_DMA$dma -argv $DEFAULT_ARGV -verilog_top_modules glbl
	} else {
	    define_sim_config "$cname\_V" "visionchip RTL_V $cname" "tb TESTBENCH_DMA$dma" -io_config IOCFG_DMA$dma -argv $DEFAULT_ARGV
	}
    }
}

#
# Compile Flags
#
set_attr hls_cc_options "$INCLUDES"

#
# Simulation Options
#
use_systemc_simulator incisive
set_attr cc_options "$INCLUDES -DCLOCK_PERIOD=$SIM_CLOCK_PERIOD"
# enable_waveform_logging -vcd
set_attr end_of_sim_command "make saySimPassed"
