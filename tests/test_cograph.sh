g++ -std=c++17 -o cografando src/cografando.cc
echo "$1 0" > cograph_in
./cografando  < cograph_in > cograph_out
rm cograph_in
./a < cograph_out
