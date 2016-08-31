//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "PNGTextureLoader.h"
#include "DEFLATECompress.h"
#include "FileSystem.h"
#include <sstream>
#include <cassert>


namespace CriticalChunk {
	static constexpr uint32_t ID_IHDR = (static_cast<uint32_t>('I') << 24)
		+ (static_cast<uint32_t>('H') << 16)
		+ (static_cast<uint32_t>('D') << 8)
		+ (static_cast<uint32_t>('R'));

	static constexpr uint32_t ID_PLTE = (static_cast<uint32_t>('P') << 24)
		+ (static_cast<uint32_t>('L') << 16)
		+ (static_cast<uint32_t>('T') << 8)
		+ (static_cast<uint32_t>('E'));

	static constexpr uint32_t ID_IDAT = (static_cast<uint32_t>('I') << 24)
		+ (static_cast<uint32_t>('D') << 16)
		+ (static_cast<uint32_t>('A') << 8)
		+ (static_cast<uint32_t>('T'));

	static constexpr uint32_t ID_IEND = (static_cast<uint32_t>('I') << 24)
		+ (static_cast<uint32_t>('E') << 16)
		+ (static_cast<uint32_t>('N') << 8)
		+ (static_cast<uint32_t>('D'));

	struct IHDR {
		uint32_t Width;
		uint32_t Height;
		BYTE BitDepth;
		BYTE ColorType;
		BYTE CompressionMethod;
		BYTE FilterMode;
		BYTE InterlacedMethod;
	};
}

// Helper
inline BYTE PaethPredictor(BYTE a, BYTE b, BYTE c) {
	auto ai = static_cast<int>(a);
	auto bi = static_cast<int>(b);
	auto ci = static_cast<int>(c);
	auto p = ai + bi - ci;
	auto pa = abs(p - ai);
	auto pb = abs(p - bi);
	auto pc = abs(p - ci);
	if (pa <= pb && pa <= pc) return a;
	else if (pb <= pc) return b;
	else return c;
}

