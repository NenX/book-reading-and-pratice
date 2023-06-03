#ifndef __VECTOR_H__
#define __VECTOR_H__
// typedef long long valuetype;
typedef long long  valuetype;
extern "C"
{
    void addvec(valuetype *x, valuetype *y, valuetype *z, valuetype n);
    void multvec(valuetype *x, valuetype *y, valuetype *z, valuetype n);
    void _addvecImpl(valuetype *x, valuetype *y, valuetype *z, valuetype n);
}

#endif // __VECTOR_H__
