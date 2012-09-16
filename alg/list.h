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

#ifndef __ALG_LIST_H__
#define __ALG_LIST_H__

#include "fun.h"

struct list_elem
{
    struct list_elem *next, *prev;
    void *elem;
};

struct list
{
    struct list_elem *first, *last, *current;
    int size, esize, error;
    char status;
};

int list_init(int elemsize, struct list **l);
int list_finish(struct list* l);
int list_finish_custom(alg_foldfun *fun, void *state, struct list *l);

void* list_at(int pos, struct list *l);
void* list_at_c(int pos, struct list *l);
void* list_get(int pos, void *dst, struct list *l);
void* list_get_c(int pos, void *dst, struct list *l);
void* list_find(alg_foldfun fun, void *state, struct list *l);
void* list_find_c(alg_foldfun fun, void *state, struct list *l);
void* list_current(struct list *l);
void* list_first(struct list *l);
void* list_last(struct list *l);
void* list_next(struct list *l);
void* list_next_c(struct list *l);
void* list_prev(struct list *l);
void* list_prev_c(struct list *l);
int   list_size(struct list *l);

void* list_push(void *elem, struct list *l);
void* list_push_c(void *elem, struct list *l);
void* list_ins(int pos, void *elem, struct list* l);
void* list_ins_c(int pos, void *elem, struct list* l);
void* list_ins_after(alg_foldfun fun, void *state, void *elem, struct list *l);
void* list_ins_after_c(alg_foldfun fun, void *state, void *elem, struct list *l);
void* list_ins_before(alg_foldfun fun, void *state, void *elem, struct list *l);
void* list_ins_before_c(alg_foldfun fun, void *state, void *elem, struct list *l);

void list_pop(void *dst, struct list *l);
void list_pop_custom(void *dst, alg_mapfun fun, struct list *l);
void list_del(int pos, struct list *l);
void list_del_custom(int pos, alg_mapfun fun, struct list *l);
void list_del_current(struct list *l);
void list_rem(int pos, void *dst, struct list *l);
void list_rem_custom(int pos, void *dst, alg_mapfun fun, struct list *l);
void list_rem_current(void *dst, struct list *l);
void list_find_del(alg_foldfun fun, void *state, struct list *l);
void list_find_del_custom(alg_foldfun ffun, void *state, alg_mapfun dfun, struct list *l);
void list_find_rem(alg_foldfun fun, void *state, void *dst, struct list *l);
void list_find_rem_custom(alg_foldfun ffun, void *state, void *dst, alg_mapfun dfun, struct list *l);

void list_fold(alg_foldfun fun, void *state, struct list *l);

void list_clear(struct list *l);
void list_clear_custom(alg_foldfun fun, void *state, struct list *l);

#endif

