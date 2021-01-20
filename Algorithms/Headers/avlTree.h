/** Makes available an AVL binary search tree.
 ** important: nodes must have Integer (int) keys.
 **/

// do I need to declare structs? (can I even??)

// this ought not be allowed as static func in source
static void printNode(AVLnode *node);


/* A generic heap node allocator for the AVL tree
 * If creating your own, the required properties are:
 *  - int key
 *  - int height
 *  - parent that is ptr to another node
 *  - left that is ptr to another node
 *  - right that is ptr to another node
 * @param key: integer key of node
 * @return pointer to the created node */
AVLnode* allocateNode(int key);


/* If the nodes in the AVL tree require freeing,
 * this function, if given the root node, will
 * free all the nodes currently in the tree. */
void AVLtreeFreeNodes(AVLnode *root);


