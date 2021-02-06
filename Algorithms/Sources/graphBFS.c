/*  Graphs and Breadth-First Search (BFS)

 Graphs are trees with loops. Nodes, called vertices, can connect to any number of other vertices.
 Unlike trees, there is no natural root nor any natural end to graphs, you can loop infinately.
 The connections are called edges. Graphs can be directed or undirected. In the former, you
 may move only in one direction along an edge, specified by the edge. For example from vertix A -> C.
 (Twitter uses directed graph to represent "followership" [John follows Mary, Mary does not
 neccessarily follow John]). In undirected graph, you may move both directions along an edge.
 (Facebook uses undirected graph to represent "friendship" (if A is friends with B, then B is
 also friends with A)).

 Connected components: set of components (vertices) in which any component can reach any other
  component. In other words, all vertices that can reach each other via edges are part of a
  connected component.

 Degree:
  Undirected graph: degree is the number of vertices that a vertix can call its neighbor.
  Directed   graph: indeg(v) = number of edges that are entering vertix v.
                    outdeg(v) = number of edges taht are leaving vertix v.

 How vertices and edges are represented in code depends on the
 data structure you wish to use to represent the graph. The two most common are:
  1. Adjacenty-lists: array where each element is a pointer to a linked list.
      Array element 0 is vertex zero, 1 is one, etc. The linked list for each index contain all
      of the vertices that are the neighbors of the vertix at that index in the array.
      (so, traversing the linked list will enumerate all of the direct neighbors of that vertix.)
      We generally prefer this data type when the graph is sparse, much less than v^2 edges.
      Both the memory required to store this structure and performance of search is O(V + E).
      Also, because vertices are nodes (structs in implementation) we can easily enhance them to
      fulfill other requirements we may have of our graphs. Principally, adding weights on edges.
  2. Adjacency-matrix: a 2-Dimensional array, matrix[n][n], where n is number of vertices. Each
      element within the matrix is a single bit, 0 or 1, corresponding to whether vertix i has an
      edge with vertix j, in matrix[i][j]. This allows us to very quickly identify if an vertix has
      an edge with another vertix, O(1). But searching the graph requires O(V^2) over square matrix.
      Ofter preferred when the graph is very dense, as we'll save memory. Matrix can also be adapted
      for weighted graphs by replacing bit with float representing edge weight, but then the memory
      requirement to store graph will be no better than the Adjacency list.


 Breadth-First Search (BFS)
  Given an directed or undirected graph and a source vertix, it will "explore" every reachable
  vertix in the graph. It begins on the souce node, it then visits all of the source vertix's
  neighbors and takes note that they have been visited, then proceeds to visit all of their neighbors,
  and so on until no vertix has neighbors that have not been visited.

  The algorithm returns the means to build a breadth-first tree. Either by mutating the vertices'
  sattelite data or by returning a data structure that cointains the necessary information.
  This tree will be rooted at the souce vertix of the BFS function and contain all vertices that
  are reachable from it. The simple path in the tree from the root (source) to any other vertix
  is the shortest possible path in the graph between these two vertices! Careful, the path between
  any two vertices, where neither is the source, is not neccessarily the shortest path between them.

  Running time: Adjacency-list: O(V+E); Adjacency-matrix: O(V^2).
  */


#include <stdio.h>
#include <stdlib.h>

#include "../Headers/hashTables.h"


int main(void)
{



  return EXIT_SUCCESS;
}

