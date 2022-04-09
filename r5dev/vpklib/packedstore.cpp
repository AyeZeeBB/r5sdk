﻿/***********************************************************************
* ██████╗ ██████╗     ██╗   ██╗██████╗ ██╗  ██╗    ██╗     ██╗██████╗  *
* ██╔══██╗╚════██╗    ██║   ██║██╔══██╗██║ ██╔╝    ██║     ██║██╔══██╗ *
* ██████╔╝ █████╔╝    ██║   ██║██████╔╝█████╔╝     ██║     ██║██████╔╝ *
* ██╔══██╗██╔═══╝     ╚██╗ ██╔╝██╔═══╝ ██╔═██╗     ██║     ██║██╔══██╗ *
* ██║  ██║███████╗     ╚████╔╝ ██║     ██║  ██╗    ███████╗██║██████╔╝ *
* ╚═╝  ╚═╝╚══════╝      ╚═══╝  ╚═╝     ╚═╝  ╚═╝    ╚══════╝╚═╝╚═════╝  *
***********************************************************************/

#include "core/stdafx.h"
#include "tier1/cvar.h"
#include "mathlib/adler32.h"
#include "mathlib/crc32.h"
#include "engine/sys_utils.h"
#include "vpklib/packedstore.h"

//-----------------------------------------------------------------------------
// Purpose: initialize parameters for compression algorithm
//-----------------------------------------------------------------------------
void CPackedStore::InitLzCompParams(void)
{
	/*| PARAMETERS ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
	m_lzCompParams.m_dict_size_log2 = RVPK_DICT_SIZE;
}

//-----------------------------------------------------------------------------
// Purpose: initialize parameters for decompression algorithm
//-----------------------------------------------------------------------------
void CPackedStore::InitLzDecompParams(void)
{
	/*| PARAMETERS ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
	m_lzDecompParams.m_dict_size_log2   = RVPK_DICT_SIZE;
	m_lzDecompParams.m_decompress_flags = LZHAM_DECOMP_FLAG_OUTPUT_UNBUFFERED | LZHAM_DECOMP_FLAG_COMPUTE_CRC32;
	m_lzDecompParams.m_struct_size      = sizeof(lzham_decompress_params);
}

//-----------------------------------------------------------------------------
// Purpose: obtains archive chunk path for specific file
//-----------------------------------------------------------------------------
string CPackedStore::GetPackChunkFile(string svPackDirFile, int iArchiveIndex)
{
	/*| ARCHIVES ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
	string svPackChunkFile = StripLocalePrefix(svPackDirFile);
	ostringstream oss;

	oss << std::setw(3) << std::setfill('0') << iArchiveIndex;
	string svPackChunkIndex = "pak000_" + oss.str();

	StringReplace(svPackChunkFile, "pak000_dir", svPackChunkIndex);
	return svPackChunkFile;
}

//-----------------------------------------------------------------------------
// Purpose: returns populated pack dir struct for specified pack dir file
//-----------------------------------------------------------------------------
VPKDir_t CPackedStore::GetPackDirFile(string svPackDirFile)
{
	/*| PACKDIRFILE |||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
	std::regex rgArchiveRegex("pak000_([0-9]{3})");
	std::smatch smRegexMatches;

	std::regex_search(svPackDirFile, smRegexMatches, rgArchiveRegex);

	if (smRegexMatches.size() != 0)
	{
		StringReplace(svPackDirFile, smRegexMatches[0], "pak000_dir");

		for (int i = 0; i < LANGUAGE_PACKS; i++)
		{
			if (strstr(svPackDirFile.c_str(), DIR_LIBRARY_PREFIX[i].c_str()))
			{
				for (int j = 0; j < LIBRARY_PACKS; j++)
				{
					if (strstr(svPackDirFile.c_str(), DIR_LIBRARY_PREFIX[j].c_str()))
					{
						string svPackDirPrefix = DIR_LOCALE_PREFIX[i] + DIR_LOCALE_PREFIX[i];
						StringReplace(svPackDirFile, DIR_LOCALE_PREFIX[i].c_str(), svPackDirPrefix.c_str());
						goto escape;
					}
				}
			}
		}escape:;
	}

	VPKDir_t vpk_dir(svPackDirFile);
	return vpk_dir;
}

//-----------------------------------------------------------------------------
// Purpose: obtains and returns the entry block to the vector
//-----------------------------------------------------------------------------
vector<VPKEntryBlock_t> CPackedStore::GetEntryBlocks(CIOStream* reader)
{
	/*| ENTRYBLOCKS |||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
	string svName, svPath, svExtension;
	vector<VPKEntryBlock_t> vBlocks;
	while (!(svExtension = reader->readString()).empty())
	{
		while (!(svPath = reader->readString()).empty())
		{
			while (!(svName = reader->readString()).empty())
			{
				string svFilePath = FormatBlockPath(svName, svPath, svExtension);
				vBlocks.push_back(VPKEntryBlock_t(reader, svFilePath));
			}
		}
	}
	return vBlocks;
}

//-----------------------------------------------------------------------------
// Purpose: formats the entry block path
//-----------------------------------------------------------------------------
string CPackedStore::FormatBlockPath(string svName, string svPath, string svExtension)
{
	if (!svPath.empty())
	{
		svPath += "\\";
	}
	return svPath + svName + "." + svExtension;
}

//-----------------------------------------------------------------------------
// Purpose: strips locale prefix from file path
//-----------------------------------------------------------------------------
string CPackedStore::StripLocalePrefix(string svPackDirFile)
{
	std::filesystem::path fspPackDirFile(svPackDirFile);
	string svFileName = fspPackDirFile.filename().u8string();

	for (int i = 0; i < LANGUAGE_PACKS; i++)
	{
		if (strstr(svFileName.c_str(), DIR_LOCALE_PREFIX[i].c_str()))
		{
			StringReplace(svFileName, DIR_LOCALE_PREFIX[i].c_str(), "");
			break;
		}
	}
	return svFileName;
}

//-----------------------------------------------------------------------------
// Purpose: validates extraction result with precomputed ADLER32 hash
//-----------------------------------------------------------------------------
void CPackedStore::ValidateAdler32PostDecomp(string svAssetFile)
{
	uint32_t adler_init = {};
	ifstream istream(svAssetFile, fstream::binary);

	istream.seekg(0, fstream::end);
	m_vHashBuffer.resize(istream.tellg());
	istream.seekg(0, fstream::beg);
	istream.read((char*)m_vHashBuffer.data(), m_vHashBuffer.size());

	m_nAdler32 = adler32::update(adler_init, m_vHashBuffer.data(), m_vHashBuffer.size());

	if (m_nAdler32 != m_nAdler32_Internal)
	{
		Warning(eDLL_T::FS, "Warning: ADLER32 checksum mismatch for entry '%s' computed value '0x%lX' doesn't match expected value '0x%lX'. File may be corrupt!\n", svAssetFile.c_str(), m_nAdler32, m_nAdler32_Internal);
		m_nAdler32          = 0;
		m_nAdler32_Internal = 0;
	}

	istream.close();
	m_vHashBuffer.clear();
}

//-----------------------------------------------------------------------------
// Purpose: validates extraction result with precomputed CRC32 hash
//-----------------------------------------------------------------------------
void CPackedStore::ValidateCRC32PostDecomp(string svDirAsset)
{
	uint32_t crc32_init = {};
	ifstream istream(svDirAsset, fstream::binary);

	istream.seekg(0, fstream::end);
	m_vHashBuffer.resize(istream.tellg());
	istream.seekg(0, fstream::beg);
	istream.read((char*)m_vHashBuffer.data(), m_vHashBuffer.size());

	m_nCrc32 = crc32::update(crc32_init, m_vHashBuffer.data(), m_vHashBuffer.size());

	if (m_nCrc32 != m_nCrc32_Internal)
	{
		Warning(eDLL_T::FS, "Warning: CRC32 checksum mismatch for entry '%s' computed value '0x%lX' doesn't match expected value '0x%lX'. File may be corrupt!\n", svDirAsset.c_str(), m_nCrc32, m_nCrc32_Internal);
		m_nCrc32          = 0;
		m_nCrc32_Internal = 0;
	}

	istream.close();
	m_vHashBuffer.clear();
}

//-----------------------------------------------------------------------------
// Purpose: extracts all files from specified vpk file
//-----------------------------------------------------------------------------
void CPackedStore::UnpackAll(VPKDir_t vpk_dir, string svPathOut)
{
	for (int i = 0; i < vpk_dir.m_vsvArchives.size(); i++)
	{
		std::filesystem::path fspVpkPath(vpk_dir.m_svDirPath);
		string svPath = fspVpkPath.parent_path().u8string() + "\\" + vpk_dir.m_vsvArchives[i];
		ifstream packChunkStream(svPath, std::ios_base::binary); // Create stream to read from each archive.

		for ( VPKEntryBlock_t block : vpk_dir.m_vvEntryBlocks)
		{
			// Escape if block archive index is not part of the extracting archive chunk index.
			if (block.m_iArchiveIndex != i) { goto escape; }
			else
			{
				string svFilePath = CreateDirectories(svPathOut + "\\" + block.m_svBlockPath);
				ofstream outFileStream(svFilePath, std::ios_base::binary | std::ios_base::out);

				if (!outFileStream.is_open())
				{
					Error(eDLL_T::FS, "Error: unable to access file '%s'!\n", svFilePath.c_str());
				}
				outFileStream.clear(); // Make sure file is empty before writing.
				for (VPKEntryDescr_t entry : block.m_vvEntries)
				{
					char* pCompressedData = new char[entry.m_nCompressedSize];
					memset(pCompressedData, 0, entry.m_nCompressedSize); // Compressed region.

					packChunkStream.seekg(entry.m_nArchiveOffset);       // Seek to entry offset in archive.
					packChunkStream.read(pCompressedData, entry.m_nCompressedSize); // Read compressed data from archive.

					if (entry.m_bIsCompressed)
					{
						lzham_uint8* pLzOutputBuf = new lzham_uint8[entry.m_nUncompressedSize];
						m_lzDecompStatus = lzham_decompress_memory(&m_lzDecompParams, pLzOutputBuf, (size_t*)&entry.m_nUncompressedSize, (lzham_uint8*)pCompressedData, entry.m_nCompressedSize, &m_nAdler32_Internal, &m_nCrc32_Internal);

						if (fs_packedstore_entryblock_stats->GetBool())
						{
							DevMsg(eDLL_T::FS, "--------------------------------------------------------------\n");
							DevMsg(eDLL_T::FS, "] Block path            : '%s'\n", block.m_svBlockPath.c_str());
							DevMsg(eDLL_T::FS, "] Entry count           : '%llu'\n", block.m_vvEntries.size());
							DevMsg(eDLL_T::FS, "] Compressed size       : '%llu'\n", entry.m_nCompressedSize);
							DevMsg(eDLL_T::FS, "] Uncompressed size     : '%llu'\n", entry.m_nUncompressedSize);
							DevMsg(eDLL_T::FS, "] Static CRC32 hash     : '0x%lX'\n", block.m_nCrc32);
							DevMsg(eDLL_T::FS, "] Computed CRC32 hash   : '0x%lX'\n", m_nCrc32_Internal);
							DevMsg(eDLL_T::FS, "] Computed ADLER32 hash : '0x%lX'\n", m_nAdler32_Internal);
							DevMsg(eDLL_T::FS, "--------------------------------------------------------------\n");
						}

						if (block.m_vvEntries.size() == 1) // Internal checksum can only match block checksum if entry size is 1.
						{
							if (block.m_nCrc32 != m_nCrc32_Internal)
							{
								Warning(eDLL_T::FS, "Warning: CRC32 checksum mismatch for entry '%s' computed value '0x%lX' doesn't match expected value '0x%lX'. File may be corrupt!\n", block.m_svBlockPath.c_str(), m_nCrc32_Internal, block.m_nCrc32);
							}
						}
						else { m_nEntryCount++; }

						if (m_lzDecompStatus != lzham_decompress_status_t::LZHAM_DECOMP_STATUS_SUCCESS)
						{
							Error(eDLL_T::FS, "Error: failed decompression for an entry within block '%s' in archive '%d'!\n", block.m_svBlockPath.c_str(), i);
							Error(eDLL_T::FS, "'lzham_decompress_memory_func' returned with status '%d'.\n", m_lzDecompStatus);
						}
						else
						{
							// If successfully decompressed, write to file.
							outFileStream.write((char*)pLzOutputBuf, entry.m_nUncompressedSize);
							delete[] pLzOutputBuf, pCompressedData;
						}
					}
					else
					{
						// If not compressed, write raw data into output file.
						outFileStream.write(pCompressedData, entry.m_nUncompressedSize);
					}
				}
				outFileStream.close();
				if (m_nEntryCount == block.m_vvEntries.size()) // Only validate after last entry in block had been written.
				{
					// Set internal hash to precomputed entry hash for post decompress validation.
					m_nCrc32_Internal = block.m_nCrc32;

					ValidateCRC32PostDecomp(svFilePath);
					//ValidateAdler32PostDecomp(svFilePath);
					m_nEntryCount = 0;
				}
			}escape:;
		}
		packChunkStream.close();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 'vpk_entry_block' constructor
//-----------------------------------------------------------------------------
VPKEntryBlock_t::VPKEntryBlock_t(CIOStream* reader, string svPath)
{
	std::replace(svPath.begin(), svPath.end(), '/', '\\'); // Flip forward slashes in filepath to windows-style backslash.

	this->m_svBlockPath = svPath; // Set path of block.
	reader->read<uint32_t>(this->m_nCrc32);        //
	reader->read<uint16_t>(this->m_nPreloadBytes); //
	reader->read<uint16_t>(this->m_iArchiveIndex); //

	do // Loop through all entries in the block and push them to the vector.
	{
		VPKEntryDescr_t entry(reader);
		this->m_vvEntries.push_back(entry);
	} while (reader->readR<uint16_t>() != 65535);
}

//-----------------------------------------------------------------------------
// Purpose: 'vpk_entry_h' constructor
//-----------------------------------------------------------------------------
VPKEntryDescr_t::VPKEntryDescr_t(CIOStream* reader)
{
	reader->read<uint32_t>(this->m_nEntryFlags);       //
	reader->read<uint16_t>(this->m_nTextureFlags);     //
	reader->read<uint64_t>(this->m_nArchiveOffset);    //
	reader->read<uint64_t>(this->m_nCompressedSize);   //
	reader->read<uint64_t>(this->m_nUncompressedSize); //
	this->m_bIsCompressed = (this->m_nCompressedSize != this->m_nUncompressedSize);
}

//-----------------------------------------------------------------------------
// Purpose: 'vpk_dir_h' constructor
//-----------------------------------------------------------------------------
VPKDir_t::VPKDir_t(string svPath)
{
	CIOStream reader;

	reader.open(svPath, eStreamFileMode::READ);
	reader.read<uint32_t>(this->m_nFileMagic);

	if (this->m_nFileMagic != RVPK_DIR_MAGIC)
	{
		Error(eDLL_T::FS, "Error: vpk_dir file '%s' has invalid magic!\n", svPath.c_str());
		return;
	}

	reader.read<uint16_t>(this->m_nMajorVersion); //
	reader.read<uint16_t>(this->m_nMinorVersion); //
	reader.read<uint32_t>(this->m_nTreeSize);     //
	reader.read<uint32_t>(this->m_nFileDataSize); //

	DevMsg(eDLL_T::FS, "______________________________________________________________\n");
	DevMsg(eDLL_T::FS, "] HEADER_DETAILS ---------------------------------------------\n");
	DevMsg(eDLL_T::FS, "] File Magic     : '%lu'\n", this->m_nFileMagic);
	DevMsg(eDLL_T::FS, "] Major Version  : '%hu'\n", (this->m_nMajorVersion));
	DevMsg(eDLL_T::FS, "] Minor Version  : '%hu'\n", (this->m_nMinorVersion));
	DevMsg(eDLL_T::FS, "] Tree Size      : '%lu'\n", this->m_nTreeSize);
	DevMsg(eDLL_T::FS, "] File Data Size : '%lu'\n", this->m_nFileDataSize);

	this->m_vvEntryBlocks = g_pPackedStore->GetEntryBlocks(&reader);
	this->m_svDirPath = svPath; // Set path to vpk_dir file.

	for (VPKEntryBlock_t block : this->m_vvEntryBlocks)
	{
		if (block.m_iArchiveIndex > this->m_iArchiveCount)
		{
			this->m_iArchiveCount = block.m_iArchiveIndex;
		}
	}

	DevMsg(eDLL_T::FS, "______________________________________________________________\n");
	DevMsg(eDLL_T::FS, "] PACK_CHUNKS ------------------------------------------------\n");

	for (int i = 0; i < this->m_iArchiveCount + 1; i++)
	{
		string svArchivePath = g_pPackedStore->GetPackChunkFile(svPath, i);
		DevMsg(eDLL_T::FS, "] '%s'\n", svArchivePath.c_str());
		this->m_vsvArchives.push_back(svArchivePath);
	}
}
///////////////////////////////////////////////////////////////////////////////
CPackedStore* g_pPackedStore = new CPackedStore();
