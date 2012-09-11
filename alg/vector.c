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

#include "vector.h"
#include "errors.h"
#include <stdlib.h>
#include <string.h>

int vector_grow(int capacity, struct vector *vec)
{
    void *tmp = realloc(vec->mem, capacity*vec->esize);
    if(!tmp)
        return ALG_ERROR_NO_MEMORY;
    vec->mem = tmp;
    vec->capacity = capacity;
    
    return ALG_SUCCESS;
}

int vector_autogrow(struct vector *vec)
{
    if(vec->pos-vec->mem == vec->capacity)
        return vector_grow(vec->capacity*ALG_VECTOR_GROW, vec);
    return ALG_SUCCESS;
}

int vector_shrink(int capacity, struct vector *vec)
{
    void *tmp = realloc(vec->mem, capacity*vec->esize);
    if(!tmp)
        return ALG_ERROR_NO_MEMORY;
    vec->mem = tmp;
    vec->capacity = capacity;
    
    return ALG_SUCCESS;
}

int vector_autoshrink(struct vector *vec)
{
    if(vec->pos-vec->mem <= vec->capacity/ALG_VECTOR_SHRINK && vec->capacity > vec->capacited)
        return vector_shrink(vec->capacity/ALG_VECTOR_GROW, vec);
    return ALG_SUCCESS;
}

int vector_init(int elemsize, struct vector *vec)
{
    int malloced = 0;
    
    if(elemsize <= 0)
        return ALG_ERROR_BAD_SIZE;
    
    if(!vec)
    {
        malloced = 1;
        vec = malloc(sizeof(struct vector));
        if(!vec)
            return ALG_ERROR_NO_MEMORY;
    }
    
    vec->esize = elemsize;
    vec->capacity = ALG_VECTOR_CAPACITY;
    vec->malloced = malloced;
    vec->capacited = ALG_VECTOR_CAPACITY;
    vec->mem = malloc(elemsize*ALG_VECTOR_CAPACITY);
    
    if(!vec->mem)
    {
        if(malloced)
            free(vec);
        return ALG_ERROR_NO_MEMORY;
    }
    
    vec->pos = vec->mem;
    
    return ALG_SUCCESS;
}

int vector_finish(struct vector *vec)
{
    return vector_finish_custom(0, 0, vec);
}

int vector_finish_custom(vector_finishfun fun, void *state, struct vector *vec)
{
    int i, ret;
    void *ptr;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(fun)
        for(i=0, ptr=vec->mem; ptr != vec->pos; i++, ptr += vec->esize)
            if((ret = fun(i, state, ptr)) != ALG_SUCCESS)
                return ret;
    
    free(vec->mem);
    if(vec->malloced)
        free(vec);
    
    return ALG_SUCCESS;
}

void* vector_at(int pos, struct vector *vec)
{
    if(!vec || pos < 0 || pos >= vec->pos-vec->mem)
        return 0;
    
    return vec->mem + pos*vec->esize;
}

int vector_size(struct vector *vec)
{
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    return vec->pos - vec->mem;
}

int vector_capacity(struct vector *vec)
{
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    return vec->capacity;
}

int vector_push(void *elem, struct vector *vec)
{
    int ret;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(!elem)
        return ALG_ERROR_BAD_SOURCE;
    
    if((ret = vector_autogrow(vec)) != ALG_SUCCESS)
        return ret;
    
    memcpy(vec->pos, elem, vec->esize);
    vec->pos += vec->esize;
    
    return ALG_SUCCESS;
}

int vector_pop(void *dst, struct vector *vec)
{
    return vector_pop_custom(dst, 0, vec);
}

int vector_pop_custom(void *dst, vector_delfun fun, struct vector *vec)
{
    int ret;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(!dst)
        return ALG_ERROR_BAD_DESTINATION;
    
    if(vec->pos == vec->mem)
        return ALG_ERROR_EMPTY;
    
    vec->pos -= vec->esize;
    memcpy(dst, vec->pos, vec->esize);
    
    if(fun && (ret = fun(vec->pos)) != ALG_SUCCESS)
        return ret;
    
    return vector_autoshrink(vec);
}

int vector_ins(int pos, void *elem, struct vector *vec)
{
    int ret;
    void *ptr;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(!elem)
        return ALG_ERROR_BAD_SOURCE;
    
    if(pos < 0)
        return ALG_ERROR_INDEX_RANGE;
    
    if((ret = vector_autogrow(vec)) != ALG_SUCCESS)
        return ret;
    
    ptr = vec->mem+pos*vec->esize;
    memmove(ptr+vec->esize, ptr, (vec->pos-vec->mem-pos)*vec->esize);
    vec->pos += vec->esize;
    memcpy(ptr, elem, vec->esize);
    
    return ALG_SUCCESS;
}

int vector_del(int pos, struct vector *vec)
{
    return vector_rem_custom(pos, 0, 0, vec);
}

int vector_del_custom(int pos, vector_delfun fun, struct vector *vec)
{
    return vector_rem_custom(pos, fun, 0, vec);
}

int vector_rem(int pos, void *elem, struct vector *vec)
{
    return vector_rem_custom(pos, elem, 0, vec);
}

int vector_rem_custom(int pos, void *elem, vector_delfun fun, struct vector *vec)
{
    int ret;
    void *ptr;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(pos < 0 || pos >= vec->pos-vec->mem)
        return ALG_ERROR_INDEX_RANGE;
    
    ptr = vec->mem+pos*vec->esize;
    
    if(elem)
        memcpy(elem, ptr, vec->esize);
    
    if(fun && (ret = fun(ptr)) != ALG_SUCCESS)
        return ret;
    
    memmove(ptr, ptr+vec->esize, (vec->pos-vec->mem-pos-1)*vec->esize);
    vec->pos -= vec->esize;
    
    return vector_autoshrink(vec);
}

int vector_clear(struct vector *vec)
{
    return vector_clear_custom(0, 0, vec);
}

int vector_clear_custom(vector_finishfun fun, void *state, struct vector *vec)
{
    int i, ret;
    void *ptr;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(fun)
        for(i=0, ptr=vec->mem; ptr != vec->pos; i++, ptr += vec->esize)
            if((ret = fun(i, state, ptr)) != ALG_SUCCESS)
                return ret;
    
    vec->pos = vec->mem;
    
    return vector_autoshrink(vec);
}

int vector_set_capacity(int capacity, struct vector *vec)
{
    return vector_set_capacity_custom(capacity, 0, 0, vec);
}

int vector_set_capacity_custom(int capacity, vector_finishfun fun, void *state, struct vector *vec)
{
    int i, ret;
    void *ptr, *last;
    
    if(!vec)
        return ALG_ERROR_BAD_STRUCTURE;
    
    if(capacity <= 0)
        return ALG_ERROR_BAD_SIZE;
    
    if(capacity < vec->capacity)
    {
        if(capacity < vec->pos-vec->mem && fun)
        {
            last = vec->pos;
            vec->pos = vec->mem + capacity*vec->esize;
            for(i=capacity, ptr=vec->pos; ptr != last; i++, ptr += vec->esize)
                if((ret = fun(i, state, ptr)) != ALG_SUCCESS)
                    return ret;
        }
        if((ret = vector_shrink(capacity, vec)) != ALG_SUCCESS)
            return ret;
    }
    else if(capacity > vec->capacity)
    {
        if((ret = vector_grow(capacity, vec)) != ALG_SUCCESS)
            return ret;
    }
    
    vec->capacited = capacity;
    
    return ALG_SUCCESS;
}

