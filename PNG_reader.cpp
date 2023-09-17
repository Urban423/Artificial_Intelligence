#include "PNG_reader.h"
#include "zlib.h"

PNG_reader::PNG_reader(const char* file_directory)
{
	PNG_SIGNATURE     PSignature;
	PNG_CHUNK_HEAD    PHead;
	PNG_IHDR_DATA     PData;
	PNG_PALITRA       PaliteData;
	DWORD             crc;

	png_chunk chunk;

	BYTE* data = new BYTE[1];

	FILE* pIn;
	fopen_s(&pIn, file_directory, "rb");

	if (pIn == 0 || fread(&PSignature, 8, 1, pIn) != 1)
	{
		return;
	}

	while (!feof(pIn))
	{
		fread(&PHead, 8, 1, pIn);
		int size = RevertDWord(PHead._dwLength);
		if (checkTrue(PHead._byType, "IHDR"))
		{
			fread(&PData, 13, 1, pIn);

			width = RevertDWord(PData._dwWidth);
			height = RevertDWord(PData._dwHeight);
			if ((int)PData._byType == 0 || (int)PData._byType == 4)
			{
				bytes_per_pixel = 1;
			}
			else if((int)PData._byType == 2)
			{
				bytes_per_pixel = 3;
			}
			else
			{
				bytes_per_pixel = 4;
			}
			image_data = new Byte[width * height * bytes_per_pixel * 3 + height];
		}
		else if (checkTrue(PHead._byType, "PLTE"))
		{
			delete[] data;
			data = new BYTE[size];
			fread(data, size, 1, pIn);

			std::cout << "plte" << "\n";
		}
		else if (checkTrue(PHead._byType, "IDAT"))
		{
			delete[] data;
			data = new BYTE[size];
			fread(data, size, 1, pIn);

			infltate(data, size, image_data);
			decode_png_filter(image_data, PData);
		}
		else if (checkTrue(PHead._byType, "IEND"))
		{
			break;
		}
		else
		{
			delete[] data;
			data = new BYTE[size];
			fread(data, size, 1, pIn);
		}
		fread(&crc, 4, 1, pIn);
	}
}

void PNG_reader::make_larger(int number)
{
	if (number < 1)
	{
		return;
	}

	BYTE* temp_image_data = new BYTE[width * height * number * number * bytes_per_pixel];
	for (int y = 0; y < height; y++)
	{
		for (int y1 = 0; y1 < number; y1++)
		{
			for (int x = 0; x < width; x++)
			{
				for (int x1 = 0; x1 < number; x1++)
				{
					for (int q = 0; q < bytes_per_pixel; q++)
					{
						temp_image_data[((y * number + y1) * width * number + (x * number + x1)) * bytes_per_pixel + q] = image_data[(y * width + x) * bytes_per_pixel + q];
					}
				}
			}
		}
	}

	width *= number;
	height *= number;
	delete[] image_data;
	image_data = temp_image_data;
}

void PNG_reader::make_larger(int number_x, int number_y)
{
	if (number_x < 1 || number_y < 1)
	{
		return;
	}

	BYTE* temp_image_data = new BYTE[width * height * number_x * number_y * bytes_per_pixel];
	for (int y = 0; y < height; y++)
	{
		for (int y1 = 0; y1 < number_y; y1++)
		{
			for (int x = 0; x < width; x++)
			{
				for (int x1 = 0; x1 < number_x; x1++)
				{
					for (int q = 0; q < bytes_per_pixel; q++)
					{
						temp_image_data[((y * number_y + y1) * width * number_x + (x * number_x + x1)) * bytes_per_pixel + q] = image_data[(y * width + x) * bytes_per_pixel + q];
					}
				}
			}
		}
	}

	width *= number_x;
	height *= number_y;
	delete[] image_data;
	image_data = temp_image_data;
}

void PNG_reader::ClearBuffer(float r, float g, float b, float a)
{
	for (int i = 0; i < width * height * bytes_per_pixel; i += bytes_per_pixel)
	{
		image_data[i] = BYTE(r * 255);
		if (bytes_per_pixel > 1)
		{
			image_data[i + 1] = BYTE(g * 255);
			image_data[i + 2] = BYTE(b * 255);
			if (bytes_per_pixel > 3)
			{
				image_data[i + 3] = BYTE(a * 255);
			}
		}
	}
}


