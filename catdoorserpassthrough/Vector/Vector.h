#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "Arduino.h"
#include <math.h>

template <class T>
class  Vector {
public:

    typedef T* Iterator;

    Vector()
	{
		_capacity = 0;
		_size = 0;
		buffer = 0;
		Log = 0;
	}
	
    Vector(unsigned int size){
		_size = size;
    	Log = ceil(log((double) size) / log(2.0));
		_capacity = 1 << Log;
		buffer = new T[_capacity];
	}
	
    Vector(unsigned int size, const T & initial){
		_size = size;
		Log = ceil(log((double) size) / log(2.0));
		_capacity = 1 << Log;
		buffer = new T [_capacity];
		for (unsigned int i = 0; i < size; i++)
			buffer[i] = initial;
	}
	
    Vector(const Vector<T>& v){
		_size = v._size;
		Log = v.Log;
		_capacity = v._capacity;
		buffer = new T[_size];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = v.buffer[i];
	}
	
    ~Vector(){
		delete[] buffer;
	}

    unsigned int capacity() const {
		return _capacity;
	}
	
    unsigned int size() const{
		return _size;
	}
	
    bool empty() const{
		return _size == 0;
	}
	
    Iterator begin(){
		return buffer;
	}
	
    Iterator end(){
		return buffer + size();
	}
    T& front(){
		return buffer[0];		
	}
	
    T& back(){
		return buffer[_size - 1];	
	}
	
    void push_back(const T& v){
		/*
        Incidentally, one common way of regrowing an array is to double the size as needed.
        This is so that if you are inserting n items at most only O(log n) regrowths are performed
        and at most O(n) space is wasted.
		*/
		if (_size >= _capacity) {
			reserve(1 << Log);
			Log++;
		}
		buffer [_size++] = v;
	}
	
    void pop_back(){
		_size--;
	}

    void reserve(unsigned int capacity){
		T * newBuffer = new T[capacity];

		for (unsigned int i = 0; i < _size; i++)
			newBuffer[i] = buffer[i];

		_capacity = capacity;
		delete[] buffer;
		buffer = newBuffer;
	}
    void resize(unsigned int size){
		Log = ceil(log((double) size) / log(2.0));
		reserve(1 << Log);
		_size = size;
	}

    T & operator[](unsigned int index){
		return buffer[index];	
	}
	
    Vector<T> & operator = (const Vector<T> &v){
		delete[] buffer;
		_size = v._size;
		Log = v.Log;
		_capacity = v._capacity;
		buffer = new T [_capacity];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = v.buffer[i];
		return *this;
	}
	
    void clear(){
		_capacity = 0;
		_size = 0;
		buffer = 0;
		Log = 0;
	}
	
	bool contains(const T v) {
		for(int i = 0; i< size(); i++){
			if(operator[](i) == v){
				return true;
			}
		}
		return false;
	}
	
private:
    unsigned int _size;
    unsigned int _capacity;
    unsigned int Log;
    T* buffer;
};
#endif
