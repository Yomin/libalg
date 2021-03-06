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

#ifndef __ALG_VECTOR_H__
#define __ALG_VECTOR_H__

#include "fun.h"

#define ALG_VECTOR_CAPACITY 10
#define ALG_VECTOR_GROW     2   // factor to grow or shrink
#define ALG_VECTOR_SHRINK   3   // shrink threshold (capacity/size)

struct vector
{
    void *mem, *pos;
    int size, esize, capacity, error, capacited;
    char status;
};

int vector_init(int elemsize, struct vector **vec);
int vector_finish(struct vector *vec);
int vector_finish_custom(alg_foldfun fun, void *state, struct vector *vec);

void* vector_at(int pos, struct vector *vec);
void* vector_get(int pos, void *dst, struct vector *vec);
int   vector_size(struct vector *vec);
int   vector_capacity(struct vector *vec);

void* vector_push(void *elem, struct vector *vec);
void* vector_ins(int pos, void *elem, struct vector *vec);

void vector_pop(void *dst, struct vector *vec);
void vector_pop_custom(void *dst, alg_mapfun fun, struct vector *vec);
void vector_del(int pos, struct vector *vec);
void vector_del_custom(int pos, alg_mapfun fun, struct vector *vec);
void vector_rem(int pos, void *dst, struct vector *vec);
void vector_rem_custom(int pos, void *dst, alg_mapfun fun, struct vector *vec);
void vector_clear(struct vector *vec);
void vector_clear_custom(alg_foldfun fun, void *state, struct vector *vec);

void vector_set_capacity(int capacity, struct vector *vec);
void vector_set_capacity_custom(int capacity, alg_foldfun fun, void *state, struct vector *vec);

#endif

