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

// 判断链表是否为空
bool link_list_is_empty(LINK_LIST_S *list);

#define link_list_destroy(list, cb, type, element) \
    LINK_LIST_S *head = list; \
    LINK_LIST_S *pos = NULL; \
    LINK_LIST_S *data = NULL; \
    while(head != NULL) {   \
        pos = head;         \    
        head = pos->next;   \
        pos->next = NULL;   \
        if (pos != list) {  \
            data = (void *)PTR_MEM(pos, type, element); \
            cb(data);        \
        }                   \
    }                       \
// 跳过了链表的头节点

#define link_list_for_each_entry(list, type, element, data) \
    LINK_LIST_S *pos = NULL;\
    for (pos = (list)->next; pos != NULL, data = PTR_MEM(pos, type, element); pos = pos->next)


// #define link_list_destroy(list, type, element, data, cb) \
//     link_list_for_each_entry(list, type, element, data) {  \
//         link_list_del_node(list, data->);

// 跳过了链表的头节点



#endif