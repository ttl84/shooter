#ifndef images_H
#define images_H
#include "Image.h"
namespace bytes{
std::string players[] = {R"(
        a        
       aaa       
       a a       
      aabaa      
      abbba      
     aa b aa     
     aa b aa     
      a b a      
     accbcaa     
     ccaaacc     
    ccaaaaacc    
   ccc ddd ccc   
 ccccc ddd ccccc 
ccccc ddddd ccccc
 ccc   eee   ccc 
        e        
        e        
)"};
std::string enemies[] = {R"(
        a        
        a        
       b b       
      ba ab      
     bba abb     
    b aa aa b    
   b   a a   b   
      ca ac      
    ccccacccc    
   ccccc ccccc   
  cdd c a c ddc  
 ccdd a a a ddcc 
ccd    aaa    dcc
cd     aaa     dc
 d      a      d 
 d      a      d 
        a
)"};
std::string bullets[] = {R"(
a
a
a
a 
a
a 
a
a 
a
a 
a
a 
)"
,R"(
  a  
  a  
aaaaa
  a  
  a
)"};
std::string stars[] = {
	"a"
};
std::string explosions[] = {
R"(
    a
   b  ac
  abcdcd
   acbc
    c
)",
R"(
     a ba
 bcadc  acb
cb b cdd
aca cbcb
  cdabc  cb
abcbddb
  bdcb  abcd
)",
R"(
   ccbca
  cabacbaba
    acba
 c abcaa
)"
};
}

namespace palettes{
Palette players[] = {
	{
		{'a', {0, 0, 125}},
		{'b', {254, 0, 0}},
		{'c', {100, 155, 50}},
		{'d', {100, 100, 100}},
		{'e', {255, 255, 0}}
		
	}
};
Palette enemies[] = {
	{
		{'a', {0, 255, 255}},
		{'b', {254, 0, 0}},
		{'c', {0, 0, 250}},
		{'d', {255, 255, 0}}
	}
};
	


Palette explosions[] = {
	{
		{'a', {254, 254, 0}},
		{'b', {254, 0, 0}},
		{'c', {0, 254, 0}},
		{'d', {255, 255, 255}}
	}
};

Palette bullets[] = {
	{
		{'a', {255, 255, 0}}
	},
	{
		{'a', {0, 255, 0}}
	}
};
Palette stars[] = {
	{
		{'a', {255, 255, 255}}
	}
};
}
Pixel colorkey{255, 0, 0};

namespace images{
Image players[] = {
	{bytes::players[0], palettes::players[0], colorkey}
};
Image enemies[] = {
	{bytes::enemies[0], palettes::enemies[0], colorkey}
};
Image bullets[] = {
	{bytes::bullets[0], palettes::bullets[0], colorkey},
	{bytes::bullets[1], palettes::bullets[1], colorkey}
};
Image explosions[] = {
	{bytes::explosions[0], palettes::explosions[0], colorkey},
	{bytes::explosions[1], palettes::explosions[0], colorkey},
	{bytes::explosions[2], palettes::explosions[0], colorkey}
};
Image stars[] = {
	{bytes::stars[0], palettes::stars[0], colorkey}
};
}
#endif
