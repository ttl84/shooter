#include <iterator>
#include <string>

namespace db{
enum Type{
	none, word, col, row, err
};
class Token{
public:
	std::string text;
	Type type;
	Token()
		:type(Type::none)
		{
		}
};

template<class It>
class TokenIterator;

template<class It>
class TokenStream {
	friend class TokenIterator<It>;

	Token tok;    // current token
	It itBegin, itEnd;   // underlying character iterators	
public:
	template<class Itt>
	TokenStream(Itt &&  itbegin, Itt && itend)
		:itBegin(std::forward<Itt>(itbegin)), itEnd(std::forward<Itt>(itend))
		{
		}
	TokenStream(std::istream& is)
		:TokenStream(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>())
		{
		}
	TokenIterator<It> begin();
	TokenIterator<It> end();
};

template<class It>
class TokenIterator
	: public std::iterator<std::input_iterator_tag, Token, void> {
	TokenStream<It> * s;
public:
	TokenIterator(TokenStream<It>* stream)
		:s(stream)
		{
		}
	TokenIterator()
		:s(nullptr)
		{
		}
	bool operator==(TokenIterator const & other) const
		{
			return s == other.s;
		}
	bool operator!=(TokenIterator const & other) const
		{
			return s != other.s;
		}
	TokenIterator& operator++()
		{
			if(s == 0)
				return *this;

			Token & tok = s->tok;
			tok.text.clear();
			tok.type = Type::none;
			It & it = s->itBegin;
			It & end = s->itEnd;

			bool quote = false;
			bool escape = false;
			while(it != end){
				char c = *it;
				if(escape) {
					escape = false;
					tok.text += c;
					++it;
				} else if(quote) {
					if(c == '\'' || c == '\"') {
						quote = false;
						++it;
						break;
					} else if(c == '\\') {
						escape = true;
					} else {
						tok.text += c;
						++it;
					}
				} else {
					if(c == '\'' || c == '\"') {
						if(tok.type == Type::none) {
							tok.type = Type::word;
							quote = true;
							++it;
						} else {
							break;
						}
					} else if(c == ' ' || c == '\t' || c == '\n' || c == '\r') {
						if(tok.type == Type::none) {
							++it;
						} else {
							break;
						}
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
						tok.text += c;
						++it;
					}
				}
			}


			if(quote) {
				// if quote is still on, then the end has been reached without closing it
				tok.type = Type::err;
			} else if(tok.type == Type::none) {
				// if type is still none after all that, then the end has been reached
				s = 0;
			}

			return *this;
		}

	const Token& operator*() const
		{
			return s->tok;
		}
};

template<class It>
TokenIterator<It> TokenStream<It>::begin()
{
	TokenIterator<It> it(this);
	++it;
	return it;
	
}

template<class It>
TokenIterator<It> TokenStream<It>::end()
{
	return TokenIterator<It>();
}

} // end namespace db
