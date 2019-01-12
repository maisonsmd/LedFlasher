#pragma once

// A generic smart pointer class 
template <class T>
class SmartPtr {
protected:
	T * ptr;  // Actual pointer 
public:
	// Constructor 
	explicit SmartPtr(T *p = nullptr) { ptr = p; }

	// Destructor 
	virtual ~SmartPtr() { delete(ptr); }

	// Overloading dereferncing operator 
	T & operator * () { return *ptr; }

	// Overloding arrow operator so that members of T can be accessed 
	// like a pointer (useful if T represents a class or struct or  
	// union type) 
	T * operator -> () { return ptr; }
};

template <class T>
class SmartPtrArray : SmartPtr<T> {
public:
	~SmartPtrArray(){
		delete[] SmartPtr<T>::ptr;
	}
	explicit SmartPtrArray(T *p = nullptr) : SmartPtr<T>(p){}
};