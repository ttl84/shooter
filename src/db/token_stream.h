#include <iterator>
#include <string>

enum Type{
	word, col, row, err
};
struct Token{
	Type type;
	std::string lexeme;
};
template<class It>
class TokenIterator
	: public std::iterator<std::input_iterator_tag, std::string, void> {
	It it, end;   // underlying character iterators
	Token tok;    // current token
	bool done;    // done flag for past-the-end iterator.
	
public:
	
	TokenIterator(It iter, It iterEnd)
		:it(iter), end(iterEnd), done(false)
		{
		}
	TokenIterator()
		:done(true)
		{
		}
	bool operator==(TokenIterator const & other) const
		{
			return done && other.done;
		}
	bool operator!=(TokenIterator const & other) const
		{
			return !done || !other.done;
		}
	TokenIterator& operator++()
		{
			tok.clear();
			unsigned count = 0;
			while(it != end){
				char c = *it;
				if(c == ' ' || c == '\t' || c == '\n') {
					++it;
				} else if(c == ',') {
					if(count == 0) {
						tok.type = Type::col;
						tok.lexeme.clear();
						++it;
						++count;
					}
					break;
				} else if(c == ';') {
					if(count == 0) {
						tok.type = Type::row;
						tok.lexeme.clear();
						++it;
						++count;
					}
					break;
				} else {
					tok.type = Type::word;
					tok.lexeme.push_back(c);
					++it;
					++count;
				}
			}
			if(count == 0) {
				done = true;
			}
			return *this;
		}
};
