/*  Breadth-First Search (BFS)

  Given an directed or undirected graph and a source vertex, it will "explore" every reachable
  vertex in the graph. It begins on the souce node, it then visits all of the source vertex's
  neighbors and takes note that they have been visited, then proceeds to visit all of their neighbors,
  and so on until no vertex has neighbors that have not been visited.

  The algorithm returns the means to build a breadth-first tree. Either by mutating the vertices'
  sattelite data or by returning a data structure that cointains the necessary information.
  This tree will be rooted at the souce vertex of the BFS function and contain all vertices that
  are reachable from it. The simple path in the tree from the root (source) to any other vertex
  is the shortest possible path in the graph between these two vertices! Careful, the path between
  any two vertices, where neither is the source, is not neccessarily the shortest path between them.

  Running time: Adjacency-list: O(V+E); Adjacency-matrix: O(V^2).



 */