Luxko::Anuthur::PNGData Luxko::Anuthur::PNGData::ReadFromFile(const std::wstring& fileName)
{
	PNGData result;
	using namespace FileSystem;
	auto file = File::Create(fileName, FileAccess::Read, FileShareMode::Read, FileCreationOption::OpenExisting,
		FileAttribute::Normal, FileFlag::SequentialScan, nullptr);
	if (!file.Valid()) {
		std::stringstream ss;
		ss << "File Creation Failed. Error code: " << GetLastError() << std::flush;
		throw PNGReadingException(ss.str());
	}

	// First of all, check the PNG signature
	{
		BYTE sigPlaceHolder[8];
		DWORD br;
		if (!file.Read(sigPlaceHolder, 8u, br) || br != 8) {
			throw PNGReadingException("File data corrupted: reading failure.");
		}

		if (sigPlaceHolder[0] == 137
			&& sigPlaceHolder[1] == 80
			&& sigPlaceHolder[2] == 78
			&& sigPlaceHolder[3] == 71
			&& sigPlaceHolder[4] == 13
			&& sigPlaceHolder[5] == 10
			&& sigPlaceHolder[6] == 26
			&& sigPlaceHolder[7] == 10) {

		}
		else {
			throw PNGReadingException("Not a valid PNG file: PNG signature mismatch.");
		}
	}

	// Then a header chunk is expected.
	{
		uint32_t length = ReadNext4AsUint32(file);
		uint32_t typeID = ReadNext4AsUint32(file);
		if (CriticalChunk::ID_IHDR != typeID) {
			throw PNGReadingException("Not a valid PNG file: PNG header missing.");
		}
		if (length != sizeof(CriticalChunk::IHDR)) {
			throw PNGReadingException("Not a  valid PNG file: invalid PNG header length.");
		}

		PNGDataChunk header(length, typeID);
		header.ReadDataAndCRC(file);


		CriticalChunk::IHDR headerData = {};
		memcpy(&headerData, header._data.data(), sizeof(headerData));
		if (headerData.CompressionMethod != static_cast<BYTE>(0)) {
			throw PNGReadingException("Decompress failure: unexpected compression method used.");
		}
		if (headerData.FilterMode != static_cast<BYTE>(0)) {
			throw PNGReadingException("Decompress failure: unexpected filter method used.");
		}
		if (headerData.InterlacedMethod != static_cast<BYTE>(0)) {
			throw PNGReadingException("Decompress failure: unsupported interlaced method used.");
		}

		result._width = headerData.Width;
		result._height = headerData.Height;
		result._channelDepth = headerData.BitDepth;
		result._colorType = static_cast<PNG_COLOR_TPYE>(headerData.ColorType);
	}

	std::vector<BYTE> decompressedRawBytes;

	switch (result._colorType)
	{
	case PNG_COLOR_TYPE_INDEXED:
		// If indexed color is used, a PLTE chunk is expected.
	{
		PNGDataChunk plteChunk;
		while (true) {
			uint32_t length = ReadNext4AsUint32(file);
			uint32_t typeID = ReadNext4AsUint32(file);

			if (CriticalChunk::ID_PLTE == typeID) {
				plteChunk = PNGDataChunk(length, typeID);
				break;
			}
			else {
				auto header = PNGDataChunk(length, typeID);
				if (header.IsCritical()) {
					throw PNGReadingException("Not a valid PNG file: invalid chunk orientation.");
				}
				header.ReadDataAndCRC(file);
			}
		}

		plteChunk.ReadDataAndCRC(file);

		// TODO: finish decoding indexed color.
	}
		break;
	case PNG_COLOR_TYPE_GRAYSCALE:
	case PNG_COLOR_TYPE_GRAYSCALE_ALPHA:
	case PNG_COLOR_TYPE_RGB:
	case PNG_COLOR_TYPE_RGBA:
	{
		while (true) {
			uint32_t length = ReadNext4AsUint32(file);
			uint32_t typeID = ReadNext4AsUint32(file);
			if (CriticalChunk::ID_IDAT == typeID) {
				auto dataChunk = PNGDataChunk(length, typeID);
				dataChunk.ReadDataAndCRC(file);
				if (dataChunk._data[0] != static_cast<BYTE>(8)) {
					throw PNGReadingException("Decompress failure: unexpected compression method.");
				}

				// Decompress
				
				DEFLATEDecoder decoder;
				try
				{
					decoder.Init(std::unique_ptr<IDEFLATEDataSource>(new DEFLATEDataSourceRawBytes(dataChunk._data.data() + 2)));
					decoder.Decompress();
				}
				catch (DecompressException de)
				{
					throw PNGReadingException("Decompress failure: decompressing failed. Internal Exception string: " + de.message);
				}
				decompressedRawBytes.insert(decompressedRawBytes.end(), decoder.GetOutput().begin(), decoder.GetOutput().end());
			}
			else if (CriticalChunk::ID_IEND) {
				// TODO: 
				break;
			}
			else {
				auto irrelaventChunk = PNGDataChunk(length, typeID);
				irrelaventChunk.ReadDataAndCRC(file);
				if (irrelaventChunk.IsCritical()
					&& irrelaventChunk._typeID != CriticalChunk::ID_PLTE) {
					throw PNGReadingException("Not a valid PNG file: invalid critical chunk encountered.");
				}
			}
		}

		// now decompression is done. The actual data should be reconstructed scanline by scanline.
		{
			uint32_t scanlineByteSize;
			// first, compute the byte length per scanline.
			switch (result._colorType)
			{
			case Luxko::Anuthur::PNG_COLOR_TYPE_RGB:
				scanlineByteSize = 3u * result._channelDepth * result._width / 8u;
				break;
			case Luxko::Anuthur::PNG_COLOR_TYPE_GRAYSCALE:
			case Luxko::Anuthur::PNG_COLOR_TYPE_INDEXED:
				scanlineByteSize = result._channelDepth * result._width / 8u;
				break;
			case Luxko::Anuthur::PNG_COLOR_TYPE_GRAYSCALE_ALPHA:
				scanlineByteSize = 2u * result._channelDepth * result._width / 8u;
				break;
			case Luxko::Anuthur::PNG_COLOR_TYPE_RGBA:
				scanlineByteSize = 4u * result._channelDepth * result._width / 8u;
				break;
			default:
				throw PNGReadingException("Impossible.");
				break;
			}

			size_t i = 0u;
			while (i < decompressedRawBytes.size()) {
				// acquire filter type, then reconstruct the line accordingly.
				switch (decompressedRawBytes[i++])
				{
				case 0:
					result._rawImageReaded.insert(result._rawImageReaded.end(),
						decompressedRawBytes.begin() + i, decompressedRawBytes.begin() + i + scanlineByteSize);
					break;
				case 1:
					result._rawImageReaded.push_back(decompressedRawBytes[i++]);
					for (auto j = 1u; j < scanlineByteSize; ++j) {
						auto recona = result._rawImageReaded[result._rawImageReaded.size() - 1u];
						result._rawImageReaded.push_back(recona + decompressedRawBytes[i++]);
					}
					break;
				case 2:
					for (auto j = 0u; j < scanlineByteSize; ++j) {
						auto reconb = static_cast<BYTE>(0);
						auto index = static_cast<int64_t>(result._rawImageReaded.size()) - 1ll - static_cast<int64_t>(scanlineByteSize);
						if (index >= 0) {
							reconb = result._rawImageReaded[index];
						}
						result._rawImageReaded.push_back(reconb + decompressedRawBytes[i++]);
					}
					break;
				case 3:
					for (auto j = 0u; j < scanlineByteSize; ++j) {
						auto recona = static_cast<BYTE>(0);
						if (j > 0u) {
							recona = result._rawImageReaded[result._rawImageReaded.size() - 1u];
						}
						auto reconb = static_cast<BYTE>(0);
						auto index = static_cast<int64_t>(result._rawImageReaded.size()) - 1ll - static_cast<int64_t>(scanlineByteSize);
						if (index >= 0) {
							reconb = result._rawImageReaded[index];
						}
						auto average = static_cast<BYTE>((static_cast<int>(recona) + static_cast<int>(reconb)) / 2);
						result._rawImageReaded.push_back(average + decompressedRawBytes[i++]);
					}
					break;
				case 4:
					for (auto j = 0u; j < scanlineByteSize; ++j) {
						auto recona = static_cast<BYTE>(0);
						if (j > 0u) {
							recona = result._rawImageReaded[result._rawImageReaded.size() - 1u];
						}
						auto reconb = static_cast<BYTE>(0);
						auto index = static_cast<int64_t>(result._rawImageReaded.size()) - 1ll - static_cast<int64_t>(scanlineByteSize);
						if (index >= 0) {
							reconb = result._rawImageReaded[index];
						}
						auto reconc = static_cast<BYTE>(0);
						index -= 1;
						if (index >= 0 && j > 0u) {
							reconc = result._rawImageReaded[index];
						}
						
						result._rawImageReaded.push_back(PaethPredictor(recona,reconb,reconc) + decompressedRawBytes[i++]);
					}
					break;
				default:
					throw PNGReadingException("Not a valid PNG file: invalid filter type encountered.");
					break;
				}
			}
		}
	}
		break;
	default:
		throw PNGReadingException("Unrecognized color type.");
		break;
	}
}

