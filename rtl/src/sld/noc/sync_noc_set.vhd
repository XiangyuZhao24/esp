-- Copyright (c) 2011-2019 Columbia University, System Level Design Group
-- SPDX-License-Identifier: Apache-2.0

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

use work.sldcommon.all;
use work.nocpackage.all;

entity sync_noc_set is
  generic (
    PORTS     : std_logic_vector(4 downto 0);
    local_x   : std_logic_vector(2 downto 0);
    local_y   : std_logic_vector(2 downto 0);
    HAS_SYNC  : integer range 0 to 1 := 0);
  port (
    clk           : in  std_logic;
    clk_tile      : in  std_logic;
    rst           : in  std_logic;
    noc1_data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_void_in  : in  std_logic_vector(4 downto 0);
    noc1_stop_in       : in  std_logic_vector(4 downto 0);
    noc1_data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc1_data_void_out : out std_logic_vector(4 downto 0);
    noc1_stop_out      : out std_logic_vector(4 downto 0);
    noc2_data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_void_in  : in  std_logic_vector(4 downto 0);
    noc2_stop_in       : in  std_logic_vector(4 downto 0);
    noc2_data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc2_data_void_out : out std_logic_vector(4 downto 0);
    noc2_stop_out      : out std_logic_vector(4 downto 0);
    noc3_data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_void_in  : in  std_logic_vector(4 downto 0);
    noc3_stop_in       : in  std_logic_vector(4 downto 0);
    noc3_data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc3_data_void_out : out std_logic_vector(4 downto 0);
    noc3_stop_out      : out std_logic_vector(4 downto 0);
    noc4_data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_void_in  : in  std_logic_vector(4 downto 0);
    noc4_stop_in       : in  std_logic_vector(4 downto 0);
    noc4_data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc4_data_void_out : out std_logic_vector(4 downto 0);
    noc4_stop_out      : out std_logic_vector(4 downto 0);
    noc5_data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_void_in  : in  std_logic_vector(4 downto 0);
    noc5_stop_in       : in  std_logic_vector(4 downto 0);
    noc5_data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc5_data_void_out : out std_logic_vector(4 downto 0);
    noc5_stop_out      : out std_logic_vector(4 downto 0);
    noc6_data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_void_in  : in  std_logic_vector(4 downto 0);
    noc6_stop_in       : in  std_logic_vector(4 downto 0);
    noc6_data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
    noc6_data_void_out : out std_logic_vector(4 downto 0);
    noc6_stop_out      : out std_logic_vector(4 downto 0);

    -- Monitor output. Can be left unconnected
    noc1_mon_noc_vec   : out monitor_noc_type;
    noc2_mon_noc_vec   : out monitor_noc_type
    noc3_mon_noc_vec   : out monitor_noc_type;
    noc4_mon_noc_vec   : out monitor_noc_type
    noc5_mon_noc_vec   : out monitor_noc_type;
    noc6_mon_noc_vec   : out monitor_noc_type

    );

end sync_noc_set;

architecture mesh of sync_noc_set is

  component sync_noc_xy
    generic (
      PORTS     : std_logic_vector(4 downto 0);
      local_x   : std_logic_vector(2 downto 0);
      local_y   : std_logic_vector(2 downto 0);
      has_sync  : integer range 0 to 1); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           : in  std_logic;
      clk_tile      : in  std_logic;
      rst           : in  std_logic;
      data_n_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_s_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_w_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_e_in     : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      input_port    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_void_in  : in  std_logic_vector(4 downto 0);
      stop_in       : in  std_logic_vector(4 downto 0);
      data_n_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_s_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_w_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_e_out    : in  std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      output_port   : out std_logic_vector(NOC_FLIT_SIZE-1 downto 0);
      data_void_out : out std_logic_vector(4 downto 0);
      stop_out      : out std_logic_vector(4 downto 0);
      -- Monitor output. Can be left unconnected
      mon_noc       : out monitor_noc_type
      );
  end component;

