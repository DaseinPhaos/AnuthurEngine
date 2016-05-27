//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#ifdef ANUTHURAPPLICATION_EXPORTS
#define ANUTHURAPPLICATION_API __declspec(dllexport)
#else
#define ANUTHURAPPLICATION_API __declspec(dllimport)
#endif
//#define ANUTHURAPPLICATION_API
#include "CommonHeader.h"

namespace Luxko {
	namespace Application {
		class ANUTHURAPPLICATION_API BaseApp abstract {
		public:
			BaseApp(UINT width, UINT height, std::wstring name);
			virtual ~BaseApp();

			// @Luxko: Runs the application until quit.
			//			This method should be ready to get called
			//			after the application class instance's initialization.
			//			It Delegates work to (in sequence)
			//			RegisterWindowClass,
			//			OnInit, OnEvent, OnUpdate, OnRender, OnDestroy.
			int Run(HINSTANCE hInstance, int nCmdShow);

			void SetCustomWindowText(const std::wstring& text);

		protected:
			virtual void OnInit() = 0;
			virtual void OnUpdate() = 0;
			virtual void OnRender() = 0;
			virtual void OnDestroy() = 0;
			virtual bool OnEvent(MSG msg) = 0;
			virtual void ParseCommandLineArgs() {}
			virtual ATOM RegisterWindowClass(HINSTANCE hInstance);
			std::wstring GetAssetFullPath(const std::wstring& assetName)const;
			
			static LRESULT CALLBACK WindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			UINT _width;
			UINT _height;
			

			HWND _hWindow;
			std::wstring _title;


		private:
			

			std::wstring _assetsPath;

		};
	}
}