//=============================================================================//
//
// Purpose: streamed & buffered pak file decoder
//
//=============================================================================//
#include "thirdparty/zstd/zstd.h"
#include "thirdparty/zstd/decompress/zstd_decompress_internal.h"

#include "rtech/ipakfile.h"
#include "pakdecode.h"

//-----------------------------------------------------------------------------
// lookup table for default pak decoder
//-----------------------------------------------------------------------------
static const unsigned char /*141313180*/ s_defaultDecoderLUT[] =
{
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF9, 0x04, 0xFD, 0xFC, 0x07, 0x04, 0x05, 0xFF, 0xF4,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF6, 0x04, 0xFD, 0xFC, 0xFB, 0x04, 0x06, 0xFF, 0x0B,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF8, 0x04, 0xFD, 0xFC, 0x0C, 0x04, 0x05, 0xFF, 0xF7,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF5, 0x04, 0xFD, 0xFC, 0xFA, 0x04, 0x06, 0xFF, 0xF3,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF9, 0x04, 0xFD, 0xFC, 0x07, 0x04, 0x05, 0xFF, 0xF4,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF6, 0x04, 0xFD, 0xFC, 0xFB, 0x04, 0x06, 0xFF, 0x0E,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF8, 0x04, 0xFD, 0xFC, 0x0C, 0x04, 0x05, 0xFF, 0x09,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF5, 0x04, 0xFD, 0xFC, 0xFA, 0x04, 0x06, 0xFF, 0xF1,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF9, 0x04, 0xFD, 0xFC, 0x07, 0x04, 0x05, 0xFF, 0xF4,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF6, 0x04, 0xFD, 0xFC, 0xFB, 0x04, 0x06, 0xFF, 0x0D,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF8, 0x04, 0xFD, 0xFC, 0x0C, 0x04, 0x05, 0xFF, 0xF7,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF5, 0x04, 0xFD, 0xFC, 0xFA, 0x04, 0x06, 0xFF, 0xF2,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF9, 0x04, 0xFD, 0xFC, 0x07, 0x04, 0x05, 0xFF, 0xF4,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF6, 0x04, 0xFD, 0xFC, 0xFB, 0x04, 0x06, 0xFF, 0x0F,
	0x04, 0xFE, 0xFC, 0x08, 0x04, 0xEF, 0x11, 0xF8, 0x04, 0xFD, 0xFC, 0x0C, 0x04, 0x05, 0xFF, 0x0A,
	0x04, 0xFE, 0xFC, 0x10, 0x04, 0xEF, 0x11, 0xF5, 0x04, 0xFD, 0xFC, 0xFA, 0x04, 0x06, 0xFF, 0xF0,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x11,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0C,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x09,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0E,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x11,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0B,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0A,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x10,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x11,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0C,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x09,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0F,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x11,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0D,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x07, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x0A,
	0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0x08, 0x04, 0x05, 0x04, 0x06, 0x04, 0x05, 0x04, 0xFF,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x06,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x07,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x06,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x06,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x07,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x06,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x02, 0x04, 0x03, 0x05, 0x02, 0x04, 0x04, 0x06, 0x02, 0x04, 0x03, 0x06, 0x02, 0x05, 0x04, 0x08,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x07,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x07,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x08,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x08,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x07,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x08,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x07,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x07,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x08,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x08,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x07,
	0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x08,
	0x00, 0x08, 0x00, 0x04, 0x00, 0x08, 0x00, 0x06, 0x00, 0x08, 0x00, 0x01, 0x00, 0x08, 0x00, 0x0B,
	0x00, 0x08, 0x00, 0x0C, 0x00, 0x08, 0x00, 0x09, 0x00, 0x08, 0x00, 0x03, 0x00, 0x08, 0x00, 0x0E,
	0x00, 0x08, 0x00, 0x04, 0x00, 0x08, 0x00, 0x07, 0x00, 0x08, 0x00, 0x02, 0x00, 0x08, 0x00, 0x0D,
	0x00, 0x08, 0x00, 0x0C, 0x00, 0x08, 0x00, 0x0A, 0x00, 0x08, 0x00, 0x05, 0x00, 0x08, 0x00, 0x0F,
	0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06,
	0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x06,
	0x4A, 0x00, 0x00, 0x00, 0x6A, 0x00, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00,
	0xCA, 0x00, 0x00, 0x00, 0xEA, 0x00, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x00, 0x2A, 0x01, 0x00, 0x00,
	0x4A, 0x01, 0x00, 0x00, 0x6A, 0x01, 0x00, 0x00, 0x8A, 0x01, 0x00, 0x00, 0xAA, 0x01, 0x00, 0x00,
	0xAA, 0x03, 0x00, 0x00, 0xAA, 0x05, 0x00, 0x00, 0xAA, 0x25, 0x00, 0x00, 0xAA, 0x25, 0x02, 0x00,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x09, 0x09, 0x0D, 0x11, 0x15,
	0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x2A, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x05, 0x05,
	0x11, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0xBF,
	0xA8, 0xAA, 0x2A, 0xBE, 0xA8, 0xAA, 0x2A, 0xBE, 0xA8, 0xAA, 0x2A, 0xBE, 0xA8, 0xAA, 0x2A, 0xBE,
	0xD2, 0x85, 0x08, 0x3C, 0xD2, 0x85, 0x08, 0x3C, 0xD2, 0x85, 0x08, 0x3C, 0xD2, 0x85, 0x08, 0x3C,
	0x83, 0xF9, 0x22, 0x3F, 0x83, 0xF9, 0x22, 0x3F, 0x83, 0xF9, 0x22, 0x3F, 0x83, 0xF9, 0x22, 0x3F,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x10, 0xC9, 0x3F, 0x00, 0x10, 0xC9, 0x3F, 0x00, 0x10, 0xC9, 0x3F, 0x00, 0x10, 0xC9, 0x3F,
	0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F,
	0x02, 0x61, 0x4D, 0xB9, 0x02, 0x61, 0x4D, 0xB9, 0x02, 0x61, 0x4D, 0xB9, 0x02, 0x61, 0x4D, 0xB9,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0xC2, 0x14, 0xCF, 0x37, 0xC2, 0x14, 0xCF, 0x37, 0xC2, 0x14, 0xCF, 0x37, 0xC2, 0x14, 0xCF, 0x37,
	0x9E, 0x4B, 0x6F, 0xB0, 0x9E, 0x4B, 0x6F, 0xB0, 0x9E, 0x4B, 0x6F, 0xB0, 0x9E, 0x4B, 0x6F, 0xB0,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xF1, 0x1D, 0xC1, 0xF6, 0x7F, 0x00, 0x00,
	0x22, 0x0B, 0xB6, 0xBA, 0x22, 0x0B, 0xB6, 0xBA, 0x22, 0x0B, 0xB6, 0xBA, 0x22, 0x0B, 0xB6, 0xBA,
	0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x3F,
	0x02, 0x61, 0x4D, 0xB9, 0x02, 0x61, 0x4D, 0xB9, 0x02, 0x61, 0x4D, 0xB9, 0x02, 0x61, 0x4D, 0xB9,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0xC2, 0x14, 0xCF, 0x37, 0xC2, 0x14, 0xCF, 0x37, 0xC2, 0x14, 0xCF, 0x37, 0xC2, 0x14, 0xCF, 0x37,
	0x9E, 0x4B, 0x6F, 0xB0, 0x9E, 0x4B, 0x6F, 0xB0, 0x9E, 0x4B, 0x6F, 0xB0, 0x9E, 0x4B, 0x6F, 0xB0,
	0x22, 0x0B, 0xB6, 0xBA, 0x22, 0x0B, 0xB6, 0xBA, 0x22, 0x0B, 0xB6, 0xBA, 0x22, 0x0B, 0xB6, 0xBA,
	0x00, 0x70, 0x95, 0xB6, 0x00, 0x70, 0x95, 0xB6, 0x00, 0x70, 0x95, 0xB6, 0x00, 0x70, 0x95, 0xB6,
	0xA9, 0xAA, 0x2A, 0x3D, 0xA9, 0xAA, 0x2A, 0x3D, 0xA9, 0xAA, 0x2A, 0x3D, 0xA9, 0xAA, 0x2A, 0x3D,
	0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F,
	0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0xBF,
	0xA8, 0xAA, 0x2A, 0xBE, 0xA8, 0xAA, 0x2A, 0xBE, 0xA8, 0xAA, 0x2A, 0xBE, 0xA8, 0xAA, 0x2A, 0xBE,
	0xD2, 0x85, 0x08, 0x3C, 0xD2, 0x85, 0x08, 0x3C, 0xD2, 0x85, 0x08, 0x3C, 0xD2, 0x85, 0x08, 0x3C,
	0x83, 0xF9, 0x22, 0x3F, 0x83, 0xF9, 0x22, 0x3F, 0x83, 0xF9, 0x22, 0x3F, 0x83, 0xF9, 0x22, 0x3F,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x10, 0xC9, 0x3F, 0x00, 0x10, 0xC9, 0x3F, 0x00, 0x10, 0xC9, 0x3F, 0x00, 0x10, 0xC9, 0x3F,
	0x4C, 0x39, 0x56, 0x75, 0x42, 0x52, 0x65, 0x75, 0x70, 0x35, 0x31, 0x77, 0x4C, 0x51, 0x64, 0x61,
};

