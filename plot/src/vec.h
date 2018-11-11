#pragma once
#include<iostream>
#include <cmath>
#include <vector>

struct vec
{
    float x;
    float y;

    vec(): x(0),y(0){}
    vec(float x, float y): x(x),y(y){}
    vec(const vec& rhs): x(rhs.x),y(rhs.y){}
    vec(float* rhs): x(rhs[0]),y(rhs[1]){}

    float operator[](uint8_t i)const{ if (i==0) return x; else return y; }
    float& operator[](uint8_t i){ if (i==0) return x; else return y; }
    vec operator=(const vec& rhs){ x=rhs.x; y=rhs.y; return *this; }
    float operator*(const vec& rhs)const{ return x*rhs.x+y*rhs.y; }
    vec operator*(float rhs)const{ return vec(x*rhs,y*rhs); }
    vec operator*=(float rhs){ x*=rhs; y*=rhs; return *this; }
    vec operator/(float rhs)const{ return vec(x/rhs,y/rhs); }
    vec operator/=(float rhs){ x/=rhs; y/=rhs; return *this; }
    vec operator+(const vec& rhs)const{ return vec(x+rhs.x,y+rhs.y); }
    vec operator+=(const vec& rhs){ x+=rhs.x; y+=rhs.y; return *this; }
    vec operator-(const vec& rhs)const{ return vec(x-rhs.x,y-rhs.y); }
    vec operator-=(const vec& rhs){ x-=rhs.x; y-=rhs.y; return *this; }
    vec limit_sup(float sizeLimit){ float size2=x*x+y*y; return (size2 > sizeLimit*sizeLimit) ? *this *= (sizeLimit / sqrt(size2)) : *this; }
    vec limit_inf(float sizeLimit){ float size2=x*x+y*y; return (size2 < sizeLimit*sizeLimit) ? *this *= (sizeLimit / sqrt(size2)) : *this; }
    friend vec limit_sup(const vec& vecc, float sizeLimit){ float size2=abs2(vecc); return (size2 > sizeLimit*sizeLimit) ? vecc * (sizeLimit / sqrt(size2)) : vecc; }
    friend vec limit_inf(const vec& vecc, float sizeLimit){ float size2=abs2(vecc); return (size2 < sizeLimit*sizeLimit) ? vecc * (sizeLimit / sqrt(size2)) : vecc; }
    friend vec limit(const vec& vecc, float lowerLimit, float upperLimit){ float size2=abs2(vecc); return (size2 > upperLimit*upperLimit) ? vecc * (upperLimit / sqrt(size2)) : ((size2 < lowerLimit*lowerLimit) ? vecc * (lowerLimit / sqrt(size2)) : vecc); }

    friend vec norm(const vec& vecc){ float size2=abs2(vecc); return (size2 > 0) ? vecc / sqrt(size2) : vecc; }
    friend float abs2(const vec& vecc){ return vecc.x*vecc.x+vecc.y*vecc.y; }
    friend float abs(const vec& vecc){ return sqrt(vecc.x*vecc.x+vecc.y*vecc.y); }
    friend vec inv(const vec& vecc){ return vec(-vecc.y,vecc.x); }
    friend float maxDim(const vec& vecc){ return vecc.x*vecc.x > vecc.y*vecc.y ? vecc.x : vecc.y; }
    bool operator==(const vec& rhs)const{ return (x==rhs.x)&&(y==rhs.y); }
    bool operator<(const vec& rhs)const{ return abs2(*this)<abs2(rhs); }
    bool operator>(const vec& rhs)const{ return abs2(*this)>abs2(rhs); }
    friend vec operator^(const vec& lhs, const vec& rhs){ return vec(lhs.x*rhs.x, lhs.y*rhs.y); }
    friend vec operator*(float lhs, const vec& rhs){ return rhs*lhs; }
    friend std::ostream& operator<<(std::ostream& os, const vec& vecc){ os << vecc.x << "," << vecc.y; return os; }
    friend float sinAngleDiff2(const vec& a, const vec& b){ float cross = a.x*b.y-a.y*b.x; return cross*cross/(abs2(a)*abs2(b)); }
    friend vec* begin(float* ref){ return reinterpret_cast<vec*>(ref); }
    friend void push_back_vec(std::vector<float>& vector, const vec& vecc){ vector.push_back(vecc.x); vector.push_back(vecc.y); }

    friend struct mat;

// safe bool (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Safe_bool)
private:
    typedef void (vec::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    operator bool_type() const {
        return (x || y) ?
            &vec::this_type_does_not_support_comparisons : 0;
    }
};

//template <typename T>
//bool operator!=(const vec& lhs, const T&) {
//    lhs.this_type_does_not_support_comparisons();
//    return false;
//}
//template <typename T>
//bool operator==(const vec& lhs, const T&) {
//    lhs.this_type_does_not_support_comparisons();
//    return false;
//}
