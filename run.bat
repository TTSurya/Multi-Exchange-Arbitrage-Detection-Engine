@ECHO OFF
python fetch.py
g++ main.cpp -std=c++17 -O2 -o arb
arb.exe
PAUSE