Luxko::Anuthur::PNGData::PNGData(PNGData&& pr)
{
	_width = pr._width;
	_height = pr._height;
	_channelDepth = pr._channelDepth;
	_colorType = pr._colorType;
	//_perPixelByteLength = pr._perPixelByteLength;

	_rawImageReaded = std::move(pr._rawImageReaded);
}

uint32_t Luxko::Anuthur::PNGData::ReadNext4AsUint32(FileSystem::File& file)
{
	assert(file.Valid());


	BYTE bytes[4];
	DWORD br = -1;
	if (!file.Read(bytes, 4u, br) || br != 4) throw PNGReadingException("File data corrupted: reading failure.");
	return BS4ToUint32(bytes);
}



uint32_t Luxko::Anuthur::PNGData::BS4ToUint32(const BYTE* data)
{
	uint32_t result = 0u;
	result += static_cast<uint32_t>(data[0]) << 24;
	result += static_cast<uint32_t>(data[1]) << 16;
	result += static_cast<uint32_t>(data[2]) << 8;
	result += static_cast<uint32_t>(data[3]);
	return result;
}

Luxko::Anuthur::PNGData& Luxko::Anuthur::PNGData::operator=(PNGData&& pr)
{
	_width = pr._width;
	_height = pr._height;
	_channelDepth = pr._channelDepth;
	_colorType = pr._colorType;
	//_perPixelByteLength = pr._perPixelByteLength;

	_rawImageReaded = std::move(pr._rawImageReaded);

	return *this;
}


void Luxko::Anuthur::PNGDataChunk::ReadDataAndCRC(FileSystem::File& file)
{
	DWORD br = -1;
	if (_byteLength != 0u) {
		if (!file.Read(_data.data(), static_cast<DWORD>(_byteLength), br)
			|| br != _byteLength) {
			throw PNGReadingException("File data corrupted: reading failure.");
		}
	}
	if (!file.Read(&_crc, 1u, br) || br != 1) {
		throw PNGReadingException("File data corrupted: reading failure.");
	}
}
