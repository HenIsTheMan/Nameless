#include "Engine.h"

int main(){
	double list1[]{4.5, 3.9, 4.0, 5.6, 7.8, 29.3, 4.5, 2.1};
	double list2[]{4.5, 3.9, 4.0, 5.6, 7.8, 29.3, 4.5, 2.1};
	double list3[]{4.5, 3.9, 4.0, 5.6, 7.8, 29.3, 4.5, 2.1};
	double list4[]{4.5, 3.9, 4.0, 5.6, 7.8, 29.3, 4.5, 2.1};
	double list5[]{4.5, 3.9, 4.0, 5.6, 7.8, 29.3, 4.5, 2.1};
	SortArr(list1, 0, sizeof(list1) / sizeof(list1[0]) - 1, SortOrder::Descending, SortingAlgType::Bubble);
	SortArr(list2, 0, sizeof(list2) / sizeof(list2[0]) - 1, SortOrder::Descending, SortingAlgType::Selection);
	SortArr(list3, 0, sizeof(list3) / sizeof(list3[0]) - 1, SortOrder::Descending, SortingAlgType::Insertion);
	SortArr(list4, 0, sizeof(list4) / sizeof(list4[0]) - 1, SortOrder::Descending, SortingAlgType::Quick);
	SortArr(list5, 0, sizeof(list5) / sizeof(list5[0]) - 1, SortOrder::Descending, SortingAlgType::Merge);

	PrintArrElements(list1, 0, sizeof(list1) / sizeof(list1[0]) - 1, PrintFormat::CSListHoriz);
	std::cout << std::endl;
	PrintArrElements(list2, 0, sizeof(list2) / sizeof(list2[0]) - 1, PrintFormat::CSListHoriz);
	std::cout << std::endl;
	PrintArrElements(list3, 0, sizeof(list3) / sizeof(list3[0]) - 1, PrintFormat::CSListHoriz);
	std::cout << std::endl;
	PrintArrElements(list4, 0, sizeof(list4) / sizeof(list4[0]) - 1, PrintFormat::CSListHoriz);
	std::cout << std::endl;
	PrintArrElements(list5, 0, sizeof(list5) / sizeof(list5[0]) - 1, PrintFormat::CSListHoriz);
}