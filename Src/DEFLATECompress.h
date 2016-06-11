//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once

#include "CommonHeader.h"
#include <cstddef>
#include <sstream>
#include <memory>
#include <map>


// DEFLATE compression is a general purpose compression proposed at
// www.ietf.org/rfc/rfc1951.txt
//
// When using Huffman coding, the "deflate" format ensures that
// - All codes of a given bit length have lexicographically consecutive
//   values, in the same order as the symbols they represent.
// - Shorter codes lexicographically precede longer codes.
//
// With this rule, the Huffman code can be defined just by giving the
// bit lengths of the codes for each symbol of the alphabet in order.
//
//
// DEFLATE compressed data is stored in bytes separated in blocks.
// Each block of compressed data begins with 3 bits containing
// first bit BFINAL, next2 BTYPE.
// BFINAL == 0x1 iff. this is the last block of the data set.
// BTYPE == 00, no compression
//			01, compressed with fixed Huffman codes
//			10, compressed with dynamic Huffman codes
//			11, reserved. Encountering this is considered a syntax error.
//
// If encountered a non-compressed block, bits up to the next byte boundaries
// are ignored, following with
//   0   1   2   3   4...
// +---+---+---+---+==========================+
// |  LEN  | NLEN  | ... LEN bytes of data... |
// +---+---+---+---+==========================+
// LEN is the number of data bytes in this block. NLEM == ~LEN
//
// Compressed blocks
// Encoded data blocks consist of sequences of symbols drawn from three
// alphabets: literal bytes from the alphabet of byte values (0..255),
// or <l, bd> pairs, where l is drawn from (3..258) and bd from (1..32,768).
//
// The literal and length alphabet are in fact merged into a single alphabet
// (0..285), where (0..255) represent literal bytes, 256 indicates EOB, and
// (257..285) represent length codes as follows:
//
//      Extra                 Extra                 Extra
// Code Bits  Length	 Code Bits  Length	   Code Bits  Length
// ---- ---- -------     ---- ---- -------     ---- ---- --------
// 257   0      3        267    1   15-16      277   4    67-82
// 258   0      4        268    1   17-18      278   4    83-98
// 259   0      5        269    2   19-22      279   4    99-114
// 260   0      6        270    2   23-26      280   4    115-130
// 261   0      7        271    2   27-30      281   5    131-162
// 262   0      8        272    2   31-34      282   5    163-194
// 263   0      9        273    3   35-42      283   5    195-226
// 264   0     10        274    3   43-50      284   5    227-257
// 265   1   11-12       275    3   51-58      285   0    258
// 266   1   13-14       276    3   59-66
//
// The extra bits should be interpreted as a machine integer, stored
// with the most significant bit first. It indicates the offset from
// the base length of that code.
//
// The backward distance alphabet is as follows:
//      Extra                 Extra                 Extra
// Code Bits   Dist		 Code Bits   Dist	   Code Bits   Dist
// ---- ---- -------     ---- ---- -------     ---- ---- -----------
//  0    0      1         10   4    33-48       20    9   1025-1536
//  1    0      2         11   4    49-64       21    9   1537-2048
//  2    0      3         12   5    65-96       22   10   2049-3072
//  3    0      4         13   5    97-128      23   10   3073-4096
//  4    1     5-6        14   6   129-192      24   11   4097-6144
//  5    1     7-8        15   6   193-256      25   11   6145-8192
//  6    2     9-12       16   7   257-384      26   12   8193-12288
//  7    2    13-16       17   7   385-512      27   12  12289-16384
//  8    3    17-24       18   8   513-768      28   13  16385-24576
//  9    3    25-32       19   8  769-1024      29   13  24577-32768
//
//********************************************
// When compressing with fixed Huffman codes (i.e. BTYPE == 01), the
// Huffman codes for two alphabets listed above are fixed, and are
// not represented explicitly in the data. For literal/length alphabet,
// the Huffman codes are:
//Lit Value  Bits  Codes
//---------  ----  -------------------------
//  0-143     8      0011 0000 - 1011 1111
// 144-255    9    1 1001 0000 - 1 1111 1111
// 256-279    7       000 0000 - 001 0111
// 280-287    8      1100 0000 - 1100 0111
//
// Distance codes 0-31 are represented by (fixed-length) 5-bit codes,
// with possible additional extra bits listed above.
//
//********************************************
// When compressed with dynamic Huffman codes (i.e. BTYPE == 10), the
// codes for the two alphabets appear immediately after the header,
// before the actual compressed data.
// Codes are defined by a sequence of code length, with rules specified
// above. The code length sequence itself is compressed using Huffman
// code, with the alphabet as follows:
//
// 0-15: Represent code lengths 0-15
//   16: Copy previous code length. The next 2 bits indicates repeat
//       times, mapping from [0..3] to [3..6].
//   17: Repeat 0 code length for [3..10] times, the next 3 bits indicates
//       repeat times, rules as specified above.
//   18: Repeat 0 code length for [11-138] times, next 7 bits indicates
//       repeat times.
//
// In this alphabet, a code length of 0 indicates that the corresponding
// symbol won't occur in the block, thus doesn't participate the dynamic
// Huffman code construction process.
// One distance code of zero bits means that there are no distance codes
// used at all.
//
// The layout of this segment is as follows:
// - 5 Bits: HLIT == NLIT - 257, NLIT == number of Literal/Length codes.
// - 5 Bits: HDIST == NDIST - 1, NDIST == number of distance codes.
// - 4 Bits: HCLEN == NCLEN - 4, NCLEN == number of code length codes.
// - 3*NCLEN Bits: code lengths for the code length alphabet given above,
//               in the order 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12,
//               3, 13, 2, 14, 1, 15. These are then interpreted as 3-bit
//               integers.
// - Code lengths for the literal/length alphabet, encoded using the
//   code length Huffman code.
// - Code lengths for the distance alphabet, encoded using the....
// - The actual compressed data of the block, encoded using the literal/length
//   and distance Huffman codes.
// - The literal/length symbol 256, representing EOF, encoded using the literal/
//   length Huffman code.

