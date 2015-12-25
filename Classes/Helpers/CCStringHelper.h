//
//  CCStringHelper.h
//  Cheetah
//
//  Created by Teerapat Avalable on 4/2/57 BE.
//
//

#ifndef __Cheetah__CCStringHelper__
#define __Cheetah__CCStringHelper__

//#include <iostream>
#include <string>

#define SEPARATOR ' ' //','

class CCStringHelper {
    
public:
    

    /** change long long integer to char const* with comma and add suffix*/
    static std::string createWithNumberFormatWithSuffix(long long n, char const* _suffix);

    /** change int integer to char const* with comma */
    static std::string createWithNumberFormat(unsigned int n);
    
    /** change int integer to char const* with comma */
    static std::string createWithNumberFormat(int n);
    
    /** change long long integer to char const* with comma */
    static std::string createWithNumberFormat(long long n);
    
    /** change char const* to char const* with comma */
    static std::string createWithNumberFormat(char const* n_char);
    
    /** limit number of characters in string and add .. at the end */
    static std::string  createWithLimitText(std::string string, int max);
    
};

#endif /* defined(__Cheetah__CCStringHelper__) */