begin

  sync_noc_set_1: sync_noc_set
    generic map (
      PORTS    =>  PORTS,
      local_x  =>  local_x,
      local_y  =>  local_y,
      has_sync =>  HAS_SYNC); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           => clk,
      ck_tile       => clk_tile,
      rst           => rst,
      data_n_in     => noc1_data_n_in,
      data_s_in     => noc1_data_s_in,
      data_w_in     => noc1_data_w_in,
      data_e_in     => noc1_data_e_in,
      input_port    => noc1_input_port,
      data_void_in  => noc1_data_void_in,
      stop_in       => noc1_stop_in,
      data_n_out    => noc1_data_n_out,
      data_s_out    => noc1_data_s_out,
      data_w_out    => noc1_data_w_out,
      data_e_out    => noc1_data_e_out,
      output_port   => noc1_output_port,
      data_void_out => noc1_data_void_out,
      stop_out      => noc1_stop_out,
      mon_noc       => noc1_mon_noc
      );

  sync_noc_set_2: sync_noc_set
    generic map (
      PORTS    =>  PORTS,
      local_x  =>  local_x,
      local_y  =>  local_y,
      has_sync =>  HAS_SYNC); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           => clk,
      ck_tile       => clk_tile,
      rst           => rst,
      data_n_in     => noc2_data_n_in,
      data_s_in     => noc2_data_s_in,
      data_w_in     => noc2_data_w_in,
      data_e_in     => noc2_data_e_in,
      input_port    => noc2_input_port,
      data_void_in  => noc2_data_void_in,
      stop_in       => noc2_stop_in,
      data_n_out    => noc2_data_n_out,
      data_s_out    => noc2_data_s_out,
      data_w_out    => noc2_data_w_out,
      data_e_out    => noc2_data_e_out,
      output_port   => noc2_output_port,
      data_void_out => noc2_data_void_out,
      stop_out      => noc2_stop_out,
      mon_noc       => noc2_mon_noc
      );

  sync_noc_set_3: sync_noc_set
    generic map (
      PORTS    =>  PORTS,
      local_x  =>  local_x,
      local_y  =>  local_y,
      has_sync =>  HAS_SYNC); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           => clk,
      ck_tile       => clk_tile,
      rst           => rst,
      data_n_in     => noc3_data_n_in,
      data_s_in     => noc3_data_s_in,
      data_w_in     => noc3_data_w_in,
      data_e_in     => noc3_data_e_in,
      input_port    => noc3_input_port,
      data_void_in  => noc3_data_void_in,
      stop_in       => noc3_stop_in,
      data_n_out    => noc3_data_n_out,
      data_s_out    => noc3_data_s_out,
      data_w_out    => noc3_data_w_out,
      data_e_out    => noc3_data_e_out,
      output_port   => noc3_output_port,
      data_void_out => noc3_data_void_out,
      stop_out      => noc3_stop_out,
      mon_noc       => noc3_mon_noc
      );
  sync_noc_set_4: sync_noc_set
    generic map (
      PORTS    =>  PORTS,
      local_x  =>  local_x,
      local_y  =>  local_y,
      has_sync =>  HAS_SYNC); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           => clk,
      ck_tile       => clk_tile,
      rst           => rst,
      data_n_in     => noc4_data_n_in,
      data_s_in     => noc4_data_s_in,
      data_w_in     => noc4_data_w_in,
      data_e_in     => noc4_data_e_in,
      input_port    => noc4_input_port,
      data_void_in  => noc4_data_void_in,
      stop_in       => noc4_stop_in,
      data_n_out    => noc4_data_n_out,
      data_s_out    => noc4_data_s_out,
      data_w_out    => noc4_data_w_out,
      data_e_out    => noc4_data_e_out,
      output_port   => noc4_output_port,
      data_void_out => noc4_data_void_out,
      stop_out      => noc4_stop_out,
      mon_noc       => noc4_mon_noc
      );

  sync_noc_set_5: sync_noc_set
    generic map (
      PORTS    =>  PORTS,
      local_x  =>  local_x,
      local_y  =>  local_y,
      has_sync =>  HAS_SYNC); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           => clk,
      ck_tile       => clk_tile,
      rst           => rst,
      data_n_in     => noc5_data_n_in,
      data_s_in     => noc5_data_s_in,
      data_w_in     => noc5_data_w_in,
      data_e_in     => noc5_data_e_in,
      input_port    => noc5_input_port,
      data_void_in  => noc5_data_void_in,
      stop_in       => noc5_stop_in,
      data_n_out    => noc5_data_n_out,
      data_s_out    => noc5_data_s_out,
      data_w_out    => noc5_data_w_out,
      data_e_out    => noc5_data_e_out,
      output_port   => noc5_output_port,
      data_void_out => noc5_data_void_out,
      stop_out      => noc5_stop_out,
      mon_noc       => noc5_mon_noc
      );

  sync_noc_set_6: sync_noc_set
    generic map (
      PORTS    =>  PORTS,
      local_x  =>  local_x,
      local_y  =>  local_y,
      has_sync =>  HAS_SYNC); --further, pass these param to module through CFG_HAS_SYNC parameter pkg file
    port (
      clk           => clk,
      ck_tile       => clk_tile,
      rst           => rst,
      data_n_in     => noc6_data_n_in,
      data_s_in     => noc6_data_s_in,
      data_w_in     => noc6_data_w_in,
      data_e_in     => noc6_data_e_in,
      input_port    => noc6_input_port,
      data_void_in  => noc6_data_void_in,
      stop_in       => noc6_stop_in,
      data_n_out    => noc6_data_n_out,
      data_s_out    => noc6_data_s_out,
      data_w_out    => noc6_data_w_out,
      data_e_out    => noc6_data_e_out,
      output_port   => noc6_output_port,
      data_void_out => noc6_data_void_out,
      stop_out      => noc6_stop_out,
      mon_noc       => noc6_mon_noc
      );

end;
