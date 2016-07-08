# Framework

This provides you a portable framework for parallel directed acyclic graph (DAG) traversal. You can choose the level of parallelism (i.e., how many nodes can be processed concurrently) with respect to your hardware capacity. And this scheduling can give you a good runtime (Please refer to [1](http://www.math.ucsd.edu/~ronspubs/72_04_two_processors.pdf) and [2](http://ieeexplore.ieee.org/xpl/login.jsp?tp=&arnumber=7357180)). Uses adjacency list to represent the graph for the sake of sapce.

This framework gives a scheduled graph for parallel processing, input and output of it are as below:

In: Graph(nodes, edges(represented by adj_list))

Out: List of levels sorted by topological order of Graph. A level is a list of nodes which can be processed at the same time.

Example:
Directed Acyclic Graph:

            2
            
          /   \
          
        1       3       (assume connections are 2->1 and 2->3)
        
You need to give the framework 

1) list of nodes (i.e., [1,2,3])

2) list of connections (i.e., [[2,1], [2,3]])

The framework will give you the scheduled result: [2, [1,3]]

(For adjacency list please refer 1. [wiki](https://en.wikipedia.org/wiki/Adjacency_list) 2. [igraph](http://igraph.org/redirect.html))
