//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"

namespace Luxko {
	namespace Anuthur {
		enum class ANUTHURRENDERER_API MeshPrimitiveType {
			Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT
		};

		class ANUTHURRENDERER_API IMeshPrimitive abstract {
		public:

		};
	}
}