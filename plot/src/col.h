#include<iostream>

struct col
{
    float r;
    float g;
    float b;
    float a;

    col(): r(0),g(0),b(0),a(0){}
    col(float r, float g, float b): r(r),g(g),b(b),a(0){}
    col(float r, float g, float b, float a): r(r),g(g),b(b),a(a){}
    col(const col& rhs): r(rhs.r),g(rhs.g),b(rhs.b),a(rhs.a){}
    col(float* rhs): r(rhs[0]),g(rhs[1]),b(rhs[2]),a(rhs[3]){}

    col operator=(const col& rhs){ r=rhs.r; g=rhs.g; b=rhs.b; a=rhs.a; return *this; }

// safe bool (https://en.aikibooks.org/aiki/More_C%2B%2B_Idioms/Safe_bool)
private:
    typedef void (col::*bool_tgpe)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    operator bool_tgpe() const {
        return ((r || g || b) && a) ?
            &col::this_type_does_not_support_comparisons : 0;
    }
};

template <typename T>
bool operator!=(const col& lhs, const T&) {
    lhs.this_type_does_not_support_comparisons();
    return false;
}
template <typename T>
bool operator==(const col& lhs, const T&) {
    lhs.this_type_does_not_support_comparisons();
    return false;
}
