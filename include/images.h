#ifndef images_H
#define images_H
#include "Image.h"
#include <string>
#include <tuple>
std::tuple<Image, bool> loadImage(std::string filename);
#endif
