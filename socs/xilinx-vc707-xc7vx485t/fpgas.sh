#!/bin/bash

unset UART_IP
unset UART_PORT
unset SSH_IP
unset SSH_PORT
unset ESPLINK_IP
unset ESPLINK_PORT
unset LINUX_MAC

COL_CYAN='\033[0;96m'
COL_NORMAL='\033[0;39m'

normal=$(tput sgr0; echo -e $COL_NORMAL)
def=$(tput bold; echo -e $COL_CYAN)

echo "=== SLD FPGAs environment for ESP ==="
echo ""
echo "  [1] - vc707-01"
echo "  [2] - vc707-02"
echo "  [3] - profgpa-01 (bertha)"
echo "  [4] - profgpa-02 (agatha)"
echo "  [5] - vcu118-01"
echo ""

read -p "  Plase select your target FPGA board (1-5) ${def}[1]${normal}: " opt
opt=${opt:-1}
case $opt in
    1 ) echo "  == vc707-01 =="
	export UART_IP=espdev
	export UART_PORT=4322
	export SSH_IP=espgate
	export SSH_PORT=5502
	export ESPLINK_IP="128.59.22.75"
	export ESPLINK_PORT=46307
	export LINUX_MAC=000A3502CB80
	;;
    2 ) echo "  == vc707-02 =="
	export UART_IP=espdev
	export UART_PORT=4323
	export SSH_IP=espgate
	export SSH_PORT=5503
	export ESPLINK_IP="128.59.22.75"
	export ESPLINK_PORT=46308
	export LINUX_MAC=000A3502CB79
	;;
    3 ) echo "  == profpga-01 =="
	export UART_IP=espdev
	export UART_PORT=4324
	export SSH_IP=espgate
	export SSH_PORT=5504
	export ESPLINK_IP="128.59.22.75"
	export ESPLINK_PORT=46309
	export LINUX_MAC=00006EA1542B
	;;
    4 ) echo "  == profpga-02 =="
	export UART_IP=espdev
	export UART_PORT=4325
	export SSH_IP=espgate
	export SSH_PORT=5505
	export ESPLINK_IP="128.59.22.75"
	export ESPLINK_PORT=46310
	export LINUX_MAC=00006EA1542C
	;;
    5 ) echo "  == vcu118 =="
	export UART_IP=espdev
	export UART_PORT=4326
	export SSH_IP=espgate
	export SSH_PORT=5506
	export ESPLINK_IP="128.59.22.75"
	export ESPLINK_PORT=46311
	export LINUX_MAC=000A3504EFB1
	;;
    * ) echo "  ERROR: invalid selection"
	;;
esac

echo "  UART_IP      : ${UART_IP}"
echo "  UART_PORT    : ${UART_PORT}"
echo "  SSH_IP       : ${SSH_IP}"
echo "  SSH_PORT     : ${SSH_PORT}"
echo "  ESPLINK_IP   : ${ESPLINK_IP}"
echo "  ESPLINK_PORT : ${ESPLINK_PORT}"
echo "  LINUX_MAC    : ${LINUX_MAC}"
echo ""
echo "=== Configuration complete ==="
