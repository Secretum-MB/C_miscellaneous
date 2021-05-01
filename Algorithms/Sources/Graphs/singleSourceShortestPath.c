/*
 Sinlge-Source Shortest Paths
 For unweighted graphs we use Breadth-First Search to calculate the shortest paths from a
 source vertex to any reachable destination vertex. This will not work however if the edges
 are weighted and the shortest paths are therefore required to have the smallest cummulative
 cost, as measured by the sum of the weights of the edges crossed.

 These algorithms work with directed and weighted graphs. Edges that have negative weights
 will often force us to use an algorithm that has a worse asymptotic complexity. Some problems
 may allow you to negate the negative edges without changing the result of the analysis.

 An related problem is finding the longest paths. This can be done by negating the edges and
 determining the shortest paths, which now correspond to the longest paths. This works great
 in DAGs (see below). However, what if this negation causes the creation of negative cycles?
 None of the algorithms below will compute best paths in that case; in fact, no polynomial
 time algorithms exist for computing the "simple" shortest paths where negative cycles exist.
 (simple means that no cycles are taken in path). We are left only with exponential solutions.
 This problem is considered NP-hard (no polynomial solutions believed to exist).

 Realize that solving this problem by brute force would require generating all permutations
 of the paths between the source and all destinations and then picking the best one. This
 would be incredibly slow as the number of permutations would be massive.

 Optimal Substructure:
 An important logical truth that the algorithms take advantage of in solving these problems
 efficiently. Sub-paths of shortest paths are themselves shortest paths. Or, all vertices
 between the source and the shortest path destination are themselves reached in the shortest
 manner by that very path, stopping of course on the vertices in question.
 Proof: if there had been a shorter path from the source to the end of the sub-path, that
 alternative path would have been taken in the overall shortest path.

 Cycles:
 Shortest paths do not contain cycles. Cycles have a net weight that is either positive,
 neutral, or negative. If positive, they would add to the overall distance, therefore not
 be included. If neutral they would be excluded so as to give the simplest path. Cycles
 that are net negatively weighted could be take again and again until the overall cost of
 the path becomes negative infinity. This is not desirable, so such paths are undefined.

 Relaxation:
 The central operation of these algorithms for arriving at the best paths.
 (at the beginning: we initialize these algorithms by setting all vertices' distances to
 infinity except the source, set to 0. In the implementation: INT_MAX is used for infinity).
 Relaxation is an operation performed on an edge, particularly an vertex's outgoing edge to
 another vertex. It checks whether the destination vertex's distance is greater than the
 source vertex's distance plus the weight between the two along their edge. If the later
 is less, then it makes sense to update the destination vertex's distance to the sources'
 plus the weight on the connecting edge. We have thereby improved the distance to that
 vertex. Also updated at this time is the destination vertex's predecessor - which is now
 the source vertex. Source here refers to the source of the outgoing edge being relaxed, not
 the single-source of the algorithm.
 The game-plan then for these algorithms is to keep calling this function in such a way as
 to minimize the number of such calls and to have all distances and paths be optimal at end.

 DAGs - Directed Acyclic Graphs:  O(V + E)
 The easiest graphs to analyze are DAGs. We can also achieve the best running time on these.
 This is achiavable because DAGs do NOT have cycles. We can therefore lay them out in a
 linear manner and relax all outgoing edges, beginning at the start of that ordering and
 continuing until we have relaxed all outgoing edges of all vertices in the graph.
 We flatten the graph, ordering it, using topologial sort.
 Application: analyzing PERT charts (used for project scheduling) - we can easily compute
  minimum time for project completion (or task completion). Find maximal times by negating
  the edges. (Negative edges are okay in DAGs.)

 Dijkstra's Algorithm:  O(E lg V)  [slightly better performance possible with fibonacci heap]
 If your directed and weighted graph has cycles, this is likely your fastest algorithm.
 It requires that all your edges have NON-NEGATIVE weights.
 This is a "greedy" algorithm and it requires the use of a priority queue.
 The running time above is achieved when we implement the priority queue with a binary heap.
 If instead an array is used the running time becomes O(V^2).
 The heap is a min-heap and its keys are the vertice's distances. The greedy algorithm then
 extracts from the heap (initially containing all vertices) the min vertex, loops through
 all its outgoing edges, relaxing them. Then it extracts the new minimum from the heap, and
 repeats until the heap is empty.
 Implementation detail: make sure that if an relaxation event occured that you maintain the
  heap invariant. This can be done with a modified decrease_key function. Unlike its standard
  implementation, it needs not update the key (done by relax), just needs to consider calling
  heapify from the position of the updated vertex. The vertex struct is updated to contain a
  property that carries the vertex's heap position at all times.

 Bellman-Ford:  O(VE)
 Solves the single-source problem in the general case, where there may be edges with negative
 weights and negatively weighted cycles.
 The algorithm has two parts. The first loops over every edge relaxing each. This is done as
 many times as there are vertices in the graph (minus 1 time). This is the cause of O(VE).
 If there are no negative cycles in the graph between the source and any of its reachable
 destinations, this process will find the shortest paths for all destinations. The proof goes
 like this: imagine a shortest path, v0,v1,v2,...vk. Among the edges we relax in our first
 pass through every edge is the edge between v0 and v1, which now creates that shortest path.
 On the second pass over every edge we must relax the edge between v1 and v2 (v1 now having
 the optimal distance from last pass), we find its opimal distance. After k-1 passes over
 every edge all edges in the shortest path have been optimized.
 Part two of the algorithm identifies whether there are any negative cycles effecting any of
 the destinations identified above. It does it by looping over all the edges one additional
 time looking for whether a relaxation event could be/or is triggered. If a relaxation event
 is possible then only a negative cycle can be the cause - part 1 finds all optimal paths
 otherwise.
 In the cases where part 2 above identifies a negative cycle, the algorithm returns false and
 no distances or paths are returned. In this implementation the hash table returned is empty.

 Speeding up Dijkstra:
 Besides using a Fibonacci heap rather than a binary one, there are other means of getting
 practical (not asymptotic) speed improvements.
 One:
 If we wish to find the distance and path to a particular target vertex, t, we can run
 dijkstra as before but terminate the moment we extract t from the min-heap. At that point
 we will already have the optimum distance and path to t.
 Two:
 An even better improvement, if we want path, again to a particular target, t, is to do a
 bi-directional search. You basically do two dijkstra searches, one forward from source, the
 other backward from target. In each, you extract min once and relax its edges. After doing
 one, you do one from the other side. You will need two min-heaps, two collections for
 distances, etc. You keep going until a single vertex, w, has been removed from both heaps.
 Then the shortest path is that from source to w plus the best from target to w.
 Warning: its possible that w is not on the shortest path; a simple correction is used for
   for this, it involves a final check that a better middle vertex does not exist.
 Three:
 Using Potential Functions to modify the weight along the edges. Important that what was the
 shortest path to all destinations remains the same after applying the function, you merely
 want to shift the weights around. If this is done correctly, Dijkstra will be guided, as it
 were, towards the target (assuming function captures real knowledge or good guesswork of
 what movements would be more profitable). Algorithm could then terminate as soon as target
 is found. Related to this is the use of "landmarks", vertices that the path must go through.
 You know you must go through landmark so algorithm can find fastest way from source to it,
 and from destination to landmark. Example: traveling from Maine to Huston, landmark being
 some place in the middle of the US that any good path must go through.
*/


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "../../Headers/graph.h"
#include "../../Headers/Graphs/hashTables.h"