bool Pak_RStreamDecode(PakDecoder_t* const decoder, const size_t inLen, const size_t outLen)
{
	bool result; // al
	uint64_t m_decompBytePosition; // r15
	uint8_t* m_outputBuf; // r11
	uint32_t m_currentBit; // ebp
	uint64_t m_currentByte; // rsi
	uint64_t m_fileBytePosition; // rdi
	size_t qword70; // r12
	const uint8_t* m_inputBuf; // r13
	uint32_t dword6C; // ecx
	uint64_t v13; // rsi
	unsigned __int64 i; // rax
	unsigned __int64 v15; // r8
	__int64 v16; // r9
	int v17; // ecx
	unsigned __int64 v18; // rax
	uint64_t v19; // rsi
	__int64 v20; // r14
	int v21; // ecx
	unsigned __int64 v22; // r11
	int v23; // edx
	uint64_t m_outputMask; // rax
	int v25; // r8d
	unsigned int v26; // r13d
	uint64_t v27; // r10
	uint8_t* v28; // rax
	uint8_t* v29; // r10
	size_t m_decompSize; // r9
	uint64_t m_inputInvMask; // r10
	uint64_t m_headerOffset; // r8
	uint64_t v33; // rax
	uint64_t v34; // rax
	uint64_t v35; // rax
	size_t v36; // rcx
	__int64 v37; // rdx
	size_t v38; // r14
	size_t v39; // r11
	uint64_t v40; // cl
	uint64_t v41; // rsi
	__int64 v42; // rcx
	uint64_t v43; // r8
	int v44; // r11d
	unsigned __int8 v45; // r9
	uint64_t v46; // rcx
	uint64_t v47; // rcx
	__int64 v48; // r9
	__int64 m; // r8
	__int64 v50; // r9d
	__int64 v51; // r8
	__int64 v52; // rdx
	__int64 k; // r8
	signed __int64 v54; // r10
	__int64 v55; // rdx
	unsigned int v56; // r14d
	const uint8_t* v57; // rdx
	uint8_t* v58; // r8
	uint64_t v59; // al
	uint64_t v60; // rsi
	__int64 v61; // rax
	uint64_t v62; // r9
	int v63; // r10d
	unsigned __int8 v64; // cl
	uint64_t v65; // rax
	unsigned int v66; // r14d
	unsigned int j; // ecx
	__int64 v68; // rax
	uint64_t v69; // rcx
	uint8_t* v70; // [rsp+0h] [rbp-58h]
	uint32_t v71; // [rsp+60h] [rbp+8h]
	const uint8_t* v74; // [rsp+78h] [rbp+20h]

	m_decompBytePosition = decoder->outBufBytePos;

	m_outputBuf = decoder->outputBuf;
	m_currentBit = decoder->currentBit;
	m_currentByte = decoder->currentByte;
	m_fileBytePosition = decoder->inBufBytePos;
	qword70 = decoder->qword70;
	m_inputBuf = decoder->inputBuf;

	if (decoder->compressedStreamSize < qword70)
		qword70 = decoder->compressedStreamSize;

	dword6C = decoder->dword6C;
	v74 = m_inputBuf;
	v70 = m_outputBuf;
	v71 = dword6C;
	if (!m_currentBit)
		goto LABEL_11;

	v13 = (*(_QWORD*)&m_inputBuf[m_fileBytePosition & decoder->inputMask] << (64 - (unsigned __int8)m_currentBit)) | m_currentByte;
	for (i = m_currentBit; ; i = m_currentBit)
	{
		m_currentBit &= 7u;
		m_fileBytePosition += i >> 3;
		dword6C = v71;
		m_currentByte = (0xFFFFFFFFFFFFFFFFui64 >> m_currentBit) & v13;
	LABEL_11:
		v15 = (unsigned __int64)dword6C << 8;
		v16 = dword6C;
		v17 = s_defaultDecoderLUT[(unsigned __int8)m_currentByte + 512 + v15];
		v18 = (unsigned __int8)m_currentByte + v15;
		m_currentBit += v17;
		v19 = m_currentByte >> v17;
		v20 = (unsigned int)(char)s_defaultDecoderLUT[v18];
		if ((s_defaultDecoderLUT[v18] & 0x80u) != 0)
		{
			v56 = -(int)v20;
			v57 = &m_inputBuf[m_fileBytePosition & decoder->inputMask];
			v71 = 1;
			v58 = &m_outputBuf[m_decompBytePosition & decoder->outputMask];
			if (v56 == s_defaultDecoderLUT[v16 + 1248])
			{
				if ((~m_fileBytePosition & decoder->inputInvMask) < 0xF || (decoder->outputInvMask & ~m_decompBytePosition) < 0xF || decoder->decompSize - m_decompBytePosition < 0x10)
					v56 = 1;
				v59 = v19;
				v60 = v19 >> 3;
				v61 = v59 & 7;
				v62 = v60;
				if (v61)
				{
					v63 = s_defaultDecoderLUT[v61 + 1232];
					v64 = s_defaultDecoderLUT[v61 + 1240];
				}
				else
				{
					v62 = v60 >> 4;
					v65 = v60 & 0xF;
					m_currentBit += 4;
					v63 = *(_DWORD*)&s_defaultDecoderLUT[4 * v65 + 1152];
					v64 = s_defaultDecoderLUT[v65 + 1216];
				}
				m_currentBit += v64 + 3;
				v19 = v62 >> v64;
				v66 = v63 + (v62 & ((1 << v64) - 1)) + v56;
				for (j = v66 >> 3; j; --j)
				{
					v68 = *(_QWORD*)v57;
					v57 += 8;
					*(_QWORD*)v58 = v68;
					v58 += 8;
				}
				if ((v66 & 4) != 0)
				{
					*(_DWORD*)v58 = *(_DWORD*)v57;
					v58 += 4;
					v57 += 4;
				}
				if ((v66 & 2) != 0)
				{
					*(_WORD*)v58 = *(_WORD*)v57;
					v58 += 2;
					v57 += 2;
				}
				if ((v66 & 1) != 0)
					*v58 = *v57;
				m_fileBytePosition += v66;
				m_decompBytePosition += v66;
			}
			else
			{
				*(_QWORD*)v58 = *(_QWORD*)v57;
				*((_QWORD*)v58 + 1) = *((_QWORD*)v57 + 1);
				m_fileBytePosition += v56;
				m_decompBytePosition += v56;
			}
		}
		else
		{
			v21 = v19 & 0xF;
			v71 = 0;
			v22 = ((unsigned __int64)(unsigned int)v19 >> (((unsigned int)(v21 - 31) >> 3) & 6)) & 0x3F;
			v23 = 1 << (v21 + ((v19 >> 4) & ((24 * (((unsigned int)(v21 - 31) >> 3) & 2)) >> 4)));
			m_currentBit += (((unsigned int)(v21 - 31) >> 3) & 6) + s_defaultDecoderLUT[v22 + 1088] + v21 + ((v19 >> 4) & ((24 * (((unsigned int)(v21 - 31) >> 3) & 2)) >> 4));
			m_outputMask = decoder->outputMask;
			v25 = 16 * (v23 + ((v23 - 1) & (v19 >> ((((unsigned int)(v21 - 31) >> 3) & 6) + s_defaultDecoderLUT[v22 + 1088]))));
			v19 >>= (((unsigned int)(v21 - 31) >> 3) & 6) + s_defaultDecoderLUT[v22 + 1088] + v21 + ((v19 >> 4) & ((24 * (((unsigned int)(v21 - 31) >> 3) & 2)) >> 4));
			v26 = v25 + s_defaultDecoderLUT[v22 + 1024] - 16;
			v27 = m_outputMask & (m_decompBytePosition - v26);
			v28 = &v70[m_decompBytePosition & m_outputMask];
			v29 = &v70[v27];
			if ((_DWORD)v20 == 17)
			{
				v40 = v19;
				v41 = v19 >> 3;
				v42 = v40 & 7;
				v43 = v41;
				if (v42)
				{
					v44 = s_defaultDecoderLUT[v42 + 1232];
					v45 = s_defaultDecoderLUT[v42 + 1240];
				}
				else
				{
					m_currentBit += 4;
					v46 = v41 & 0xF;
					v43 = v41 >> 4;
					v44 = *(_DWORD*)&s_defaultDecoderLUT[4 * v46 + 1152];
					v45 = s_defaultDecoderLUT[v46 + 1216];
					if (v74 && m_currentBit + v45 >= 61)
					{
						v47 = m_fileBytePosition++ & decoder->inputMask;
						v43 |= (unsigned __int64)v74[v47] << (61 - (unsigned __int8)m_currentBit);
						m_currentBit -= 8;
					}
				}
				m_currentBit += v45 + 3;
				v19 = v43 >> v45;
				v48 = ((unsigned int)v43 & ((1 << v45) - 1)) + v44 + 17;
				m_decompBytePosition += v48;
				if (v26 < 8)
				{
					v50 = v48 - 13;
					m_decompBytePosition -= 13i64;
					if (v26 == 1)
					{
						v51 = *v29;
						//++dword_14D40B2BC;
						v52 = 0i64;
						for (k = 0x101010101010101i64 * v51; (unsigned int)v52 < v50; v52 = (unsigned int)(v52 + 8))
							*(_QWORD*)&v28[v52] = k;
					}
					else
					{
						//++dword_14D40B2B8;
						if (v50)
						{
							v54 = v29 - v28;
							v55 = v50;
							do
							{
								*v28 = v28[v54];
								++v28;
								--v55;
							} while (v55);
						}
					}
				}
				else
				{
					//++dword_14D40B2AC;
					for (m = 0i64; (unsigned int)m < (unsigned int)v48; m = (unsigned int)(m + 8))
						*(_QWORD*)&v28[m] = *(_QWORD*)&v29[m];
				}
			}
			else
			{
				m_decompBytePosition += v20;
				*(_QWORD*)v28 = *(_QWORD*)v29;
				*((_QWORD*)v28 + 1) = *((_QWORD*)v29 + 1);
			}
			m_inputBuf = v74;
		}
		if (m_fileBytePosition >= qword70)
			break;
	LABEL_29:
		m_outputBuf = v70;
		v13 = (*(_QWORD*)&m_inputBuf[m_fileBytePosition & decoder->inputMask] << (64 - (unsigned __int8)m_currentBit)) | v19;
	}
	if (m_decompBytePosition != decoder->decompressedStreamSize)
		goto LABEL_25;
	m_decompSize = decoder->decompSize;
	if (m_decompBytePosition == m_decompSize)
	{
		result = true;
		goto LABEL_69;
	}
	m_inputInvMask = decoder->inputInvMask;
	m_headerOffset = decoder->headerOffset;
	v33 = m_inputInvMask & -(__int64)m_fileBytePosition;
	v19 >>= 1;
	++m_currentBit;
	if (m_headerOffset > v33)
	{
		m_fileBytePosition += v33;
		v34 = decoder->qword70;
		if (m_fileBytePosition > v34)
			decoder->qword70 = m_inputInvMask + v34 + 1;
	}
	v35 = m_fileBytePosition & decoder->inputMask;
	m_fileBytePosition += m_headerOffset;
	v36 = m_decompBytePosition + decoder->outputInvMask + 1;
	v37 = *(_QWORD*)&m_inputBuf[v35] & ((1i64 << (8 * (unsigned __int8)m_headerOffset)) - 1);
	v38 = v37 + decoder->bufferSizeNeeded;
	v39 = v37 + decoder->compressedStreamSize;
	decoder->bufferSizeNeeded = v38;
	decoder->compressedStreamSize = v39;
	if (v36 >= m_decompSize)
	{
		v36 = m_decompSize;
		decoder->compressedStreamSize = m_headerOffset + v39;
	}
	decoder->decompressedStreamSize = v36;
	if (inLen >= v38 && outLen >= v36)
	{
	LABEL_25:
		qword70 = decoder->qword70;
		if (m_fileBytePosition >= qword70)
		{
			m_fileBytePosition = ~decoder->inputInvMask & (m_fileBytePosition + 7);
			qword70 += decoder->inputInvMask + 1;
			decoder->qword70 = qword70;
		}
		if (decoder->compressedStreamSize < qword70)
			qword70 = decoder->compressedStreamSize;
		goto LABEL_29;
	}
	v69 = decoder->qword70;
	if (m_fileBytePosition >= v69)
	{
		m_fileBytePosition = ~m_inputInvMask & (m_fileBytePosition + 7);
		decoder->qword70 = v69 + m_inputInvMask + 1;
	}
	decoder->dword6C = v71;
	result = false;
	decoder->currentByte = v19;
	decoder->currentBit = m_currentBit;
LABEL_69:
	decoder->outBufBytePos = m_decompBytePosition;
	decoder->inBufBytePos = m_fileBytePosition;
	return result;
}


