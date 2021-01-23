/*
 * AVL Tree - self balancing binary search Tree
 *
 * All operations on tree: insert, delete, min, max, predecessor, successor, search
 * take O(lg n) time as the tree is balanced.
 * The tree has only one invariant in addition to the binary search tree invariant,
 * that being that a node's left and righ subtree may not have a height difference
 * greater than 1. This means that these trees are very strictly balanced.
 *
 * Benefits: provides very fast lookup times. Used often in database implementations.
 * Costs: strict balancing requires a great number of rotations during insertion/deletion.
 *
 * Each node stores in addition to a key, pointer to parent, left/right children, an
 * integer value for its height in the tree. Nodes at the bottom have a height of 0.
 * Height inreases via the parents. Insertions/deletions may mutate heights of nearby nodes.
 *
 * Comparing to Red-Black trees:
 * AVL provide faster lookups as better balanced; however, slower insertions/deletions.
 * Red-Black trees are often used in language librarires (like C++); and AVL in databases.
 */


#include <stdio.h>
#include <stdlib.h>

#include "../Headers/avlTree.h"



// helper function prints node to stdout for debugging
static void printNode(AVLnode *node)
{
  printf("\nKey: %d\nHeight: %d\nParent: %d\nLeft: %d\tRight: %d\n",
	 node->key, node->height,
	 node->parent == NULL ? -99 : node->parent->key,
	 node->left   == NULL ? -99 : node->left->key,
	 node->right  == NULL ? -99 : node->right->key);
}

static int max(int first, int second)
{
  return first > second ? first : second;
}

// allocates a new node on the heap with input parameter as key
static AVLnode* allocateNode(int key)
{
  AVLnode *new_node = malloc(sizeof(*new_node));
  new_node->key = key;
  new_node->left = new_node->right = NULL;
  new_node->parent = NULL;
  new_node->height = 0;

  return new_node;
}


/*                                */
/*      AVL TREE FUNCTIONS        */
/*                                */

// frees all nodes in tree
void AVLtreeFreeNodes(AVLnode *root)
{
  if (root != NULL) {
    AVLtreeFreeNodes(root->left);
    AVLtreeFreeNodes(root->right);
    free(root);
  }
}

// InOrder traverses and prints to stdout
static void AVLtreeTraversal(AVLnode *root)
{
  if (root != NULL) {
    AVLtreeTraversal(root->left);
    printNode(root);
    AVLtreeTraversal(root->right);
  }
}

/* @param tree: AVL tree in which to insert
 * @param key: the key or value of the node to find
 * @return returns pointer to found node or NULL */
