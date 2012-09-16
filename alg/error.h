/*
 * Copyright (c) 2012 Martin Rödel aka Yomin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 */

#ifndef __ALG_ERROR_H__
#define __ALG_ERROR_H__

#define ALG_SUCCESS                  0
#define ALG_ERROR_BAD_STRUCTURE     -1
#define ALG_ERROR_BAD_SIZE          -2
#define ALG_ERROR_BAD_SOURCE        -3
#define ALG_ERROR_BAD_DESTINATION   -4
#define ALG_ERROR_NO_MEMORY         -5
#define ALG_ERROR_INDEX_RANGE       -6
#define ALG_ERROR_EMPTY             -7
#define ALG_ERROR_NOT_FOUND         -8
#define ALG_ERROR_UNSET             -9

char* alg_str_error(int error)
{
    switch(error)
    {
        case ALG_SUCCESS:               return "operation successfull";
        case ALG_ERROR_BAD_STRUCTURE:   return "bad structure";
        case ALG_ERROR_BAD_SIZE:        return "bad size";
        case ALG_ERROR_BAD_SOURCE:      return "bad source";
        case ALG_ERROR_BAD_DESTINATION: return "bad destination";
        case ALG_ERROR_NO_MEMORY:       return "unsufficient memory";
        case ALG_ERROR_INDEX_RANGE:     return "index out of range";
        case ALG_ERROR_EMPTY:           return "empty";
        case ALG_ERROR_NOT_FOUND:       return "not found";
        case ALG_ERROR_UNSET:           return "property unset";
    }
    return "unknown error";
}

#endif

