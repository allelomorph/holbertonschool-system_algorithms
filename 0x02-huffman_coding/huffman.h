#ifndef HUFFMAN_H
#define HUFFMAN_H

/* size_t */
#include <stddef.h>


/**
 * struct symbol_s - Stores a char and its associated frequency
 *
 * @data: The character
 * @freq: The associated frequency
 */
typedef struct symbol_s
{
	char data;
	size_t freq;
} symbol_t;


/* task 5. Symbols: Create symbol */
/* symbol.c */
symbol_t *symbol_create(char data, size_t freq);

/* task 6. Huffman coding - Step 1: Priority queue */
/* huffman_priority_queue.c */
heap_t *huffman_priority_queue(char *data, size_t *freq, size_t size);

/* task 7. Huffman coding - Step 2: Extract */
/* huffman_extract_and_insert.c */
int huffman_extract_and_insert(heap_t *priority_queue);

/* task 8. Huffman coding - Step 3: Build Huffman tree */
/* huffman_tree.c */
binary_tree_node_t *huffman_tree(char *data, size_t *freq, size_t size);

/* task 9. Huffman coding - Step 4: Huffman codes */
/* huffman_codes.c */
int huffman_codes(char *data, size_t *freq, size_t size);


#endif /* HUFFMAN_H */
