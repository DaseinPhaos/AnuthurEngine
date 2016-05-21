#include "D3D12App.h"
#include "HelperMethods.h"

using namespace Luxko;
using namespace Luxko::Anuthur;
class Demo : public D3D12App {
public:
	Demo(UINT width, UINT height, const std::wstring& name) :
		D3D12App(width, height, name) {}
	virtual ~Demo() {}
protected:
	virtual void OnUpdate() override
	{	
	}


	virtual void OnRender() override
	{
		LogFPSToTitle();
		ThrowIfFailed(_mainCmdAllocator->Reset());

		ThrowIfFailed(_mainCmdList->Reset(_mainCmdAllocator.Get(), nullptr));

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Transition.pResource = _swapChainBuffer[_currentBackBufferIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		_mainCmdList->ResourceBarrier(1u, &barrier);

		_mainCmdList->RSSetViewports(1, &_mainViewport);

		_mainCmdList->RSSetScissorRects(1, &_mainScissorRect);

		static FLOAT initColor[4] = { 0.f, 0.5f, 0.1f, 1.f };
		static auto deltaC = 0.01f;
		_mainTimer.Elapse();
		if (_mainTimer.DeltaInMs() > 30.f) {
			_mainTimer.Reset(true);
			initColor[0] += deltaC;
			if (initColor[0] > 1.f
				|| initColor[0] < 0.f) deltaC = -deltaC;
		}
		_mainCmdList->ClearRenderTargetView(GetCurrentBackBufferView(),
		initColor, 0, nullptr);

		_mainCmdList->ClearDepthStencilView(_mainDSVHeap->GetCPUDescriptorHandleForHeapStart(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

		_mainCmdList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), TRUE,
			&_mainDSVHeap->GetCPUDescriptorHandleForHeapStart());

		std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
		_mainCmdList->ResourceBarrier(1, &barrier);

		ThrowIfFailed(_mainCmdList->Close());

		ID3D12CommandList* cmdlsts[] = { _mainCmdList.Get() };
		_cmdQueue->ExecuteCommandLists(1u, cmdlsts);
		FlushCommandQueue();
		ThrowIfFailed(_swapChain->Present(0, 0));
		_currentBackBufferIndex = (_currentBackBufferIndex + 1) % SwapChainBufferCount;
		
	}

};

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Demo demo(800, 600, L"Fuck");
	return demo.Run(hInstance, nShowCmd);
}