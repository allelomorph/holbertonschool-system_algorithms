#include "_huffman.h"
/* binary_tree_node_t */
#include "heap.h"
/* FILE fopen printf */
#include <stdio.h>
/* free */
#include <stdlib.h>
/* strncmp */
#include <string.h>


void binary_tree_print(const binary_tree_node_t *heap, int (*print_data)(char *, void *));

/**
 * char_print - prints a char
 *
 * @buffer: Buffer to print into
 * @data: Pointer to a node's data
 *
 * Return: TBD
 */
int char_print(char *buffer, void *data)
{
    char c;
    int length;

    if (data == NULL)
	    length = sprintf(buffer, "($)");
    else
    {
	    c = *((char *)data);

	    if (c < ' ' || c > '~')
		    length = sprintf(buffer, "(%o)", c);
	    else
		    length = sprintf(buffer, "('%c')", c);
    }

    return (length);
}


/**
 * freeChar - meant to be used as free_data parameter to heap_delete or
 *   binaryTreeDelete; frees memory allocated for a char
 *
 * @data: void pointer intended to be cast into data pointer
 */
void freeChar(void *data)
{
        if (data)
                free((char *)data);
}


void decompressCleanup(unsigned char *r_buff, bit_t *r_bit,
			  huffman_header_t *header, binary_tree_node_t *h_tree)
{
	/* explicit NULL check since allocated with calloc */

	if (r_buff != NULL)
		free(r_buff);

	if (r_bit != NULL)
		free(r_bit);

	if (header != NULL)
		free(header);

	if (h_tree)
		binaryTreeDelete(h_tree, freeChar);
}


/**
 * huffmanDecompress - TBD
 *
 * @in_file: TBD
 * @out_file: TBD
 * Return: TBD
 */
int huffmanDecompress(FILE *in_file, FILE *out_file, long int in_file_size)
{
	unsigned char *r_buff = NULL;
	bit_t *r_bit = NULL;
	huffman_header_t *header = NULL;
	binary_tree_node_t *h_tree = NULL;
	size_t read_bytes = 0;

	if (!in_file || !out_file || in_file_size < 0)
		return (1);

	r_buff = calloc(BUF_SIZE, sizeof(unsigned char));
	r_bit = calloc(1, sizeof(bit_t));
	header = calloc(1, sizeof(huffman_header_t));
	if (r_buff == NULL || r_bit == NULL || header == NULL)
	{
		decompressCleanup(r_buff, r_bit, header, h_tree);
		return (1);
	}
	if (fread(header, sizeof(huffman_header_t), 1, in_file) != 1)
		return (1);
	if (strncmp((char *)(header->huff_id), "\177HUF", 4) != 0)
	{
		printf("Input is not a file compressed by this program!\n");
		decompressCleanup(r_buff, r_bit, header, h_tree);
		return (1);
	}

	/* serialized trees should be <= 2 * CHAR_RANGE, and < BUF_SIZE */
	read_bytes = fread(r_buff, sizeof(unsigned char),
			   BUF_SIZE, in_file);
	printf("\tread_bytes from compressed file:%lu\n", read_bytes);

	h_tree = huffmanDeserialize(in_file, r_buff, r_bit, NULL);
	if (!h_tree)
	{
		decompressCleanup(r_buff, r_bit, header, h_tree);
		return (1);
	}
	binary_tree_print(h_tree, char_print);
#ifdef ZZZ
	/* reset read buffer to begin intake at start of encoded text */
	if (fseek(in_file, sizeof(header) + r_bit->byte_idx, SEEK_SET) != 0)
	{
		decompressCleanup(r_buff, r_bit, header, h_tree);
		return (1);
	}

	r_bit->byte_idx = 0; /* bit index still needed */
	memset(r_buff, 0, BUF_SIZE);
#endif
	if (huffmanDecode(out_file, h_tree, header, (size_t)in_file_size,
			  in_file, r_buff, r_bit) == 1)
	{
		decompressCleanup(r_buff, r_bit, header, h_tree);
		return (1);
	}

	decompressCleanup(r_buff, r_bit, header, h_tree);
	return (0);
}
