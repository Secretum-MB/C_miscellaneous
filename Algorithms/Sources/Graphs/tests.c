#include <stdio.h>
#include <stdlib.h>

#include "../../Headers/graph.h"


static
void _print_list_address(graph_vertex *head)
{
  while (head) {
    printf("vertex: %p\n", (void *)head);
    head = head->next;
  }
}

static
void _print_list_id(graph_vertex *head)
{
  while (head) {
    printf("vertex: %d\n", head->id);
    head = head->next;
  }
}

static
void _free_list(graph_vertex *head)
{
  while (head) {
    graph_vertex *tmp = head;
    head = head->next;
    free(tmp);
  }
}


static
void test_topologicalSort()
{
  graph_t *graph = graphBuild(false, false);
  graph_vertex *v1 = graphVertexNew(1, 42);  graph_vertex *v2 = graphVertexNew(2, 42);
  graph_vertex *v3 = graphVertexNew(3, 42);  graph_vertex *v4 = graphVertexNew(4, 42);
  graph_vertex *v5 = graphVertexNew(5, 42);  graph_vertex *v6 = graphVertexNew(6, 42);
  graph_vertex *v7 = graphVertexNew(7, 42);  graph_vertex *v8 = graphVertexNew(8, 42);
  graph_vertex *v9 = graphVertexNew(9, 42);

  graphAddVertex(&graph, v1);  graphAddVertex(&graph, v2); graphAddVertex(&graph, v3);
  graphAddVertex(&graph, v4);  graphAddVertex(&graph, v5); graphAddVertex(&graph, v6);
  graphAddVertex(&graph, v7);  graphAddVertex(&graph, v8); graphAddVertex(&graph, v9);

  graphAddEdgeD(graph, v1, v2);  graphAddEdgeD(graph, v4, v2);  graphAddEdgeD(graph, v4, v5);
  graphAddEdgeD(graph, v5, v6);  graphAddEdgeD(graph, v6, v7);  graphAddEdgeD(graph, v8, v5);
  graphAddEdgeD(graph, v8, v9);  graphAddEdgeD(graph, v9, v7);

  graphPrint(graph);

  graphFree(graph);
}

static
void test_strongly_connected_components()
{
  graph_t *graph = graphBuild(true, true);
  graph_vertex *v1 = graphVertexNew(1, 42);  graph_vertex *v2 = graphVertexNew(2, 42);
  graph_vertex *v3 = graphVertexNew(3, 42);  graph_vertex *v4 = graphVertexNew(4, 42);
  graph_vertex *v5 = graphVertexNew(5, 42);  graph_vertex *v6 = graphVertexNew(6, 42);
  graph_vertex *v7 = graphVertexNew(7, 42);  graph_vertex *v8 = graphVertexNew(8, 42);

  graphAddVertex(&graph, v1);  graphAddVertex(&graph, v2); graphAddVertex(&graph, v3);
  graphAddVertex(&graph, v4);  graphAddVertex(&graph, v5); graphAddVertex(&graph, v6);
  graphAddVertex(&graph, v7);  graphAddVertex(&graph, v8);

  graphAddEdgeD(graph, v1, v2); graphAddEdgeD(graph, v2, v3); graphAddEdgeD(graph, v3, v4);
  graphAddEdgeD(graph, v4, v3); graphAddEdgeD(graph, v2, v5); graphAddEdgeD(graph, v2, v6);
  graphAddEdgeD(graph, v5, v6); graphAddEdgeD(graph, v6, v7); graphAddEdgeD(graph, v7, v6);
  graphAddEdgeD(graph, v3, v7); graphAddEdgeD(graph, v7, v8); graphAddEdgeD(graph, v4, v8);
  graphAddEdgeD(graph, v8, v8); graphAddEdgeD(graph, v5, v1);

  //hashTable *forest = stronglyConnectedComponents(graph);
  //printHashTable(forest);

  printStronglyConnectedComponents(graph);

  //hashTableFree(forest);
  graphFree(graph);
}

