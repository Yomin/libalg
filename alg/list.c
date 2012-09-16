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

#include "list.h"
#include "error.h"
#include "help.h"
#include <stdlib.h>
#include <string.h>

struct list_fold_state
{
    alg_foldfun *fun;
    void *state;
};

struct list_elem* list_intern_get(int pos, struct list *l)
{
    struct list_elem *elem;
    
    if(pos >= l->size/2)
    {
        pos = l->size - pos -1;
        elem = l->last;
        while(elem && --pos >= 0)
            elem = elem->prev;
    }
    else
    {
        elem = l->first;
        while(elem && --pos >= 0)
            elem = elem->next;
    }
    
    RET(elem, l);
}

struct list_elem* list_intern_gen(void *elem, struct list *l)
{
    struct list_elem* lelem = malloc(sizeof(struct list_elem));
    if(!lelem)
        RETZ(ALG_ERROR_NO_MEMORY, l);
    
    lelem->elem = malloc(l->esize);
    if(!lelem->elem)
        RETZ(ALG_ERROR_NO_MEMORY, l);
    
    lelem->next = 0;
    lelem->prev = 0;
    
    if(elem)
        memcpy(lelem->elem, elem, l->esize);
    else
        memset(lelem->elem, 0, l->esize);
    
    RET(lelem, l);
}

struct list_elem* list_intern_add(void *elem, struct list *l)
{
    struct list_elem *lelem = list_intern_gen(elem, l);
    CATCHZ(l);
    
    if(!l->first)
    {
        l->first = lelem;
        l->last = lelem;
        lelem->prev = 0;
    }
    else
    {
        l->last->next = lelem;
        lelem->prev = l->last;
        l->last = lelem;
    }
    
    (l->size)++;
    
    return lelem;
}

void list_intern_remove(struct list_elem *elem, struct list *l)
{
    if(!elem->prev)
        l->first = elem->next;
    else
        elem->prev->next = elem->next;
    
    if(!elem->next)
        l->last = elem->prev;
    else
        elem->next->prev = elem->prev;
    
    free(elem->elem);
    free(elem);
    
    (l->size)--;
}

struct list_elem* list_intern_iterate(alg_foldfun fun, void *state, struct list *l)
{
    struct list_elem* current = l->first;
    int pos = 0, ret;
    
    while(current)
    {
        ret = fun(pos, current->elem, state);
        if(ret < 0)
            RETZ(ret, l);
        if(ret > 0)
            RET(current, l);
        current = current->next;
        pos++;
    }
    
    RETZ(ALG_ERROR_NOT_FOUND, l);
}

int list_intern_fold(int pos, void *elem, void *vstate)
{
    struct list_fold_state *state = vstate;
    int ret = state->fun(pos, elem, state->state);
    if(ret != ALG_SUCCESS)
        return ret;
    return 0;
}

int list_init(int elemsize, struct list **pl)
{
    int malloced = 0;
    struct list *l;
    
    if(elemsize <= 0)
        return ALG_ERROR_BAD_SIZE;
    
    if(!pl)
        return ALG_ERROR_BAD_DESTINATION;
    
    if(!*pl)
    {
        malloced = 1;
        *pl = malloc(sizeof(struct list));
        if(!*pl)
            return ALG_ERROR_NO_MEMORY;
    }
    
    l = *pl;
    l->esize = elemsize;
    l->size = 0;
    l->first = 0;
    l->last = 0;
    l->current = 0;
    l->status |= ALG_STATUS_MALLOCED*malloced;
    
    RET(ALG_SUCCESS, l);
}

int list_finish(struct list *l)
{
    return list_finish_custom(0, 0, l);
}

int list_finish_custom(alg_foldfun *fun, void *state, struct list *l)
{
    struct list_elem *current, *next = l->first;
    int ret, pos;
    
    if(!l)
        return ALG_ERROR_BAD_STRUCTURE;
    
    pos = 0;
    while(next)
    {
        current = next;
        next = current->next;
        if(fun && (ret = fun(pos, current->elem, state)) != ALG_SUCCESS)
            return ret;
        free(current->elem);
        free(current);
        pos++;
    }
    
    l->first = 0;
    l->last = 0;
    l->current = 0;
    l->size = 0;
    if(l->status & ALG_STATUS_MALLOCED)
        free(l);
    
    RET(ALG_SUCCESS, l);
}

