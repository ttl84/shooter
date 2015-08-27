#include "db.h"
#include <fstream>
#include <iostream>
int main()
{
	DB db;
	db.read(std::ifstream("test/db/input"));
	for(auto & row : db) {
		for(auto & col : row) {
			std::cout << col << ", ";
		}
		std::cout << ";\n";
	}
	return 0;
}
