/*  Make Graphs available - utilizing Adjacency Lists  */

#ifndef GRAPHS_ADJ_H
#define GRAPHS_ADJ_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "Graphs/hashTables.h"


/* Feel free to modify this struct here as you please.
 * Leave alone the id property as it is used by the graph.
 * Leave alone the next pointer as well.
 * @CAUTION functions below that free memory assumes this
 *          struct can be freed in a single call to free().
 *          Otherwies, further modify below initializer.
 */
typedef struct graph_vertex {
  unsigned int id;
  int value;
  struct graph_vertex *next;
} graph_vertex;


/* This struct represents an edges in the adjacency list.
 * Contains a pointer to the vertex the edge leads to, and
 * the weight of the edge (if applicable).
 * The user is shielded from the need to use this struct,
 * unless you wish to create new functionality which needs
 * to traverse the adjacency list.
 */
typedef struct adjacencyListNode_t {
  graph_vertex *vertex;
  double        weight;
  struct adjacencyListNode_t *next;
} adjacencyListNode_t;


/* This struct represents the graph.
 * Maintains the needed properties to manage memory
 * allocation.
 */
typedef struct graph_t {
  unsigned int  numVertex;
  unsigned int  listSize;
  bool          multiGraph;
  bool          pseudoGraph;
  bool          weighted;
  graph_vertex* vertex_head;
  adjacencyListNode_t* list[];
} graph_t;


/* Sample function to instantiate a vertex for the graph
 * @return pointer to a node on the heap
 */
static
graph_vertex* graphVertexNew(int id, int value)
{
  graph_vertex *new = (graph_vertex*)malloc(sizeof(*new));
  if (!new) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  new->id = id;
  new->value = value;
  new->next = NULL;

  return new;
}


/*                                */
/*            Quick Summary       */
/*            details below       */

graph_t* graphBuild    (bool, bool);
int  graphAddVertex    (graph_t**, graph_vertex*);
void graphRemoveVertexU(graph_t*, graph_vertex*);
void graphRemoveVertexD(graph_t*, graph_vertex*);

/*            Adding Edges        */
void graphAddEdgeU      (graph_t*, graph_vertex*, graph_vertex*);
void graphAddEdgeD      (graph_t*, graph_vertex*, graph_vertex*);
void graphAddEdgeWeightU(graph_t*, graph_vertex*, graph_vertex*, double);
void graphAddEdgeWeightD(graph_t*, graph_vertex*, graph_vertex*, double);

/*            Removing Edges      */
void graphRemoveEdgeU      (graph_t*, graph_vertex*, graph_vertex*);
void graphRemoveEdgeD      (graph_t*, graph_vertex*, graph_vertex*);
void graphRemoveEdgeWeightU(graph_t*, graph_vertex*, graph_vertex*, double);
void graphRemoveEdgeWeightD(graph_t*, graph_vertex*, graph_vertex*, double);

/*            Analysis            */
bool graphExistsVertex   (graph_t*, graph_vertex*);
bool graphExistsEdge     (graph_t*, graph_vertex*, graph_vertex*);
int  graphVertexDegreeU  (graph_t*, graph_vertex*);
int  graphVertexDegreeOut(graph_t*, graph_vertex*);
int  graphVertexDegreeIn (graph_t*, graph_vertex*);
bool vertexReachable     (graph_t*, graph_vertex*, graph_vertex*);

/*            Traversal           */
hashTable* breadthFirstSearch(graph_t*, graph_vertex*);
void graphShortestPathEnum   (hashTable*, int);
void breadthFirstApply       (graph_t*, graph_vertex*,
                              void (*apply)(graph_vertex*, int, void*), void*);
  // example functions to use in place of apply():
  void printVertex     (graph_vertex*, int, void*);
  void scaleVertexValue(graph_vertex*, int, void*);

hashTable*    depthFirstSearch(graph_t*);
graph_vertex* topologicalSort (graph_t*);
int  graphExistsCycle         (graph_t*);
void graphCycleEnum           (graph_t*);

/*            Miscellaneous       */
void graphFree (graph_t*);
void graphPrint(graph_t*);

/*                                */
/*            End of Summary      */
/*                                */


/* Returns a new, empty graph
 * @param true if you want a MultiGraph, otherwise false
 * @param true if MultiGraph is also to be a PseudoGraph
 * @NOTE PseudoGraphs must be MultiGraphs
 */
graph_t* graphBuild(bool, bool);


/* Add a vertex to the graph
 * @param the address of the pointer to the graph
 * @param struct representing the vertex to be added
 * @return if node's id is a duplicate of a vertex already
 *         in graph, function fails with 1; otherwise, 0.
 * @NOTE will grow the memory allocated to graph as needed;
 *       it will take care of all memory considerations.
 */
int graphAddVertex(graph_t**, graph_vertex*);


