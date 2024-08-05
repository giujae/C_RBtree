#include "rbtree.h"

#include <stdlib.h>

void left_rotate(rbtree *t, node_t *x);

void right_rotate(rbtree *t, node_t *y);

void insert_fixup(rbtree *t, node_t *z);

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // 1
  if (!p)
  {
    return NULL;
  }

  p->nil = (node_t *)calloc(1, sizeof(node_t));
  if (!p->nil)
  {
    free(p);
    return NULL;
  }

  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t)
{ // 6
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{ // 2
  node_t *z = (node_t *)malloc(sizeof(node_t));

  z->key = key;

  node_t *y = t->nil;
  node_t *x = t->root;

  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }
  z->parent = y;

  if (y == t->nil)
  {
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  insert_fixup(t, z);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{ // 4
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t)
{ // 5
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t)
{ // 5
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p)
{ // 7
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{ // 3
  // TODO: implement to_array
  return 0;
}

void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;
  x->right = y->left;

  if (y->left != t->nil)
  {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->left)
  {
    x->parent->left = y;
  }
  else
  {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *y)
{
  node_t *x = y->left;
  y->left = x->right;

  if (x->right != t->nil)
  {
    x->right->parent = y;
  }

  x->parent = y->parent;

  if (x->parent == t->nil)
  {
    t->root = x;
  }
  else if (y == y->parent->left)
  {
    y->parent->left = x;
  }
  else
  {
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}

void insert_fixup(rbtree *t, node_t *z)
{
  node_t *y = t->nil;
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      y = z->parent->parent->right;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent = RBTREE_RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->right)
      {
        z = z->parent;
        left_rotate(t, z);
      }
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      right_rotate(t, z->parent->parent);
    }
    else
    {
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent = RBTREE_RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->left)
      {
        z = z->parent;
        right_rotate(t, z);
      }
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      left_rotate(t, z->parent->parent);
    }
  }
  t->root->color = RBTREE_BLACK;
}