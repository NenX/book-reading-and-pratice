rm *.o *.a *.out

gcc -Iinclude -c addvec.cpp multvec.cpp
ar rcs libvector.a addvec.o multvec.o

gcc -Iinclude -c main2.cpp
gcc -static main2.o ./libvector.a  -o main2_1.out
gcc -static main2.o  -L. -lvector -o main2_2.out