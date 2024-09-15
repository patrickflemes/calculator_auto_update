Compilar calculator.cpp
g++ -o calculator calculator.cpp `pkg-config --cflags --libs gtk+-3.0`

Compilar calculator_new.cpp
g++ -o calculator_new calculator_new.cpp `pkg-config --cflags --libs gtk+-3.0`

A nova versão teste contem um adicional do "." e "%". 
