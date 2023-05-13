#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace gui
{
	inline bool open = true;
	inline bool setup = false;
	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };
	inline WNDPROC originalWindowProcess = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline LPDIRECT3D9 d3d9 = nullptr;

	bool SetupWindowClass(const char* windowClassName) noexcept;
	void DestroyWindowClass() noexcept;
	bool SetupWindow(const char* windowName) noexcept;
	void DestroyWindow() noexcept;
	bool SetupDirectX() noexcept;
	void DestroyDirectX() noexcept;
	void Setup();
	void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;
	void Destroy() noexcept;
	void Render() noexcept;
}

namespace features {
	inline bool godmode = false;
	inline bool modifyWeaponDamage = false;
	inline int weaponDamage = 100;
	inline bool instantBreak = false;
	inline bool maxPickaxePower = false;
	inline bool flyNoClip = false;
	inline bool fly = false;
	inline int flySpeed = 5;
	inline bool cloudJump = false;
	inline bool infiniteMana = false;
	inline bool noWeaponCooldown = false;
	inline bool infiniteFlyTime = false;
	inline bool infiniteSpelunkerPotion = false;
}