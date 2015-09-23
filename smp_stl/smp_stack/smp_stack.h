#ifndef SMP_STACK_H
#define SMP_STACK_H


#define SMP_STACK_UNLIMITED     (0)
#define SMP_STACK_NULL          (-1)

typedef int (smp_stk_appoint) (void *stack_node_data, void *elem_data);


typedef struct smp_stack_node_s {
    struct smp_stack_node_s *below;     /* 栈的实现是链表栈，指向下面的栈节点 */
    char data[0];                       /* 0长度数组，兼容各种数据结构 */
}smp_stk_node_t;


typedef struct smp_stack_s {
    size_t size;                        /* 栈的总大小 */
    size_t curr_size;                   /* 栈当前元素个数 */
    smp_stk_appoint *appoint;           /* 元素赋值函数 */
    struct smp_stack_node_s *top;       /* 栈顶 */
    struct smp_stack_node_s *bottom;    /* 栈底，好像没有什么用 */
    struct smp_stack_node_s *idel;      /* 空闲节点的队列，被pop出的节点可以重新利用 */
    void *pool;                         /* 用来分配给栈内存的内存池 */
    uint elem_size;                     /* 每个栈元素的大小 */
}smp_stk_t;




struct smp_stack_s *smp_stack_init(uint elem_size, size_t stack_size, smp_stk_appoint *appoint);

inline SMP_VALUE smp_stack_elem_size(struct smp_stack_s *stack);
inline SMP_VALUE smp_stack_size(struct smp_stack_s *stack);
inline SMP_VALUE smp_stack_length(struct smp_stack_s *stack);
inline SMP_BOOLEAN smp_stack_empty(struct smp_stack_s *stack);
inline SMP_BOOLEAN smp_stack_filled(struct smp_stack_s *stack);

void *smp_stack_gettop(struct smp_stack_s *stack);
void *smp_stack_push(struct smp_stack_s *stack, void *elem_data);
struct smp_stack_node_s *smp_stack_pop(struct smp_stack_s *stack);

inline void smp_stack_recover(struct smp_stack_s *stack, struct smp_stack_node_s *node);
inline SMP_STATUS smp_stack_destory(struct smp_stack_s *stack);


#endif // SMP_STACK_H
