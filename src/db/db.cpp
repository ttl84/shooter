#include "db.h"
#include <vector>
#include <string>
#include <iterator>

bool DB::read(std::istream& in)
{
	std::string word;
	std::vector<std::string> row;
	bool escape = false;
	for(std::istreambuf_iterator<char> it(in), end; it != end; ++it) {
		char c = *it;
		if(c == '\\') {
			escape = true;
			++it;
			if(it != end) {
				c = *it;
			} else {
				break;
			}
		} else {
			escape = false;
		}
		if(c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		} else if(escape) {
			if(c == 't') {
				word += '\t';
			} else if(c == 's') {
				word += ' ';
			} else if(c == 'n') {
				word += '\n';
			} else {
				word += c;
			}
		} else if(c == ',' || c == ';') {
			row.push_back(std::move(word));
			word.clear();
			if(c == ';') {
				if(schema.empty()) {
					std::swap(schema, row);
				}
				if(schema.size() == row.size()) {
					rows.push_back(std::move(row));
				}
			}
		} else {
			word += c;
		}
	}
	return !schema.empty();
}

bool DB::write(std::ostream& out)
{
	return false;
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

