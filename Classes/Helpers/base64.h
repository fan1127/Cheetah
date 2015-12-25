//
//  base64.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/7/2557 BE.
//
//

#ifndef __Cheetah__base64__
#define __Cheetah__base64__

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif /* defined(__Cheetah__base64__) */
