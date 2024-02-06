#include "hooks.h"
#include <stdexcept>
#include <intrin.h>
#include "..\ext\minhook\minhook.h"
#include "..\ext\imgui\imgui.h"
#include "..\ext\imgui\imgui_impl_win32.h"
#include "..\ext\imgui\imgui_impl_dx9.h"
#include "iostream"

DWORD GetFnFromSig(std::vector<int> sig, int min, int max)
{
	DWORD Entry = hooks::GetAddressFromSignature(sig, min, max);
	if (Entry == NULL)
	{
		// God forbid
		std::cout << "yeah, you've got a problem, whatever sig you're on right now it uhh def isn't within the memory bounds, you might want to fix that" << std::endl;
		Entry = hooks::GetAddressFromSignature(sig);
	}
	std::cout << std::hex << Entry << std::endl;
	return Entry;
}

DWORD lighting;
DWORD end;

void __declspec(naked) LightingEngineGetColor() noexcept
{
	__asm {
		cmp features::fullBright, 1
		je Enabled
		push ebp // 1
		mov ebp, esp // 2, 3
		push edi // 4
		push esi // 5
		jmp[end]
	Enabled:
		call[hooks::Vector3GetOneOriginal]
		ret 8
	}
}

void hooks::Setup()
{
	if (MH_Initialize())
	{
		throw std::runtime_error("Unable to initialize minhook.");
	}

	if (MH_CreateHook(VirtualFunction(gui::device, 42), &EndScene, reinterpret_cast<void**>(&EndSceneOriginal)))
	{
		throw std::runtime_error("Unable to hook EndScene().");
	}

	if (MH_CreateHook(VirtualFunction(gui::device, 16), &Reset, reinterpret_cast<void**>(&ResetOriginal)))
	{
		throw std::runtime_error("Unable to hook Reset().");
	}

	if (MH_CreateHook((LPVOID)GetFnFromSig(HurtSig, HurtMin, HurtMax), &Hurt, reinterpret_cast<void**>(&HurtOriginal)))
	{
		throw std::runtime_error("Unable to hook Hurt().");
	}

	if (MH_CreateHook((LPVOID)GetFnFromSig(GetWeaponDamageSig, GetWeaponDamageMin, GetWeaponDamageMax), &GetWeaponDamage, reinterpret_cast<void**>(&GetWeaponDamageOriginal)))
	{
		throw std::runtime_error("Unable to hook GetWeaponDamage().");
	}

	if (MH_CreateHook((LPVOID)GetFnFromSig(AddDamageSig, AddDamageMin, AddDamageMax), &AddDamage, reinterpret_cast<void**>(&AddDamageOriginal)))
	{
		throw std::runtime_error("Unable to hook AddDamage().");
	}

	if (MH_CreateHook((LPVOID)GetFnFromSig(UpdateSig, UpdateMin, UpdateMax), &Update, reinterpret_cast<void**>(&UpdateOriginal)))
	{
		throw std::runtime_error("Unable to hook Update().");
	}

	lighting = GetFnFromSig(LightingEngineGetColorSig, LightingEngineGetColorMin, LightingEngineGetColorMax);

	if (MH_CreateHook((LPVOID)lighting, &LightingEngineGetColor, NULL))
	{
		throw std::runtime_error("Unable to hook LightingEngine::GetColor().");
	}

	get_LocalPlayerOriginal = (hooks::get_LocalPlayerFn)GetFnFromSig(get_LocalPlayerSig, get_LocalPlayerMin, get_LocalPlayerMax);
	Vector3GetOneOriginal = (hooks::Vector3GetOneFn)GetFnFromSig(Vector3GetOneSig, Vector3GetOneMin, Vector3GetOneMax);

	end = lighting + 5;

	if (MH_EnableHook(MH_ALL_HOOKS))
	{
		throw std::runtime_error("Unable to enable hooks.");
	}

	gui::DestroyDirectX();
}

void hooks::Destroy() noexcept
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

