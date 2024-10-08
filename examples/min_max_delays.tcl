# min/max delay calc example
read_liberty -max nangate45_slow.lib.gz
read_liberty -min nangate45_fast.lib.gz
read_verilog example1.v
link_design top
create_clock -name clk -period 10 {clk1 clk2 clk3}
set_input_delay -clock clk 0 {in1 in2}
report_checks -path_delay min_max