//-----------------------------------------------------------------------------
// checks if we have enough output buffer room to decode the data stream
//-----------------------------------------------------------------------------
bool Pak_HasEnoughDecodeBufferLeft(PakDecoder_t* const decoder, const size_t outLen)
{
	const uint64_t bytesWritten = (decoder->outBufBytePos & ~decoder->outputInvMask);
	return (outLen >= decoder->outputInvMask + (bytesWritten + 1) || outLen >= decoder->decompSize);
}

//-----------------------------------------------------------------------------
// checks if we have enough source data streamed to decode the next block
//-----------------------------------------------------------------------------
bool Pak_HasEnoughStreamedDataForDecode(PakDecoder_t* const decoder, const size_t inLen, const bool useZStream)
{
	// the decoder needs at least this many bytes to decode the current block
	// buffer contiguously
	return (inLen >= decoder->bufferSizeNeeded);
}

struct PakRingBufferFrame_t
{
	uint64_t bufIndex;
	uint64_t frameLen;
};

//-----------------------------------------------------------------------------
// gets the frame for the data in the ring buffer, the frame returned is always
// either the end of the ring buffer, or the end of the data itself
//-----------------------------------------------------------------------------
PakRingBufferFrame_t Pak_ComputeRingBufferFrame(const uint64_t bufMask, const uint64_t seekPos, const uint64_t dataLen)
{
	PakRingBufferFrame_t ring;

	ring.bufIndex = seekPos & bufMask;
	const size_t bufSize = bufMask + 1;

	const size_t bytesUsed = ring.bufIndex % bufSize;
	const size_t totalAvail = bufSize - bytesUsed;

	// the last part of the data might be smaller than the remainder of the ring
	// buffer; clamp it
	ring.frameLen = Min(dataLen - seekPos, totalAvail);
	return ring;
}

