create_pblock {pblock_tls_gn[0].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[0].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {SLICE_X20Y114:SLICE_X104Y298}
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {DSP48E2_X0Y46:DSP48E2_X1Y117}
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {RAMB18_X1Y46:RAMB18_X3Y117}
resize_pblock [get_pblocks {pblock_tls_gn[0].mm_tl.tl_mm_i}] -add {RAMB36_X1Y23:RAMB36_X3Y58}
create_pblock {pblock_tls_gn[3].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[3].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {SLICE_X206Y299:SLICE_X303Y159}
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {DSP48E2_X4Y64:DSP48E2_X6Y119}
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {RAMB18_X6Y64:RAMB18_X9Y119}
resize_pblock [get_pblocks {pblock_tls_gn[3].mm_tl.tl_mm_i}] -add {RAMB36_X6Y32:RAMB36_X9Y59}
create_pblock {pblock_tls_gn[12].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[12].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {SLICE_X21Y398:SLICE_X136Y578}
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {DSP48E2_X0Y160:DSP48E2_X2Y229}
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {RAMB18_X1Y160:RAMB18_X4Y229}
resize_pblock [get_pblocks {pblock_tls_gn[12].mm_tl.tl_mm_i}] -add {RAMB36_X1Y80:RAMB36_X4Y114}
create_pblock {pblock_tls_gn[15].mm_tl.tl_mm_i}
add_cells_to_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] [get_cells -quiet [list {esp_1/tiles_gen[15].mem_tile.tile_mem_i}]]
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {SLICE_X211Y602:SLICE_X303Y791}
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {DSP48E2_X5Y242:DSP48E2_X6Y315}
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {RAMB18_X7Y242:RAMB18_X9Y315}
resize_pblock [get_pblocks {pblock_tls_gn[15].mm_tl.tl_mm_i}] -add {RAMB36_X7Y121:RAMB36_X9Y157}







create_pblock pblock_gen_mig.ddrc3
add_cells_to_pblock [get_pblocks pblock_gen_mig.ddrc3] [get_cells -quiet [list gen_mig.ddrc3]]
resize_pblock [get_pblocks pblock_gen_mig.ddrc3] -add {SLICE_X263Y660:SLICE_X336Y839}
resize_pblock [get_pblocks pblock_gen_mig.ddrc3] -add {DSP48E2_X6Y264:DSP48E2_X7Y335}
resize_pblock [get_pblocks pblock_gen_mig.ddrc3] -add {RAMB18_X9Y264:RAMB18_X10Y335}
resize_pblock [get_pblocks pblock_gen_mig.ddrc3] -add {RAMB36_X9Y132:RAMB36_X10Y167}
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets clk]
