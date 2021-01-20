/** Makes available an AVL binary search tree.
 ** important: nodes must have Integer (int) keys.
 **/

#ifndef AVL_TREE_INT
#define AVL_TREE_INT


/* minimal implementation of an AVL tree node */
struct AVLnode {
  int key;
  int height;
  struct AVLnode *left;
  struct AVLnode *right;
  struct AVLnode *parent;
};

typedef struct AVLnode AVLnode;


/* The tree is implemented as a struct with root property.
 * It can be initialized on the stack or as ptr to the heap. */
struct AVLtree {
  struct AVLnode *root;
};

typedef struct AVLtree AVLtree;


/* If the nodes in the AVL tree require freeing,
 * this function, if given the root node, will
 * free all the nodes currently in the tree. */
void AVLtreeFreeNodes(AVLnode *root);


/* Search AVL tree for a node with a particular key.
 * If duplicate keys exist, returns the first found.
 * @param tree: pointer to the AVL tree
 * @param key:  the key to find in the tree
 * @return a pointer to the found node or NULL */
AVLnode* AVLtreeSearch(AVLtree *tree, int key);


/* Returns a pointer to the node with the largest key
 * from the reference point of the parameter input node.
 * @param tree: pointer to the AVL tree
 * @param from: reference point from which to find max key
 * @return pointer to the max node */
AVLnode* AVLtreeMax(AVLtree *tree, AVLnode *from);


/* Returns a pointer to the node with the smallest key
 * from the reference point of the parameter input node.
 * @param tree: pointer to the AVL tree
 * @param from: reference point from which to find min key
 * @return pointer to the min node */
AVLnode* AVLtreeMin(AVLtree *tree, AVLnode *from);


/* Return the node that succeeds the parameter node or NULL
 * if one does not exists in the tree.
 * @param tree: pointer to the AVL tree
 * @param node: the node from which to find a successor
 * @return pointer to the successor node or NULL if none */
AVLnode* AVLtreeSuccessor(AVLtree *tree, AVLnode *node);


/* Return the node that preceeds the parameter node or NULL
 * if one does not exists in the tree.
 * @param tree: pointer to the AVL tree
 * @param node: the node from which to find a predecessor
 * @return pointer to the predecessor node or NULL if none */
AVLnode* AVLtreePredecessor(AVLtree *tree, AVLnode *node);


/* Insert a node into the AVL tree
 * @param tree: pointer to the AVL tree
 * @param node: the node to insert into the tree */
void AVLtreeInsert(AVLtree *tree, AVLnode *node);


/* Delete a node from the AVL tree.
 * If node is not in the tree, the behavior is undefined.
 * @param tree: pointer to the AVL tree
 * @param node: the node to delete from the tree
 * @return returns a pointer to the node that was deleted */
AVLnode* AVLtreeDelete(AVLtree *tree, AVLnode *node);


#endif

