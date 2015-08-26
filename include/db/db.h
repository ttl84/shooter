#ifndef DB_H
#define DB_H
#include <vector>
#include <string>
#include <iostream>
class DB{
	typedef std::vector<std::string> RowT;
	typedef std::vector<RowT> TableT;
	std::vector<std::string> schema;
	std::vector<std::vector<std::string>> rows;
public:
	bool read(std::istream& );
	bool write(std::ostream& );

	RowT& operator[](unsigned i);
	const RowT& operator[](unsigned i) const;

	TableT::iterator begin();
	TableT::iterator end();
	TableT::const_iterator begin() const;
	TableT::const_iterator end() const;
};
#endif

