#pragma once
#include <stdint.h>
#include <cmath>
#include <iostream>

template<typename T, uint32_t N>
struct array{

    T data[N] = {};
    T* _end;

    array(){ _end = data; }

    array(std::initializer_list<T> initialize){
        auto startIt = initialize.begin();
        auto endIt = initialize.end();
        for (auto it = startIt; it != endIt; ++it)
            data[std::distance(startIt,it)] = *it;
        _end = data + std::distance(startIt,endIt);
    }

    T& operator[](uint32_t index){ return data[index]; }
    const T& operator[](uint32_t index)const{ return data[index]; }

    T* begin(){ return data; }
    T* end(){ return _end; }

    T& back(){ return *(_end-1); }

    T* fill(){ _end = data + N; return _end; }

    T* fill(const T& value){
        for (auto& dat : data)
            dat = value;
        return _end = data + N;
    }

    T* push_back(const T& value){
        if (_end != data + N)
            *(_end++) = value;
        return _end;
    }

    T* push_back(T& value){
        if (_end != begin() + N)
            *(_end++) = value;
        return _end;
    }

    T* push_back(T* otherData, uint32_t n){
        uint32_t ext = std::min(n, N - size());
        memcpy(_end, otherData, ext*sizeof(T));
        _end += ext;
        return _end;
    }

    T* push_back(std::initializer_list<T> initialize){
        auto startIt = initialize.begin();
        auto endIt = initialize.end();
        for (auto it = startIt; it != endIt; ++it)
            *(_end + std::distance(startIt,it)) = *it;
        _end += std::distance(startIt,endIt);
    }

    //void erase(uint32_t index){
    //    --_end;
    //    for (T* ref = data + index; ref != _end; ++ref)
    //        *ref = *(ref + 1);
    //}

    T* erase(T* reference){
        --_end;
        for (T* ref = reference; ref != _end; ++ref)
            *ref = *(ref + 1);
        return reference;
    }

    T* erase(const T& value){
        for (T* ref = data; ref != _end;)
            if (*ref == value){
                return erase(ref);
                break;
            }else
                ++ref;
        return _end;
    }

    void insert(const T& value, T* reference){
        if (size() < N)
            ++_end;
        for (T* ref = reference+1; ref != _end; ++ref)
            *ref = *(ref - 1);
        *reference = value;
    }

    T* trim_value(T value){
        while (_end != data){
            if (*(_end-1) == value)
                --_end;
            else
                return _end;
        }
        return _end;
    }

    T* limit_size(uint32_t n){
        if (size() > n)
            return _end = begin() + n;
        return _end;
    }

    void clear(){ _end = &data[0]; }

    uint32_t size()const{ return _end - data; }

    bool empty()const{ return _end == data; }

    friend array& operator>>(array& lhs, uint32_t shift_n){ lhs._end -= shift_n; return lhs; }

    friend array& operator<<(array& lhs, const array& rhs){ lhs.push_back(rhs, rhs.size()); return lhs; }

    friend T& operator<<(array& lhs, const T& rhs){ lhs.push_back(rhs); return lhs; }

    friend std::ostream& operator<<(std::ostream& os, const array& ar){ for (uint32_t i=0;i<ar.size();++i)os << ar[i] << ","; return os; }

    //template<uint32_t M>
    //array<T,N> operator=(const array<T,M>& rhs){
    //    uint32_t indices = std::min(N,M);
    //    memcpy(data, rhs.data, indices);
    //    _end = data + indices;
    //    return *this;
    //}

    //template<uint32_t M>
    //array<T,N+M> operator+(const array<T,M>& rhs)const{
    //    array<T,N+M> cat;
    //    memcpy(cat._end, data, size());
    //    memcpy(cat._end, rhs.data, rhs.size());
    //    return cat;
    //}

// safe bool (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Safe_bool)
private:
    typedef void (array::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    operator bool_type() const {
        return size() ?
            &array::this_type_does_not_support_comparisons : 0;
    }
};
