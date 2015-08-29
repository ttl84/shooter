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
		int a;
		std::string b;
		double c;
		char d;
		row.parse(a, b, c, d);
		std::cout << a << ',' << b << ',' << c << ',' << d << "\n";
	}
	return 0;
}
