//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"
#include "Delegate.h"
#include "AnyOptional.h"

namespace Luxko {
	class CmdArgParser {
		enum CmdArgType {
			Bool,
			String,
			Int,
			Double
		};
		using arguments_t = std::unordered_map<std::string, Luxko::Any>;
		using namelookup_t = std::unordered_map<std::string, std::string>;

	public:


	private:
		arguments_t _positional_args;
		arguments_t _optional_args;
		namelookup_t _namelookup;
	};
}
