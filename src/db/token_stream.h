#include <iterator>
#include <string>

enum Type{
	none, word, col, row, err
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
			tok.type = Type::none;
			tok.clear();
			bool quote = false;
			bool escape = false;
			while(it != end){
				char c = *it;
				if(escape) {
					escape = false;
					tok.lexeme += c;
					++it;
				} else if(quote) {
					if(c == '\'' || c == '\"') {
						quote = false;
						++it;
						break;
					} else if(c == '\\') {
						escape = true;
					} else {
						tok.lexeme += c;
						++it;
					}
				} else {
					if(c == '\'' || c == '\"') {
						tok.type = Type::word;
						quote = true;
						++it;
					} else if(c == ' ' || c == '\t' || c == '\n' || c == '\r') {
						++it;
					} else if(c == ',') {
						if(tok.type == Type::none) {
							tok.type = Type::col;
							++it;
						}
						break;
					} else if(c == ';') {
						if(tok.type == Type::none) {
							tok.type = Type::row;
							++it;
						}
						break;
					} else {
						tok.type = Type::word;
						tok.lexeme += c;
						++it;
					}
				}
			}


			if(quote) {
				// if quote is still on, then the end has been reached without closing it
				tok.type = Type::err;
			} else if(tok.type == Type::none) {
				// if type is still none after all that, then the end has been reached
				done = true;
			}
			return *this;
		}
};