DWORD hooks::GetAddressFromSignature(std::vector<int> signature, DWORD startaddress, DWORD endaddress)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (startaddress == 0)
	{
		startaddress = (DWORD)(si.lpMinimumApplicationAddress);
	}
	if (endaddress == 0)
	{
		endaddress = (DWORD)(si.lpMaximumApplicationAddress);
	}

	MEMORY_BASIC_INFORMATION mbi{ 0 };
	DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

	for (DWORD i = startaddress; i < endaddress - signature.size(); i++)
	{
		if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi)))
		{
			if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT))
			{
				i += mbi.RegionSize;
				continue;
			}
			for (DWORD k = (DWORD)mbi.BaseAddress; k < (DWORD)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++)
			{
				for (DWORD j = 0; j < signature.size(); j++)
				{
					if (signature.at(j) != -1 && signature.at(j) != *(BYTE*)(k + j))
					{
						break;
					}
					if (j + 1 == signature.size())
					{
						return k;
					}
				}
			}
			i = (DWORD)mbi.BaseAddress + mbi.RegionSize;
		}
	}
	return NULL;
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	if (_ReturnAddress() == returnAddress)
	{
		return result;
	}

	if (!gui::setup)
	{
		gui::SetupMenu(device);
	}

	if (gui::open)
	{
		gui::Render();
	}

	return result;
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

player* localPlayer = nullptr;

float __fastcall hooks::Hurt(void* __1, void* damageSource, int damage, int hitDirection, bool pvp, bool quiet, bool crit, int cooldownCounter, bool dodgeable) noexcept
{
	if (features::godmode)
	{
		return 0.0f;
	}
	return HurtOriginal(__1, damageSource, damage, hitDirection, pvp, quiet, crit, cooldownCounter, dodgeable);
}

int __fastcall hooks::GetWeaponDamage(void* __1, void* sItem) noexcept
{
	if (features::modifyWeaponDamage)
	{
		return features::weaponDamage;
	}
	return GetWeaponDamageOriginal(__1, sItem);
}

int __fastcall hooks::AddDamage(void* __1, int tileId, int damageAmount, bool updateAmount) noexcept
{
	if (features::instantBreak)
	{
		damageAmount = 999;
	}
	return AddDamageOriginal(__1, tileId, damageAmount, updateAmount);
}

void __fastcall hooks::Update(void* __1, int whoAmI) noexcept
{
	UpdateOriginal(__1, whoAmI);

	int p = get_LocalPlayerOriginal();
	localPlayer = (player*)(p);

	if (features::flyNoClip)
	{
		localPlayer->velocity.x = 0.0f;
		localPlayer->velocity.y = -0.4f; // gravity
		if (GetAsyncKeyState(0x57))
		{
			localPlayer->position.y -= (10 * features::flySpeed);
		}
		if (GetAsyncKeyState(0x53))
		{
			localPlayer->position.y += (10 * features::flySpeed);
		}
		if (GetAsyncKeyState(0x41))
		{
			localPlayer->position.x -= (10 * features::flySpeed);
		}
		if (GetAsyncKeyState(0x44))
		{
			localPlayer->position.x += (10 * features::flySpeed);
		}
	}
	else if (features::fly)
	{
		localPlayer->velocity.x = 0.0f;
		localPlayer->velocity.y = -0.4f; // gravity, but im retarded
		if (GetAsyncKeyState(0x57))
		{
			localPlayer->velocity.y -= (10 * features::flySpeed) + 0.4;
		}
		if (GetAsyncKeyState(0x53))
		{
			localPlayer->velocity.y += (10 * features::flySpeed) - 0.4;
		}
		if (GetAsyncKeyState(0x41))
		{
			localPlayer->velocity.x -= 10 * features::flySpeed;
		}
		if (GetAsyncKeyState(0x44))
		{
			localPlayer->velocity.x += 10 * features::flySpeed;
		}
	}

	if (features::godmode)
	{
		localPlayer->health = localPlayer->maxHealth;
	}

	if (features::cloudJump)
	{
		localPlayer->cloudJump = true;
	}

	if (features::infiniteMana)
	{
		localPlayer->mana = localPlayer->maxMana;
	}

	if (features::noCooldown)
	{
		if (features::cooldown1)
			localPlayer->firstAttackCooldown = 0;
		if (features::cooldown2)
			localPlayer->secondAttackCooldown = 0;
		if (features::cooldown3)
			localPlayer->thirdAttackCooldown = 0;
	}

	if (features::infiniteFlyTime)
	{
		localPlayer->flyTime = 999999;
	}
}

/*
int __stdcall hooks::LightingEngineGetColor(int x, int y) noexcept
{
	if (features::fullBright)
	{
		__asm {
			call [hooks::Vector3GetOneOriginal]
			pop ecx
			pop ebx
			pop esi
			pop edi
			pop ebp
			ret 8
		}
	}

	return LightingEngineGetColorOriginal(x, y);
}
*/