AVLnode* AVLtreeSearch(AVLtree *tree, int key)
{
  AVLnode *curr = tree->root;
  while (curr != NULL && curr->key != key) {
    if (key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;
  }
  return curr;
}

AVLnode* AVLtreeMax(AVLtree *tree, AVLnode *from)
{
  AVLnode *curr = from;
  if (curr == NULL)
    return NULL;

  while (curr->right != NULL) {
    curr = curr->right;
  }
  return curr;
}

AVLnode* AVLtreeMin(AVLtree *tree, AVLnode *from)
{
  AVLnode *curr = from;
  if (curr == NULL)
    return NULL;

  while (curr->left != NULL) {
    curr = curr->left;
  }
  return curr;
}

// returns node that succeeds input node or NULL if none do
AVLnode* AVLtreeSuccessor(AVLtree *tree, AVLnode *node)
{
  if (node->right != NULL)
    return AVLtreeMin(tree, node->right);

  AVLnode *parent = node->parent;
  AVLnode *curr = node;
  while (parent != NULL && curr == curr->parent->right) {
    curr = parent;
    parent = parent->parent;
  }

  return parent;
}

AVLnode* AVLtreePredecessor(AVLtree *tree, AVLnode *node)
{
  if (node->left != NULL)
    return AVLtreeMax(tree, node->left);

  AVLnode *parent = node->parent;
  AVLnode *curr = node;
  while (parent != NULL && curr == curr->parent->left) {
    curr = parent;
    parent = parent->parent;
  }

  return parent;
}

// reminder that left rotate requires right child
static void rotateLeft(AVLtree *tree, AVLnode *node)
{
  AVLnode *R_child = node->right;
  node->right = R_child->left;
  if (node->right != NULL) { node->right->parent = node; }
  R_child->left = node;

  if (node->parent == NULL)
    tree->root = R_child;
  else if (node == node->parent->left)
    node->parent->left = R_child;
  else
    node->parent->right = R_child;

  R_child->parent = node->parent;
  node->parent = R_child;
}

// reminder that right rotate requires left child
static void rotateRight(AVLtree *tree, AVLnode *node)
{
  AVLnode *L_child = node->left;
  node->left = L_child->right;
  if (node->left != NULL) { node->left->parent = node; }
  L_child->right = node;

  if (node->parent == NULL)
    tree->root = L_child;
  else if (node == node->parent->left)
    node->parent->left = L_child;
  else
    node->parent->right = L_child;

  L_child->parent = node->parent;
  node->parent = L_child;
}

// insertions/deletions may violate strong AVL tree invariant:
// a node's subtrees may not have a height difference
// greater than one. If violated, rotate to correct.
static void AVLtreeFixup(AVLtree *tree, AVLnode *node)
{
  AVLnode *curr = node->parent;
  while (curr != NULL) {
    int left_height  = curr->left  ? curr->left->height  : -1;
    int right_height = curr->right ? curr->right->height : -1;
    int height_diff  = abs(left_height - right_height);
    int desired_height = max(left_height, right_height) + 1;

    // if node does not need height changed or local rotation, tree is good
    if (curr->height == desired_height && height_diff <= 1)
      return;
    else
      curr->height = desired_height;

    // subtrees violate AVL tree property, requiring rotation(s) to correct
    if (height_diff > 1) {

      // right subtree is the taller subtree
      if (right_height > left_height) {
	int grandchild_left  = curr->right->left  ? curr->right->left->height  : -1;
	int grandchild_right = curr->right->right ? curr->right->right->height : -1;

	// single rotation sufficient when right grandchild is taller or equal to left
	if (grandchild_right >= grandchild_left) {
	  rotateLeft(tree, curr);
	  curr->height = 1 + max( curr->left  ? curr->left->height  : -1,
				  curr->right ? curr->right->height : -1);
	  curr = curr->parent->parent;

        // double rotation needed when left grandchild is taller than right
	} else {
	  curr->right->height       -= 1;  // account for rotation
	  curr->right->left->height += 1;  // account for rotation
	  rotateRight(tree, curr->right);
	  rotateLeft(tree, curr);
	  curr->height = 1 + max( curr->left  ? curr->left->height  : -1,
				  curr->right ? curr->right->height : -1);
	  curr = curr->parent->parent;
	}

      // left subtree is the taller subtree; mirror above, just flip left/right
      } else {
	int grandchild_left  = curr->left->left  ? curr->left->left->height  : -1;
	int grandchild_right = curr->left->right ? curr->left->right->height : -1;

        // single rotation sufficient when left grandchild is taller or equal to right
	if (grandchild_left >= grandchild_right) {
	  rotateRight(tree, curr);
	  curr->height = 1 + max( curr->left  ? curr->left->height  : -1,
				  curr->right ? curr->right->height : -1);
	  curr = curr->parent->parent;

        // double rotation needed when right grandchild is taller than left
	} else {
	  curr->left->height        -= 1;  // account for rotation
	  curr->left->right->height += 1;  // account for rotation
	  rotateLeft(tree, curr->left);
	  rotateRight(tree, curr);
	  curr->height = 1 + max( curr->left  ? curr->left->height  : -1,
				  curr->right ? curr->right->height : -1);
	  curr = curr->parent->parent;
	}
      }
    } else { curr = curr->parent; }  // executes when curr does not require correction
  }
}

/* Inserting a node into tree mutates tree struct and linked nodes.
 * @param tree: AVL tree in which to insert
 * @param node: an AVLnode to insert into tree */
void AVLtreeInsert(AVLtree *tree, AVLnode *node)
{
  AVLnode *parent = NULL;
  AVLnode *curr = tree->root;

  while (curr != NULL) {
    parent = curr;

    if (node->key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;
  }
  node->parent = parent;

  if (parent == NULL)
    tree->root = node;
  else if (node->key < parent->key)
    parent->left = node;
  else
    parent->right = node;

  AVLtreeFixup(tree, node);
}

/* helper function for delete operation: standard for binary trees
 * @param tree:       AVL tree which we're mutating
 * @param to_replace: non-NULL node that to be removed from its place
 * @param by:         node to take above node's place; may be NULL */
static void transplant(AVLtree *tree, AVLnode *to_replace, AVLnode *by)
{
  if (to_replace->parent == NULL)
    tree->root = by;
  else if (to_replace == to_replace->parent->left)
    to_replace->parent->left = by;
  else
    to_replace->parent->right = by;

  if (by != NULL)
    by->parent = to_replace->parent;
}

/* Deletes given node from the AVL tree
 * @param tree: AVL tree from which to delete
 * @param node: node to delete from tree
 * @return returns a pointer to the node that was deleted */
AVLnode* AVLtreeDelete(AVLtree *tree, AVLnode *node)
{
  // delete node has 0 or 1 children
  if (node->left == NULL) {
    transplant(tree, node, node->right);
    AVLtreeFixup(tree, node);
    return node;
  } else if (node->right == NULL) {
    transplant(tree, node, node->left);
    AVLtreeFixup(tree, node);
    return node;

  // delete node has 2 children - replace node with greatest below it
  } else {
    AVLnode *next = AVLtreeMin(tree, node->right);

    // simple situation where node's next is its right child
    if (next == node->right) {
      transplant(tree, node, next);
      next->left = node->left;
      next->left->parent = next;
      AVLtreeFixup(tree, next->left);
      return node;

    // node's next is deeper down in tree
    } else{
      AVLnode *next_parent = next->parent;  // for fixup at end
      transplant(tree, next, next->right);
      transplant(tree, node, next);
      next->left = node->left;
      next->left->parent = next;
      next->right = node->right;
      next->right->parent = next;

      next_parent->height = 1 + max( next_parent->left  ? next_parent->left->height  : -1,
				     next_parent->right ? next_parent->right->height : -1);
      AVLtreeFixup(tree, next_parent);
      return node;
    }
  }
}