bool Pak_ZStreamDecode(PakDecoder_t* const decoder, const size_t inLen, const size_t outLen)
{
	// must have a zstream decoder at this point
	assert(decoder->zstreamContext);

	if (decoder->inBufBytePos >= inLen)
		return false;

	PakRingBufferFrame_t outFrame = Pak_ComputeRingBufferFrame(decoder->outputMask, decoder->outBufBytePos, outLen);

	ZSTD_outBuffer outBuffer = {
		&decoder->outputBuf[outFrame.bufIndex],
		outFrame.frameLen,
		NULL
	};

	PakRingBufferFrame_t inFrame = Pak_ComputeRingBufferFrame(decoder->inputMask, decoder->inBufBytePos, inLen);

	ZSTD_inBuffer inBuffer = {
		&decoder->inputBuf[inFrame.bufIndex],
		inFrame.frameLen,
		NULL
	};

	const size_t ret = ZSTD_decompressStream(decoder->zstreamContext, &outBuffer, &inBuffer);

	if (ZSTD_isError(ret))
	{
		DevWarning(eDLL_T::RTECH, "%s: decode error: %s\n", __FUNCTION__, ZSTD_getErrorName(ret));
		assert(0);

		return false;
	}

	// on the next call, we need at least this amount of data streamed in order
	// to decode the rest of the pak file, as this is where reading has stopped
	decoder->bufferSizeNeeded = inBuffer.pos;

	// advance buffer io positions, required so the main parser could already
	// start parsing the headers while the rest is getting decoded still
	decoder->inBufBytePos += inBuffer.pos;
	decoder->outBufBytePos += outBuffer.pos;

	return ret == NULL;
}

