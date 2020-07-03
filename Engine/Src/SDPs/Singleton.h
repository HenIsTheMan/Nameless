#pragma once

template <class T>
class Singleton{
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) noexcept = delete;

	static T* objPtr;
protected:
	Singleton() = default;
public:
	static T* const& GetObjPtr(){
		if(!objPtr){
			objPtr = new T;
		}
		return objPtr;
	}
	static void Destroy(){
		if(objPtr){
			delete objPtr;
			objPtr = nullptr;
		}
	}
};

//virtual dtor??

template <class T>
T* Singleton<T>::objPtr = nullptr;