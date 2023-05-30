rm *.o *.a *.out

gcc -Iinclude -c addvec.cpp multvec.cpp vectorImpl.cpp
ar rcs libvector.a addvec.o multvec.o
ar rcs libvectorimpl.a vectorImpl.o

gcc -Iinclude -c main2.cpp

# The libraries are not independent, then they must be ordered correctly.
gcc -static main2.o ./libvector.a ./libvectorimpl.a  -o main2_1.out # or
gcc -static main2.o  -L. -lvector -lvectorimpl -o main2_2.out