/*                         */
/*    Heap for Dijkstra    */
/*                         */

static
int _parent_pos(int vertex)
{
  return (vertex - 1) / 2;
}

static
int _left_pos(int vertex)
{
  return (vertex * 2) + 1;
}

static
int _right_pos(int vertex)
{
  return (vertex * 2) + 2;
}

static
void _heapify(graph_vertex **heap_array, hashTable *paths, int heap_size, int heap_pos)
{
  int l = _left_pos(heap_pos);
  int r = _right_pos(heap_pos);
  int smallest = heap_pos;

  int heap_pos_dist = hashTableSearchNode(paths, &heap_array[heap_pos]->id, keyConvertFromInt)->value;
  if (l < heap_size) {
    int l_dist = hashTableSearchNode(paths, &heap_array[l]->id, keyConvertFromInt)->value;
    if (l_dist < heap_pos_dist) { smallest = l; }
  }
  int smallest_dist = hashTableSearchNode(paths, &heap_array[smallest]->id, keyConvertFromInt)->value;
  if (r < heap_size) {
    int r_dist = hashTableSearchNode(paths, &heap_array[r]->id, keyConvertFromInt)->value;
    if (r_dist < smallest_dist) { smallest = r; }
  }

  if (smallest != heap_pos) {
    // swap vertices position in heap
    graph_vertex *tmp = heap_array[heap_pos];
    heap_array[heap_pos] = heap_array[smallest];
    heap_array[smallest] = tmp;

    // swap vertices heap position struct property
    int tmp1 = heap_array[heap_pos]->heap_pos;
    heap_array[heap_pos]->heap_pos = heap_array[smallest]->heap_pos;
    heap_array[smallest]->heap_pos = tmp1;

    _heapify(heap_array, paths, heap_size, smallest);
  }
}

