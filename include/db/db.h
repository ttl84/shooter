#ifndef DB_H
#define DB_H
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
namespace db{

	class RowT {
		std::vector<std::string> data;
	public:
		RowT(std::vector<std::string> && d)
			:data(d)
			{
			}
		decltype(data)::iterator begin()
			{
				return data.begin();
			}
		decltype(data)::iterator end()
			{
				return data.end();
			}
		template<unsigned i, class ... Ts>
		bool parse(Ts & ... ts)
			{
				return true;
			}
		template<unsigned i, class T, class ... Ts>
		bool parse(T & t, Ts & ... ts)
			{
				bool good1 = std::istringstream(data[i]) >> t;
				bool good2 = this->parse<i+1>(ts...);
				return good1 && good2;
			}
		template<class ... Ts>
		bool parse(Ts & ... ts)
			{
				if(sizeof...(Ts) == data.size())
					return this->parse<0>(ts...);
				else
					return false;
			}
	};
	
	class DB{
		typedef std::vector<RowT> TableT;
		std::vector<std::string> schema;
		std::vector<RowT> rows;
	public:
		bool read(std::istream&);
		bool write(std::ostream& );

		unsigned nRows() const;
		unsigned nCols() const;
		const RowT & getSchema() const;
	
		RowT& operator[](unsigned i);
		const RowT& operator[](unsigned i) const;

		TableT::iterator begin();
		TableT::iterator end();
		TableT::const_iterator begin() const;
		TableT::const_iterator end() const;
	};
} // end namespace db
#endif

