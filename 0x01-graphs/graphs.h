#ifndef GRAPHS_H
#define GRAPHS_H

/* size_t */
#include <stddef.h>

/**
 * enum edge_type_e - Enumerates the different types of
 * connection between two vertices
 *
 * @UNIDIRECTIONAL: The connection is made only in one way
 * @BIDIRECTIONAL: The connection is made in two ways
 */
typedef enum edge_type_e
{
	UNIDIRECTIONAL = 0,
	BIDIRECTIONAL
} edge_type_t;


typedef struct vertex_s vertex_t;

/**
 * struct edge_s - Node in the linked list of edges for a given vertex
 * A single vertex can have many edges
 *
 * @dest: Pointer to the connected vertex
 * @next: Pointer to the next edge
 */
typedef struct edge_s
{
	vertex_t *dest;
	struct edge_s *next;
} edge_t;


/**
 * struct vertex_s - Node in the linked list of vertices in the adjacency list
 *
 * @index: Index of the vertex in the adjacency list.
 * @content: Custom data stored in the vertex (here, a string)
 * @nb_edges: Number of conenctions with other vertices in the graph
 * @edges: Pointer to the head node of the linked list of edges
 * @next: Pointer to the next vertex in the adjacency linked list
 *   This pointer points to another vertex in the graph, but it
 *   doesn't stand for an edge between the two vertices
 */
struct vertex_s
{
	size_t index;
	char   *content;
	size_t nb_edges;
	edge_t *edges;
	struct vertex_s *next;
};


/**
 * struct graph_s - Representation of a graph
 * We use an adjacency linked list to represent our graph
 *
 * @nb_vertices: Number of vertices in our graph
 * @vertices: Pointer to the head node of our adjacency linked list
 */
typedef struct graph_s
{
	size_t      nb_vertices;
	vertex_t    *vertices;
} graph_t;


/**
 * struct BFS_queue_s - used during a breadth-first traversal of a graph to
 *   create a singly linked FIFO list of vertices awaiting processing
 *
 * @v: pointer to a vertex in adjacency list
 * @depth: degrees of separation from node that began traversal
 * @next: pointer to next vertex in queue
 */
typedef struct BFS_queue_s
{
	vertex_t *v;
	size_t depth;
	struct BFS_queue_s *next;
} BFS_queue_t;


/* graph_display.c - used by example mains */
void graph_display(const graph_t *graph);

/* task 0. Create graph */
graph_t *graph_create(void);

/* task 1. Add a vertex */
vertex_t *graph_add_vertex(graph_t *graph, const char *str);

/* task 2. Add an edge */
edge_t *graph_add_single_edge(vertex_t *src, vertex_t *dest);
int graph_add_edge(graph_t *graph, const char *src,
		   const char *dest, edge_type_t type);

/* task 3. Delete graph */
void graph_delete(graph_t *graph);

/* task 4. Depth-first traversal */
size_t depth_first_traverse(const graph_t *graph,
			    void (*action)(const vertex_t *v, size_t depth));

/* task 5. Breadth-first traversal */
void nextInQueue(BFS_queue_t **queue);
BFS_queue_t *addToQueue(BFS_queue_t **queue, size_t depth, vertex_t *next_v);
int BFS_visit(BFS_queue_t **queue, unsigned char *enqueued,
	      void (*action)(const vertex_t *v, size_t depth));
size_t breadth_first_traverse(const graph_t *graph,
			      void (*action)(const vertex_t *v, size_t depth));

#endif /* GRAPHS_H */
