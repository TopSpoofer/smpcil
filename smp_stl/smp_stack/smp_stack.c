#include "../../smpcil.h"
#include "../../smp_mpool/smp_mpool.h"
#include "../../smp_linux/smp_error/smp_error.h"
#include "smp_stack.h"

static struct smp_stack_node_s *smp_stack_node_create(struct smp_stack_s *stack);
inline static void __smp_stack_init(struct smp_stack_s *stack, smp_stk_appoint *appoint,
                           uint elem_size, size_t stack_size);



static struct smp_stack_node_s *smp_stack_node_create(struct smp_stack_s *stack)
{
    struct smp_stack_node_s *node = NULL;

    if (smp_unlikely(stack == NULL)) return NULL;

    if (stack->idel != NULL) {
        printf("reuse\n");
        node = stack->idel;
        stack->idel = node->below;
        memset(node->data, '\0', stack->elem_size);
        node->below = NULL;
        return node;
    }

    node = smp_mpalloc((smp_pool_t *) stack->pool, sizeof(struct smp_stack_node_s) + stack->elem_size);
    if (node == NULL) return NULL;

    node->below = NULL;

    return node;
}


inline static void __smp_stack_init(struct smp_stack_s *stack, smp_stk_appoint *appoint,
                           uint elem_size, size_t stack_size)
{
    if (smp_unlikely(stack == NULL)) return;
    stack->bottom = NULL;
    stack->curr_size = 0;
    stack->elem_size = elem_size;
    stack->size = stack_size;
    stack->top = NULL;
    stack->appoint = appoint;
    stack->idel = NULL;
}


struct smp_stack_s *smp_stack_init(uint elem_size, size_t stack_size, smp_stk_appoint *appoint)
{
    size_t size = 0;
    struct smp_stack_s *stack = NULL;
    smp_pool_t *pool = NULL;

    if (stack_size == SMP_STACK_UNLIMITED) size = 2048;
    else size = elem_size * stack_size;

    /* 分配多一点的空间，防止出现问题 */
    pool = smp_mpool_create(size + sizeof(smp_pool_t) + sizeof(smp_big_pool_t));
    if (pool == NULL) return NULL;

    if ((stack = smp_mpalloc(pool, sizeof(struct smp_stack_s))) == NULL) {
        smp_mpdestory(pool);
        return NULL;
    }

    __smp_stack_init(stack, appoint, elem_size, stack_size);
    stack->pool = (void *) pool;

    return stack;
}


inline SMP_VALUE smp_stack_elem_size(struct smp_stack_s *stack)
{
    if (smp_unlikely(stack == NULL)) return SMP_STACK_NULL;

    return stack->elem_size;
}


inline SMP_VALUE smp_stack_size(struct smp_stack_s *stack)
{
    if (smp_unlikely(stack == NULL)) return SMP_STACK_NULL;

    return stack->size;
}


inline SMP_VALUE smp_stack_length(struct smp_stack_s *stack)
{
    if (smp_unlikely(stack == NULL)) return SMP_STACK_NULL;

    return stack->curr_size;
}


inline SMP_BOOLEAN smp_stack_empty(struct smp_stack_s *stack)
{
    if (smp_unlikely(stack == NULL)) return SMP_STACK_NULL;
    if (smp_stack_length(stack) == 0 || stack->top == NULL) return SMP_TRUE;
    return SMP_FALSE;
}


inline SMP_BOOLEAN smp_stack_filled(struct smp_stack_s *stack)
{
    size_t len = 0;
    /* 栈是无限大的，不会溢出，除非内存已经用完 */
    if (smp_unlikely(stack == NULL)) return SMP_STACK_NULL;
    if ((len = smp_stack_size(stack)) == SMP_STACK_UNLIMITED) return SMP_FALSE;
    if ((size_t) smp_stack_length(stack) >= len) return SMP_TRUE;
    return SMP_FALSE;
}


void *smp_stack_gettop(struct smp_stack_s *stack)
{
    if (smp_unlikely(stack == NULL)) return NULL;

    if (stack->top != NULL) return (void *) stack->top->data;

    return NULL;
}


void *smp_stack_push(struct smp_stack_s *stack, void *elem_data)
{
    struct smp_stack_node_s *node = NULL;

    if (smp_unlikely(stack == NULL || elem_data == NULL)) return NULL;

    if (smp_stack_filled(stack) == SMP_TRUE) {
        smp_set_error("stack filled");
        return NULL;
    }

    if ((node = smp_stack_node_create(stack)) == NULL) return NULL;
    if (stack->appoint != NULL) stack->appoint((void *) node->data, elem_data);

    if (stack->top == NULL) node->below = stack->bottom;    /* point to NULL */
    else node->below = stack->top;

    stack->top = node;
    stack->curr_size++;

    return (void *) node->data;
}


struct smp_stack_node_s *smp_stack_pop(struct smp_stack_s *stack)
{
    struct smp_stack_node_s *node = NULL;

    if (smp_unlikely(stack == NULL)) return NULL;

    if (smp_stack_empty(stack) == SMP_TRUE) return NULL;

    node = stack->top;
    stack->top = node->below;
    node->below = NULL;
    stack->curr_size--;

    return node;
}


inline void smp_stack_recover(struct smp_stack_s *stack, struct smp_stack_node_s *node)
{
    if (smp_unlikely(stack == NULL || node == NULL)) return;

    node->below = stack->idel;
    stack->idel = node;
}


inline SMP_STATUS smp_stack_destory(struct smp_stack_s *stack)
{
    if (smp_unlikely(stack == NULL)) return SMP_OK;

    /* stack的所有数据结构的内存都是来自于内存池的 */
    if (stack->pool != NULL) return smp_mpdestory((smp_pool_t *) stack->pool);
    return SMP_OK;
}
