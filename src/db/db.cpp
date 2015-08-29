#include "db.h"
#include <vector>
#include <string>
#include <iterator>
#include "token_stream.h"
namespace db{
bool DB::read(std::istream &in)
{
	std::vector<std::string> row;
	std::string word;

	TokenStream<std::istreambuf_iterator<char>> stream(in);
	for(auto & token : stream) {
		if(token.type == Type::word) {
			word += token.text;
		} else if(token.type == Type::col) {
			row.push_back(std::move(word));
		} else if(token.type == Type::row) {
			row.push_back(std::move(word));
			if(schema.empty()) {
				std::swap(schema, row);
			} else if(row.size() == schema.size()){
				rows.push_back(std::move(row));
			} else {
			}
		}
	}
	return !schema.empty();
}

bool DB::write(std::ostream& out)
{
	return false;
}
unsigned DB::nRows() const
{
	return rows.size();
}
unsigned DB::nCols() const
{
	return schema.size();
}
std::vector<std::string>& DB::operator[](unsigned i)
{
	return rows[i];
}
const std::vector<std::string>& DB::operator[](unsigned i) const
{
	return rows[i];
}
DB::TableT::iterator DB::begin()
{
	return rows.begin();
}
DB::TableT::iterator DB::end()
{
	return rows.end();
}
DB::TableT::const_iterator DB::begin() const
{
	return rows.begin();
}
DB::TableT::const_iterator DB::end() const
{
	return rows.end();
}
	
} // namespace db

