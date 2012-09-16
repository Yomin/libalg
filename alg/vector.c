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
#include "error.h"
#include "help.h"
#include <stdlib.h>
#include <string.h>

void vector_grow(int capacity, struct vector *vec)
{
    void *tmp = realloc(vec->mem, capacity*vec->esize);
    if(!tmp)
        RETV(ALG_ERROR_NO_MEMORY, vec);
    vec->mem = tmp;
    vec->pos = tmp + vec->size*vec->esize;
    vec->capacity = capacity;
    
    vec->error = ALG_SUCCESS;
}

void vector_autogrow(struct vector *vec)
{
    if(vec->size == vec->capacity)
        vector_grow(vec->capacity*ALG_VECTOR_GROW, vec);
    CATCHV(vec);
    vec->error = ALG_SUCCESS;
}

void vector_shrink(int capacity, struct vector *vec)
{
    void *tmp = realloc(vec->mem, capacity*vec->esize);
    if(!tmp)
        RETV(ALG_ERROR_NO_MEMORY, vec);
    vec->mem = tmp;
    vec->pos = tmp + vec->size*vec->esize;
    vec->capacity = capacity;
    
    vec->error = ALG_SUCCESS;
}

void vector_autoshrink(struct vector *vec)
{
    if(vec->size <= vec->capacity/ALG_VECTOR_SHRINK && vec->capacity > vec->capacited)
        vector_shrink(vec->capacity/ALG_VECTOR_GROW, vec);
    CATCHV(vec);
    vec->error = ALG_SUCCESS;
}

int vector_init(int elemsize, struct vector **vec)
{
    int malloced = 0;
    struct vector *v;
    
    if(elemsize <= 0)
        return ALG_ERROR_BAD_SIZE;
    
    if(!vec)
        return ALG_ERROR_BAD_DESTINATION;
    
    if(!*vec)
    {
        malloced = 1;
        *vec = malloc(sizeof(struct vector));
        if(!*vec)
            return ALG_ERROR_NO_MEMORY;
    }
    
    v = *vec;
    v->size = 0;
    v->esize = elemsize;
    v->capacity = ALG_VECTOR_CAPACITY;
    v->status = ALG_STATUS_MALLOCED*malloced;
    v->capacited = ALG_VECTOR_CAPACITY;
    v->mem = malloc(elemsize*ALG_VECTOR_CAPACITY);
    
    if(!v->mem)
    {
        if(malloced)
            free(v);
        return ALG_ERROR_NO_MEMORY;
    }
    
    v->pos = v->mem;
    
    RET(ALG_SUCCESS, v);
}

int vector_finish(struct vector *vec)
{
    return vector_finish_custom(0, 0, vec);
}