static
void test_sigle_source_DAG()
{
  graph_t *graph = graphBuild(false, false);
  graph_vertex *v1 = graphVertexNew(1, 42);  graph_vertex *v2 = graphVertexNew(2, 42);
  graph_vertex *v3 = graphVertexNew(3, 42);  graph_vertex *v4 = graphVertexNew(4, 42);
  graph_vertex *v5 = graphVertexNew(5, 42);  graph_vertex *v6 = graphVertexNew(6, 42);
  graph_vertex *v7 = graphVertexNew(7, 42);  graph_vertex *v8 = graphVertexNew(8, 42);
  graph_vertex *v9 = graphVertexNew(9, 42);

  graphAddVertex(&graph, v1);  graphAddVertex(&graph, v2); graphAddVertex(&graph, v3);
  graphAddVertex(&graph, v4);  graphAddVertex(&graph, v5); graphAddVertex(&graph, v6);
  graphAddVertex(&graph, v7);  graphAddVertex(&graph, v8); graphAddVertex(&graph, v9);

  graphAddEdgeWeightD(graph, v1, v2, 1);  graphAddEdgeWeightD(graph, v4, v2, 3);  graphAddEdgeWeightD(graph, v4, v5, 2);
  graphAddEdgeWeightD(graph, v5, v6, 2);  graphAddEdgeWeightD(graph, v6, v7, 4);  graphAddEdgeWeightD(graph, v8, v5, 1);
  graphAddEdgeWeightD(graph, v8, v9, 1);  graphAddEdgeWeightD(graph, v9, v7, 2);

  //graphPrint(graph);
  hashTable *paths = singleSourceShortestPath_DAG(graph, v8);
  printHashTable(paths);

  singleSourceShortestPath_print(paths, 7);

  hashTableFree(paths);
  graphFree(graph);
}


static
void test_sigle_source_dijkstra()
{
  graph_t *graph = graphBuild(false, false);
  graph_vertex *v0 = graphVertexNew(0, 42);  graph_vertex *v1 = graphVertexNew(1, 42);
  graph_vertex *v2 = graphVertexNew(2, 42);  graph_vertex *v3 = graphVertexNew(3, 42);
  graph_vertex *v4 = graphVertexNew(4, 42);  graph_vertex *v5 = graphVertexNew(5, 42);
  graph_vertex *v6 = graphVertexNew(6, 42);

  graphAddVertex(&graph, v0); graphAddVertex(&graph, v1); graphAddVertex(&graph, v2);
  graphAddVertex(&graph, v3); graphAddVertex(&graph, v4); graphAddVertex(&graph, v5);
  graphAddVertex(&graph, v6);

  graphAddEdgeWeightD(graph, v0, v1, 1); graphAddEdgeWeightD(graph, v0, v2, 2); graphAddEdgeWeightD(graph, v1, v2, 3);
  graphAddEdgeWeightD(graph, v1, v3, 5); graphAddEdgeWeightD(graph, v1, v5, 2); graphAddEdgeWeightD(graph, v2, v1, 1);
  graphAddEdgeWeightD(graph, v2, v4, 1); graphAddEdgeWeightD(graph, v3, v5, 3); graphAddEdgeWeightD(graph, v4, v3, 2);
  graphAddEdgeWeightD(graph, v4, v6, 1); graphAddEdgeWeightD(graph, v5, v4, 1); graphAddEdgeWeightD(graph, v5, v6, 4);

  //graphPrint(graph);
  hashTable *dij = singleSourceShortestPath_dijkstra(graph, v0);
  printHashTable(dij);

  singleSourceShortestPath_print(dij, 6);

  hashTableFree(dij);
  graphFree(graph);
}


int main(void)
{
  test_sigle_source_dijkstra();


  return EXIT_SUCCESS;
}

