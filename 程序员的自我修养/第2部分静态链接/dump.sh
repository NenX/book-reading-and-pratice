rm *.o *.dump
gcc -c lib.cpp
gcc -Iinclude -c nomain.cpp
readelf -S -r -s nomain.o > _obj.dump
objdump -d -s nomain.o >> _obj.dump

ld -T nomain.lds nomain.o lib.o -o nomain.out

readelf -S -r -s  nomain.out > _nomain.dump
objdump -d -s  nomain.out >> _nomain.dump