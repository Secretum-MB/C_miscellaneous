/*  Make available Undirected Graphs - utilizing Adjacency Lists  */

#ifndef GRAPH_UNDIRECTED_ADJ_H
#define GRAPH_UNDIRECTED_ADJ_H


/* Feel free to modify this struct here as you please.
 * Leave alone the id property as it is used by the graph.
 * Leave alone the next pointer as well.
 * @CAUTION functions below that free memory assumes this
 *          struct can be freed in a single call to free().
 *          Otherwies, further modify below initializer.
 */
typedef struct vertexUndNode {
  unsigned int id;
  int value;
  struct vertexUndNode *next;
} vertexUndNode;


/* This struct represents the undirected graph.
 * Maintains the needed properties to manage memory
 * allocation. The user is shielded from its use by
 * the implementation.
 */
struct graphUndAdj_t;
typedef struct graphUndAdj_t graphUndAdj_t;


/* Sample function to instantiate a node for the graph
 * @return pointer to a node on the heap
 */
static
vertexUndNode* vertexUndNew(int id, int value)
{
  vertexUndNode *new = (vertexUndNode*)malloc(sizeof(*new));
  if (!new) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  new->id = id;
  new->value = value;
  new->next = NULL;

  return new;
}


/* Returns a new, empty graph
 * @param true if you want a MultiGraph, otherwise false
 * @param true if MultiGraph is to also be a PseudoGraph
 * @NOTE PseudoGraphs must be MultiGraphs
 */
graphUndAdj_t* graphUndAdj_Build(bool, bool);


/* Add a vertex to the graph
 * @param the address of the pointer to the graph
 * @param struct representing the vertex to be added
 * @return if node's id is a duplicate of a vertex already
 *         in graph, function fails with 1; otherwise, 0.
 * @NOTE will grow the memory allocated to graph as needed;
 *       it will take care of all memory considerations.
 */
int graphUndAdj_AddVertex(graphUndAdj_t**, vertexUndNode*);


/* Adds an edge in the graph between two vertices.
 * Requires that both vertices have been added to graph with graphUndAdj_AddVertex.
 * @param the graph in which to add an edge
 * @param struct representing one of the vertices to have an edge added
 * @param struct representing the other vertex to have an edge added
 * @NOTE if graph not MultiGraph, redundant edges not added: no effect
 */
void graphUndAdj_AddEdge(graphUndAdj_t*, vertexUndNode*, vertexUndNode*);


/* Removes an existing edge between two vertices from the graph
 * @param the graph from which to remove an edge
 * @param struct representing one of the vertices to have an edge removed
 * @param struct representing the other vertex from which to remove edge
 * @NOTE no effect if edge or vertices do not exist in graph
 */
void graphUndAdj_RemoveEdge(graphUndAdj_t*, vertexUndNode*, vertexUndNode*);


/* Removes vertex and all of its edges from the graph
 * @param the graph from which to remove the vertex
 * @param struct representing vertex to be removed
 * @NOTE no effect if vertex not in graph
 * @NOTE caller is responsible for freeing vertex, if desired
 */
void graphUndAdj_RemoveVertex(graphUndAdj_t*, vertexUndNode*);


/* Determines whether an vertex in a member of a given graph
 * @param the graph to search
 * @param the vertex in which to check for membership
 * @return true if vertex is member, false otherwise
 */
bool graphUndAdj_ExistVertex(graphUndAdj_t*, vertexUndNode*);


/* Determines whether an edge exists between two vertices
 * @param the graph where both vertices are members
 * @return true if edge exists, false otherwise
 */
bool graphUndAdj_ExistEdge(graphUndAdj_t*, vertexUndNode*, vertexUndNode*);


/* Returns the vertices degree.
 * In undirected graph, this is the number of edges connected to it
 * This funtion accounts for plain graphs, MultiGraphs, and PseudoGraphs
 * @param the graph where the vertex is a member
 * @param the vertex you wish to know the degree of
 * @return the vertices degree as an integer
 */
int graphUndAdj_Degree(graphUndAdj_t*, vertexUndNode*);


/* Frees the memory allocated to the graph and all member vertices
 * @param the graph to be freed.
 */
void graphUndAdj_Free(graphUndAdj_t*);


/* Prints to STDOUT a convenient textual representation of the
 * Adjacency list and some metadata from the graph.
 * @param the graph to print
 */
void graphUndAdj_Print(graphUndAdj_t*);


#endif

