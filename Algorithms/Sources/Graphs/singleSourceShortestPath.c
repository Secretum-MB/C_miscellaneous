/*
 Sinlge-Source Shortest Paths
 For unweighted graphs we use Breadth-First Search to calculate the shortest paths from a
 source vertex to any reachable destination vertex. This will not work however if the edges
 are weighted and the shortest paths are therefore required to have the smallest cummulative
 cost, as measured by the sum of the weights of the edges crossed.

 These algorithms work with directed and weighted graphs. Edges that have negative weights
 will often force us to use an algorithm that has a worse asymptotic complexity. Some problems
 may allow you to negate the negative edges without changing the result of the analysis.

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
 to minimize the number of such calls and to have all distances and paths be optimal.

 DAGs - Directed Acyclic Graphs:  O(V + E)
 The easiest graphs to analyze are DAGs. We can also achieve the best running time on these.
 This is achiavable because DAGs do NOT have cycles. We can therefore lay them out in a
 linear manner and relax all outgoing edges, beginning at the start of that ordering and
 continuing until we have relaxed all outgoing edges of all vertices in the graph.
 We flatten the graph, ordering it, using topologial sort.
 Application: analyzing PERT charts (used for project scheduling) - we can easily compute
  minimum time for project completion (or task completion). Find maximal times by negating
  the edges. (Negative edges are okay in DAGs.)

 Dijkstra's Algorithm:  O(E lg V)
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




*/


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "../../Headers/graph.h"
#include "../../Headers/Graphs/hashTables.h"


/*                         */
/*    Heap for Dijkstra    */
/*                         */

// DELETE ME AFTER
static void _print_heap(graph_vertex **heap_array, hashTable *paths, int heap_size)
{
  for (int i = 0; i < heap_size; i++) {
    graph_vertex *vertex = heap_array[i];
    int id = vertex->id;
    int distance = hashTableSearchNode(paths, &id, keyConvertFromInt)->value;
    printf("id: %d, dist: %d\tmem: %p\n", id, distance, (void *)vertex);
  }
}

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
  if (hashTableSearchNode(paths, &dest, keyConvertFromInt)->value == INT_MAX) {
    printf("Destination vertex is not reachable from source.\n"); return;
  }

  int parent = hashTableSearchNode(paths, &dest, keyConvertFromInt)->graph_predecessor;
  if (parent == -1) {
    printf("%d ", dest);
  } else {
    singleSourceShortestPath_print(paths, parent);
    printf("%d ", dest);
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

