#include "gui.h"
#include "..\ext\imgui\imgui.h"
#include "..\ext\imgui\imgui_impl_win32.h"
#include "..\ext\imgui\imgui_impl_dx9.h"
#include <stdexcept>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool gui::SetupWindowClass(const char* windowClassName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	if (!RegisterClassEx(&windowClass))
	{
		return false;
	}

	return true;
}

void gui::DestroyWindowClass() noexcept
{
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::SetupWindow(const char* windowName) noexcept
{
	window = CreateWindow(windowClass.lpszClassName, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 0, 0, windowClass.hInstance, 0);

	if (!window)
	{
		return false;
	}

	return true;
}

void gui::DestroyWindow() noexcept
{
	if (window)
	{
		DestroyWindow(window);
	}
}

bool gui::SetupDirectX() noexcept
{
	const HMODULE handle = GetModuleHandle("d3d9.dll");

	if (!handle)
	{
		return false;
	}

	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress(handle, "Direct3DCreate9"));

	if (!create)
	{
		return false;
	}

	d3d9 = create(D3D_SDK_VERSION);

	if(!d3d9)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS params = { };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &params, &device) < 0)
	{
		return false;
	}

	return true;
}

void gui::DestroyDirectX() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}

void gui::Setup()
{
	if (!SetupWindowClass("windowass"))
	{
		throw std::runtime_error("Failed to create window class.");
	}

	if (!SetupWindow("windo"))
	{
		throw std::runtime_error("Failed to create window.");
	}

	if (!SetupDirectX())
	{
		throw std::runtime_error("Failed to create device.");
	}

	DestroyWindow();
	DestroyWindowClass();
}

void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{ };
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;
	originalWindowProcess = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void gui::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWindowProcess));

	DestroyDirectX();
}

const char* cooldowns[] = { "Cooldown 1", "Cooldown 2", "Cooldown 3" };

void gui::Render() noexcept
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Terraria Haxx", &open);
	ImGui::BeginTabBar("maintab");
	if (ImGui::BeginTabItem("Combat"))
	{
		ImGui::Checkbox("Godmode", &features::godmode);
		ImGui::Checkbox("Infinite Mana", &features::infiniteMana);
		ImGui::Checkbox("Modify Weapon Damage", &features::modifyWeaponDamage);
		ImGui::SliderInt("Weapon Damage", &features::weaponDamage, 1, 9999);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Movement"))
	{
		ImGui::Checkbox("Fly", &features::fly);
		ImGui::Checkbox("Fly + NoClip", &features::flyNoClip);
		ImGui::SliderInt("Fly Speed", &features::flySpeed, 1, 10);
		ImGui::Checkbox("Infinite Cloud Jump", &features::cloudJump);
		ImGui::Checkbox("Infinite Wings Fly Time", &features::infiniteFlyTime);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Misc"))
	{
		ImGui::Checkbox("Instant Break", &features::instantBreak);
		ImGui::Checkbox("No Weapon / Tool Cooldown", &features::noCooldown);
		if (features::noCooldown) {
			ImGui::Checkbox("Cooldown 1", &features::cooldown1);
			ImGui::Checkbox("Cooldown 2", &features::cooldown2);
			ImGui::Checkbox("Cooldown 3", &features::cooldown3);
		}
		ImGui::Checkbox("Full Bright", &features::fullBright);
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		gui::open = !gui::open;
		if (gui::open)
		{
			while (ShowCursor(true) < 0);
		}
		else
		{
			while (ShowCursor(false) >= 0);
		}
	}

	if (gui::open && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return 1L;
	}

	return CallWindowProc(gui::originalWindowProcess, hWnd, msg, wParam, lParam);
}
