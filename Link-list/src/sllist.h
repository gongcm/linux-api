#ifndef _SLL_LIB_H_
#define _SLL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif


#define FALSE 0
#define TRUE  1



#define offset_of(type,member) ((unsigned int)(&(type*(0)->member)))
#define contain_of(ptr,type,member) ({ \
        typedef type *  __p_type; \
        __p_type __p = 0; \
        unsigned int offset = offset_of(type,member); \
        __p =  (__p_type)((char *)ptr - offset); \
        __p; \
})

/* Node of a linked list */
typedef struct slnode 
{
    struct slnode *next; /* Points at the next node in the list */
} SL_NODE;

/* Header for a linked list */
typedef struct sl_list 
{
    SL_NODE *head;	/* header of list */
    SL_NODE *tail;	/* tail of list */
} SL_LIST;

/* Singly linked list macros */
#define SLL_FIRST(pList) (((SL_LIST *)pList)->head)
#define SLL_LAST(pList)	 (((SL_LIST *)pList)->tail)
#define SLL_NEXT(pNode)	 (((SL_NODE *)pNode)->next)
#define SLL_EMPTY(pList) (((SL_LIST *)pList)->head == NULL)
#define SLL_INIT(pList) \
do {                    \
	((SL_LIST *)(pList))->head = NULL; \
	((SL_LIST *)(pList))->tail = NULL; \
}while(0)

/* function declarations */

SL_NODE* sllGet( SL_LIST *pList );
SL_NODE* sllPrevious( SL_LIST *pList, SL_NODE *pNode );
int sllCount( SL_LIST *pList );
void sllPutAtHead( SL_LIST *pList, SL_NODE *pNode );
void sllPutAtTail( SL_LIST *pList, SL_NODE *pNode );
void sllRemove( SL_LIST *pList, SL_NODE *pDeleteNode, SL_NODE *pPrevNode );
SL_NODE* sllEach( SL_LIST *pList, int(*routine)(SL_NODE*,void*), void* arg );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */

