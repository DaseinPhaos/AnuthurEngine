//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "RendererPCH.h"

namespace Luxko {
	namespace DXGI {
		enum class ANUTHURRENDERER_API Error :HRESULT {
			DeviceHung = DXGI_ERROR_DEVICE_HUNG, // The application's device failed due to badly formed commands sent by the application.
												// This is a design time issue that should be investigated and fixed.

			DeviceRemoved = DXGI_ERROR_DEVICE_REMOVED, // The video card has been physically removed, or a driver upgrade has occurred.
														// The application should destroy and recreate the device.

			DeviceReset = DXGI_ERROR_DEVICE_RESET, // The device failed due to badly formed command.
													// This is a run-time issue. The application should destroy and recreate the device.

			DriverInternalError = DXGI_ERROR_DRIVER_INTERNAL_ERROR, // The driver encountered a problem and was put into removed state.

			FrameStatisticsDisjoint = DXGI_ERROR_FRAME_STATISTICS_DISJOINT, // An event interrupted the gathering of presentation statistics.

			VidpnSourceInUse = DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE, // The application's acquisition of exclusive ownership of an output failed because other applications already own it.

			InvalidCall = DXGI_ERROR_INVALID_CALL, // The application provided invalid parameter data.

			MoreData = DXGI_ERROR_MORE_DATA, // The buffer supplied by the application is not big enough to hold the requested data.

			NonExclusive = DXGI_ERROR_NONEXCLUSIVE, // A global counter resource is in use, thus the D3D device can't use it for now.

			CurrentlyUnavailable = DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, // The resource or request is not currently available, but might be later.

			NotFound = DXGI_ERROR_NOT_FOUND,

			RemoteClientDisconnected = DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED, // Reserved.

			RemoteOutofMemory = DXGI_ERROR_REMOTE_OUTOFMEMORY, // Reserved.

			WasStillDrawing = DXGI_ERROR_WAS_STILL_DRAWING, // The GPU was busy at the moment when a call was to perform an operation.

			Unsupported = DXGI_ERROR_UNSUPPORTED, // The requested functionality is not supported by the device or the driver.

			AccessLost = DXGI_ERROR_ACCESS_LOST, // The desktop duplication interface is invalid.

			WaitTimeout = DXGI_ERROR_WAIT_TIMEOUT, // The time-out interval elapsed before the next desktop frame was available.

			SessionDisconnected = DXGI_ERROR_SESSION_DISCONNECTED, // The Remote Desktop Services session is currently disconnected.

			RestrictToOutputStale = DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE, // The DXGI output is now disconnected or changed.

			CannotProtectContent = DXGI_ERROR_CANNOT_PROTECT_CONTENT, // DXGI can't provide content on the swap chain. Typically cause by an older driver.

			AccessDenied = DXGI_ERROR_ACCESS_DENIED, // You tried to use a resource to which you don't have required access privilege.

			NameAlreadyExists = DXGI_ERROR_NAME_ALREADY_EXISTS, // The supplied name of a resource is already associated with some other.

			SdkComponentMissing = DXGI_ERROR_SDK_COMPONENT_MISSING, 

			Ok = S_OK // Method succeeded without error.


		};


		using FactoryPtr = Microsoft::WRL::ComPtr<IDXGIFactory4>;
		using AdapterPtr = Microsoft::WRL::ComPtr<IDXGIAdapter1>;

		ANUTHURRENDERER_API FactoryPtr CreateFactory();
		
		ANUTHURRENDERER_API AdapterPtr GetWarpAdapter(IDXGIFactory4* pFactory);

		ANUTHURRENDERER_API AdapterPtr GetFirstHardwareAdapter(IDXGIFactory4* pFactory);
	}
}

