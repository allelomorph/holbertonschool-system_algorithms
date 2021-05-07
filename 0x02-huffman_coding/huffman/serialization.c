#include "_huffman.h"
/* binary_tree_node() binary_tree_node_t */
#include "heap.h"
/* symbol_t */
#include "huffman.h"
/* malloc */
#include <stdlib.h>


/**
 * huffmanSerialize - TBD
 *
 * @huffman_tree: TBD
 * @buff: TBD
 * @w_bit: TBD
 */
void huffmanSerialize(binary_tree_node_t *huffman_tree, FILE *out_file,
		      unsigned char *buff, bit_t *w_bit)
{
	if (!huffman_tree || !buff || !out_file || !w_bit)
		return;

	if (!huffman_tree->left && !huffman_tree->right)
	{
		writeBit(out_file, buff, w_bit, 1);
		writeByte(out_file, buff, w_bit,
			  ((symbol_t *)(huffman_tree->data))->data);
		return;
	}

	writeBit(out_file, buff, w_bit, 0);

	huffmanSerialize(huffman_tree->left, out_file, buff, w_bit);
	huffmanSerialize(huffman_tree->right, out_file, buff, w_bit);
}


/**
 * huffmanDeserialize - TBD
 *
 * @buff: TBD
 * @r_bit: TBD
 * @parent: TBD
 * Return: TBD
 */
binary_tree_node_t *huffmanDeserialize(FILE *in_file, unsigned char *buff,
				       bit_t *r_bit, binary_tree_node_t *parent)
{
	binary_tree_node_t *new = NULL;
	unsigned char bit, *c = NULL;

	if (!buff || !r_bit)
		return (NULL);

	readBit(in_file, buff, r_bit, &bit);

	if (bit == 1)
	{
		c = malloc(sizeof(unsigned char));
		if (!c)
			return (NULL);
		readByte(in_file, buff, r_bit, c);
		new = binary_tree_node(parent, c);
		return (new);
	}

	new = binary_tree_node(parent, NULL);
	new->left = huffmanDeserialize(in_file, buff, r_bit, new);
	new->right = huffmanDeserialize(in_file, buff, r_bit, new);

	return (new);
}
