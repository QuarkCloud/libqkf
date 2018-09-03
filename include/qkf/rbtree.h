
#ifndef __QKF_RBTREE_H
#define __QKF_RBTREE_H 1

#include "qkf/compile.h"
#include <stddef.h>

__BEGIN_DECLS

#define	RB_RED		0
#define	RB_BLACK	1

typedef struct st_rb_node {
	struct st_rb_node *parent;
	struct st_rb_node *right;
	struct st_rb_node *left;
	int color;
}rb_node_t ;

typedef struct st_rb_tree {
	rb_node_t * root;
    int (*key_compare)(const rb_node_t * src , const rb_node_t * dst) ;     //·µ»ØÖµ=src - dst¡£
} rb_tree_t ;

#define RB_ROOT { NULL , NULL}
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

QKFAPI void rb_insert_color(rb_tree_t * root , rb_node_t * node);
QKFAPI bool rb_insert(rb_tree_t * root, rb_node_t * node);
QKFAPI void rb_erase(rb_tree_t * root , rb_node_t * node);
QKFAPI rb_node_t * rb_find(rb_tree_t * root , const rb_node_t * node);
QKFAPI rb_node_t * rb_lower_bound(rb_tree_t * root , const rb_node_t * node);   //rbnode->key <= key ;
QKFAPI rb_node_t * rb_upper_bound(rb_tree_t * root , const rb_node_t * node);   //rbnode->key >= key ;

/* Find logical next and previous nodes in a tree */
QKFAPI rb_node_t *rb_next(rb_node_t * node);
QKFAPI rb_node_t *rb_prev(rb_node_t * node);
QKFAPI rb_node_t *rb_first(rb_tree_t * root);
QKFAPI rb_node_t *rb_last(rb_tree_t * root);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
QKFAPI void rb_replace_node(rb_tree_t * root , rb_node_t *victim, rb_node_t * new_node );

static inline void rb_link_node(rb_node_t * node, rb_node_t * parent,	rb_node_t ** rb_link)
{
	node->parent = parent;
	node->color = RB_RED;
	node->left = node->right = NULL;

	*rb_link = node;
}

__END_DECLS

#endif /** __QKF_RBTREE_H */