void PNG_reader::drawLine(int x0, int y0, int x1, int y1, float r, float g, float b)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			setPixel(y, x, r, g, b);
		}
		else 
		{
			setPixel(x, y, r, g, b);
		}
		error2 += derror2;

		if (error2 > dx) 
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void PNG_reader::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, float r, float g, float b)
{

}

void PNG_reader::setPixel(int x, int y, float r, float g, float b)
{
	int first_color = (x * width + y) * bytes_per_pixel;
	image_data[first_color] = BYTE(r * 255);
	if (bytes_per_pixel > 1)
	{
		image_data[first_color + 1] = BYTE(g * 255);
		image_data[first_color + 2] = BYTE(b * 255);
	}
}

PNG_reader::~PNG_reader()
{
}

bool PNG_reader::checkTrue(BYTE* a, const char* b)
{
	if (a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3])
	{
		return true;
	}
	return false;
}

void PNG_reader::decode_png_filter(BYTE* data, PNG_IHDR_DATA header)
{ 
	BYTE* row = data;
	int stride = width * bytes_per_pixel;

	image_data = new BYTE[width * height * bytes_per_pixel];
	BYTE* working = image_data;

	for (int i = 0; i < height; ++i) 
	{
		working = image_data + i * stride;
		BYTE filter = *row++;

		switch (filter)
		{
		case sh_no_filter:
		{
			for (int j = 0; j < stride; j++)
			{
				working[j] = row[j];
			}
			break;
		}

		case sh_sub_filter:
		{
			for (int j = 0; j < stride; j++)
			{
				BYTE a = 0;
				if (j > bytes_per_pixel - 1)
				{
					a = working[j - bytes_per_pixel];
				}
				BYTE value = row[j] + a;
				working[j] = value;
			}
			break;
		}

		case sh_up_filter:
		{
			BYTE* prev_row;
			if (i > 0)
			{
				prev_row = working - stride;
			}
			else
			{
				prev_row = new Byte[stride];
				memset(prev_row, 0, stride);
			}

			for (int j = 0; j < stride; j++)
			{
				BYTE b = prev_row[j];
				BYTE value = row[j] + b;
				working[j] = value;
			}
			break;
		}

		case sh_avg_filter: 
		{
			BYTE* prev_row;
			if (i > 0)
			{
				prev_row = working - stride;
			}
			else
			{
				prev_row = new Byte[stride];
				memset(prev_row, 0, stride);
			}

			for (int j = 0; j < stride; j++)
			{
				BYTE a = 0;
				BYTE b = prev_row[j];
				if (j > bytes_per_pixel - 1)
				{
					a = working[j - bytes_per_pixel];
				}

				BYTE value = row[j] + ((a + b) >> 1);
				working[j] = value;
			}
			break;
		}

		case sh_paeth_filter:
		{
			BYTE* prev_row;
			if (i > 0)
			{
				prev_row = working - stride;
			}
			else
			{
				prev_row = new Byte[stride];
				memset(prev_row, 0, stride);
			}

			for (int j = 0; j < stride; j++)
			{
				BYTE a = 0;
				BYTE b = prev_row[j];
				BYTE c = 0;
				if (j > bytes_per_pixel - 1)
				{
					a = working[j - bytes_per_pixel];
					c = prev_row[j - bytes_per_pixel];
				}

				BYTE value = row[j] + sh_png_paeth_predict((int)a, (int)b, (int)c);
				working[j] = value;
			}
			break;
		}

		}

		row += stride;
	}
}

DWORD __fastcall PNG_reader::RevertDWord(DWORD dwValue)
{
	return ((dwValue & 0x000000FF) << 24) | ((dwValue & 0x0000FF00) << 8) | ((dwValue & 0x00FF0000) >> 8) | ((dwValue & 0xFF000000) >> 24);
}

int PNG_reader::sh_abs(int number)
{
	return (number > 0) ? number : -1 * number;
}

int PNG_reader::sh_png_paeth_predict(int a, int b, int c)
{
	int p = a + b - c;
	int pa = sh_abs(p - a);
	int pb = sh_abs(p - b);
	int pc = sh_abs(p - c);

	if (pa <= pb && pa <= pc) return a;
	if (pb <= pc) return b;
	return c;
}
