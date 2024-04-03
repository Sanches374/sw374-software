/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#ifndef _QC_TEST_LIST_H_
#define _QC_TEST_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stddef.h>

struct qc_list;

struct qc_list {
  struct qc_list *next;
  struct qc_list *prev;
};

static inline void qc_list_init(struct qc_list *list) {
  list->next = list;
  list->prev = list;
}

static inline void qc_list_add_head(struct qc_list *elem,
                                       struct qc_list *head) {
  struct qc_list *prev = head;
  struct qc_list *next = head->next;

  assert(elem != NULL);
  assert(head != NULL);

  next->prev = elem;
  elem->next = next;
  elem->prev = prev;
  prev->next = elem;
}

static inline void qc_list_add_tail(struct qc_list *elem,
                                       struct qc_list *head) {
  struct qc_list *prev = head->prev;
  struct qc_list *next = head;

  assert(elem != NULL);
  assert(head != NULL);

  next->prev = elem;
  elem->next = next;
  elem->prev = prev;
  prev->next = elem;
}

static inline void qc_list_del(struct qc_list *elem) {
  struct qc_list *prev = elem->prev;
  struct qc_list *next = elem->next;

  next->prev = prev;
  prev->next = next;
  elem->next = NULL;
  elem->prev = NULL;
}

static inline void qc_list_del_init(struct qc_list *entry) {
  struct qc_list *prev = entry->prev;
  struct qc_list *next = entry->next;

  next->prev = prev;
  prev->next = next;
  entry->next = entry;
  entry->prev = entry;
}

static inline int qc_list_empty(struct qc_list *head) {
  return head->next == head;
}

#define qc_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#ifndef container_of
#define container_of(ptr, type, member) ( { \
const typeof( ((type *)0)->member ) *__mptr = (ptr); \
(type *)( (char *)__mptr - qc_offsetof(type,member) ); } )
#endif

#define qc_list_entry(ptr, type, member) \
  container_of(ptr, type, member)

#define qc_list_first_entry(ptr, type, member) \
  qc_list_entry((ptr)->next, type, member)

#define qc_list_first_entry_or_null(ptr, type, member) ({ \
  struct qc_list *head__ = (ptr); \
  struct qc_list *pos__ = head__->next; \
  pos__ != head__ ? qc_list_entry(pos__, type, member) : NULL; \
})

#define qc_list_next_entry(pos, member) \
  qc_list_entry((pos)->member.next, typeof(*(pos)), member)

#define qc_list_entry_is_head(pos, head, member)    \
  (&pos->member == (head))

#define qc_list_for_each_entry(pos, head, member)               \
  for (pos = qc_list_first_entry(head, typeof(*pos), member);   \
    !qc_list_entry_is_head(pos, head, member);          \
      pos = qc_list_next_entry(pos, member))

#define qc_list_for_each_entry_safe(pos, n, head, member)           \
  for (pos = qc_list_first_entry(head, typeof(*pos), member),   \
    n = qc_list_next_entry(pos, member);            \
      !qc_list_entry_is_head(pos, head, member);            \
      pos = n, n = qc_list_next_entry(n, member))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_TEST_LIST_H_ */