namespace Luxko {
	class DecompressException {
	public:
		explicit DecompressException(std::string&& msg, bool isFatal = true)
			:message(msg), isFatal(isFatal){ }
		bool isFatal;
		std::string message;
	};

	class TinyBitSegment {
	public:
		TinyBitSegment() {}
		TinyBitSegment(const TinyBitSegment& tb) = default;
		TinyBitSegment& operator=(const TinyBitSegment& tb) = default;
		TinyBitSegment(uint32_t l) { _data = static_cast<uint32_t>(0x0u); _length = l; }

		bool operator==(TinyBitSegment tb) {
			return _length == tb._length && _data == tb._data;
		}
		bool operator!=(TinyBitSegment tb) {
			return !((*this) == tb);
		}
		bool operator<(TinyBitSegment tb) {
			if (_length < tb._length) {
				auto delta = tb._length - _length;
				auto ndata = _data << delta;
				return ndata < tb._data;
			}
			else {
				auto delta = _length - tb._length;
				auto ndata = tb._data << delta;
				return _data < ndata;
			}
		}
		bool operator<=(TinyBitSegment tb) {
			return (*this) == tb || (*this) < tb;
		}
		bool operator>(TinyBitSegment tb) {
			return !((*this) <= tb);
		}
		bool operator>=(TinyBitSegment tb) {
			return !((*this) < tb);
		}

		inline uint32_t GetMask() { return (static_cast<uint32_t>(0x1u) << _length) - 1; }

		uint32_t _data;
		uint32_t _length;
	};



	struct DEFLATETreeNode{
		DEFLATETreeNode(bool isLeaf = false, uint32_t value = 0u)
			:zeroChild(nullptr), oneChild(nullptr), isLeaf(isLeaf), valueIndex(value) {}
		DEFLATETreeNode* zeroChild;
		DEFLATETreeNode* oneChild;
		bool isLeaf;
		uint32_t valueIndex;
		
	};


