#ifndef Size_H
#define Size_H
namespace{
struct Size{
	float w, h;
	Size(void): w(0), h(0) {}
	Size(float W, float H) : w(W), h(H) {}
};
}
#endif
