#pragma once
typedef unsigned long DWORD;
typedef unsigned char BYTE;

struct PNG_SIGNATURE
{
	BYTE _byData[8];
};

struct PNG_CHUNK_HEAD
{
	DWORD    _dwLength;
	BYTE     _byType[4];
};

struct PNG_IHDR_DATA
{
	DWORD    _dwWidth;
	DWORD    _dwHeight;
	BYTE     _byBitDepth;
	BYTE     _byType;
	BYTE     _byCompression;
	BYTE     _byFiltration;
	BYTE     _byInterlace;
};

struct PNG_PALITRA
{
	BYTE     red;
	BYTE     green;
	BYTE     blue;
};

struct png_chunk
{
	DWORD data_length;
	BYTE type[4];
	BYTE* data;
	DWORD crc32;
};

enum sh_png_filters {
	sh_no_filter,
	sh_sub_filter,
	sh_up_filter,
	sh_avg_filter,
	sh_paeth_filter
};

class PNG_reader
{
public:
	PNG_reader(const char* file_directory);

	void make_larger(int number);
	void make_larger(int number_x, int number_y);
	void ClearBuffer(float r, float g, float b, float a);

	void drawLine(int x0, int y0, int x1, int y1, float r, float g, float b);
	void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, float r, float g, float b);
	void setPixel(int x, int y, float r, float g, float b);

	~PNG_reader();
private:
	bool checkTrue(BYTE* a, const char* b);
	void decode_png_filter(BYTE* data, PNG_IHDR_DATA header);
	DWORD __fastcall RevertDWord(DWORD dwValue);
	int sh_abs(int number);
	int sh_png_paeth_predict(int a, int b, int c);
public:
	int width, height;
	int bytes_per_pixel = 1;
	BYTE* image_data;
};