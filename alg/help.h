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

#ifndef __ALG_HELP_H__
#define __ALG_HELP_H__

#define ALG_STATUS_MALLOCED 1
#define ALG_STATUS_INTERN   2

#define RET(ret, obj)   { (obj)->error = ALG_SUCCESS; return (ret); }
#define RETV(stat, obj) { (obj)->error = (stat); return; }
#define RETZ(stat, obj) { (obj)->error = (stat); return 0; }
#define CATCHV(obj)     if((obj)->error != ALG_SUCCESS) return;
#define CATCHZ(obj)     if((obj)->error != ALG_SUCCESS) return 0;

#define RETI(i, e, obj) \
{ \
    (obj)->error = ALG_SUCCESS; \
    if((obj)->status & ALG_STATUS_INTERN) \
        return (i); \
    else \
        return (e); \
}

#define EXEC_INTERN(f, obj) \
{ \
    (obj)->status |= ALG_STATUS_INTERN; \
    (f); \
    (obj)->status &= ~ALG_STATUS_INTERN; \
}

#endif

