#pragma once
#include <iostream>

typedef unsigned char Byte;

struct png_bit_stream
{
	Byte* data_stream;
	int bit_buffer;
	int bits_remaining;
};

Byte base_length_extra_bit[] = {
	0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
	1, 1, 1, 1, //265 - 268
	2, 2, 2, 2, //269 - 273 
	3, 3, 3, 3, //274 - 276
	4, 4, 4, 4, //278 - 280
	5, 5, 5, 5, //281 - 284
	0           //285
};

int base_lengths[] = {
	3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
	11, 13, 15, 17,          //265 - 268
	19, 23, 27, 31,          //269 - 273 
	35, 43, 51, 59,          //274 - 276
	67, 83, 99, 115,         //278 - 280
	131, 163, 195, 227,      //281 - 284
	258                      //285
};

int dist_bases[] = {
	/*0*/ 1, 2, 3, 4,    //0-3
	/*1*/ 5, 7,          //4-5
	/*2*/ 9, 13,         //6-7
	/*3*/ 17, 25,        //8-9
	/*4*/ 33, 49,        //10-11
	/*5*/ 65, 97,        //12-13
	/*6*/ 129, 193,      //14-15
	/*7*/ 257, 385,      //16-17
	/*8*/ 513, 769,      //18-19
	/*9*/ 1025, 1537,    //20-21
	/*10*/ 2049, 3073,   //22-23
	/*11*/ 4097, 6145,   //24-25
	/*12*/ 8193, 12289,  //26-27
	/*13*/ 16385, 24577, //28-29
			 0   , 0      //30-31, error, shouldn't occur
};

int dist_extra_bits[] = {
	/*0*/ 0, 0, 0, 0, //0-3
	/*1*/ 1, 1,       //4-5
	/*2*/ 2, 2,       //6-7
	/*3*/ 3, 3,       //8-9
	/*4*/ 4, 4,       //10-11
	/*5*/ 5, 5,       //12-13
	/*6*/ 6, 6,       //14-15
	/*7*/ 7, 7,       //16-17
	/*8*/ 8, 8,       //18-19
	/*9*/ 9, 9,       //20-21
	/*10*/ 10, 10,    //22-23
	/*11*/ 11, 11,    //24-25
	/*12*/ 12, 12,    //26-27
	/*13*/ 13, 13,     //28-29
			0 , 0      //30-31 error, they shouldn't occur
};

void read_bits(Byte& buffer, int& index, int read_bits, int len_data, Byte* data)
{
	int helper = 0;
	int final_bit = read_bits + index;
	int ret = 0;
	for (int i1 = index / 8; i1 < len_data; i1++)
	{
		for (int i = index % 8; i < 8; i++)
		{
			ret += ((data[i1] >> i) & 0x01) * pow(2, helper);
			helper += 1;
			index += 1;
			if (index == final_bit)
			{
				buffer = ret;
				return;
			}
		}
	}
}

void read_bits(int& buffer, int& index, int read_bits, int len_data, Byte* data)
{
	if (read_bits == 0) return;
	int helper = 0;
	int final_bit = read_bits + index;
	int ret = 0;
	for (int i1 = index / 8; i1 < len_data; i1++)
	{
		for (int i = index % 8; i < 8; i++)
		{
			ret += ((data[i1] >> i) & 0x01) * pow(2, helper);
			helper += 1;
			index += 1;
			if (index == final_bit)
			{
				buffer = ret;
				return;
			}
		}
	}
}

void read_bits(int& buffer, int& index, int read_bits, int len_data, Byte* data, bool reverse)
{
	int helper = 0;
	if (reverse)
	{
		helper = read_bits - 1;
	}
	int final_bit = read_bits + index;
	int ret = 0;
	for (int i1 = index / 8; i1 < len_data; i1++)
	{
		for (int i = index % 8; i < 8; i++)
		{
			ret += ((data[i1] >> i) & 0x01) * pow(2, helper);
			if (reverse)
			{
				helper -= 1;
			}
			else {
				helper += 1;
			}
			index += 1;
			if (index == final_bit)
			{
				buffer = ret;
				return;
			}
		}
	}
}

int* memalloc(int size)
{
	int* a = new int[size];
	for (int i = 0; i < size; i++)
	{
		a[i] = 0;
	}
	return a;
}

int get_maximum_bit_length(Byte* code_bit_lengths, int len_code_bit_lengths)
{
	int max_bit_length = 0;
	for (int i = 0; i < len_code_bit_lengths; i++)
	{
		if (code_bit_lengths[i] > max_bit_length)
		{
			max_bit_length = code_bit_lengths[i];
		}
	}
	return max_bit_length;
}

void first_code_for_bitlen(int* first_codes, int* code_count, int max_bit_length) {
	int code = 0;
	for (int i = 2; i <= max_bit_length; i++)
	{
		code = (code + code_count[i - 1]) << 1;
		if (code_count[i] > 0)
		{
			first_codes[i] = code;
		}
	}
}

void assign_Huffman_code(int* assigned_codes, int* first_codes, Byte* code_bit_lengths, int len_assign_code)
{
	for (int i = 0; i < len_assign_code; ++i)
	{
		if (code_bit_lengths[i])
		{
			assigned_codes[i] = first_codes[code_bit_lengths[i]]++;
		}
	}
}

void get_bit_length_count(int* code_count, Byte* code_bit_length, int bit_len_array_len) 
{
	for (int i = 0; i < bit_len_array_len; i++)
	{
		code_count[(int)code_bit_length[i]] += 1;
	}
}

int* build_huffman_code(Byte* code_bit_lengths, int len_code_bit_lengths)
{
	int max_bit_length = get_maximum_bit_length(code_bit_lengths, len_code_bit_lengths);
	int* assigned_codes = new int[len_code_bit_lengths];
	int* code_counts = new int[max_bit_length + 1];
	int* help_table_2 = new int[max_bit_length + 1];

	for (int i = 0; i < len_code_bit_lengths; i++)
	{
		assigned_codes[i] = 0;
	}

	for (int i = 0; i < max_bit_length + 1; i++)
	{
		code_counts[i] = 0;
		help_table_2[i] = 0;
	}

	get_bit_length_count(code_counts, code_bit_lengths, len_code_bit_lengths);
	first_code_for_bitlen(help_table_2, code_counts, max_bit_length);
	assign_Huffman_code(assigned_codes, help_table_2, code_bit_lengths, len_code_bit_lengths);


	return assigned_codes;
}

int decode_Huffman(Byte* bits, int* assigned_codes, Byte* code_bit_lengths, int assigned_code_length, int size, int& index)
{
	int code = 0;
	for (int i = 0; i < assigned_code_length; i++)
	{
		if (code_bit_lengths[i] == 0) continue;
		read_bits(code, index, (int)code_bit_lengths[i], size, bits, true);
		if (assigned_codes[i] == code)
		{
			return i;
		}
		index -= (int)code_bit_lengths[i];
	}
	return 0;
}

void decode_by_trees(Byte* data, int size, int& index, int* literal_length_huff_tree, int hlit, int* distance_huff_tree, int hdist, Byte* two_trees_code_bit_lengths, Byte* decompressed_data)
{
	int data_index = 0;
	while (true)
	{
		int decoded_value = decode_Huffman(data, literal_length_huff_tree, two_trees_code_bit_lengths, hlit, size, index);

		if (decoded_value == 256)
		{
			break;
		}
		else if (decoded_value < 256)
		{

			decompressed_data[data_index] = decoded_value;
			data_index += 1;
			continue;
		}
		else
		{
			int base_index = decoded_value - 257;
			int duplicate_length = 0;
			read_bits(duplicate_length, index, (int)base_length_extra_bit[base_index], size, data);
			duplicate_length += base_lengths[base_index];

			int distance_index = decode_Huffman(data, distance_huff_tree, two_trees_code_bit_lengths + hlit, hdist, size, index);

			int distance_length = 0;
			read_bits(distance_length, index, dist_extra_bits[distance_index], size, data);
			distance_length += dist_bases[distance_index];
			int back_pointer_index = data_index - distance_length;

			while (duplicate_length--)
			{
				decompressed_data[data_index] = decompressed_data[back_pointer_index];
				data_index += 1;
				back_pointer_index += 1;
			}
		}
	}
}

void toBinary(int num, int size)
{
	for (int i = size - 1; i >= 0; i--)
	{
		std::cout << ((num >> i) & 0x01);
	}
}

void decomress_type1(Byte* data, int size, int& index, Byte* decompressed_data)
{
	int hlit = 288;
	int hdist = 32;

	Byte two_trees_code_bit_lengths[288 + 32];
	for (int i = 0; i < 144; i++)
	{
		two_trees_code_bit_lengths[i] = 8;
	}
	for (int i = 144; i < 256; i++)
	{
		two_trees_code_bit_lengths[i] = 9;
	}
	for (int i = 256; i < 280; i++)
	{
		two_trees_code_bit_lengths[i] = 7;
	}
	for (int i = 280; i < 288; i++)
	{
		two_trees_code_bit_lengths[i] = 8;
	}
	for (int i = 288; i < 288 + 32; i++)
	{
		two_trees_code_bit_lengths[i] = 5;
	}

	int* literal_length_huff_tree = build_huffman_code(two_trees_code_bit_lengths, hlit);
	int* distance_huff_tree = build_huffman_code(two_trees_code_bit_lengths, hdist);

	decode_by_trees(data, size, index, literal_length_huff_tree, hlit, distance_huff_tree, hdist, two_trees_code_bit_lengths, decompressed_data);
}

void decomress_type2(Byte* data, int size, int &index, Byte* decompressed_data)
{
	int hlit = 0;
	int hdist = 0;
	int hclen = 0;

	read_bits(hlit, index, 5, size, data);
	read_bits(hdist, index, 5, size, data);
	read_bits(hclen, index, 4, size, data);

	hlit += 257;
	hdist += 1;
	hclen += 4;

	Byte code_lengths_of_code_length_order[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	Byte code_length_of_code_length[19];
	memset(code_length_of_code_length, 0, 19);

	for (int i = 0; i < hclen; i++)
	{
		read_bits(code_length_of_code_length[code_lengths_of_code_length_order[i]], index, 3, size, data);
	}
	int* tree = build_huffman_code(code_length_of_code_length, 19);
	
	Byte* two_trees_code_bit_lengths = new Byte[hlit + hdist];
	for (int i = 0; i < hlit + hdist; i++)
	{
		two_trees_code_bit_lengths[i] = 0;
	}
	
	int code_index = 0;
	int code_to_repeat = 0;
	int repeat_count = 0;

	while (code_index < (hdist + hlit))
	{
		int decoded_value = decode_Huffman(data, tree, code_length_of_code_length, 19, size, index);

		if (decoded_value < 16)
		{
			two_trees_code_bit_lengths[code_index++] = decoded_value;
			continue;
		}

		switch (decoded_value)
		{
		case 16:
			read_bits(repeat_count, index, 2, size, data);
			repeat_count += 3;
			code_to_repeat = two_trees_code_bit_lengths[code_index - 1];
			break;
		case 17:
			read_bits(repeat_count, index, 3, size, data);
			repeat_count += 3;
			code_to_repeat = 0;
			break;
		case 18:
			read_bits(repeat_count, index, 7, size, data);
			repeat_count += 11;
			code_to_repeat = 0;
			break;
		}

		for (int i1 = 0; i1 < repeat_count; i1++)
		{
			two_trees_code_bit_lengths[code_index + i1] = code_to_repeat;
		}

		code_index += repeat_count;
	}

	int* literal_length_huff_tree = build_huffman_code(two_trees_code_bit_lengths, hlit);
	int* distance_huff_tree = build_huffman_code(two_trees_code_bit_lengths + hlit, hdist);

	decode_by_trees(data, size, index, literal_length_huff_tree, hlit, distance_huff_tree, hdist, two_trees_code_bit_lengths, decompressed_data);
}

static void infltate(Byte* data, int size, Byte* decompressed_data)
{
	int index = 0;

	int temp = 0;
	read_bits(temp, index, 8, size, data);
	read_bits(temp, index, 8, size, data);

	int final_block = 0;
	do
	{
		int type = 0;

		read_bits(final_block, index, 1, size, data);
		read_bits(type, index, 2, size, data);

		if (type == 2)
		{
			decomress_type2(data, size, index, decompressed_data);
		}
		else 
		{
			decomress_type1(data, size, index, decompressed_data);
		}
	} while (!final_block);
}