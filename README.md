# Framework

This provides you a portable framework for parallel graph traversal. You can choose the level of parallelism (i.e., how many nodes can be processed concurrently) with respect to your hardware capacity. And this scheduling can give you a good runtime (Please refer to ___ and ___). Uses adjacent list to represent the graph for the sake of sapce.

In: Graph(nodes, connections represented by adj_list)

Out: List of levels sorted by topological order of Graph. A level is a list of nodes which can be processed at the same time.

(For adjacent list please refer 1. wiki 2. igraph)
