/*  GRAPHS

 -various types and their representation
 -this file implements, using adjacency lists, graphs that are directed/undirected, weighted/unweighted,
  standard/Multigraphs, and Pseudographs.

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

 Directed Acylic graph: a directed graph that does not contain any cycles.

 Transposing Graphs: the transpose of graph G is G^T and G^T = (V,E^T).
   E^T = { (u,v) : (v,u) in E }
   that is, a transposed graph is another graph with same vertices but with all edges pointing
   the reverse direction. The transpose of an undirected graph is simply itself.
   Many graph algorithms use the graph's transpose in their computations.


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
    Add Vertex/Edge: O(1)
    Remove Vertex:   O(V+E) (worst case if directional- need span all adjacency list for In edges)
    Remove Edge:     O(E)   (worst case all edges are in one linked list)
    Traverse graph:  O(V+E) (with Breadth-First Search.)
    Edge Exists(v):  O(V)   (an vertex can have at most number of vertex neighbors)

  2. Adjacency-matrix: a 2-Dimensional array, matrix[n][n], where n is number of vertices. Each
      element within the matrix is a single bit, 0 or 1, corresponding to whether vertex i has an
      edge with vertex j, in matrix[i][j]. This allows us to very quickly identify if an vertex has
      an edge with another vertex, O(1). But searching the graph requires O(V^2) over square matrix.
      Ofter preferred when the graph is very dense, as we'll save memory. But memory requirement is
      O(V^2). Matrix can also be adapted for weighted graphs by replacing bit with float representing
      edge weight, but then the memory requirement to store graph will be no better than the Adjacency list.
    Add Vertex:      O(V^2) (allocate new 2D array and copy from old into new)
    Add Edge:        O(1)   (update a single index)
    Remove Vertex:   O(V^2) (allocate new 2D array and copy from old into new)
    Remove Edge:     O(1)   (update a single index)
    Traverse graph:  O(V^2) (need to check all positions of square matrix)
    Edge Exists(v):  O(1)   (simply check 2D array at [i][j])

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
  This container consists of: a pointer to the vertex; a next pointer to the next container; and, a property to
  store the weight of the edge it represents.
  My Add-edge function encapsulates the input vertices into these containers and adds the containers to the
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

#include "../../Headers/graph.h"


static
graph_t* _graph_init(size_t arr_size, size_t num_vertex, graph_vertex *vertex_head,
		     bool multiGraph, bool pseudoGraph, bool weighted)
{
  graph_t *new = (graph_t*)malloc(sizeof(*new) + sizeof(adjacencyListNode_t*) * arr_size);
  if (!new) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }
  new->numVertex   = num_vertex;
  new->listSize    = arr_size;
  new->multiGraph  = multiGraph;
  new->pseudoGraph = pseudoGraph;
  new->weighted    = weighted;
  new->vertex_head = vertex_head;
  memset(new->list, 0, sizeof(adjacencyListNode_t*) * arr_size);

  return new;
}


graph_t* graphBuild(bool multiGraph, bool pseudoGraph)
{
  if (!multiGraph && pseudoGraph) {
    fprintf(stderr, "PseudoGraphs must be MultiGraphs\n");  exit(EXIT_FAILURE);
  }
  return _graph_init(8, 0, NULL, multiGraph, pseudoGraph, false);
}


graph_t* graphBuildTranspose(graph_t *graph)
{
  graph_t *transpose = graphBuild(graph->multiGraph, graph->pseudoGraph);

  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    graphAddVertex(&transpose, graphVertexNew(curr->id, curr->value));
    curr = curr->next;
  }

  // add to new graph the transpose of each edge in adjacency list
  for (size_t i = 0; i < graph->listSize; i++) {
    adjacencyListNode_t *edge = graph->list[i];
    graph_vertex *edge_src = NULL;
    graph_vertex *edge_dst = NULL;
    while (edge) {

      // retrieve the source/dest vertix instances from graph's linked list
      int updated = 0;
      curr = transpose->vertex_head;
      while (curr) {
	if (curr->id == i)                { edge_src = curr; updated++; }
	if (curr->id == edge->vertex->id) { edge_dst = curr; updated++; }
	if (updated == 2) break;
	curr = curr->next;
      }
      graphAddEdgeD(transpose, edge_dst, edge_src);
      edge = edge->next;
    }
  }
  return transpose;
}


int graphAddVertex(graph_t **graph, graph_vertex *vertex)
{
  graph_vertex *curr = (*graph)->vertex_head;  // exit with error if duplicate id
  while (curr) {
    if (curr->id == vertex->id) return 1;
    curr = curr->next;
  }
  vertex->next = (*graph)->vertex_head;        // insert node at head of list
  (*graph)->vertex_head = vertex;
  (*graph)->numVertex++;

  // Table-Doubling on adjacency list

  unsigned int numVertex   = (*graph)->numVertex;
  graph_vertex *Ve_head    = (*graph)->vertex_head;
  bool         multiGraph  = (*graph)->multiGraph;
  bool         pseudoGraph = (*graph)->pseudoGraph;
  bool         weighted    = (*graph)->weighted;

  while ((*graph)->listSize <= vertex->id) {
    graph_t *new_graph = _graph_init( (*graph)->listSize * 2, numVertex, Ve_head, multiGraph, pseudoGraph, weighted);
    memcpy(new_graph->list, (*graph)->list, (*graph)->listSize * sizeof(adjacencyListNode_t*));
    graph_t *old = *graph;
    *graph = new_graph;
    free(old);
  }
  return 0;
}


void graphAddEdgeU(graph_t *graph, graph_vertex *one, graph_vertex *two)
{
  if (!graphExistsVertex(graph, one) || !graphExistsVertex(graph, two)) {
    fprintf(stderr, "Can't add edge to vertex not in graph\n"); exit(EXIT_FAILURE); }

  if (one == two && !graph->pseudoGraph) {
    fprintf(stderr, "Only PseudoGraphs may have self-referencing or circular edges\n");
    exit(EXIT_FAILURE); }

  if (!graph->multiGraph && graphExistsEdge(graph, one, two)) {
    return; }

  // create adjacency list nodes for graph's adjacency list
  adjacencyListNode_t *for_one = (adjacencyListNode_t*)malloc(sizeof(*for_one));
  adjacencyListNode_t *for_two = (adjacencyListNode_t*)malloc(sizeof(*for_two));
  if (!for_one || !for_two) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }

  for_one->vertex = one;
  for_one->next = graph->list[two->id];
  for_one->weight = 0;
  graph->list[two->id] = for_one;

  for_two->vertex = two;
  for_two->next = graph->list[one->id];
  for_two->weight = 0;
  graph->list[one->id] = for_two;
}


void graphAddEdgeD(graph_t *graph, graph_vertex *one, graph_vertex *two)
{
  if (!graphExistsVertex(graph, one) || !graphExistsVertex(graph, two)) {
    fprintf(stderr, "Can't add edge to vertex not in graph\n"); exit(EXIT_FAILURE); }

  if (one == two && !graph->pseudoGraph) {
    fprintf(stderr, "Only PseudoGraphs may have self-referencing or circular edges\n");
    exit(EXIT_FAILURE); }

  if (!graph->multiGraph && graphExistsEdge(graph, one, two)) {
    return; }

  // create adjacency list node for graph's adjacency list
  adjacencyListNode_t *edge = (adjacencyListNode_t*)malloc(sizeof(*edge));
  if (!edge) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }

  edge->vertex = two;
  edge->next = graph->list[one->id];
  edge->weight = 0;
  graph->list[one->id] = edge;
}


void graphAddEdgeWeightU(graph_t *graph, graph_vertex *one, graph_vertex *two, int weight)
{
  if (!graphExistsVertex(graph, one) || !graphExistsVertex(graph, two)) {
    fprintf(stderr, "Can't add edge to vertex not in graph\n"); exit(EXIT_FAILURE); }

  if (one == two && !graph->pseudoGraph) {
    fprintf(stderr, "Only PseudoGraphs may have self-referencing or circular edges\n");
    exit(EXIT_FAILURE); }

  if (!graph->multiGraph && graphExistsEdge(graph, one, two)) {
    return; }

  // create adjacency list nodes for graph's adjacency list
  adjacencyListNode_t *for_one = (adjacencyListNode_t*)malloc(sizeof(*for_one));
  adjacencyListNode_t *for_two = (adjacencyListNode_t*)malloc(sizeof(*for_two));
  if (!for_one || !for_two) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }

  for_one->vertex = one;
  for_one->next = graph->list[two->id];
  for_one->weight = weight;
  graph->list[two->id] = for_one;

  for_two->vertex = two;
  for_two->next = graph->list[one->id];
  for_two->weight = weight;
  graph->list[one->id] = for_two;

  graph->weighted = true;
}


void graphAddEdgeWeightD(graph_t *graph, graph_vertex *one, graph_vertex *two, int weight)
{
  if (!graphExistsVertex(graph, one) || !graphExistsVertex(graph, two)) {
    fprintf(stderr, "Can't add edge to vertex not in graph\n"); exit(EXIT_FAILURE); }

  if (one == two && !graph->pseudoGraph) {
    fprintf(stderr, "Only PseudoGraphs may have self-referencing or circular edges\n");
    exit(EXIT_FAILURE); }

  if (!graph->multiGraph && graphExistsEdge(graph, one, two)) {
    return; }

  // create adjacency list node for graph's adjacency list
  adjacencyListNode_t *edge = (adjacencyListNode_t*)malloc(sizeof(*edge));
  if (!edge) {
    perror("malloc");  fprintf(stderr, "failed to allocate memory\n");  exit(EXIT_FAILURE);
  }

  edge->vertex = two;
  edge->next = graph->list[one->id];
  edge->weight = weight;
  graph->list[one->id] = edge;

  graph->weighted = true;
}


void graphRemoveEdgeU(graph_t *graph, graph_vertex *one, graph_vertex *two)
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


void graphRemoveEdgeD(graph_t *graph, graph_vertex *one, graph_vertex *two)
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
}


void graphRemoveEdgeWeightU(graph_t *graph, graph_vertex *one, graph_vertex *two, int weight)
{
  adjacencyListNode_t *prev = NULL;    // loop through linked list of node one
  adjacencyListNode_t *curr = graph->list[one->id];
  while (curr != NULL) {
    if ( (curr->vertex == two && !graph->multiGraph) || (curr->vertex == two && curr->weight == weight)) {
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
    if ( (curr->vertex == one && !graph->multiGraph) || (curr->vertex == one && curr->weight == weight)) {
      if (!prev) { graph->list[two->id] = curr->next; }
      else       { prev->next = curr->next; }
      free(curr);
      break;
    }
    prev = curr;
    curr = curr->next;
  }
}


void graphRemoveEdgeWeightD(graph_t *graph, graph_vertex *one, graph_vertex *two, int weight)
{
  adjacencyListNode_t *prev = NULL;    // loop through linked list of node one
  adjacencyListNode_t *curr = graph->list[one->id];
  while (curr != NULL) {
    if ( (curr->vertex == two && !graph->multiGraph) || (curr->vertex == two && curr->weight == weight)) {
      if (!prev) { graph->list[one->id] = curr->next; }
      else       { prev->next = curr->next; }
      free(curr);
      break;
    }
    prev = curr;
    curr = curr->next;
  }
}


void graphRemoveVertexU(graph_t *graph, graph_vertex *vertex)
{
  int found_vertex = 0;       // find and eliminate node from vertex list
  graph_vertex *curr = graph->vertex_head;
  graph_vertex *prev = NULL;
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
      graphRemoveEdgeU(graph, vertex, graph->list[adj_slot]->vertex);
    }
  }
}


void graphRemoveVertexD(graph_t *graph, graph_vertex *vertex)
{
  bool found_vertex = false;       // find and eliminate node from vertex list
  graph_vertex *curr = graph->vertex_head;
  graph_vertex *prev = NULL;
  while (curr) {
    if (curr == vertex) {
      found_vertex = true;
      graph->numVertex--;

      if (!prev) { graph->vertex_head = curr->next; }
      else       { prev->next = curr->next; }
      break;
    }
    prev = curr;
    curr = curr->next;
  }
  if (found_vertex) {              // loop over edges, removing them
    adjacencyListNode_t *curr;
    adjacencyListNode_t *prev;
    for (size_t i = 0; i < graph->listSize; i++) {
      curr = graph->list[i];
      prev = NULL;

      while (curr) {
	if (i == vertex->id) {    // free all out edges from vertex
	  graph->list[i] = NULL;
	  prev = curr;
	  curr = curr->next;
	  free(prev);
	} else {                  // free all in edges to vertex
	  if (curr->vertex == vertex) {
	    if (!prev) {
	      graph->list[i] = curr->next;
	      prev = curr;
	      curr = curr->next;
	      free(prev);
	    } else {
	      prev->next = curr->next;
	      prev = curr;
	      curr = curr->next;
	      free(prev);
	    }
	  } else { prev = curr; curr = curr->next; }
	}
      }
    }
  }
}


bool graphExistsVertex(graph_t *graph, graph_vertex *vertex)
{
  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    if (curr == vertex) { return true; }
    curr = curr->next;
  }
  return false;
}


bool graphExistsEdge(graph_t *graph, graph_vertex *one, graph_vertex *two)
{
  adjacencyListNode_t *curr = graph->list[one->id];
  while (curr) {
    if (curr->vertex == two) { return true; }
    curr = curr->next;
  }
  return false;
}


int graphVertexDegreeU(graph_t *graph, graph_vertex *vertex)
{
  int num_neighbors = 0;
  adjacencyListNode_t *curr = graph->list[vertex->id];

  while (curr) {
    num_neighbors++;
    curr = curr->next;
  }
  return num_neighbors;
}


int graphVertexDegreeOut(graph_t *graph, graph_vertex *vertex)
{
  return graphVertexDegreeU(graph, vertex);
}


int graphVertexDegreeIn(graph_t *graph, graph_vertex *vertex)
{
  int in_deg = 0;
  adjacencyListNode_t *curr;

  for (size_t i = 0; i < graph->listSize; i++) {
    if (i == vertex->id && !graph->pseudoGraph) continue;
    curr = graph->list[i];

    while (curr) {
      if (curr->vertex == vertex) { in_deg++; }
      curr = curr->next;
    }
  }
  return in_deg;
}


void graphFree(graph_t *graph)
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
  graph_vertex *curr2 = graph->vertex_head;
  graph_vertex *temp2;
  while (curr2) {
    temp2 = curr2;
    curr2 = curr2->next;
    free(temp2);
  }
  free(graph);
}


void graphPrint(graph_t *graph)
{
  printf("\nnumVertex: %d\tlistSize: %d\tMultiGraph: %d\tPseudoGraph: %d\n",
	 graph->numVertex, graph->listSize, graph->multiGraph, graph->pseudoGraph);

  adjacencyListNode_t *curr;
  for (size_t i = 0; i < graph->listSize; i++) {
    if ( (curr = graph->list[i]) == NULL) {
      printf("%ld:-> \\\n", i);
    }
    else {
      printf("%ld:-> ", i);
      if (!graph->weighted) {   // graph is not weighted
	while (curr) {
	  printf("(%d)->", curr->vertex->id);
	  curr = curr->next;
	}
      } else {                  // graph is weighted
	while (curr) {
	  printf("(%d: %d)->", curr->vertex->id, curr->weight);
	  curr = curr->next;
	}
      }
      printf("\n");
    }
  }
}

