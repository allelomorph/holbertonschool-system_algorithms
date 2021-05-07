#include "_huffman.h"
/* binary_tree_node_t */
#include "heap.h"
/* readBit */
#include "huffman.h"
/* fwrite */
#include <stdio.h>
/* memset */
#include <string.h>


/**
 * decodeSingleChar - TBD
 *
 * @h_tree: TBD
 * @r_buff: TBD
 * @w_bit: TBD
 * Return: TBD
 */
char *decodeSingleChar(binary_tree_node_t *h_tree, FILE *in_file,
		       unsigned char *r_buff, bit_t *r_bit)
{
	unsigned char bit;

	if (!h_tree || !in_file || !r_buff || !r_bit)
		return (NULL);

	if (!h_tree->left && !h_tree->right)
		return ((char *)(h_tree->data));

	if (readBit(in_file, r_buff, r_bit, &bit) == 1)
		return (NULL);

	if (bit == 0)
		return (decodeSingleChar(h_tree->left,
					 in_file, r_buff, r_bit));

	return (decodeSingleChar(h_tree->right, in_file, r_buff, r_bit));
}


/**
 * huffmanDecode - TBD
 *
 * @out_file: TBD
 * @h_tree: TBD
 * @header: TBD
 * @input_file_size: TBD
 * @r_buff: TBD
 * @r_bit: TBD
 * Return: TBD
 */
int huffmanDecode(FILE *out_file, binary_tree_node_t *h_tree,
		  huffman_header_t *header, size_t in_file_size,
		  FILE *in_file, unsigned char *r_buff, bit_t *r_bit)
{
	unsigned char w_buff[BUF_SIZE] = {0};
	char *c = NULL;
	size_t i, remainder, hc_size, write_bytes,/* read_bytes,*/ r_buffered, headerless_size;
/*
	fprintf(stderr, "\thuffmanDecode: out_file @ %p\n", (void *)out_file);
	fprintf(stderr, "\thuffmanDecode: h_tree @ %p\n", (void *)h_tree);
	fprintf(stderr, "\thuffmanDecode: header @ %p\n", (void *)header);
	fprintf(stderr, "\thuffmanDecode: in_file @ %p\n", (void *)in_file);
	fprintf(stderr, "\thuffmanDecode: r_file @ %p\n", (void *)r_buff);
	fprintf(stderr, "\thuffmanDecode: r_bit @ %p\n\n", (void *)r_bit);

	fprintf(stderr, "\thuffmanDecode: w_buff @ %p\n", (void *)&w_buff);
	fprintf(stderr, "\thuffmanDecode: c @ %p\n", (void *)c);
	fprintf(stderr, "\thuffmanDecode: i @ %p\n", (void *)&i);
	fprintf(stderr, "\thuffmanDecode: j @ %p\n", (void *)&j);
	fprintf(stderr, "\thuffmanDecode: remainder @ %p\n", (void *)&remainder);
	fprintf(stderr, "\thuffmanDecode: hc_size @ %p\n", (void *)&hc_size);
	fprintf(stderr, "\thuffmanDecode: write_bytes @ %p\n", (void *)&write_bytes);
*/

	if (!out_file || !h_tree || !header || !in_file || !r_buff || !r_bit)
 		return (1);
/*
	fprintf(stderr, "\thuffmanDecode 1\n");
*/
	hc_size = in_file_size - header->hc_byte_offset;
	remainder = hc_size % BUF_SIZE;
	printf("\thuffmanDecode: in_file_size:%lu header->hc_byte_offset:%u hc_size:%lu remainder:%lu\n", in_file_size, header->hc_byte_offset, hc_size, remainder);
/*
	fprintf(stderr, "\thuffmanDecode 2\n");
*/
/*
	read_bytes = fread(r_buff, sizeof(unsigned char),
			   BUF_SIZE, in_file);
	printf("\thuffmanDecode: first read from encoded input read_bytes:%lu\n", read_bytes);
	if (!(read_bytes == BUF_SIZE || read_bytes == remainder))
	        return (1);
*/
	headerless_size = in_file_size - sizeof(header);
	/* for each byte in hcode (readBit updates r_buff) */
        for (r_buffered = 0, i = 0;
	     (r_buffered + r_bit->byte_idx < headerless_size - 1 ||
	      (r_buffered + r_bit->byte_idx == headerless_size - 1 &&
	       r_bit->bit_idx < header->hc_last_bit_i)); i++)
	{
/*
		printf("\t\titerating %lu until %lu  ", read_bytes, in_file_size);
*/
		c = decodeSingleChar(h_tree, in_file, r_buff, r_bit);
		if (c == NULL)
			return (1);
		w_buff[i % BUF_SIZE] = *c;

		if (r_bit->byte_idx == 0)
		        r_buffered += BUF_SIZE;

		if (!((i + 1) % BUF_SIZE))
		{
			printf("huffmanDecode: writing and refreshing w_buff\n");
			if (fwrite(w_buff, sizeof(unsigned char),
				   BUF_SIZE, out_file) != BUF_SIZE)
				return (1);
			memset(w_buff, 0, BUF_SIZE);
		}
	}
/*
	fprintf(stderr, "\thuffmanDecode 3\n");
*/
	if (i % BUF_SIZE)
	{
	        write_bytes = fwrite(w_buff, sizeof(unsigned char),
				     i % BUF_SIZE, out_file);
		printf("huffmanDecode: final i:%lu write_bytes: %lu\n",
		       i, write_bytes);
		if (write_bytes != i % BUF_SIZE)
			return (1);
	}
/*
	fprintf(stderr, "\thuffmanDecode 4\n");
*/
	return (0);
}
