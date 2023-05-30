rm *.o *.a *.out *.dump

gcc -Iinclude -c addvec.cpp multvec.cpp vectorImpl.cpp
ar rcs libvector.a addvec.o multvec.o
ar rcs libvectorimpl.a vectorImpl.o

gcc -Iinclude -c main2.cpp

# The libraries are not independent, then they must be ordered correctly.
gcc  main2.o ./libvector.a ./libvectorimpl.a  -o main2_1.out # or
gcc -static main2.o  -L. -lvector -lvectorimpl -o main2_2.out

readelf -S -r -s main2.o > _obj.dump
objdump -d -s main2.o >> _obj.dump


readelf -S -r -s main2_1.out > _main2_1.dump
objdump -d main2_1.out >> _main2_1.dump

readelf -S -r -s main2_2.out > _main2_2.dump
objdump -d main2_2.out >> _main2_2.dump

