/*  GRAPHS

 -various types and their representation
 -this file implements an undirected graph, utilizing Adjacency Lists

 Graphs are trees with loops. Nodes, called vertices, can connect to any number of other vertices.
 Unlike trees, there is no natural root nor any natural end to graphs, you can loop infinately.
 The connections are called edges. Graphs can be directed or undirected. In the former, you
 may move only in one direction along an edge, specified by the edge. For example from vertex A -> C.
 (Twitter uses directed graph to represent "followership" [John follows Mary, Mary does not
 neccessarily follow John]). In undirected graph, you may move both directions along an edge.
 (Facebook uses undirected graph to represent "friendship" (if A is friends with B, then B is
 also friends with A)).

 MultiGraphs - Vertices may have multiple edges connecting them to another vertex. That is, vertex
  A may have multiple edges connecting it to vertex B.

 PseudoGraphs - MultiGraphs that allow vertices to have edges to themselves. Each such edge counts
  as one edge but add two to the vertex's degree (see below) (assuming undirected graph).

 Connected components: set of components (vertices) in which any component can reach any other
  component. In other words, all vertices that can reach each other via edges are part of a
  connected component. A connected graph is one where every vertex is in a connected component.

 Degree:
  Undirected graph: degree is the number of edges connected to the vertex.
  Directed   graph: indeg(v) = number of edges that are entering vertex v.
                    outdeg(v) = number of edges that are leaving vertex v.

 Regular graph: the degree of every vertex is equal to the degree of every other vertex.
   (i.e. if degree is 4 for all of them, then the graph is 4-Regular)

 Complete graph: graph in which every vertex has edge to every other vertex.

 Directed Acylic graph: a directed graph that does not contain any cycle.

 How vertices and edges are represented in code depends on the
 data structure you wish to use to represent the graph. The two most common are:
  1. Adjacenty-lists: array where each element is a pointer to a linked list.
      Array element 0 is vertex zero, 1 is one, etc. The linked list for each index contain all
      of the vertices that are the neighbors of the vertex at that index in the array.
      (so, traversing the linked list will enumerate all of the direct neighbors of that vertex.)
      We generally prefer this data type when the graph is sparse, much less than v^2 edges.
      Both the memory required to store this structure and performance of search is O(V + E).
      Also, because vertices are nodes (structs in implementation) we can easily enhance them to
      fulfill other requirements we may have of our graphs. Principally, adding weights on edges.
  2. Adjacency-matrix: a 2-Dimensional array, matrix[n][n], where n is number of vertices. Each
      element within the matrix is a single bit, 0 or 1, corresponding to whether vertex i has an
      edge with vertex j, in matrix[i][j]. This allows us to very quickly identify if an vertex has
      an edge with another vertex, O(1). But searching the graph requires O(V^2) over square matrix.
      Ofter preferred when the graph is very dense, as we'll save memory. Matrix can also be adapted
      for weighted graphs by replacing bit with float representing edge weight, but then the memory
      requirement to store graph will be no better than the Adjacency list.

 Implementation notes:
  Seems that it is standard to pass functions that add/remove edges two integers representing vertex ids.
  The functions then encapsulate these integers into new vertices and add them to the adjacency list.
  This results in unique vertices with the same id throughout the adjacency list. So the struct with id 1,
  for example, is NOT the same vertex when it appears in differing places in the adjacency list.
  Drawbacks seem huge: you couldn't update a given vertices satellite data and have it be reflected in
  the graph. Also, if vertex struct needed to be large, you would waste a large amount of memory, compared
  to my approach, see below.

  In order to have an adjacency list with multiple linked lists of the same vertices that point to one another,
  (realize vertex x can't point to y in one spot of adjacency list and point to vertex z in another
  [would require arbitrary number of pointers: equal to number of neighbors the vertex has]),
  I decided to use a intermediary struct, a container if you will, to be the vehicles of the linked lists.
  This container consists of only: a pointer to the vertex it represents; and, a next pointer to the next container.
  My add edge function encapsulates the input vertices into these containers and adds the containers to the
  adjacency list. The memory requirements of this approach are at least as good as the standard implementation.
  Nor will this effect the speed of traversal algorithms.

  Additionaly: my vertex struct still contains a next pointer, and this is used to build a linked list of all vertices
  that are in the graph, the head of which is kept in the graph struct. This allows me to quickly loop over all vertices
  in the graph, useful for, among other things, freeing the memory allocated to the graph.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../Headers/graphUndirected.h"


typedef struct adjacencyListNode_t {
  vertexUndNode *vertex;
  struct adjacencyListNode_t *next;
} adjacencyListNode_t;


struct graphUndAdj_t {
  unsigned int numVertex;
  unsigned int listSize;
  bool multiGraph;
  bool pseudoGraph;
  vertexUndNode *vertex_head;
  adjacencyListNode_t* list[];
};


static
graphUndAdj_t* graphUndAdj_init(size_t arr_size, size_t num_vertex, vertexUndNode *vertex_head,
				bool multiGraph, bool pseudoGraph)
{
  graphUndAdj_t *new = (graphUndAdj_t*)malloc(sizeof(*new) + sizeof(adjacencyListNode_t*) * arr_size);
  if (!new) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }
  new->numVertex = num_vertex;
  new->listSize = arr_size;
  new->multiGraph = multiGraph;
  new->pseudoGraph = pseudoGraph;
  new->vertex_head = vertex_head;
  memset(new->list, 0, sizeof(adjacencyListNode_t*) * arr_size);

  return new;
}


graphUndAdj_t* graphUndAdj_Build(bool multiGraph, bool pseudoGraph)
{
  if (!multiGraph && pseudoGraph) {
    fprintf(stderr, "PseudoGraphs must be MultiGraphs\n");  exit(EXIT_FAILURE);
  }
  return graphUndAdj_init(8, 0, NULL, multiGraph, pseudoGraph);
}


int graphUndAdj_AddVertex(graphUndAdj_t **graph, vertexUndNode *vertex)
{
  vertexUndNode *curr = (*graph)->vertex_head;  // exit with error if duplicate id
  while (curr) {
    if (curr->id == vertex->id) return 1;
    curr = curr->next;
  }
  vertex->next = (*graph)->vertex_head;         // insert node at head of list
  (*graph)->vertex_head = vertex;
  (*graph)->numVertex++;

  // Table-Doubling on adjacency list

  unsigned int numVertex = (*graph)->numVertex;
  vertexUndNode *Ve_head = (*graph)->vertex_head;
  bool multiGraph        = (*graph)->multiGraph;
  bool pseudoGraph       = (*graph)->pseudoGraph;

  while ((*graph)->listSize <= vertex->id) {
    graphUndAdj_t *new_graph = graphUndAdj_init( (*graph)->listSize * 2, numVertex, Ve_head, multiGraph, pseudoGraph);
    memcpy(new_graph->list, (*graph)->list, (*graph)->listSize * sizeof(adjacencyListNode_t*));
    graphUndAdj_t *old = *graph;
    *graph = new_graph;
    free(old);
  }
  return 0;
}


void graphUndAdj_AddEdge(graphUndAdj_t *graph, vertexUndNode *one, vertexUndNode *two)
{
  if (!graphUndAdj_ExistVertex(graph, one) || !graphUndAdj_ExistVertex(graph, two)) {
    fprintf(stderr, "Can't add edge to vertex not in graph\n"); exit(EXIT_FAILURE); }

  if (one == two && !graph->pseudoGraph) {
    fprintf(stderr, "Only PseudoGraphs may have self-referencing or circular edges\n");
    exit(EXIT_FAILURE); }

  if (!graph->multiGraph && graphUndAdj_ExistEdge(graph, one, two)) {
    return; }

  // create adjacency list nodes for graph's adjacency list
  adjacencyListNode_t *for_one = (adjacencyListNode_t*)malloc(sizeof(*for_one));
  adjacencyListNode_t *for_two = (adjacencyListNode_t*)malloc(sizeof(*for_two));
  if (!for_one || !for_two) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }

  for_one->vertex = one;
  for_one->next = graph->list[two->id];
  graph->list[two->id] = for_one;

  for_two->vertex = two;
  for_two->next = graph->list[one->id];
  graph->list[one->id] = for_two;
}


void graphUndAdj_RemoveEdge(graphUndAdj_t *graph, vertexUndNode *one, vertexUndNode *two)
{
  adjacencyListNode_t *prev = NULL;    // loop through linked list of node one
  adjacencyListNode_t *curr = graph->list[one->id];
  while (curr != NULL) {
    if (curr->vertex == two) {
      if (!prev) { graph->list[one->id] = curr->next; }
      else       { prev->next = curr->next; }
      free(curr);
      break;
    }
    prev = curr;
    curr = curr->next;
  }
  prev = NULL;                         // loop through linked list of node two
  curr = graph->list[two->id];
  while (curr != NULL) {
    if (curr->vertex == one) {
      if (!prev) { graph->list[two->id] = curr->next; }
      else       { prev->next = curr->next; }
      free(curr);
      break;
    }
    prev = curr;
    curr = curr->next;
  }
}


void graphUndAdj_RemoveVertex(graphUndAdj_t *graph, vertexUndNode *vertex)
{
  int found_vertex = 0;        // find and eliminate node from vertex list
  vertexUndNode *curr = graph->vertex_head;
  vertexUndNode *prev = NULL;
  while (curr) {
    if (curr == vertex) {
      found_vertex = 1;
      graph->numVertex--;

      if (!prev) { graph->vertex_head = curr->next; }
      else       { prev->next = curr->next; }
      break;
    }
    prev = curr;
    curr = curr->next;
  }
  if (found_vertex) {         // loop over vertices neighbors, remove edges
    u_int adj_slot = vertex->id;
    while (graph->list[adj_slot] != NULL) {
      graphUndAdj_RemoveEdge(graph, vertex, graph->list[adj_slot]->vertex);
    }
  }
}


bool graphUndAdj_ExistVertex(graphUndAdj_t *graph, vertexUndNode *vertex)
{
  vertexUndNode *curr = graph->vertex_head;
  while (curr) {
    if (curr == vertex) { return true; }
    curr = curr->next;
  }
  return false;
}


bool graphUndAdj_ExistEdge(graphUndAdj_t *graph, vertexUndNode *one, vertexUndNode *two)
{
  adjacencyListNode_t *curr = graph->list[one->id];
  while (curr) {
    if (curr->vertex == two) { return true; }
    curr = curr->next;
  }
  return false;
}


int graphUndAdj_Degree(graphUndAdj_t *graph, vertexUndNode *vertex)
{
  int num_neighbors = 0;
  adjacencyListNode_t *curr = graph->list[vertex->id];

  while (curr) {
    num_neighbors++;
    curr = curr->next;
  }
  return num_neighbors;
}


void graphUndAdj_Free(graphUndAdj_t *graph)
{
  // free adjacency list nodes
  adjacencyListNode_t *curr;
  adjacencyListNode_t *temp;
  for (size_t i = 0; i < graph->listSize; i++) {
    if ((curr = graph->list[i])) {
      while (curr) {
	temp = curr;
	curr = curr->next;
	free(temp);
      }
    }
  }
  // free vertices of graph
  vertexUndNode *curr2 = graph->vertex_head;
  vertexUndNode *temp2;
  while (curr2) {
    temp2 = curr2;
    curr2 = curr2->next;
    free(temp2);
  }
  free(graph);
}


void graphUndAdj_Print(graphUndAdj_t *graph)
{
  printf("\nnumVertex: %d\tlistSize: %d\tMultiGraph: %d\tPseudoGraph: %d\n",
	 graph->numVertex, graph->listSize, graph->multiGraph, graph->pseudoGraph);

  adjacencyListNode_t *head;
  for (size_t i = 0; i < graph->listSize; i++) {
    if ( (head = graph->list[i]) == NULL) {
      printf("%ld:-> \\\n", i);
    }
    else {
      printf("%ld:-> ", i);
      while (head) {
	printf("(%d)->", head->vertex->id);
	head = head->next;
      }
      printf("\n");
    }
  }
}