/* Adds an undirected edge in the graph between two vertices.
 * Requires that both vertices have been added to graph with graphAddVertex.
 * @param the graph in which to add an edge
 * @param struct representing one of the vertices to have an edge added
 * @param struct representing the other vertex to have an edge added
 * @NOTE if graph not MultiGraph, redundant edges not added: no effect
 */
void graphAddEdgeU(graph_t*, graph_vertex*, graph_vertex*);


/* Adds a directed edge in the graph from vertex one to vertex two.
 * Requires that both vertices have been added to graph with graphAddVertex.
 * @param the graph in which to add an edge
 * @param struct representing the source vertex from which the edge leaves
 * @param struct representing the destination vertex, where the edge ends
 * @NOTE if graph not MultiGraph, redundant edges not added: no effect
 */
void graphAddEdgeD(graph_t*, graph_vertex*, graph_vertex*);


/* Adds an undirected, weighted, edge in the graph between the two vertices.
 * Requires that both vertices have been added to graph with graphAddVertex.
 * @param the graph in which to add an edge
 * @param struct representing one of the vertices to have an edge added
 * @param struct representing the other vertex to have an edge added
 * @param double representing the weight of the edge
 * @NOTE if graph not MultiGraph, redundant edges not added: no effect,
 *       (even if weight provided is different from existing edge).
 */
void graphAddEdgeWeightU(graph_t*, graph_vertex*, graph_vertex*, double);


/* Adds an directed, weighted, edge in the graph from vertex one to vertex two.
 * Requires that both vertices have been added to graph with graphAddVertex.
 * @param the graph in which to add an edge
 * @param struct representing the source vertex from which the edge leaves
 * @param struct representing the destination vertex, where the edge ends
 * @param double representing the weight of the edge
 * @NOTE if graph not MultiGraph, redundant edges not added: no effect,
 *       (even if weight provided is different from existing edge).
 */
void graphAddEdgeWeightD(graph_t*, graph_vertex*, graph_vertex*, double);


/* Removes an existing undirected edge between two vertices from the graph
 * @param the graph from which to remove an edge
 * @param struct representing one of the vertices to have an edge removed
 * @param struct representing the other vertex from which to remove edge
 * @NOTE no effect if edge does not exist
 */
void graphRemoveEdgeU(graph_t*, graph_vertex*, graph_vertex*);


/* Removes an existing directed edge, running from vertex one to vertex two
 * @param the graph from which to remove an edge
 * @param the vertex that is the source of the edge to be removed
 * @param the vertex that is the destination of the edge to be removed
 * @NOTE no effect if the edge does not exist
 */
void graphRemoveEdgeD(graph_t*, graph_vertex*, graph_vertex*);


/* Removes an existing undirected, weighted, edge between two vertices
 * @param the graph from which to remove an edge
 * @param struct representing one of the vertices to have an edge removed
 * @param struct representing the other vertex from which to remove edge
 * @param double that matches the weight of the edge to remove
 * @NOTE no effect if edge does not exist
 * @NOTE if graph is Simple, weight is ignored and edge is removed if it exists;
 *       Otherwise, edge is only removed if input weight matches edge's weight.
 */
void graphRemoveEdgeWeightU(graph_t*, graph_vertex*, graph_vertex*, double);


/* Removes an existing directed, weighted, edge from vertex one to two
 * @param the graph from which to remove an edge
 * @param struct representing one of the vertices to have an edge removed
 * @param struct representing the other vertex from which to remove edge
 * @param double that matches the weight of the edge to remove
 * @NOTE no effect if edge does not exist
 * @NOTE if graph is Simple, weight is ignored and edge is removed if it exists;
 *       Otherwise, edge is only removed if input weight matches edge's weight.
 */
void graphRemoveEdgeWeightD(graph_t*, graph_vertex*, graph_vertex*, double);


/* Removes vertex and all undirected edges connected to it
 * @param the graph from which to remove the vertex
 * @param struct representing vertex to be removed
 * @NOTE no effect if vertex not in graph
 * @NOTE caller is responsible for freeing vertex, if desired
 */
void graphRemoveVertexU(graph_t*, graph_vertex*);


/* Removes vertex and all directed edges connected to it
 * @param the graph from which to remove the vertex
 * @param struct representing vertex to be removed
 * @NOTE no effect if vertex not in graph
 * @NOTE caller is responsible for freeing vertex, if desired
 */
void graphRemoveVertexD(graph_t*, graph_vertex*);


/* Determines whether an vertex in a member of a given graph
 * @param the graph to search
 * @param the vertex in which to check for membership
 * @return true if vertex is member, false otherwise
 */
bool graphExistsVertex(graph_t*, graph_vertex*);


/* Determines whether an edge exists from vertex one to two
 * @param the graph where both vertices are members
 * @return true if edge exists, false otherwise
 */
bool graphExistsEdge(graph_t*, graph_vertex*, graph_vertex*);


