//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "FileSystem.h"

namespace Luxko {
	namespace Anuthur {

		enum PNG_COLOR_TPYE {
			PNG_COLOR_TYPE_GRAYSCALE = 0,
			PNG_COLOR_TYPE_RGB = 2,
			PNG_COLOR_TYPE_INDEXED = 3,
			PNG_COLOR_TYPE_GRAYSCALE_ALPHA = 4,
			PNG_COLOR_TYPE_RGBA = 6
		};
		
		class PNGReadingException {
		public:
			explicit PNGReadingException(const std::string& msg) :
				ErrorMessage(msg) {}
			~PNGReadingException() = default;

			const std::string ErrorMessage;
		};

		class PNGData {
		public:
			static PNGData ReadFromFile(const std::wstring& fileName);
			PNGData(const PNGData&) = default;
			PNGData(PNGData&& pr);
			PNGData& operator=(const PNGData&) = default;
			PNGData& operator=(PNGData&& pr);
			~PNGData() = default;

			unsigned int GetWidth()const { return _width; }
			unsigned int GetHeight()const { return _height; }
			unsigned int GetChannelDepth()const { return _channelDepth; }
			PNG_COLOR_TPYE GetColorType()const { return _colorType; }
			//unsigned int GetPixelByteLength()const { return _perPixelByteLength; }
			const BYTE* GetDataPtr()const { return _rawImageReaded.data(); }

		private:
			// helper methods.
			PNGData() = default;
			static uint32_t ReadNext4AsUint32(FileSystem::File& file);
			static inline uint32_t BS4ToUint32(const BYTE* data);
			// Data members
			unsigned int _width;
			unsigned int _height;
			unsigned int _channelDepth;
			PNG_COLOR_TPYE	_colorType;
			//unsigned int _perPixelByteLength;

			std::vector<BYTE> _rawImageReaded;

		};

		class PNGDataChunk {
			friend class PNGData;
			bool IsCritical()const {
				static uint32_t criticalMask = static_cast<uint32_t>(0x1u) << 27;
				return (_typeID&criticalMask) == 0u;
			}
		private:
			PNGDataChunk() = default;
			PNGDataChunk(uint32_t length, uint32_t typeID) :_byteLength(length), _typeID(typeID), _data(length) { }
			void ReadDataAndCRC(FileSystem::File& file);
			uint32_t _byteLength;
			uint32_t _typeID;
			std::vector<BYTE> _data;
			uint32_t _crc;
		};
	}
}