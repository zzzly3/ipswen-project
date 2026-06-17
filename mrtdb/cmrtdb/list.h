#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

struct list_head
{
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
   struct list_head name = LIST_HEAD_INIT(name)


/*

指针ptr指向结构体type中的成员member；
通过指针ptr，返回结构体type的起始地址，也就是list_entry返回指向type类型的指针

((type *)0)->member把0地址强制转化为type结构的指针，
再访问type结构体中的member成员，(&((type *)0)->member)获得了member在type结构中的偏移量。
其中(char *)(ptr)求出的是ptr的绝对地址，二者相减，于是得到type类型结构体的起始地址。


*/

#define list_entry(ptr, type, member) \
    ((type *)((char*)(ptr)-(unsigned long)(&((type *)0)->member)))


#define list_for_each_entry(pos, head, member)    \
 for (pos = list_entry((head)->next, typeof(*pos), member); \
      &pos->member != (head);  \
      pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member)   \
   for (pos = list_entry((head)->prev, typeof(*pos), member); \
        &pos->member != (head);  \
        pos = list_entry(pos->member.prev, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)  \
    for (pos = list_entry((head)->next, typeof(*pos), member), \
        n = list_entry(pos->member.next, typeof(*pos), member);   \
        &pos->member != (head); \
        pos = n, n = list_entry(n->member.next, typeof(*n), member))

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
	struct list_head *prev,
	struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}


static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);

	/* 指向特定的位置，反初始化 */
	//entry->next = LIST_POISON1;
	//entry->prev = LIST_POISON2;

	// 指向自己
	INIT_LIST_HEAD(entry);
}


#endif