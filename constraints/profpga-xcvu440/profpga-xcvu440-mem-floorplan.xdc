create_pblock {pblock_tls_gn[0].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[0].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {SLICE_X31Y152:SLICE_X111Y298}
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {DSP48E2_X0Y62:DSP48E2_X1Y117}
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {RAMB18_X1Y62:RAMB18_X3Y117}
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {RAMB36_X1Y31:RAMB36_X3Y58}
create_pblock {pblock_tls_gn[3].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[3].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {SLICE_X192Y166:SLICE_X296Y299}
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {DSP48E2_X4Y68:DSP48E2_X5Y119}
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {RAMB18_X6Y68:RAMB18_X8Y119}
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {RAMB36_X6Y34:RAMB36_X8Y59}
create_pblock {pblock_tls_gn[12].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[12].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {SLICE_X33Y418:SLICE_X109Y559}
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {DSP48E2_X0Y168:DSP48E2_X1Y223}
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {RAMB18_X1Y168:RAMB18_X3Y223}
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {RAMB36_X1Y84:RAMB36_X3Y111}
create_pblock {pblock_tls_gn[15].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[15].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {SLICE_X193Y651:SLICE_X296Y788}
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {DSP48E2_X4Y262:DSP48E2_X5Y313}
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {RAMB18_X6Y262:RAMB18_X8Y313}
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {RAMB36_X6Y131:RAMB36_X8Y156}
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets clk]
