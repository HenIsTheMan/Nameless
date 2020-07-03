#pragma once
#include <iostream>

enum struct PrintFormat{ //Can be extended
	ListHoriz = 0,
	ListVert = 1,
	CSListHoriz = 2,
	CSListVert = 3,
};

template <class T>
void PrintArrElements(T* arr, const int& start, const int& end, const PrintFormat& format){
	for(int i = start; i <= end; ++i){
		switch(int(format)){
			case 0:
				std::cout << arr[i] << (i == end ? "" : " ");
				break;
			case 1:
				std::cout << arr[i] << (i == end ? "" : "\n");
				break;
			case 2:
				std::cout << arr[i] << (i == end ? "" : ", ");
				break;
			case 3:
				std::cout << arr[i] << (i == end ? "" : ",\n");
				break;
		}
	}
}