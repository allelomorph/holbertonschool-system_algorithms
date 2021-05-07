#include "_huffman.h"
/* memset */
#include <string.h>

/**
 * readBit - TBD
 *
 * @buff: TBD
 * @r_bit: TBD
 * @value: TBD
 * Return: TBD
 */
int readBit(FILE *in_file, unsigned char *buff,
	    bit_t *r_bit, unsigned char *value)
{
	size_t read_bytes;

	if (!in_file || !buff || !r_bit || !value)
		return (1);
/*
	if (test_bit)
		printf("readBit: r_bit->byte_idx:%u r_bit->bit_idx:%u k:%lu test_bit->byte_idx:%u\n", r_bit->byte_idx, r_bit->bit_idx, k, test_bit->byte_idx);
*/
/*
	printf("readBit: r_bit->byte_idx:%u r_bit->bit_idx:%u \n", r_bit->byte_idx, r_bit->bit_idx);
*/
	r_bit->byte = buff[r_bit->byte_idx];

	if (r_bit->byte & (1 << (7 - r_bit->bit_idx)))
		*value = 1;
	else
		*value = 0;

	r_bit->bit_idx++;
	/* zeroing out byte buffer not necessary unless in writePartialByte */
	if (r_bit->bit_idx == 8)
	{
		r_bit->byte_idx++;
		r_bit->bit_idx = 0;

		if (r_bit->byte_idx == BUF_SIZE)
		{
			memset(buff, 0, BUF_SIZE);
		        read_bytes = fread(buff, sizeof(unsigned char),
					   BUF_SIZE, in_file);
			printf("readBit: refereshing r_buff, read_bytes:%lu\n", read_bytes);
			if (read_bytes != BUF_SIZE && !feof(in_file))
				return (1);
			r_bit->byte_idx = 0;
		}
	}


	return (0);
}


/**
 * readByte - TBD
 *
 * @buff: TBD
 * @r_bit: TBD
 * @byte: TBD
 * Return: TBD
 */
int readByte(FILE *in_file, unsigned char *buff,
	     bit_t *r_bit, unsigned char *byte)
{
	size_t i;
	unsigned char bit_value;

	if (!in_file || !buff || !r_bit || !byte)
		return (1);

	for (i = 0; i < 8; i++)
	{
		if (readBit(in_file, buff, r_bit, &bit_value) == 1)
			return (1);

		if (bit_value)
			*byte |= (1 << (7 - i));
		else
		        *byte &= ~(1 << (7 - i));
	}

	return (0);
}


/**
 * writeBit - TBD
 *
 * @buff: TBD
 * @w_bit: TBD
 * @toggle: TBD
 * Return: TBD
 */
int writeBit(FILE *out_file, unsigned char *buff, bit_t *w_bit,
	     unsigned char toggle)
{
	if (!out_file || !buff || !w_bit || (toggle != 0 && toggle != 1))
		return (1);

	if (toggle)
		w_bit->byte |= (1 << (7 - w_bit->bit_idx));
	else
		w_bit->byte &= ~(1 << (7 - w_bit->bit_idx));
	w_bit->bit_idx++;

	/* zeroing out byte buffer not necessary unless in writePartialByte */
	if (w_bit->bit_idx == 8)
	{
	        buff[w_bit->byte_idx] = w_bit->byte;
		w_bit->byte_idx++;
		w_bit->bit_idx = 0;

		if (w_bit->byte_idx == BUF_SIZE)
		{
			if (fwrite(buff, sizeof(unsigned char), BUF_SIZE,
				   out_file) != BUF_SIZE)
				return (1);
			memset(buff, 0, BUF_SIZE);
			w_bit->byte_idx = 0;
			printf("writeBit: refereshing w_buff\n");

		}
	}

	return (0);
}


/**
 * writeByte - TBD
 *
 * @buff: TBD
 * @w_bit: TBD
 * @byte: TBD
 * Return: TBD
 */
int writeByte(FILE *out_file, unsigned char *buff, bit_t *w_bit,
	      unsigned char byte)
{
	size_t i;

	if (!out_file || !buff || !w_bit)
		return (1);
/*
	if ((char)byte < ' ' || (char)byte > '~')
	        printf("writeByte: %#x\n", (char)byte);
	else
	        printf("writeByte: '%c'\n", (char)byte);
*/
	for (i = 0; i < 8; i++)
	{
		if (writeBit(out_file, buff, w_bit,
			     (byte & (1 << (7 - i))) ? 1 : 0) == 1)
			return (1);
	}
/*
	printf("writeByte exit: w_bit byte_idx:%lu bit_idx:%u\n", w_bit->byte_idx, w_bit->bit_idx);
*/
	return (0);
}


/**
 * writePartialByte - TBD
 *
 * @buff: TBD
 * @w_bit: TBD
 * Return: TBD
 */
int writePartialByte(FILE *out_file, unsigned char *buff, bit_t *w_bit)
{
	unsigned int orig_bit_i;
	size_t i;

	if (!out_file || !buff || !w_bit)
		return (1);

	if (w_bit->bit_idx != 0)
	{
		/* zero out any bits in byte buffer beyond current bit_idx */
		for (i = w_bit->bit_idx; i < 8; i++)
			w_bit->byte &= ~(1 << (7 - i));

		orig_bit_i = w_bit->bit_idx;
		w_bit->bit_idx = 0;
		/* zero-padded by unassigned bits to left */
		writeByte(out_file, buff, w_bit, w_bit->byte);
		/* reset to original values */
		w_bit->bit_idx = orig_bit_i;
		w_bit->byte_idx -= 1;
	}

	printf("\twritePartialByte exit: w_bit byte_idx:%u bit_idx:%u\n", w_bit->byte_idx, w_bit->bit_idx);

	return (0);
}
