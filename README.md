# Framework

This provides you a portable framework for parallel graph traversal. You can choose the level of parallelism (i.e., how many nodes can be processed concurrently) with respect to your hardware capacity. And this scheduling can give you a good runtime (Please refer to [1](http://www.math.ucsd.edu/~ronspubs/72_04_two_processors.pdf) and [2](http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=7357180)). Uses adjacency list to represent the graph for the sake of sapce.

In: Graph(nodes, connections represented by adj_list)

Out: List of levels sorted by topological order of Graph. A level is a list of nodes which can be processed at the same time.

(For adjacency list please refer 1. [wiki](https://en.wikipedia.org/wiki/Adjacency_list) 2. [igraph](http://igraph.org/redirect.html))