//-----------------------------------------------------------------------------
// initialize the pak decoder context
//-----------------------------------------------------------------------------
size_t Pak_InitDefaultDecoder(PakDecoder_t* const decoder, const uint8_t* const fileBuffer,
	const uint64_t inputMask, const size_t dataSize, const size_t dataOffset, const size_t headerSize)
{
	uint64_t v8; // r9
	unsigned __int64 v9; // r11
	unsigned __int64 v10; // r8
	int v11; // er8
	__int64 v12; // rbx
	unsigned int v13; // ebp
	unsigned __int64 v14; // rbx
	uint64_t v15; // rax
	unsigned int v16; // er9
	unsigned __int64 v17; // r12
	unsigned __int64 v18; // r11
	unsigned __int64 v19; // r10
	unsigned __int64 v20; // rax
	int v21; // ebp
	unsigned __int64 v22; // r10
	unsigned int v23; // er9
	uint64_t v24; // rax
	__int64 v25; // rsi
	uint64_t v26; // rdx
	size_t result; // rax
	uint64_t v28; // rdx

	v8 = dataOffset + headerSize + 8;
	v9 = *(_QWORD*)((inputMask & (dataOffset + headerSize)) + fileBuffer);

	v10 = v9;
	v9 >>= 6;
	v11 = v10 & 0x3F;
	decoder->decompSize = (1i64 << v11) | v9 & ((1i64 << v11) - 1);
	v12 = *(_QWORD*)((inputMask & v8) + fileBuffer) << (64 - ((unsigned __int8)v11 + 6));
	decoder->inBufBytePos = v8 + ((unsigned __int64)(unsigned int)(v11 + 6) >> 3);
	v13 = ((v11 + 6) & 7) + 13;
	v14 = (0xFFFFFFFFFFFFFFFFui64 >> ((v11 + 6) & 7)) & ((v9 >> v11) | v12);
	v15 = inputMask & decoder->inBufBytePos;
	v16 = (((_BYTE)v14 - 1) & 0x3F) + 1;
	v17 = 0xFFFFFFFFFFFFFFFFui64 >> (64 - (unsigned __int8)v16);
	decoder->inputInvMask = v17;
	v18 = 0xFFFFFFFFFFFFFFFFui64 >> (64 - ((((v14 >> 6) - 1) & 0x3F) + 1));
	decoder->outputInvMask = v18;
	v19 = (v14 >> 13) | (*(_QWORD*)(v15 + fileBuffer) << (64 - (unsigned __int8)v13));
	v20 = v13;
	v21 = v13 & 7;
	decoder->inBufBytePos += v20 >> 3;
	v22 = (0xFFFFFFFFFFFFFFFFui64 >> v21) & v19;

	if (v17 == -1i64)
	{
		decoder->headerOffset = 0;
		decoder->bufferSizeNeeded = dataSize;
	}
	else
	{
		v23 = v16 >> 3;
		v24 = inputMask & decoder->inBufBytePos;
		decoder->headerOffset = v23 + 1;
		v25 = *(_QWORD*)(v24 + fileBuffer) & ((1i64 << (8 * ((unsigned __int8)v23 + 1))) - 1);
		decoder->inBufBytePos += v23 + 1;
		decoder->bufferSizeNeeded = v25;
	}

	decoder->bufferSizeNeeded += dataOffset;
	v26 = decoder->bufferSizeNeeded;
	decoder->currentByte = v22;
	decoder->currentBit = v21;
	decoder->qword70 = v17 + dataOffset - 6;
	result = decoder->decompSize;
	decoder->dword6C = 0;
	decoder->compressedStreamSize = v26;
	decoder->decompressedStreamSize = result;

	if ((((unsigned __int8)(v14 >> 6) - 1) & 0x3F) != -1i64 && result - 1 > v18)
	{
		v28 = v26 - decoder->headerOffset;
		decoder->decompressedStreamSize = v18 + 1;
		decoder->compressedStreamSize = v28;
	}

	return result;
}

