#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "base.h"

// 链式链表，内存全部给外部管理的版本
typedef void (*TRRAVELSAL_CB) (void *data);
typedef struct link_list_s {
    struct link_list_s *next;
} LINK_LIST_S;

// 初始化链表
int link_list_init(LINK_LIST_S* list);

// 在链表尾部加入节点
int link_list_add_tail_node(LINK_LIST_S *list, LINK_LIST_S *node);

// 删除链表中指定节点
int link_list_del_node(LINK_LIST_S *list, LINK_LIST_S *node);

// 清空链表
int link_list_destroy(LINK_LIST_S *list);

// 判断链表是否为空
bool link_list_is_empty(LINK_LIST_S *list);

// 遍历链表
int link_list_for_each_entry(LINK_LIST_S *list, TRRAVELSAL_CB cb);


// 初始化链表
int link_list_init(LINK_LIST_S* list)
{
    list->next = NULL;
}

// 在链表尾部加入节点
int link_list_add_tail_node(LINK_LIST_S *list, LINK_LIST_S *node)
{
    CHECK_NULL_RETURN_VAL(list);
    CHECK_NULL_RETURN_VAL(node);

    LINK_LIST_S *pos = NULL;
    LINK_LIST_S *last = NULL;

    for (pos = list; pos->next != NULL; pos = pos->next) {
        last = pos; // next为空 代表是最后一个元素
    }

    last->next = node;

    return 0;
}

// 删除链表中指定节点
int link_list_del_node(LINK_LIST_S *list, LINK_LIST_S *node)
{
    CHECK_NULL_RETURN_VAL(list);
    CHECK_NULL_RETURN_VAL(node);

    LINK_LIST_S *pos = NULL;
    LINK_LIST_S *prev = NULL;

    for (pos = list; pos->next != NULL; pos = pos->next) {    
        if (pos == node) {
            break; // found node
        }
        prev = pos;
    }
    
    prev->next = pos->next;  // delete node
    node->next = NULL;
    
    return 0;
}

// 清空链表
int link_list_destroy(LINK_LIST_S *list)
{
    CHECK_NULL_RETURN_VAL(list);

    list->next = NULL;
}

// 判断链表是否为空
bool link_list_is_empty(LINK_LIST_S *list)
{
    if (list == NULL) {
        return true;
    }
    return !(list->next);
}

// 遍历链表
int link_list_for_each_entry(LINK_LIST_S *list, TRRAVELSAL_CB cb)
{
    CHECK_NULL_RETURN_VAL(list);
    CHECK_NULL_RETURN_VAL(cb);   

    LINK_LIST_S *pos = NULL;
    for (pos = list; pos->next != NULL; pos = pos->next) {       
        cb(pos);
    }

    return 0;
}


typedef struct test_struct {
    LINK_LIST_S list;
    int a;
    int b;
} TEST_STRUCT_S ;

void list_travel(void *data)
{
    TEST_STRUCT_S *user_data = (TEST_STRUCT_S *)data;
    printf("node a :%d\n", user_data->a);
    printf("node b :%d\n", user_data->b);
    fflush(stdout);
}


int main(int argc, char **argv) {
    LINK_LIST_S *list = malloc(sizeof(LINK_LIST_S));
    memset(list, 0, sizeof(LINK_LIST_S));

    link_list_init(list);
    printf("init success\n");
    fflush(stdout);

    int i = 0;
    for (i = 0; i < 10; i++) {
        TEST_STRUCT_S *node = malloc(sizeof(TEST_STRUCT_S));
        memset(node, 0, sizeof(TEST_STRUCT_S));
        node->a = i;
        node->b = i;
        link_list_add_tail_node(list, &node->list);
        printf("add success\n");
        fflush(stdout);
    }

    link_list_for_each_entry(list, list_travel);
}