int vector_finish_custom(alg_foldfun fun, void *state, struct vector *vec)
{
    int i, ret;
    void *ptr;
    
    if(!vec)
        RETE(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(fun)
        for(i=0, ptr=vec->mem; i<vec->size; i++, ptr += vec->esize)
            if((ret = fun(i, state, ptr)) != ALG_SUCCESS)
                RETE(ret, vec);
    
    free(vec->mem);
    if(vec->status & ALG_STATUS_MALLOCED)
        free(vec);
    else
        memset(vec, 0, sizeof(struct vector));
    
    return ALG_SUCCESS;
}

void* vector_at(int pos, struct vector *vec)
{
    if(!vec)
        RETZ(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(pos < 0 || pos >= vec->size)
        RETZ(ALG_ERROR_INDEX_RANGE, vec);
    
    RET(vec->mem + pos*vec->esize, vec);
}

void* vector_get(int pos, void *dst, struct vector *vec)
{
    if(!vec)
        RETZ(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(!dst)
        RETZ(ALG_ERROR_BAD_DESTINATION, vec);
    
    if(pos < 0 || pos >= vec->size)
        RETZ(ALG_ERROR_INDEX_RANGE, vec);
    
    memcpy(dst, vec->mem+pos*vec->esize, vec->esize);
    
    RET(vec->mem+pos*vec->esize, vec);
}

int vector_size(struct vector *vec)
{
    if(!vec)
        RETZ(ALG_ERROR_BAD_STRUCTURE, vec);
    
    RET(vec->size, vec);
}

int vector_capacity(struct vector *vec)
{
    if(!vec)
        RETZ(ALG_ERROR_BAD_STRUCTURE, vec);
    
    RET(vec->capacity, vec);
}

void* vector_push(void *elem, struct vector *vec)
{
    if(!vec)
        RETZ(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(!elem)
        RETZ(ALG_ERROR_BAD_SOURCE, vec);
    
    vector_autogrow(vec);
    CATCHZ(vec);
    
    memcpy(vec->pos, elem, vec->esize);
    vec->pos += vec->esize;
    vec->size++;
    
    RET(vec->pos-vec->esize, vec);
}

void vector_pop(void *dst, struct vector *vec)
{
    vector_pop_custom(dst, 0, vec);
}

void vector_pop_custom(void *dst, alg_mapfun fun, struct vector *vec)
{
    int ret;
    
    if(!vec)
        RETV(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(!vec->size)
        RETV(ALG_ERROR_EMPTY, vec);
    
    vec->pos -= vec->esize;
    vec->size--;
    if(dst)
        memcpy(dst, vec->pos, vec->esize);
    
    if(fun && (ret = fun(vec->pos)) != ALG_SUCCESS)
        RETV(ret, vec);
    
    vector_autoshrink(vec);
    CATCHV(vec);
    
    vec->error = ALG_SUCCESS;
}

void* vector_ins(int pos, void *elem, struct vector *vec)
{
    void *ptr;
    
    if(!vec)
        RETZ(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(!elem)
        RETZ(ALG_ERROR_BAD_SOURCE, vec);
    
    if(pos < 0 || pos >= vec->size)
        RETZ(ALG_ERROR_INDEX_RANGE, vec);
    
    vector_autogrow(vec);
    CATCHZ(vec);
    
    ptr = vec->mem+pos*vec->esize;
    memmove(ptr+vec->esize, ptr, (vec->size-pos)*vec->esize);
    vec->pos += vec->esize;
    vec->size++;
    memcpy(ptr, elem, vec->esize);
    
    RET(ptr, vec);
}

void vector_del(int pos, struct vector *vec)
{
    vector_rem_custom(pos, 0, 0, vec);
}

void vector_del_custom(int pos, alg_mapfun fun, struct vector *vec)
{
    vector_rem_custom(pos, fun, 0, vec);
}

void vector_rem(int pos, void *dst, struct vector *vec)
{
    vector_rem_custom(pos, dst, 0, vec);
}

void vector_rem_custom(int pos, void *dst, alg_mapfun fun, struct vector *vec)
{
    int ret;
    void *ptr;
    
    if(!vec)
        RETV(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(pos < 0 || pos >= vec->size)
        RETV(ALG_ERROR_INDEX_RANGE, vec);
    
    ptr = vec->mem+pos*vec->esize;
    
    if(dst)
        memcpy(dst, ptr, vec->esize);
    
    if(fun && (ret = fun(ptr)) != ALG_SUCCESS)
        RETV(ret, vec);
    
    memmove(ptr, ptr+vec->esize, (vec->size-pos-1)*vec->esize);
    vec->pos -= vec->esize;
    vec->size--;
    
    vector_autoshrink(vec);
    CATCHV(vec);
    
    vec->error = ALG_SUCCESS;
}

void vector_clear(struct vector *vec)
{
    vector_clear_custom(0, 0, vec);
}

void vector_clear_custom(alg_foldfun fun, void *state, struct vector *vec)
{
    int i, ret;
    void *ptr;
    
    if(!vec)
        RETV(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(fun)
        for(i=0, ptr=vec->mem; i<vec->size; i++, ptr += vec->esize)
            if((ret = fun(i, state, ptr)) != ALG_SUCCESS)
                RETV(ret, vec);
    
    vec->pos = vec->mem;
    vec->size = 0;
    
    vector_autoshrink(vec);
    CATCHV(vec);
    
    vec->error = ALG_SUCCESS;
}

void vector_set_capacity(int capacity, struct vector *vec)
{
    vector_set_capacity_custom(capacity, 0, 0, vec);
}

void vector_set_capacity_custom(int capacity, alg_foldfun fun, void *state, struct vector *vec)
{
    int i, count, ret;
    void *ptr;
    
    if(!vec)
        RETV(ALG_ERROR_BAD_STRUCTURE, vec);
    
    if(capacity <= 0)
        RETV(ALG_ERROR_BAD_SIZE, vec);
    
    if(capacity < vec->capacity)
    {
        if(capacity < vec->size)
        {
            vec->pos = vec->mem + capacity*vec->esize;
            count = vec->size - capacity;
            vec->size = capacity;
            
            if(fun)
                for(i=capacity, ptr=vec->pos; i<count; i++, ptr += vec->esize)
                    if((ret = fun(i, state, ptr)) != ALG_SUCCESS)
                        RETV(ret, vec);
        }
        vector_shrink(capacity, vec);
        CATCHV(vec);
    }
    else if(capacity > vec->capacity)
    {
        vector_grow(capacity, vec);
        CATCHV(vec);
    }
    
    vec->capacited = capacity;
    
    vec->error = ALG_SUCCESS;
}

#ifdef ALG_TEST

#include <stdio.h>

void show_vector(struct vector *vec)
{
    int i;
    int *mem = (int*) vec->mem;
    
    printf("size: %i | capacity: %i | diff: %i | ", vec->size, vec->capacity, vec->pos-vec->mem);
    if(vec->size == 0)
        printf("empty");
    if(vec->size > 0)
        printf("%i", mem[0]);
    if(vec->size > 1)
        for(i=1; i<vec->size; i++)
            printf(", %i", mem[i]);
    printf("\n");
}

int catch(struct vector *vec)
{
    if(vec->error != ALG_SUCCESS)
    {
        printf("error: %s\n", alg_str_error(vec->error));
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    struct vector *vec = 0;
    int i = 23, j, count;
    
    if(vector_init(sizeof(int), &vec) != ALG_SUCCESS)
        return 1;
    show_vector(vec);
    
    vector_push(&i, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    for(i=0; i<ALG_VECTOR_CAPACITY-1; i++)
    {
        vector_push(&i, vec);
        if(catch(vec))
            return 1;
    }
    show_vector(vec);
    
    vector_push(&i, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    vector_pop(&i, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    count = vec->size - vec->capacity/ALG_VECTOR_SHRINK -1;
    for(i=0; i<count; i++)
    {
        vector_pop(&j, vec);
        if(catch(vec))
            return 1;
    }
    show_vector(vec);
    
    vector_pop(&i, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    i = 42;
    vector_ins(0, &i, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    vector_del(1, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    vector_set_capacity(23, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    vector_set_capacity(3, vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    vector_clear(vec);
    if(catch(vec))
        return 1;
    show_vector(vec);
    
    if(vector_finish(vec) != ALG_SUCCESS)
        return 1;
    
    return 0;
}

#endif