static
void _heap_build(graph_t *graph, hashTable *paths, graph_vertex **heap_array)
{
  // place each vertex in heap
  graph_vertex *curr = graph->vertex_head;
  u_int i = 0;
  while (curr) {
    heap_array[i] = curr;
    curr->heap_pos = i;
    curr = curr->next; i++;
  }

  // call heapify on top half (reverse order) of heap to establish invariant
  for (int j = graph->numVertex / 2; j >= 0; j--) {
    _heapify(heap_array, paths, graph->numVertex, j);
  }
}

static
graph_vertex* _heap_extract_min(graph_vertex **heap_array, hashTable *paths, int heap_size)
{
  graph_vertex *min = heap_array[0];
  heap_array[0] = heap_array[heap_size - 1];
  heap_array[0]->heap_pos = 0;
  _heapify(heap_array, paths, heap_size - 1, 0);
  return min;
}

static
void _heap_decrease_key(graph_vertex **heap_array, hashTable *paths, int heap_size, int heap_pos)
{
  int vertex      = heap_array[heap_pos]->id;
  int curr_dist   = hashTableSearchNode(paths, &vertex, keyConvertFromInt)->value;
  int parent      = heap_array[_parent_pos(heap_pos)]->id;
  int parent_dist = hashTableSearchNode(paths, &parent, keyConvertFromInt)->value;

  while (parent_dist > curr_dist) {
    _heapify(heap_array, paths, heap_size, _parent_pos(heap_pos));
    heap_pos = _parent_pos(heap_pos);

    vertex      = heap_array[heap_pos]->id;
    curr_dist   = hashTableSearchNode(paths, &vertex, keyConvertFromInt)->value;
    parent      = heap_array[_parent_pos(heap_pos)]->id;
    parent_dist = hashTableSearchNode(paths, &parent, keyConvertFromInt)->value;
  }
}


/*                         */
/*    helper functions     */
/*                         */

static
void _free_linked_list(graph_vertex *head)
{
  while (head) {
    graph_vertex *tmp = head;
    head = head->next;
    free(tmp);
  }
}

static
hashTable* _single_source_initialize(graph_t *graph, graph_vertex *source)
{
  hashTable *forest = hashTableBuild();

  graph_vertex *curr = graph->vertex_head;
  while (curr) {
    hashTableInsertNode(&forest, nodeHashTable_int(curr->id, INT_MAX, -1));
    curr = curr->next;
  }
  hashTableInsertNode(&forest, nodeHashTable_int(source->id, 0, -1));

  return forest;
}

