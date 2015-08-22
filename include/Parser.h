#ifndef Parser_H
#define Parser_H
#include "TokenStream.h"
#include "Object.h"
#include <map>
#include <vector>
#include <string>
typedef std::map<std::string, Object *> ObjectTable;
typedef std::vector<std::unique_ptr<Object>> ObjectPool;
void parse(TokenStream &, ObjectTable &, ObjectPool &);
#endif