void* list_at(int pos, struct list *l)
{
    struct list_elem *elem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(pos < 0 || pos >= l->size)
        RETZ(ALG_ERROR_INDEX_RANGE, l);
    
    elem = list_intern_get(pos, l);
    
    RETI(elem, elem->elem, l);
}

void* list_at_c(int pos, struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_at_c(pos, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_get(int pos, void *dst, struct list *l)
{
    struct list_elem *elem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!dst)
        RETZ(ALG_ERROR_BAD_DESTINATION, l);
    
    if(pos < 0 || pos >= l->size)
        RETZ(ALG_ERROR_INDEX_RANGE, l);
    
    elem = list_intern_get(pos, l);
    
    memcpy(dst, elem->elem, l->esize);
    
    RETI(elem, elem->elem, l);
}

void* list_get_c(int pos, void *dst, struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_get_c(pos, dst, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_find(alg_foldfun fun, void *state, struct list *l)
{
    struct list_elem *elem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    elem = list_intern_iterate(fun, state, l);
    CATCHZ(l);
    
    RETI(elem, elem->elem, l);
}

void* list_find_c(alg_foldfun fun, void *state, struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_find_c(fun, state, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_current(struct list *l)
{
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!l->current)
        RETZ(ALG_ERROR_UNSET, l);
    
    RET(l->current->elem, l);
}

void* list_first(struct list *l)
{
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!l->first)
        RETZ(ALG_ERROR_EMPTY, l);
    
    RET(l->first->elem, l);
}

void* list_last(struct list *l)
{
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!l->last)
        RETZ(ALG_ERROR_EMPTY, l);
    
    RET(l->last->elem, l);
}

void* list_next(struct list *l)
{
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!l->current)
        RETZ(ALG_ERROR_UNSET, l);
    
    if(!l->current->next)
        RETZ(ALG_ERROR_INDEX_RANGE, l);
    
    RETI(l->current->next, l->current->next->elem, l);
}

void* list_next_c(struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_next(l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_prev(struct list *l)
{
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!l->current)
        RETZ(ALG_ERROR_UNSET, l);
    
    if(!l->current->prev)
        RETZ(ALG_ERROR_INDEX_RANGE, l);
    
    RETI(l->current->prev, l->current->prev->elem, l);
}

void* list_prev_c(struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_prev(l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

int list_size(struct list *l)
{
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    RET(l->size, l);
}

void* list_push(void *elem, struct list *l)
{
    struct list_elem *lelem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    lelem = list_intern_add(elem, l);
    CATCHZ(l);
    
    RETI(lelem, lelem->elem, l);
}

void* list_push_c(void *elem, struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_push(elem, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_ins(int pos, void *elem, struct list* l)
{
    struct list_elem *felem, *lelem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(pos < 0 || pos >= l->size)
        RETZ(ALG_ERROR_INDEX_RANGE, l);
    
    felem = list_intern_get(pos, l);
    lelem = list_intern_gen(elem, l);
    CATCHZ(l);
    
    if(pos == 0)
        l->first = lelem;
    
    lelem->prev = felem->prev;
    if(felem->prev)
        felem->prev->next = lelem;
    lelem->next = felem;
    felem->prev = lelem;
    (l->size)++;
    return lelem;
}

void* list_ins_c(int pos, void *elem, struct list* l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_ins(pos, elem, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_ins_after(alg_foldfun fun, void *state, void *elem, struct list *l)
{
    struct list_elem *felem, *lelem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    felem = list_intern_iterate(fun, state, l);
    CATCHZ(l);
    
    lelem = list_intern_gen(elem, l);
    CATCHZ(l);
    
    lelem->next = felem->next;
    if(felem->next)
        felem->next->prev = lelem;
    felem->next = lelem;
    lelem->prev = felem;
    RETI(lelem, lelem->elem, l);
}

void* list_ins_after_c(alg_foldfun fun, void *state, void *elem, struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_ins_after(fun, state, elem, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void* list_ins_before(alg_foldfun fun, void *state, void *elem, struct list *l)
{
    struct list_elem *felem, *lelem;
    
    if(!l)
        RETZ(ALG_ERROR_BAD_STRUCTURE, l);
    
    felem = list_intern_iterate(fun, state, l);
    CATCHZ(l);
    
    lelem = list_intern_gen(elem, l);
    CATCHZ(l);
    
    lelem->prev = felem->prev;
    if(felem->prev)
        felem->prev->next = lelem;
    felem->prev = lelem;
    lelem->next = felem;
    RETI(lelem, lelem->elem, l);
}

void* list_ins_before_c(alg_foldfun fun, void *state, void *elem, struct list *l)
{
    struct list_elem *lelem;
    EXEC_INTERN(lelem = list_ins_before(fun, state, elem, l), l);
    CATCHZ(l);
    l->current = lelem;
    return lelem->elem;
}

void list_pop(void *dst, struct list *l)
{
    list_pop_custom(dst, 0, l);
}

void list_pop_custom(void *dst, alg_mapfun fun, struct list *l)
{
    if(!l)
        RETV(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!dst)
        RETV(ALG_ERROR_BAD_DESTINATION, l);
    
    if(!l->last)
        RETV(ALG_ERROR_EMPTY, l);
    
    memcpy(dst, l->last->elem, l->esize);
    
    if(fun)
        fun(l->last->elem);
    
    if(l->last == l->current)
        l->current = 0;
    
    list_intern_remove(l->last, l);
    
    l->error = ALG_SUCCESS;
}

void list_del(int pos, struct list *l)
{
    list_del_custom(pos, 0, l);
}

void list_del_custom(int pos, alg_mapfun fun, struct list *l)
{
    struct list_elem *elem;
    
    if(!l)
        RETV(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(pos < 0 || pos >= l->size)
        RETV(ALG_ERROR_INDEX_RANGE, l);
    
    elem = list_intern_get(pos, l);
    
    if(fun)
        fun(elem->elem);
    
    if(elem == l->current)
        l->current = 0;
    
    list_intern_remove(elem, l);
    
    l->error = ALG_SUCCESS;
}

void list_rem(int pos, void *dst, struct list *l)
{
    list_rem_custom(pos, dst, 0, l);
}

void list_rem_custom(int pos, void *dst, alg_mapfun fun, struct list *l)
{
    struct list_elem *elem;
    
    if(!l)
        RETV(ALG_ERROR_BAD_STRUCTURE, l);
    
    if(!dst)
        RETV(ALG_ERROR_BAD_DESTINATION, l);
    
    if(pos < 0 || pos >= l->size)
        RETV(ALG_ERROR_INDEX_RANGE, l);
    
    elem = list_intern_get(pos, l);
    
    memcpy(dst, elem->elem, l->esize);
    
    if(fun)
        fun(elem->elem);
    
    if(elem == l->current)
        l->current = 0;
    
    list_intern_remove(elem, l);
    
    l->error = ALG_SUCCESS;
}

void list_fold(alg_foldfun fun, void *state, struct list *l)
{
    struct list_fold_state fstate;
    fstate.fun = fun;
    fstate.state = state;
    list_intern_iterate(list_intern_fold, &fstate, l);
}

void list_clear(struct list *l)
{
    list_clear_custom(0, 0, l);
}

void list_clear_custom(alg_foldfun fun, void *state, struct list *l)
{
    struct list_elem *current, *next;
    int pos;
    
    if(!l)
        RETV(ALG_ERROR_BAD_STRUCTURE, l);
    
    next = l->first;
    pos = 0;
    while(next)
    {
        current = next;
        next = current->next;
        if(fun)
            fun(pos, current->elem, state);
        free(current->elem);
        free(current);
        pos++;
    }
    l->first = 0;
    l->last = 0;
    l->current = 0;
    l->error = ALG_SUCCESS;
}