size_t Pak_InitDecoder(PakDecoder_t* const decoder, const uint8_t* const fileBuffer,
	const uint64_t inputMask, const size_t dataSize, const size_t dataOffset, const size_t headerSize, const bool useZStream)
{
	decoder->inputBuf = fileBuffer;
	decoder->outputBuf = nullptr;

	decoder->fileSize = dataOffset + dataSize;
	decoder->dword44 = NULL;

	decoder->inputMask = inputMask;
	decoder->outputMask = NULL;

	decoder->inBufBytePos = dataOffset + headerSize;
	decoder->outBufBytePos = headerSize;

	if (useZStream)
	{
		// NOTE: on original paks, this data is passed out of the frame header,
		// but for ZStd encoded paks we are always limiting this to the ring
		// buffer size
		decoder->inputInvMask = PAK_DECODE_OUT_RING_BUFFER_MASK;
		decoder->outputInvMask = PAK_DECODE_OUT_RING_BUFFER_MASK;

		ZSTD_DStream* const dctx = ZSTD_createDStream();
		assert(dctx);

		if (!dctx)
			return NULL;

		decoder->zstreamContext = dctx;

		// this is the offset to the ZSTD header in the input buffer
		decoder->headerOffset = static_cast<uint32_t>(decoder->inBufBytePos);

		// this points to the first byte of the frame header, takes dataOffset
		// into account which is the offset in the ring buffer to the patched
		// data as we parse it contiguously after the base pak data, which
		// might have ended somewhere in the middle of the ring buffer
		const uint8_t* const frameHeaderData = (inputMask & (dataOffset + headerSize)) + fileBuffer;

		if (ZSTD_getFrameHeader(&dctx->fParams, frameHeaderData, dataSize) != 0)
		{
			if (decoder->zstreamContext)
			{
				ZSTD_freeDStream(decoder->zstreamContext);
				decoder->zstreamContext = nullptr;
			}

			return NULL; // content size error
		}

		// ideally the frame header of the block gets parsed first, the length
		// thereof is returned by initDStream and thus being processed first
		// before moving on to actual data
		const size_t frameMetaDataSize = ZSTD_initDStream(dctx);

		// we need at least this many bytes of streamed data to process the frame
		// header of the compressed block
		decoder->bufferSizeNeeded = frameMetaDataSize + decoder->headerOffset;

		// must include header size
		const uint64_t decompSize = dctx->fParams.frameContentSize + headerSize;

		decoder->decompSize = decompSize;
		return decompSize;
	}

	return Pak_InitDefaultDecoder(decoder, fileBuffer, inputMask, dataSize, dataOffset, headerSize);
}

//-----------------------------------------------------------------------------
// decode input pak data
//-----------------------------------------------------------------------------
bool Pak_StreamToBufferDecode(PakDecoder_t* const decoder, const size_t inLen, const size_t outLen, const bool useZStream)
{
	if (!Pak_HasEnoughStreamedDataForDecode(decoder, inLen, useZStream))
		return false;

	if (!Pak_HasEnoughDecodeBufferLeft(decoder, outLen))
		return false;

	if (useZStream)
		return Pak_ZStreamDecode(decoder, inLen, outLen);

	return Pak_RStreamDecode(decoder, inLen, outLen);
}
