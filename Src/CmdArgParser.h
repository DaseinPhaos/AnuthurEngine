//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"
#include "Delegate.h"

namespace Luxko {
	class CmdArgParser {
		enum class CmdArgType
		{
			Bool,
			Int,
			Float,
			String
		};
	public:
		CmdArgParser() = default;

		// @TODO: Finish this shit..
		template<class T>
		void addArgument(T& target, std::string argName, CmdArgType argType = CmdArgType::String) {
			assert(argName.size() != 0u);

		}
	private:

	};
}
