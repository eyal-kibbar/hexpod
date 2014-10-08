#! /bin/bash

delay=0.1
dev=/dev/ttyACM0

./a.out $dev 0 d 41
sleep $delay
./a.out $dev 2 d 33
sleep $delay
./a.out $dev 4 d 20
sleep $delay
./a.out $dev 1 d 135
sleep $delay
./a.out $dev 3 d 130
sleep $delay
./a.out $dev 5 d 135

sleep $delay
./a.out $dev 0 b 20
sleep $delay
./a.out $dev 2 b 95
sleep $delay
./a.out $dev 4 b 78
sleep $delay
./a.out $dev 1 b 148
sleep $delay
./a.out $dev 3 b 100
sleep $delay
./a.out $dev 5 b 110
sleep $delay

./a.out $dev 0 u 160
sleep $delay
./a.out $dev 2 u 155
sleep $delay
./a.out $dev 4 u 140
sleep $delay
./a.out $dev 1 u 35
sleep $delay
./a.out $dev 3 u 20
sleep $delay
./a.out $dev 5 u 20
sleep $delay

./a.out $dev 0 f 85
sleep $delay
./a.out $dev 2 f 162
sleep $delay
./a.out $dev 4 f 148
sleep $delay
./a.out $dev 1 f 75
sleep $delay
./a.out $dev 3 f 32
sleep $delay
./a.out $dev 5 f 38




