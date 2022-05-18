#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "base.h"
#include "list.h"



void MY_PRINTF(const char *fmt, ...)
{
#if defined(DEBUG_BASE_SDK) && (DEBUG_BASE_SDK == 1)
    printf(fmt);
    fflush(stdout);
#endif
}


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
    MY_PRINTF("list add node:%p, last node:%p\n", node, pos);
    return ;
}

// 删除链表中指定节点
void link_list_del_node(LINK_LIST_S *list, LINK_LIST_S *node)
{
    CHECK_NULL_RETURN(list);
    CHECK_NULL_RETURN(node);

    LINK_LIST_S *pos = NULL;
    LINK_LIST_S *prev = list;
    // 头节点不参与
    for (pos = list->next; pos != NULL; pos = pos->next) {    
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
        pos->next = NULL;
        MY_PRINTF("list destroy node:%p, next node:%p\n", pos, pos->next);
        if (pos != list) { // 头节点不给外部销毁处理
            cb(pos);
        }
    }

    // *list = NULL;

    return;
}

// 判断链表是否为空
bool link_list_is_empty(LINK_LIST_S *list)
{
    MY_PRINTF("link list is empty, next node:%p", list->next);
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
    for (pos = list->next; pos != NULL; pos = pos->next) {       
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
    free(data); //无法指向NULL，由外部去做，所有的取数据，遍历数据，都应该通过链表的对外接口去做，所以这里也无需指向NULL

}


int main(int argc, char **argv) {
    TEST_STRUCT_S *list = malloc(sizeof(TEST_STRUCT_S)); //头节点不参与操作
    memset(list, 0, sizeof(TEST_STRUCT_S));

    link_list_init(&list->list);
    printf("init success\n");
    printf("list dizhi a :%p\n", list);

    TEST_STRUCT_S *node[10] = {0};
    
    for (int i = 0; i < 10; i++) {
        node[i] = malloc(sizeof(TEST_STRUCT_S));
        memset(node[i], 0, sizeof(TEST_STRUCT_S));
    }

    printf("malloc dizhi a :%p\n", node);
    int i = 0;
    for (i = 0; i < 10; i++) {
        node[i]->a = i + 1;
        node[i]->b = i + 1;
        link_list_add_tail_node(&list->list, &node[i]->list);
    }

    link_list_for_each_entry(&list->list, list_travel);

    bool empty = link_list_is_empty(&list->list);
    printf("empty 0 :%d\n", empty);    
    link_list_destroy(&list->list, list_travel_destroy);


    empty = link_list_is_empty(&list->list);
    printf("empty 1 :%d\n", empty);

    //link_list_for_each_entry(&list->list, list_travel);
    // link_list_del_node(&list->list, &node[9].list);
    // link_list_del_node(&list->list, &node[0].list);
    // link_list_add_tail_node(&list->list, &node[0].list);
    // link_list_add_tail_node(&list->list, &node[9].list);
    // link_list_for_each_entry(&list->list, list_travel);

    // bool empty = link_list_is_empty(&list->list);
    // MY_PRINTF("empty 1 :%d\n", empty);

    // link_list_destroy(&list->list, list_travel_destroy);
    // empty = link_list_is_empty(&list->list);
    // MY_PRINTF("empty 2 :%d\n", empty);

    // link_list_add_tail_node(&list->list, &node[0].list);
    // link_list_for_each_entry(&list->list, list_travel);
}

