/*
 * hof - helpers of fortune.
 *
 * Copyright © 2015 Jérôme Glisse
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * Authors: Jérôme Glisse <jglisse@redhat.com>
 */
#ifndef HOF_H
#define HOF_H

#include <stddef.h>
#include <stdint.h>

uint32_t hof_random32(void);
uint32_t hof_string_hash32(const char *string);

#ifndef container_of
#define container_of(ptr, type, member)                                     \
    (void *)((char *)(ptr) - ((char *)&(type)->member - (char *)(type)))
#endif

struct hof_list {
    struct hof_list             *prev;
    struct hof_list             *next;
};

static inline void hof_list_init_head(struct hof_list *item)
{
    item->prev = item;
    item->next = item;
}

static inline void hof_list_add(struct hof_list *item, struct hof_list *list)
{
    item->prev = list;
    item->next = list->next;
    list->next->prev = item;
    list->next = item;
}

static inline void hof_list_add_tail(struct hof_list *item,
                                     struct hof_list *list)
{
    item->next = list;
    item->prev = list->prev;
    list->prev->next = item;
    list->prev = item;
}

static inline void hof_list_replace(struct hof_list *from, struct hof_list *to)
{
    to->prev = from->prev;
    to->next = from->next;
    from->next->prev = to;
    from->prev->next = to;
}

static inline void hof_list_del(struct hof_list *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
}

static inline void hof_list_del_init(struct hof_list *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = item;
    item->prev = item;
}

static inline int hof_list_is_empty(struct hof_list *list)
{
    return list->next == list;
}

#define HOF_LIST_ENTRY(__type, __item, __field)                             \
    ((__type *)(((char *)(__item)) - offsetof(__type, __field)))

#define HOF_LIST_FIRST_ENTRY(t, l, f) LIST_ENTRY(t, (l)->next, f)
#define HOF_LIST_LAST_ENTRY(t, l, f) LIST_ENTRY(t, (l)->prev, f)

#define HOF_LIST_FOR_EACH_ENTRY(pos, head, member)                          \
    for (pos = container_of((head)->next, pos, member);                     \
         &pos->member != (head);                                            \
         pos = container_of(pos->member.next, pos, member))

#define HOF_LIST_FOR_EACH_ENTRY_SAFE(pos, n, head, member)                  \
    for (pos = container_of((head)->next, pos, member),                     \
         n = container_of(pos->member.next, pos, member);                   \
         &pos->member != (head);                                            \
         pos = n, n = container_of(n->member.next, n, member))

#define HOF_LIST_FOR_EACH_ENTRY_SAFE_REV(pos, n, head, member)              \
    for (pos = container_of((head)->prev, pos, member),                     \
         n = container_of(pos->member.prev, pos, member);                   \
         &pos->member != (head);                                            \
         pos = n, n = container_of(n->member.prev, n, member))

#define HOF_LIST_FOR_EACH_ENTRY_FROM(pos, start, head, member)              \
    for (pos = container_of((start), pos, member);                          \
         &pos->member != (head);                                            \
         pos = container_of(pos->member.next, pos, member))

#define HOF_LIST_FOR_EACH_ENTRY_FROM_REV(pos, start, head, member)          \
    for (pos = container_of((start), pos, member);                          \
         &pos->member != (head);                                            \
         pos = container_of(pos->member.prev, pos, member))

#endif /* HOF_H */
