#include "rbtree.h"

#include <stdlib.h>

static void left_rotate(rbtree *t, node_t *x);

static void right_rotate(rbtree *t, node_t *y);

static void insert_fixup(rbtree *t, node_t *z);

static void inorder_to_array(const rbtree *t, node_t *n, key_t *arr, size_t *index, size_t arr_size);

static node_t *tree_minimum(const rbtree *t, node_t *x);

static void rb_transplant(rbtree *t, node_t *u, node_t *v);

static void rb_delete_fixup(rbtree *t, node_t *x);

static void delete_by_postorder(node_t *root, rbtree *t);

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
  if (t == NULL)
  {
    return;
  }
  delete_by_postorder(t->root, t);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{ // 2
  node_t *z = (node_t *)malloc(sizeof(node_t));

  if (z == NULL)
  {
    return NULL;
  }

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

  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{ // 4
  node_t *current = t->root;

  while (current != t->nil && current->key != key)
  {
    if (key < current->key)
    {
      current = current->left;
    }
    else
    {
      current = current->right;
    }
  }

  return (current != t->nil) ? current : NULL;
}

node_t *rbtree_min(const rbtree *t)
{ // 5
  if (t->root == t->nil)
  {
    return NULL;
  }
  node_t *current = t->root;
  while (current->left != t->nil)
  {
    current = current->left;
  }
  return (current != t->nil) ? current : NULL;
}

node_t *rbtree_max(const rbtree *t)
{ // 5
  if (t->root == t->nil)
  {
    return t->root;
  }
  node_t *current = t->root;
  while (current->right != t->nil)
  {
    current = current->right;
  }
  return (current != t->nil) ? current : NULL;
}

int rbtree_erase(rbtree *t, node_t *p)
{ // 7
  node_t *y = p;
  color_t y_original_color = y->color;
  node_t *x;
  if (p->left == t->nil)
  {
    x = p->right;
    rb_transplant(t, p, p->right);
  }
  else if (p->right == t->nil)
  {
    x = p->left;
    rb_transplant(t, p, p->left);
  }
  else
  {
    y = tree_minimum(t, p->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == p)
    {
      x->parent = y;
    }
    else
    {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (y_original_color == RBTREE_BLACK)
  {
    rb_delete_fixup(t, x);
  }
  free(p);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{ // 3
  if (t == NULL || arr == NULL || n == 0)
  {
    return 0;
  }

  size_t index = 0;
  inorder_to_array(t, t->root, arr, &index, n);

  return index;
}

static void left_rotate(rbtree *t, node_t *x)
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

static void right_rotate(rbtree *t, node_t *y)
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

static void insert_fixup(rbtree *t, node_t *z)
{
  while (z != t->root && z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

static void inorder_to_array(const rbtree *t, node_t *n, key_t *arr, size_t *index, size_t arr_size)
{
  if (n != t->nil)
  {
    inorder_to_array(t, n->left, arr, index, arr_size);
    if (*index < arr_size)
    {
      arr[(*index)++] = n->key;
    }
    inorder_to_array(t, n->right, arr, index, arr_size);
  }
}

static node_t *tree_minimum(const rbtree *t, node_t *x)
{
  while (x->left != t->nil)
  {
    x = x->left;
  }

  return x;
}

static void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
  return;
}

static void rb_delete_fixup(rbtree *t, node_t *x)
{
  node_t *w;
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      w = x->parent->right;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        w->right->color = RBTREE_BLACK;
        w->parent->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      w = x->parent->left;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        w->left->color = RBTREE_BLACK;
        w->parent->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

static void delete_by_postorder(node_t *root, rbtree *t)
{
  if (root == t->nil)
  {
    return;
  }
  delete_by_postorder(root->left, t);
  delete_by_postorder(root->right, t);
  free(root);
}