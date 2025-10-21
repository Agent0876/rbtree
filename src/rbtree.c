#include "rbtree.h"
#include <stdlib.h>

/**
 * 새로운 레드블랙트리를 생성하는 함수
 */
rbtree *new_rbtree(void)
{
  // 트리 구조체(rbtree) 동적 할당
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // NIL 노드 생성 (모든 리프의 공통 NIL 포인터)
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  // NIL 노드는 항상 BLACK 색상
  nil->color = RBTREE_BLACK;
  nil->key = 0;
  nil->left = nil;
  nil->right = nil;
  nil->parent = nil;

  // 트리의 NIL 및 ROOT 초기화
  p->nil = nil;
  p->root = p->nil;

  return p;
}

/**
 * 재귀적으로 노드를 해제하는 내부 함수
 */
static void delete_node(rbtree *t, node_t *n)
{
  if (n == t->nil || n == NULL)
    return;

  // 왼쪽, 오른쪽 서브트리 먼저 해제
  delete_node(t, n->left);
  delete_node(t, n->right);

  // 자기 자신 해제
  free(n);
}

/**
 * 트리 전체를 해제하는 함수
 */
void delete_rbtree(rbtree *t)
{
  if (t == NULL)
    return;

  // 루트부터 모든 노드 제거
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

/**
 * 좌회전 함수 (Left Rotate)
 * 트리 균형을 맞추기 위해 사용됨
 */
static inline void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right; // y는 x의 오른쪽 자식
  x->right = y->left;

  if (y->left != t->nil)
    y->left->parent = x;

  y->parent = x->parent;

  // x의 부모와 y를 연결
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  // y의 왼쪽 자식으로 x 연결
  y->left = x;
  x->parent = y;
}

/**
 * 우회전 함수 (Right Rotate)
 */
static inline void right_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left; // y는 x의 왼쪽 자식
  x->left = y->right;

  if (y->right != t->nil)
    y->right->parent = x;

  y->parent = x->parent;

  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;

  y->right = x;
  x->parent = y;
}

/**
 * 삽입 후 레드블랙트리 속성 유지 (Fixup)
 */
static void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  // 부모가 RED인 동안 반복
  while (z->parent->color == RBTREE_RED)
  {
    node_t *y = NULL;

    if (z->parent == z->parent->parent->left)
    {
      // 삼촌 노드 y
      y = z->parent->parent->right;

      if (y->color == RBTREE_RED)
      {
        // Case 1: 부모, 삼촌 RED
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // 할아버지로 이동
      }
      else
      {
        if (z == z->parent->right)
        {
          // Case 2: 삼각형 형태 → 좌회전
          z = z->parent;
          left_rotate(t, z);
        }
        // Case 3: 일자 형태 → 우회전
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      // 반대 방향 (대칭)
      y = z->parent->parent->left;

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

  // 루트는 항상 BLACK
  t->root->color = RBTREE_BLACK;
}

/**
 * 새로운 노드를 트리에 삽입
 */
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  z->color = RBTREE_RED;
  z->left = z->right = t->nil;

  node_t *x = t->root;
  node_t *y = t->nil;

  // 삽입 위치 탐색
  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }

  z->parent = y;

  // 부모가 NIL이면 트리가 비어있음
  if (y == t->nil)
    t->root = z;
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;

  // 삽입 후 트리 균형 복구
  rbtree_insert_fixup(t, z);

  return z;
}

/**
 * 특정 키를 가진 노드 탐색
 */
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *x = t->root;

  while (x != t->nil)
  {
    if (key == x->key)
      return x;
    x = (key < x->key) ? x->left : x->right;
  }

  return NULL;
}

/**
 * 최소값 노드를 반환하는 내부 함수
 */
static node_t *min_node(const rbtree *t, node_t *x)
{
  while (x->left != t->nil)
    x = x->left;
  return x;
}

/**
 * 트리 내 최소값 노드 반환
 */
node_t *rbtree_min(const rbtree *t)
{
  if (t->root == t->nil)
    return NULL;
  return min_node(t, t->root);
}

/**
 * 최대값 노드를 반환하는 내부 함수
 */
static node_t *max_node(const rbtree *t, node_t *x)
{
  while (x->right != t->nil)
    x = x->right;
  return x;
}

/**
 * 트리 내 최대값 노드 반환
 */
node_t *rbtree_max(const rbtree *t)
{
  if (t->root == t->nil)
    return NULL;
  return max_node(t, t->root);
}

/**
 * 서브트리 교체 (삭제 시 사용)
 */
void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

/**
 * 삭제 후 균형 유지 (Fixup)
 */
static void rbtree_erase_fixup(rbtree *t, node_t *x)
{
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right;
      if (w->color == RBTREE_RED)
      {
        // Case 1: 형제가 RED → 부모와 형제 색 교환 후 좌회전
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }

      // Case 2: 형제 자식 모두 BLACK
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // Case 3: 오른쪽 자식이 BLACK
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // Case 4: 오른쪽 자식 RED
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      // 반대 방향 (대칭)
      node_t *w = x->parent->left;
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
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

/**
 * 노드 삭제 함수
 */
int rbtree_erase(rbtree *t, node_t *z)
{
  node_t *y = z;
  color_t y_original_color = y->color;
  node_t *x;

  if (z->left == t->nil)
  {
    // 왼쪽 자식이 없을 때
    x = z->right;
    rbtree_transplant(t, z, z->right);
  }
  else if (z->right == t->nil)
  {
    // 오른쪽 자식이 없을 때
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  else
  {
    // 양쪽 자식이 있을 때: 오른쪽 서브트리의 최소 노드 찾기
    y = min_node(t, z->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == z)
    {
      x->parent = y;
    }
    else
    {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  free(z);

  // 삭제로 인해 트리 균형 깨지면 수정
  if (y_original_color == RBTREE_BLACK)
    rbtree_erase_fixup(t, x);

  return 0;
}

/**
 * 중위순회(Inorder Traversal)로 배열 채우기
 */
static void inorder_fill(const rbtree *t, node_t *x, key_t *arr, size_t n, size_t *idx)
{
  if (x == t->nil || *idx >= n)
    return;

  inorder_fill(t, x->left, arr, n, idx);

  if (*idx < n)
    arr[(*idx)++] = x->key;

  inorder_fill(t, x->right, arr, n, idx);
}

/**
 * 트리의 키들을 오름차순 배열로 변환
 */
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  size_t idx = 0;
  inorder_fill(t, t->root, arr, n, &idx);
  return 0;
}
