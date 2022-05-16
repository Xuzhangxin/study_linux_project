#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "base.h"
#include "list.h"


// 初始化链表
void link_list_init(LINK_LIST_S* list)
{
    CHECK_NULL_RETURN(list);
    list->next = NULL;

    return;
}

// 在链表尾部加入节点
void link_list_add_tail_node(LINK_LIST_S *list, LINK_LIST_S *node)
{
    CHECK_NULL_RETURN(list);
    CHECK_NULL_RETURN(node);

    LINK_LIST_S *pos = NULL;
    LINK_LIST_S *last = NULL;

    for (pos = list; pos->next != NULL; pos = pos->next);

    pos->next = node;

    return ;
}

// 删除链表中指定节点
void link_list_del_node(LINK_LIST_S *list, LINK_LIST_S *node)
{
    CHECK_NULL_RETURN(list);
    CHECK_NULL_RETURN(node);

    LINK_LIST_S *pos = NULL;
    LINK_LIST_S *prev = list;

    for (pos = list; pos != NULL; pos = pos->next) {    
        if (pos == node) {
            break; // found node
        }
        prev = pos;
    }
    
    prev->next = pos->next;  // delete node
    node->next = NULL;
    
    return ;
}


// 销毁链表（销毁内部，以及给外部提供销毁的接口）
void link_list_destroy(LINK_LIST_S *list, TRRAVELSAL_CB cb)
{
    CHECK_NULL_RETURN(list);

    LINK_LIST_S *head = list;
    LINK_LIST_S *pos = NULL;

    while(head != NULL) {
        pos = head;
        head = pos->next;
        cb(pos);
    }

    return;
}

// 判断链表是否为空
bool link_list_is_empty(LINK_LIST_S *list)
{
    if (list->next == NULL) {
        return true;
    }
    return false;
}

// 遍历链表
void link_list_for_each_entry(LINK_LIST_S *list, TRRAVELSAL_CB cb)
{
    CHECK_NULL_RETURN(list);
    CHECK_NULL_RETURN(cb);   

    LINK_LIST_S *pos = NULL;
    for (pos = list; pos != NULL; pos = pos->next) {       
        cb(pos);
    }

    return ;
}



typedef struct test_struct {
    LINK_LIST_S list;
    int a;
    int b;
} TEST_STRUCT_S ;

void list_travel(void *data)
{
    TEST_STRUCT_S *user_data = PTR_MEM(data, TEST_STRUCT_S, list);
    printf("node a :%d,", user_data->a);
    printf("node b :%d\n", user_data->b);
    fflush(stdout);
}

void list_travel_destroy(void *data)
{
    TEST_STRUCT_S *user_data = PTR_MEM(data, TEST_STRUCT_S, list);

    printf("free dizhi a :%p,", data);
    printf("free node a :%d,", user_data->a);
    printf("free node b :%d\n", user_data->b);
    free(data);

    // *data = NULL;
}


int main(int argc, char **argv) {
    TEST_STRUCT_S *list = malloc(sizeof(TEST_STRUCT_S));
    memset(list, 0, sizeof(TEST_STRUCT_S));

    link_list_init(&list->list);
    printf("init success\n");
    printf("list dizhi a :%p\n", list);
    fflush(stdout);

    TEST_STRUCT_S *node[10] = {0};
    
    for (int i = 0; i < 10; i++) {
        node[i] = malloc(sizeof(TEST_STRUCT_S));
        memset(node[i], 0, sizeof(TEST_STRUCT_S));
    }

    printf("malloc dizhi a :%p\n", node);
    fflush(stdout);
    int i = 0;
    for (i = 0; i < 10; i++) {
        node[i]->a = i + 1;
        node[i]->b = i + 1;
        link_list_add_tail_node(&list->list, &node[i]->list);
    }

    link_list_for_each_entry(&list->list, list_travel);

    link_list_destroy(&list->list, list_travel_destroy);

    // link_list_del_node(&list->list, &node[9].list);
    // link_list_del_node(&list->list, &node[0].list);
    // link_list_add_tail_node(&list->list, &node[0].list);
    // link_list_add_tail_node(&list->list, &node[9].list);
    // link_list_for_each_entry(&list->list, list_travel);

    // bool empty = link_list_is_empty(&list->list);
    // printf("empty 1 :%d\n", empty);

    // link_list_destroy(&list->list, list_travel_destroy);
    // empty = link_list_is_empty(&list->list);
    // printf("empty 2 :%d\n", empty);

    // link_list_add_tail_node(&list->list, &node[0].list);
    // link_list_for_each_entry(&list->list, list_travel);
}

