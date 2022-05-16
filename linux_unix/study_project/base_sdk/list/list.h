#ifndef __LIST_H__
#define __LIST_H__

#define OFFSET_LIST(s, l) ((unsigned long int)(&(((s*)0)->l)))
#define PTR_MEM(ptr, type, list) \
        ({(type *)( (char *)ptr - OFFSET_LIST(type,list) );})

// 链式链表，内存全部给外部管理的版本
typedef void (*TRRAVELSAL_CB) (void *data);

typedef struct link_list_s {
    struct link_list_s *next;
} LINK_LIST_S;

// 初始化链表
void link_list_init(LINK_LIST_S* list);

// 在链表尾部加入节点
void link_list_add_tail_node(LINK_LIST_S *list, LINK_LIST_S *node);

// 删除链表中指定节点
void link_list_del_node(LINK_LIST_S *list, LINK_LIST_S *node);

// 清空链表
void link_list_destroy(LINK_LIST_S *list, TRRAVELSAL_CB cb);

// 判断链表是否为空
bool link_list_is_empty(LINK_LIST_S *list);

// 遍历链表
void link_list_for_each_entry(LINK_LIST_S *list, TRRAVELSAL_CB cb);



#endif