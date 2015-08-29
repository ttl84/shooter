#include "db.h"
#include <fstream>
#include <iostream>
int main()
{
	db::DB database;
	std::ifstream is("test/db/input");
	database.read(is);
	for(auto & row : database) {
		for(auto & col : row) {
			std::cout << col << ", ";
		}
		std::cout << ";\n";
	}
	return 0;
}