	class DEFLATECodeTree {
	public:
		DEFLATECodeTree() {
			_nodes.push_back(std::make_unique<DEFLATETreeNode>());
			_root = _nodes.back().get();
		}
		DEFLATECodeTree(const DEFLATECodeTree&) = delete;
		DEFLATECodeTree& operator=(const DEFLATECodeTree&) = delete;
		DEFLATECodeTree(DEFLATECodeTree&& dct) {
			_root = dct._root;
			_nodes = std::move(dct._nodes);
		}
		DEFLATECodeTree& operator=(DEFLATECodeTree&& dct) {
			_root = dct._root;
			_nodes = std::move(dct._nodes);
			return (*this);
		}

		static DEFLATECodeTree Construct(const std::vector<std::pair<uint32_t/* alphabetIndexCount */,
			uint32_t/* bitDepth */>>&depths);

		void Insert(TinyBitSegment tb, uint32_t value);

		const DEFLATETreeNode* GetRoot()const { return _root; }

	private:
		
		DEFLATETreeNode* _root;
		std::vector<std::unique_ptr<DEFLATETreeNode>> _nodes;
	};

	class IDEFLATEDataSource abstract {
	public:
		virtual ~IDEFLATEDataSource() {}
		virtual unsigned char Peek()const = 0;
		virtual unsigned char Pop() = 0;
		virtual void Read(char* dst, uint32_t length) = 0;
		virtual bool Good()const = 0;
	};

	class DEFLATEDataSourceRawBytes : public IDEFLATEDataSource {
	public:
		explicit DEFLATEDataSourceRawBytes(const BYTE* bytes) :_ptr(bytes),_i(0) {};
		virtual ~DEFLATEDataSourceRawBytes() {}
		virtual unsigned char Peek() const override;


		virtual unsigned char Pop() override;


		virtual void Read(char* dst, uint32_t length) override;


		virtual bool Good() const override;

	private:
		const BYTE* _ptr;
		size_t _i;
	};

	class DEFLATEDataSourceIstream : public IDEFLATEDataSource {
	public:
		explicit DEFLATEDataSourceIstream(std::istream& i) :_i(i) {}
		virtual ~DEFLATEDataSourceIstream() {}

		

	

		virtual unsigned char Peek() const override;


		virtual unsigned char Pop() override;


		virtual bool Good() const override;

		std::istream& _i;

		virtual void Read(char* dst, uint32_t length) override;

	};

	class DEFLATEDecoder {
	public:
		DEFLATEDecoder() {}
		DEFLATEDecoder(const DEFLATEDecoder&) = delete;
		DEFLATEDecoder& operator=(const DEFLATEDecoder&) = delete;
		void Init(std::unique_ptr<IDEFLATEDataSource>&& input) {
			_input = std::move(input);
			ResetCurrentToNextByte();
			_output.clear();
		}
		void Decompress()throw();
		const std::vector<unsigned char>& GetOutput()const { return _output; }
	private:
		void InterpretLengthCode(uint32_t codeValue, const DEFLATETreeNode* distanceTreeRoot, const uint32_t* distanceIndexAlphabet = nullptr);
		DEFLATECodeTree ConstructCodeTreeDynamic(const uint32_t* clAlphabet, const DEFLATECodeTree& clTree,
			size_t clSize, std::vector<uint32_t>& actualIndexAlphabet);
		inline char GetNextByte();
		inline void ResetCurrentToNextByte();
		inline TinyBitSegment GetNextNBit(uint32_t length);
		inline void AdvanceByOneBit();
		inline bool CurrentBitSet();
		inline unsigned int GetCurrentBit();
		int _currentBitIndex;
		unsigned char currentBitMask;
		char _currentByte;
		std::unique_ptr<IDEFLATEDataSource> _input;
		std::vector<unsigned char> _output;
		//bool ReadNextChunk(std::stringstream& output);
	};
}