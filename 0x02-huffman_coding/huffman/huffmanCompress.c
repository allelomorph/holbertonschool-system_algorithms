#include "_huffman.h"
/* binary_tree_node_t */
#include "heap.h"
/* malloc */
#include <stdlib.h>
/* fread feof fclose */
#include <stdio.h>
/* memset */
#include <string.h>
/* offsetof */
#include <stddef.h>

/* symbol_t */
#include "huffman.h"

void binary_tree_print(const binary_tree_node_t *heap, int (*print_data)(char *, void *));

/**
 * symbol_print - Prints a symbol structure
 *
 * @buffer: Buffer to print into
 * @data: Pointer to a node's data
 *
 * Return: Number of bytes written in buffer
 */
int symbol_print(char *buffer, void *data)
{
    symbol_t *symbol;
    char c;
    int length;

    symbol = (symbol_t *)data;
    c = symbol->data;
    if (c == -1)
        c = '$';
    length = sprintf(buffer, "(%c/%lu)", c, symbol->freq);
    return (length);
}


/**
 * tallyFrequencies - TBD
 *
 * @in_file: TBD
 * Return: TBD
 */
size_t *tallyFrequencies(FILE *in_file, size_t in_file_size)
{
	size_t *freqs = NULL;
	size_t i, read_bytes, remainder;
	unsigned char r_buff[BUF_SIZE] = {0};

	if (!in_file)
		return (NULL);

	freqs = malloc(sizeof(size_t) * CHAR_RANGE);
	if (!freqs)
		return (NULL);
	memset(freqs, 0, sizeof(size_t) * CHAR_RANGE);

	remainder = in_file_size % BUF_SIZE;
        do {
		read_bytes = fread(r_buff, sizeof(unsigned char),
				   BUF_SIZE, in_file);

		printf("\ttallyFrequencies: read_bytes from text input:%lu\n", read_bytes);

		if (!(read_bytes == BUF_SIZE || read_bytes == remainder))
		{
			free(freqs);
			return (NULL);
		}

		for (i = 0; i < read_bytes; i++)
			freqs[r_buff[i]]++;

	} while (read_bytes == BUF_SIZE);

	if (!feof(in_file))
	{
		free(freqs);
		return (NULL);
	}

	rewind(in_file);

	return (freqs);
}


/* build inputs to match existing huffman_tree() prototype */
/**
 * prepareTreeInputs - TBD
 *
 * @freqs: TBD
 * @data: TBD
 * @freq: TBD
 * @freq_size: TBD
 * Return: TBD
 */
int prepareTreeInputs(size_t *freqs, char **data,
		      size_t **freq, size_t *freq_size)
{
	size_t i, j;

	if (!freqs || !data || !freq || !freq_size)
		return (1);

	for (i = 0, *freq_size = 0; i < CHAR_RANGE; i++)
		if (freqs[i])
			(*freq_size)++;

	*data = malloc(sizeof(char) * *freq_size);
	if (!(*data))
		return (1);

	*freq = malloc(sizeof(size_t) * *freq_size);
	if (!(*freq))
	{
		free((*data));
		return (1);
	}

	for (i = 0, j = 0; i < CHAR_RANGE && j < *freq_size; i++)
	{
		if (freqs[i])
		{
			(*freq)[j] = freqs[i];
			(*data)[j] = (char)i;
			j++;
		}
	}

	return (0);
}


/**
 * prepareTreeInputs - TBD
 *
 * @freqs: TBD
 * @data: TBD
 * @freq: TBD
 * @freq_size: TBD
 * Return: TBD
 */
binary_tree_node_t *huffmanTreeFromText(FILE *in_file, size_t *freq_size,
					size_t in_file_size)
{
	size_t *freqs = NULL, *freq = NULL;
	size_t i;
	char *data = NULL;
	binary_tree_node_t *h_tree = NULL;

	if (!in_file)
		return (NULL);

	freqs = tallyFrequencies(in_file, in_file_size);
	if (!freqs)
		return (NULL);

	if (prepareTreeInputs(freqs, &data, &freq, freq_size) == 1)
	{
		free(freqs);
		return (NULL);
	}
	free(freqs);

	for (i = 0; i < *freq_size; i++)
	{
		if (data[i] < ' ' || data[i] > '~')
			printf("\ti:%lu data:%x freq:%lu", i, data[i], freq[i]);
		else
			printf("\ti:%lu data:'%c' freq:%lu", i, data[i], freq[i]);
		if (i % 2)
			putchar('\n');
	}

	h_tree = huffman_tree(data, freq, *freq_size);

	free(data);
	free(freq);

	return (h_tree);
}


/**
 * huffmanCompress - TBD
 *
 * @in_file: TBD
 * @out_file: TBD
 * Return: TBD
 */
int huffmanCompress(FILE *in_file, FILE *out_file, long int in_file_size)
{
	unsigned char w_buff[BUF_SIZE] = {0};
	bit_t w_bit = {0, 0, 0};
	huffman_header_t header = {"\177HUF", 0, 0, 0};
	binary_tree_node_t *h_tree = NULL;
	size_t freq_size = 0, write_bytes = 0;

	if (!in_file || !out_file || in_file_size < 0)
		return (1);

	h_tree = huffmanTreeFromText(in_file, &freq_size,
				     (size_t)in_file_size);
	if (!h_tree)
		return (1);

	binary_tree_print(h_tree, symbol_print);

	huffmanSerialize(h_tree, out_file, w_buff, &w_bit);
	/* max leaves in tree 256, serialized tree not likely to be more than roughly 2 * 256 bytes */
	/* if partial byte reamins, write to last byte in buffer */
	if (writePartialByte(out_file, w_buff, &w_bit) == 1)
	{
		binaryTreeDelete(h_tree, freeSymbol);
		return (1);
	}

	printf("huffmanCompress: after writing tree: w_bit.byte_idx:%u w_bit.bit_idx:%u\n",
	       w_bit.byte_idx, w_bit.bit_idx);

	printf("w_buff[w_bit.byte_idx]:%x\n", w_buff[w_bit.byte_idx]);

	/* write partial header to file */
	header.hc_byte_offset = sizeof(huffman_header_t) + w_bit.byte_idx;
        header.hc_first_bit_i = w_bit.bit_idx;
        if (fwrite(&header, sizeof(huffman_header_t), 1, out_file) != 1)
	{
		binaryTreeDelete(h_tree, freeSymbol);
		return (1);
	}

	if (huffmanEncode(in_file, h_tree, freq_size, out_file,
			  w_buff, &w_bit, (size_t)in_file_size))
	{
		binaryTreeDelete(h_tree, freeSymbol);
		return (1);
	}
	binaryTreeDelete(h_tree, freeSymbol);

	/* write only/last buffer to file */
	if (!(w_bit.byte_idx == 0 && w_bit.bit_idx == 0))
	{
		write_bytes = fwrite(w_buff, sizeof(unsigned char),
				     w_bit.byte_idx + 1, out_file);

		printf("huffmanCompress: wrote last w_buff bytes:%lu\n", write_bytes);

		if (write_bytes != w_bit.byte_idx + 1)
			return (1);
	}

	/* write hc end index to header after encoding */
	if (fseek(out_file, offsetof(huffman_header_t, hc_last_bit_i),
		  SEEK_SET) != 0 ||
	    fwrite(&(w_bit.bit_idx), sizeof(unsigned char),
		   1, out_file) != 1)
		return (1);

	return (0);
}