static
bool _relax_edge(int alt_path, adjacencyListNode_t *edge, hashTable **forest)
{
  int cur_path_dist = hashTableSearchNode( (*forest), &edge->vertex->id, keyConvertFromInt)->value;
  int alt_path_dist = hashTableSearchNode( (*forest), &alt_path, keyConvertFromInt)->value;

  if (alt_path_dist != INT_MAX) {
    if (cur_path_dist > alt_path_dist + edge->weight) {
      hashTableInsertNode( &(*forest), nodeHashTable_int(edge->vertex->id, alt_path_dist + edge->weight, alt_path));
      return true;
    }
  }
  return false;
}

void singleSourceShortestPath_print(hashTable *paths, int dest)
{
  if (hashTableIsEmpty(paths)) {
    printf("A negative weighted cycle exists in the graph.");
    printf(" Distances are undefined!\n");
  }
  else if (hashTableSearchNode(paths, &dest, keyConvertFromInt)->value == INT_MAX) {
    printf("Destination vertex is not reachable from source.\n");
  }
  else {
    int parent = hashTableSearchNode(paths, &dest, keyConvertFromInt)->graph_predecessor;
    if (parent == -1) {
      printf("%d ", dest);
    } else {
      singleSourceShortestPath_print(paths, parent);
      printf("%d ", dest);
    }
  }
}


/*                      */
/*     DAGs             */
/*                      */

hashTable* singleSourceShortestPath_DAG(graph_t *graph, graph_vertex *source)
{
  hashTable *paths = _single_source_initialize(graph, source);
  graph_vertex *sorted_vertex = topologicalSort(graph);

  graph_vertex *vertex = sorted_vertex;
  while (vertex) {

    // relax all outgoing edges
    adjacencyListNode_t *edge = graph->list[vertex->id];
    while (edge) {
      _relax_edge(vertex->id, edge, &paths);
      edge = edge->next;
    }
    vertex = vertex->next;
  }

  _free_linked_list(sorted_vertex);
  return paths;
}


/*                      */
/*     Dijkstra         */
/*                      */

hashTable* singleSourceShortestPath_dijkstra(graph_t *graph, graph_vertex *source)
{
  hashTable *paths = _single_source_initialize(graph, source);
  int heap_size = graph->numVertex;
  graph_vertex *heap[heap_size];
  _heap_build(graph, paths, heap);

  while (heap_size > 0) {
    graph_vertex *vertex = _heap_extract_min(heap, paths, heap_size);
    heap_size--;

    adjacencyListNode_t *edge = graph->list[vertex->id];
    while (edge) {
      bool relaxed = _relax_edge(vertex->id, edge, &paths);
      if (relaxed) {
	_heap_decrease_key(heap, paths, heap_size, edge->vertex->heap_pos);
      }
      edge = edge->next;
    }
  }
  return paths;
}


/*                      */
/*     Bellman-Ford     */
/*                      */

hashTable* singleSourceShortestPath_bellmanFord(graph_t *graph, graph_vertex *source)
{
  hashTable *paths = _single_source_initialize(graph, source);

  // relax all edges in graph as many times as there are vertices in graph (minus 1)
  for (size_t i = 0; i < graph->numVertex - 1; i++) {
    graph_vertex *vertex = graph->vertex_head;
    while (vertex) {
      adjacencyListNode_t *edge = graph->list[vertex->id];
      while (edge) {
	_relax_edge(vertex->id, edge, &paths);
	edge = edge->next;
      }
      vertex = vertex->next;
    }
  }
  // identify negative cycles in graph by relaxing all edges again
  graph_vertex *vertex = graph->vertex_head;
  while (vertex) {
    adjacencyListNode_t *edge = graph->list[vertex->id];
    while (edge) {
      bool relaxed = _relax_edge(vertex->id, edge, &paths);
      if (relaxed) {
	hashTableEmpty(&paths);
	return paths;
      }
      edge = edge->next;
    }
    vertex = vertex->next;
  }
  return paths;
}

