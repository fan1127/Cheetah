//
//  ArrayScale.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/5/2557 BE.
//
//

#include "ArrayScale.h"

void ArrayScale(int *input, int ninput, int *output, int noutput, int multiplier, int column) {
    
    for (int i=0; i<ninput; i++) {
        for (int j=0; j<multiplier; j++) {
            for (int k=0; k<multiplier; k++) {
                output[((int(i/column*multiplier)+j)*(multiplier*column))+(i%column)*multiplier+k] = input[i];
            }
        }
    }
    
}