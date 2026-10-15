Run C:\Program Files\MinGW\set_distro_paths.bat

To start a new build dir
> cmake -G "MinGW Makefiles" -B build

To build
> cmake --build build

Run the test
> build\tests.exe

Intel(R) Core(TM) i5-6300U CPU @2.40GHz

##Not accelerated run
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from SpeedTest
[ RUN      ] SpeedTest.multiplyAndSum
A: 1277 x 1564 (rows x cols), B: 1564 x 4356, C: 1277 x 4356
Sum of A x B: -198351584
Load:        0.826 s
Multiply:    7.721 s
Sum:         0.005 s
Total:       8.570 s
[       OK ] SpeedTest.multiplyAndSum (8578 ms)
[----------] 1 test from SpeedTest (8595 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (8617 ms total)
[  PASSED  ] 1 test.