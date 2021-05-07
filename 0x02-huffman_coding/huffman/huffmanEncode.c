#include "_huffman.h"
/* binary_tree_node_t */
#include "heap.h"
/* symbol_t */
#include "huffman.h"
/* malloc calloc free */
#include <stdlib.h>
/* strlen strcpy */
#include <string.h>


/**
 * freeCodes - TBD
 *
 * @codes: TBD
 * @freq_size: TBD
 */
void freeCodes(char **codes, size_t freq_size)
 {
	size_t i;

	if (codes == NULL)
		return;

	for (i = 0; i < freq_size; i++)
		if (codes[i])
			free(codes[i]);
	free(codes);
}


/**
 * encodeText - TBD
 *
 * @codes: TBD
 * @freq_size: TBD
 * @buff: TBD
 * @w_bit: TBD
 */
int encodeText(char **codes, size_t freq_size, unsigned char *r_buff,
	       size_t read_size, FILE *out_file,
	       unsigned char *w_buff, bit_t *w_bit)
{
	size_t i, j, k;
	char *code = NULL;
	int code_found;
/*
	static size_t ii;
*/
	if (!codes || !r_buff || !out_file || !w_buff || !w_bit)
		return (1);
/*
	if (ii == 0)
	{
		printf("encodeText: after writing tree: w_bit->byte_idx:%lu w_bit->bit_idx:%u\n",
		       w_bit->byte_idx, w_bit->bit_idx);

		printf("w_buff[w_bit->byte_idx]:%x\n", w_buff[w_bit->byte_idx]);
	}
	ii++;
*/
	for (i = 0; i < read_size; i++)
	{
		code_found = 0;
		for (j = 0; j < freq_size; j++)
		{
			if (r_buff[i] == codes[j][0])
			{
				code_found = 1;
				code = codes[j] + 1;

				for (k = 0; code[k]; k++)
				{
					if (code[k] == '0')
						writeBit(out_file, w_buff,
							 w_bit, 0);
					else
						writeBit(out_file, w_buff,
							 w_bit, 1);
				}

				break;
			}
		}

		if (!code_found)
			return (1);
	}

	return (0);
}


/**
 * buildHuffmanCodes - TBD
 *
 * @h_tree: TBD
 * @depth: TBD
 * @code: TBD
 * @codes: TBD
 */
void buildHuffmanCodes(binary_tree_node_t *h_tree, size_t depth,
		       size_t *i, char *code, char **codes)
{
	symbol_t *symbol = NULL;
	char *char_code = NULL;

	if (!h_tree || i == NULL || code == NULL || codes == NULL)
		return;

	if (depth)
		code[depth - 1] = (h_tree == h_tree->parent->left) ? '0' : '1';

	if (!h_tree->left && !h_tree->right)
	{
		symbol = (symbol_t *)(h_tree->data);

		char_code = malloc(sizeof(char) * (depth + 2));
		if (!char_code)
		{
			printf("buildHuffmanCodes: failed to assign code\n");
			return;
		}

		char_code[0] = symbol->data;
		strncpy(char_code + 1, code, depth);
		char_code[depth + 1] = '\0';
		codes[*i] = char_code;
		(*i)++;
		return;
	}

        buildHuffmanCodes(h_tree->left, depth + 1, i, code, codes);
        buildHuffmanCodes(h_tree->right, depth + 1, i, code, codes);
}


/**
 * huffmanEncode - TBD
 *
 * @h_tree: TBD
 * @buff: TBD
 * @w_bit: TBD
 */
int huffmanEncode(FILE *in_file, binary_tree_node_t *h_tree, size_t freq_size,
		  FILE *out_file, unsigned char *w_buff, bit_t *w_bit,
		  size_t in_file_size)
{
	char **codes = NULL;
	char *code = NULL;
	size_t i = 0, read_bytes, remainder;
	unsigned char r_buff[BUF_SIZE] = {0};

	if (!in_file || !h_tree || !out_file || !w_buff || !w_bit)
		return (1);

	code = calloc(freq_size, sizeof(char));
	if (code == NULL)
		return (1);
	codes = calloc(freq_size + 1, sizeof(char *));
	if (codes == NULL)
	{
		free(code);
		return (1);
	}

	buildHuffmanCodes(h_tree, 0, &i, code, codes);
	free(code);
	if (i != freq_size)
	{
		freeCodes(codes, freq_size);
		return (1);
	}

	printf("\thuffman codes:\n");
	for (i = 0; i < freq_size; i++)
		printf("\tcodes[%lu]: %s\n", i, codes[i]);

	remainder = in_file_size % BUF_SIZE;
        do {
		read_bytes = fread(r_buff, sizeof(unsigned char),
				   BUF_SIZE, in_file);

		printf("\thuffmanEncode: read_bytes from text input:%lu reaminder:%lu\n", read_bytes, remainder);

		if (!(read_bytes == BUF_SIZE || read_bytes == remainder))
			break;

		if (encodeText(codes, freq_size, r_buff, read_bytes,
			       out_file, w_buff, w_bit) == 1)
			break;

	} while (read_bytes == BUF_SIZE);

	freeCodes(codes, freq_size);

	if (!feof(in_file))
		return (1);

	if (writePartialByte(out_file, w_buff, w_bit) == 1)
		return (1);

	return (0);
}
