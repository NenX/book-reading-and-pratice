rm -f *.o *.a *.out *.dump *.so

# gcc -Iinclude -c addvec.cpp multvec.cpp vectorImpl.cpp
# ar rcs libvector.a addvec.o multvec.o
# ar rcs libvectorimpl.a vectorImpl.o

# gcc -Iinclude -c main2.cpp

# # The libraries are not independent, then they must be ordered correctly.
# gcc  main2.o ./libvector.a ./libvectorimpl.a  -o main2_1.out # or
# gcc -static main2.o  -L. -lvector -lvectorimpl -o main2_2.out

# readelf -S -r -s main2.o > _obj.dump
# objdump -d -s main2.o >> _obj.dump


# readelf -S -r -s main2_1.out > _main2_1.dump
# objdump -d main2_1.out >> _main2_1.dump

# readelf -S -r -s main2_2.out > _main2_2.dump
# objdump -d main2_2.out >> _main2_2.dump
export LD_LIBRARY_PATH=$(pwd)
gcc -Iinclude  -shared -fPIC addvec.cpp multvec.cpp -o libvector.so
gcc -Iinclude  -shared -fPIC vectorImpl.cpp -o libvectorimpl.so

# # The libraries are not independent, then they must be ordered correctly.
gcc -Iinclude main2.cpp  ./libvector.so  ./libvectorimpl.so -o main2_1.out
gcc -Iinclude main2.cpp -L. -lvector  -lvectorimpl -Wl,-rpath=`pwd` -o main2_2.out