/* Returns the vertices degree in an undirected graph
 * In undirected graph, this is the number of edges connected to it
 * This function accounts for simple graphs, MultiGraphs, and PseudoGraphs
 * @param the graph where the vertex is a member
 * @param the vertex you wish to know the degree of
 * @return the vertices degree as an integer
 */
int graphVertexDegreeU(graph_t*, graph_vertex*);


/* Returns the vertices Out-Degree in an directed graph
 * This is the number of edges leaving the vertex
 * This function accounts for simple graphs, MultiGraphs, and PseudoGraphs
 * @param the graph where the vertex is a member
 * @param the vertex you wish to know the Out-Degree of
 * @return the vertices Out-Degree as an integer
 */
int graphVertexDegreeOut(graph_t*, graph_vertex*);


/* Returns the vertices In-Degree in an directed graph
 * This is the number of edges entering the vertex
 * This function accounts for simple graphs, MultiGraphs, and PseudoGraphs
 * @param the graph where the vertex is a member
 * @param the vertex you wish to know the In-Degree of
 * @return the vertices In-Degree as an integer
 */
int graphVertexDegreeIn(graph_t*, graph_vertex*);


/* Test whether one vertex is reachable from another
 * @param the graph where the vertices are members
 * @param the vertex that is the source in your query
 * @param the vertex whose reachability you're inquiring about
 * @return true if second vertex reachable from first, otherwise false
 */
bool vertexReachable(graph_t*, graph_vertex*, graph_vertex*);


/* Traverse a graph, visiting all vertices reachable from input vertex
 * @param the graph to be traversed
 * @param the vertex from which to begin the BFS traversal
 * @return a hash table to be used with graphShortestPathEnum to find the shortest
 *   path between this function's input vertex and any other that is reachable
 * NOTE: the returned hash table can be accessed directly - the keys are the
 *   reachable vertices and the values their depth from the BFS origin or root.
 * CAUTION: if accessing the hash table directly: trying to find the depth of
 *   vertices that are not reachable will cause crash - use vertexReachable first.
 */
hashTable* breadthFirstSearch(graph_t*, graph_vertex*);


/* Print to STDOUT the shortest path, via vertex id, between two vertices
 * @param the hash table that is returned from a breadthFirstSearch
 * @param the integer id of the destination vertex
 */
void graphShortestPathEnum(hashTable*, int);


/* Perform a BFS traversal of the graph, calling apply on each visited vertex
 * @param the vertex from which to begin the BFS traversal
 * @param apply function that will be called on each visited vertex
 *   @param the vertex that is currently being visited
 *   @param the dept of the current vertex from the source vertex
 *   @param optional argument the apply function can use. Use NULL if undesired
 *   NOTE: do not give apply arguments when calling breadthFirstApply, they are
 *     provided by the graph and breadthFirstApply (see below).
 * @param this argument takes the place of the last argument in apply. use Null
 *   if undesired
 */
void breadthFirstApply(graph_t*, graph_vertex*, void (*apply)(graph_vertex*, int, void*), void*);


/* Here are two functions that may be used in the place of apply (see above)
 * NOTE: do not provide these functions with arguments
 */

  /* Prints to STDOUT information about the visited vertex
   * NOTE: provide breadhFirstApply with NULL as its final argument
   */
  void printVertex(graph_vertex*, int, void*);

  /* Multiplies each visited vertex's value property by scalar (see below)
   * NOTE: provide breadhFirstApply with the address of an int variable
   */
  void scaleVertexValue(graph_vertex*, int, void*);


/* Traverses graph via depth-first approach, DFS. Starts at arbitrary vertex.
 * @param the graph to be traversed
 * @return a DFS forest of the graph. Hash table's values are the parents of 
 *  the tree edge leading to the keys. Value of -1 indicates forest entry point.
 */
hashTable* depthFirstSearch(graph_t*);


/* Determine the number of cycles that exist in a given graph
 * @param the graph to search for cycles
 * @return number of cycles; 0 if none, and so on.
 */
int graphExistsCycle(graph_t*);


/* print to STDOUT the vertices that make up each cycle in given graph.
 * @param the graph in which to identify cycles
 */
void graphCycleEnum(graph_t*);


/* Perform a topological sort on an directed acyclic graph (DAG)
 * it is required that your graph is a DAG
 * The graph itself is not mutated in any way
 * @param the graph to have the sort performed on
 * @return a pointer to the head of a linked list. The nodes are graph
 *  vertices. The list can be navigated with the structure's next attribute.
 *  The structure's implementation is at the top of this header file.
 *  The linked list's order adheres to a topologial sort of the graph.
 * @NOTE: it is the responsibility of the caller to free the memory allocated
 *        to the linked list that is returned.
 */
graph_vertex* topologicalSort(graph_t*);


/* Frees the memory allocated to the graph and all member vertices
 * @param the graph to be freed.
 */
void graphFree(graph_t*);


/* Prints to STDOUT a convenient textual representation of the
 * Adjacency list and some metadata from the graph.
 * @param the graph to print
 */
void graphPrint(graph_t*);


#endif
