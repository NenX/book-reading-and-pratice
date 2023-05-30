rm *.o *.so *.dump
gcc -fPIC -shared Lib.cpp -o Lib.so
gcc -c Program1.cpp
gcc -c Program2.cpp

gcc Program1.o ./Lib.so  -o Program1.out
gcc Program2.o ./Lib.so  -o Program2.out

readelf -S -r -s Program1.o > _obj.dump
objdump -d -s Program1.o >> _obj.dump

readelf -S -r -s Lib.so > _Lib.dump
objdump -d -s Lib.so >> _Lib.dump


readelf -S -r -s Program1.out > _Program1.dump
objdump -d -s Program1.out >> _Program1.dump