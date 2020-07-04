#pragma once
#include <Engine.h>

template<class T>
class Manager final{
	static std::vector<T*> container;
public:
	///Overload funcs??
	static int GetCount() const{
		return container.size();
	}
	static int Find(const T* const& item){
		return std::distance(container.begin(), std::find(container.begin(), container.end(), item));
	}
	static void Clear(){
		container.clear();
	}
	static void Add(const T* const& item){
		container.emplace_back(item);
	}
	static void Remove(const T* const& item){
		container.erase(container.begin() + Find());
	}
};

template<class T>
static std::vector<T*> container;