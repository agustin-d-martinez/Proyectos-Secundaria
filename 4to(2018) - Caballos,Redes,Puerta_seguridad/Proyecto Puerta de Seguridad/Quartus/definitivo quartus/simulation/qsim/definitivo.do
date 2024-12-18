onerror {quit -f}
vlib work
vlog -work work definitivo.vo
vlog -work work definitivo.vt
vsim -novopt -c -t 1ps -L max3000a_ver -L altera_ver -L altera_mf_ver -L 220model_ver -L sgate work.definitivo_vlg_vec_tst
vcd file -direction definitivo.msim.vcd
vcd add -internal definitivo_vlg_vec_tst/*
vcd add -internal definitivo_vlg_vec_tst/i1/*
add wave /*
run -all
