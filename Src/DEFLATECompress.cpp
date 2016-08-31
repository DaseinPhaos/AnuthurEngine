//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "DEFLATECompress.h"

static uint32_t DefaultCodeLengthCodeAlphabet[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

void Luxko::DEFLATEDecoder::InterpretLengthCode(uint32_t codeValue, const DEFLATETreeNode* distanceTreeRoot, const uint32_t* distanceIndexAlphabet)
{
	uint32_t length;
	//uint32_t distance;
	if (codeValue < 265) {
		// [257..264], no extra bits
		length = codeValue - 254;
	}
	else if (codeValue < 269) {
		// [265..268], 1 extra bits
		length = (codeValue - 265) * 2 + 11;
		length += GetNextNBit(1)._data;
	}
	else if (codeValue < 273) {
		// [269..272], 2 extra bits
		length = (codeValue - 269) * 4 + 19;
		length += GetNextNBit(2)._data;
	}
	else if (codeValue < 277) {
		// [273..276], 3 extra bits
		length = (codeValue - 273) * 8 + 35;
		length += GetNextNBit(3)._data;
	}
	else if (codeValue < 281) {
		// [277..280], 4 extra bits
		length = (codeValue - 277) * 16 + 67;
		length += GetNextNBit(4)._data;
	}
	else if (codeValue < 285) {
		// [281..284], 5 extra bits
		length = (codeValue - 281) * 32 + 131;
		length += GetNextNBit(5)._data;
	}
	else if (codeValue == 285) {
		length = 258;
	}
	else {
		throw DecompressException("Invalid ll code, data corrupted.");
	}


	// Now we try to interpret the distance
	while (!distanceTreeRoot->isLeaf) {
		if (GetCurrentBit()) {
			distanceTreeRoot = distanceTreeRoot->oneChild;
		}
		else {
			distanceTreeRoot = distanceTreeRoot->zeroChild;
		}
		AdvanceByOneBit();
	}
	uint32_t distance;
	uint32_t distanceIndex = distanceTreeRoot->valueIndex;
	if (distanceIndexAlphabet != nullptr) {
		distanceIndex = distanceIndexAlphabet[distanceIndex];
	}
	if (distanceIndex < 4) {
		distance = distanceIndex + 1;
	}
	else {
		auto offset = distanceIndex - 4;
		auto currentPower = offset / 2 + 1;
		auto distOffset = static_cast<uint32_t>(4);
		for (auto i = 1u; i < currentPower; ++i) {
			distOffset += 0x1 << (i + 1);
		}
		if (offset & 0x1) {
			distOffset += 0x1 << (currentPower);
		}
		distance = distOffset + 1 + GetNextNBit(currentPower)._data;
	}

	// Now copy these bytes into output
	auto beginIter = _output.end() - distance;
	auto endIter = beginIter + length;
	_output.insert(_output.end(), beginIter, endIter);
}

Luxko::DEFLATECodeTree Luxko::DEFLATEDecoder::ConstructCodeTreeDynamic(const uint32_t* clAlphabet, const DEFLATECodeTree& clTree, size_t clSize, std::vector<uint32_t>& actualIndexAlphabet)
{
	// Now interpret LL code tree
	auto llCodeLengthCounter = static_cast<uint32_t>(0u);
	actualIndexAlphabet.clear();
	auto acutalCodeTreeConstructionMaterial = std::vector<std::pair<uint32_t, uint32_t>>();
	uint32_t previousCLCode = 0u;
	while (llCodeLengthCounter < clSize) {
		// interpret code lengths for ll table
		auto CLNode = clTree.GetRoot();
		while (!CLNode->isLeaf) {
			if (CurrentBitSet()) {
				CLNode = CLNode->oneChild;
			}
			else {
				CLNode = CLNode->zeroChild;
			}
			AdvanceByOneBit();
		}

		uint32_t LLCode;
		size_t advanceCount;
		auto codeLengthCode = clAlphabet[CLNode->valueIndex];
		if (codeLengthCode < 16u) {
			// literal
			LLCode = codeLengthCode;
			advanceCount = 1u;
		}
		else if (codeLengthCode == 16u) {
			LLCode = previousCLCode;
			auto ebToken = GetNextNBit(2);
			advanceCount = 3u + ebToken._data;
		}
		else if (codeLengthCode == 17u) {
			LLCode = 0u;
			auto ebToken = GetNextNBit(3);
			advanceCount = 3u + ebToken._data;
		}
		else if (codeLengthCode == 18u) {
			LLCode = 0u;
			auto ebToken = GetNextNBit(7);
			advanceCount = 11u + ebToken._data;
		}



		if (LLCode > 0u) {
			for (auto i = 0u; i < advanceCount; ++i) {
				actualIndexAlphabet.push_back(llCodeLengthCounter + i);
			}
			acutalCodeTreeConstructionMaterial.emplace_back(advanceCount, LLCode);
		}

		llCodeLengthCounter += advanceCount;
	}

	return DEFLATECodeTree::Construct(acutalCodeTreeConstructionMaterial);
	
}

char Luxko::DEFLATEDecoder::GetNextByte()
{
	return _input->Pop();
}

void Luxko::DEFLATEDecoder::ResetCurrentToNextByte()
{
	_currentBitIndex = 0x7;
	currentBitMask = 0b10000000;
	_currentByte = _input->Pop();
}

void Luxko::DEFLATEDecoder::Decompress() throw()
{
	//currentBitIndex = 0x7;
	ResetCurrentToNextByte();

	bool finalChunk = false;
	do 
	{
		if (currentBitMask&_currentByte) finalChunk = true;
		AdvanceByOneBit();
		if (CurrentBitSet()) {
			AdvanceByOneBit();
			if (CurrentBitSet()) {
				throw DecompressException("Invalid chunk header, data corrupted");
			}

			// 10, compressed with dynamic Huffman codes.

			// First, read number of ll codes:
			auto llCodeLengthToken = GetNextNBit(5);
			auto nLiteralLength = llCodeLengthToken._data + 257;
			// Then, the number of distance code
			auto dCodeLegnthToken = GetNextNBit(5);
			auto nDistance = dCodeLegnthToken._data + 1;
			// Then, the number of code length code
			auto clCodeLengthToken = GetNextNBit(4);
			auto nCodeLengthCode = clCodeLengthToken._data + 4;
			
			auto actualCodeLengthCodeAlphabet = std::vector<uint32_t>();
			auto actualCodeLengthCodeTreeConstructionMaterial = std::vector<std::pair<uint32_t, uint32_t>>();
			for (auto i = 0u; i < nCodeLengthCode; ++i) {
				auto clct = GetNextNBit(3);
				if(clct._data == 0u) continue;

				actualCodeLengthCodeAlphabet.push_back(DefaultCodeLengthCodeAlphabet[i]);
				actualCodeLengthCodeTreeConstructionMaterial.emplace_back(1u, clct._data);
			}
			auto codeLengthCodeTree = DEFLATECodeTree::Construct(actualCodeLengthCodeTreeConstructionMaterial);

			auto LLIndexAlphabet = std::vector<uint32_t>();
			auto LLCodeTree = ConstructCodeTreeDynamic(actualCodeLengthCodeAlphabet.data(), codeLengthCodeTree,
				nLiteralLength, LLIndexAlphabet);

			auto DistanceIndexAlphabet = std::vector<uint32_t>();
			auto DistanceCodeTree = ConstructCodeTreeDynamic(actualCodeLengthCodeAlphabet.data(), codeLengthCodeTree,
				nDistance, DistanceIndexAlphabet);
			
			auto LLNode = LLCodeTree.GetRoot();
			while (!LLNode->isLeaf) {
				if (CurrentBitSet()) {
					// 1
					LLNode = LLNode->oneChild;
				}
				else {
					LLNode = LLNode->zeroChild;
				}
				AdvanceByOneBit();
			}

			// now LLNode is leaf, current bit points to the next bit. We can then interpret the value
			auto LLValue = LLIndexAlphabet[LLNode->valueIndex];
			if (LLValue == 256) {
				//EOC, proceed next chunk
				continue;
			}
			else if (LLValue < 256) {
				// string literal, copy it to output
				_output.push_back(static_cast<char>(LLValue));
			}
			else {
				// length code, interpret the <length, distance> pair
				InterpretLengthCode(LLValue, DistanceCodeTree.GetRoot(), DistanceIndexAlphabet.data());

			}
		}
		else {
			AdvanceByOneBit();
			if (CurrentBitSet()) {
				// 01, compressed with fixed Huffman codes.
				static auto fixedLLHuffmanCode = DEFLATECodeTree::Construct(
				{
					{144, 8}, {112, 9}, {24, 7}, {8, 8}
				}
				);
				static auto fixedDistHuffmanCode = DEFLATECodeTree::Construct(
				{ { 32, 5 } }
				);
				
				auto LLNode = fixedLLHuffmanCode.GetRoot();
				while (!LLNode->isLeaf) {
					if (CurrentBitSet()) {
						// 1
						LLNode = LLNode->oneChild;
					}
					else {
						LLNode = LLNode->zeroChild;
					}
					AdvanceByOneBit();
				}
				// now LLNode is leaf, current bit points to the next bit. We can then interpret the value
				if (LLNode->valueIndex == 256) {
					//EOC, proceed next chunk
					continue;
				}
				else if (LLNode->valueIndex < 256) {
					// string literal, copy it to output
					_output.push_back(static_cast<char>(LLNode->valueIndex));
				}
				else {
					// length code, interpret the <length, distance> pair
					InterpretLengthCode(LLNode->valueIndex, fixedDistHuffmanCode.GetRoot(), nullptr);

				}
			}
			else {
				// 00, no compression.
				auto length = static_cast<unsigned int>(GetNextByte());
				//length = length << 8;
				length += (static_cast<unsigned int>(GetNextByte())<<8);
				auto nlength = static_cast<unsigned int>(GetNextByte());
				nlength = nlength << 8;
				nlength += static_cast<unsigned int>(GetNextByte());
				if (length != (~nlength)) {
					throw DecompressException("Invalid data length, data corrupted");
				}
				std::vector<char> raw(length);
				_input->Read(raw.data(), length);
				_output.insert(_output.end(), raw.begin(), raw.end());
				ResetCurrentToNextByte();
			}
		}
		
	} while (finalChunk == false);

}



Luxko::TinyBitSegment Luxko::DEFLATEDecoder::GetNextNBit(uint32_t length)
{
	auto result = TinyBitSegment(length);
	auto currentbitLeft = static_cast<uint32_t>(_currentBitIndex + 1);
	while (length > currentbitLeft) {
		result._data += (_currentByte << (length - currentbitLeft));
		length -= currentbitLeft;
		ResetCurrentToNextByte();
	}
	result._data += (_currentByte >> (0x8 - length));
	_currentBitIndex -= length;
	if (_currentBitIndex < 0) ResetCurrentToNextByte();
	result._data = result._data&result.GetMask();
	return result;
}

void Luxko::DEFLATEDecoder::AdvanceByOneBit()
{
	_currentBitIndex -= 1;
	currentBitMask = currentBitMask >> 1;
	if (currentBitMask == 0) {
		_currentBitIndex = 0x7;
		currentBitMask = 0b10000000;
		_currentByte = GetNextByte();
	}
}

bool Luxko::DEFLATEDecoder::CurrentBitSet()
{
	return (currentBitMask&_currentByte) != 0;
}

unsigned int Luxko::DEFLATEDecoder::GetCurrentBit()
{
	return CurrentBitSet() ? 1u : 0u;
}

unsigned char Luxko::DEFLATEDataSourceIstream::Peek() const
{
	return static_cast<unsigned char>(_i.peek());
}

unsigned char Luxko::DEFLATEDataSourceIstream::Pop()
{
	char c;
	_i >> c;
	return static_cast<unsigned char>(c);
}

bool Luxko::DEFLATEDataSourceIstream::Good() const
{
	return _i.good();
}

void Luxko::DEFLATEDataSourceIstream::Read(char* dst, uint32_t length)
{
	_i.read(dst, length);
}

Luxko::DEFLATECodeTree Luxko::DEFLATECodeTree::Construct(
	const std::vector<std::pair<uint32_t/* alphabetIndexCount */, uint32_t/* bitDepth */>>&depths)
{
	DEFLATECodeTree dct;
	std::map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> orderedDepths;
	std::pair<uint32_t, uint32_t> startIndexIndexCountPair(0u, 0u);
	for (auto& item : depths) {
		startIndexIndexCountPair.first = startIndexIndexCountPair.first + startIndexIndexCountPair.second;
		startIndexIndexCountPair.second = item.first;
		if (orderedDepths.find(item.second) == orderedDepths.end()) {
			orderedDepths[item.second] = std::vector<std::pair<uint32_t, uint32_t>>();
		}
		orderedDepths[item.second].push_back(startIndexIndexCountPair);
	}

	auto startTBS = TinyBitSegment(0);
	for (auto& item : orderedDepths) {
		auto lDelta = item.first - startTBS._length;
		startTBS._length = item.first;
		startTBS._data = (startTBS._data << lDelta) + 1;
		if (startTBS._data == 0x1u) startTBS._data == 0x0u;
		for (auto& segment : item.second) {
			for (auto i = 0u; i < segment.second; ++i) {
				dct.Insert(startTBS, segment.first + i);
				startTBS._data = startTBS._data + 1;
			}
		}
	}

	return std::move(dct);
}

void Luxko::DEFLATECodeTree::Insert(TinyBitSegment tb, uint32_t value)
{
	DEFLATETreeNode* currentNode = _root;
	auto mask = static_cast<uint32_t>(0x1) << (tb._length - 1);
	while (mask != 0u) {
		if (tb._data & mask) {
			// go to one child, if don't have, construct it
			if (currentNode->oneChild == nullptr) {
				_nodes.push_back(std::make_unique<DEFLATETreeNode>());
				auto nc = _nodes.back().get();
				currentNode->oneChild = nc;
			}
			currentNode = currentNode->oneChild;
		}
		else {
			// go to zero child, if don't have construct it
			if (currentNode->zeroChild == nullptr) {
				_nodes.push_back(std::make_unique<DEFLATETreeNode>());
				auto nc = _nodes.back().get();
				currentNode->zeroChild = nc;
			}
			currentNode = currentNode->zeroChild;
		}
		mask = mask >> 1;
	}
	// now we've reached the leaf node, where value should be inserted
	currentNode->isLeaf = true;
	currentNode->valueIndex = value;
}

unsigned char Luxko::DEFLATEDataSourceRawBytes::Peek() const
{
	return _ptr[_i];
}

unsigned char Luxko::DEFLATEDataSourceRawBytes::Pop()
{
	return _ptr[_i++];
}

void Luxko::DEFLATEDataSourceRawBytes::Read(char* dst, uint32_t length)
{
	memcpy(dst, _ptr + _i, length);
	_i += length;
}

bool Luxko::DEFLATEDataSourceRawBytes::Good() const
{
	true;
}
