#pragma once
#include "gui.h"
#include <vector>

struct Vector2
{
	float x, y;
};

class player
{
private:
	char pad_0000[32]; //0x0000
public:
	bool canMove; //0x0020
private:
	char pad_0021[7]; //0x0021
public:
	Vector2 position; //0x0028
	Vector2 velocity; //0x0030
private:
	char pad_0038[656]; //0x0038
public:
	float flyTime; //0x02C8
private:
	char pad_02CC[308]; //0x02CC
public:
	int32_t maxHealth; //0x0400
private:
	char pad_0404[4]; //0x0404
public:
	int32_t health; //0x0408
	int32_t mana; //0x040C
	int32_t maxMana; //0x0410
private:
	char pad_0414[228]; //0x0414
public:
	bool onGround; //0x04F8
private:
	char pad_04F9[263]; //0x04F9
public:
	int32_t firstAttackCooldown; //0x0600
private:
	char pad_0604[4]; //0x0604
public:
	int32_t secondAttackCooldown; //0x0608
private:
	char pad_060C[4]; //0x060C
public:
	int32_t thirdAttackCooldown; //0x0610
private:
	char pad_0614[568]; //0x0614
public:
	bool cloudJump; //0x084C
};

namespace hooks
{
	inline int vec3one = 0;

	void Setup();
	void Destroy() noexcept;

	constexpr void* VirtualFunction(void* thisptr, size_t index) noexcept
	{
		return (*static_cast<void***>(thisptr))[index];
	}

	DWORD GetAddressFromSignature(std::vector<int> signature, DWORD startaddress = 0, DWORD endaddress = 0);

	using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
	inline EndSceneFn EndSceneOriginal = nullptr;
	long __stdcall EndScene(IDirect3DDevice9* device) noexcept;

	using ResetFn = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
	inline ResetFn ResetOriginal = nullptr;
	HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;

	inline std::vector<int> HurtSig = { 0x55, 0x8b, 0xec, 0x57, 0x56, 0x53, 0x81, 0xec, -1, -1, -1, -1, 0x8b, 0xf1, 0x8d, 0xbd, -1, -1, -1, -1, 0xb9, -1, -1, -1, -1, 0x33, 0xc0, 0xf3, 0xab, 0x8b, 0xce, 0x89, 0x8d, -1, -1, -1, -1, 0x89, 0x95, -1, -1, -1, -1, 0x8b, 0x85, -1, -1, -1, -1, 0x80, 0xb8, -1, -1, -1, -1, -1, 0x74, 0x25 };
	inline int HurtMin = 0x25000000;
	inline int HurtMax = 0x30000000;
	using HurtFn = float(__fastcall*)(void*, void*, int, int, bool, bool, bool, int, bool) noexcept;
	inline HurtFn HurtOriginal = nullptr;
	float __fastcall Hurt(void* __1, void* damageSource, int damage, int hitDirection, bool pvp, bool quiet, bool crit, int cooldownCounter, bool dodgeable) noexcept;

	inline std::vector<int> GetWeaponDamageSig = { 0x55, 0x8b, 0xec, 0x57, 0x56, 0x53, 0x83, 0xec, 0x0c, 0x8b, 0xd9, 0x8b, 0xf2, 0x8b, 0xbe, -1, -1, -1, -1, 0x85, 0xff };
	inline int GetWeaponDamageMin = 0x25000000;
	inline int GetWeaponDamageMax = 0x30000000;
	using GetWeaponDamageFn = int(__fastcall*)(void*, void*) noexcept;
	inline GetWeaponDamageFn GetWeaponDamageOriginal = nullptr;
	int __fastcall GetWeaponDamage(void* __1, void* sItem) noexcept;

	inline std::vector<int> AddDamageSig = { 0x55, 0x8b, 0xec, 0x57, 0x56, 0x53, 0x83, 0xec, 0x0c, 0x8b, 0xf9, 0x8b, 0xf2, 0x8b, 0x55, 0x0c, 0x85, 0xf6, 0x7c, 0x08, 0x81, 0xfe };
	inline int AddDamageMin = 0x22000000;
	inline int AddDamageMax = 0x30000000;
	using AddDamageFn = int(__fastcall*)(void*, int, int, bool) noexcept;
	inline AddDamageFn AddDamageOriginal = nullptr;
	int __fastcall AddDamage(void* __1, int tileId, int damageAmount, bool updateAmount) noexcept;

	inline std::vector<int> get_LocalPlayerSig = { 0xA1, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x3B, 0x50, 0x04, 0x73, 0x05, 0x8B, 0x44, 0x90, 0x08 };
	inline int get_LocalPlayerMin = 0x26000000;
	inline int get_LocalPlayerMax = 0x35000000;
	using get_LocalPlayerFn = int(__fastcall*)() noexcept;
	inline get_LocalPlayerFn get_LocalPlayerOriginal = nullptr;

	inline std::vector<int> UpdateSig = { 0x55, 0x8b, 0xec, 0x57, 0x56, 0x53, 0x81, 0xec, -1, -1, -1, -1, 0x8b, 0xf1, 0x8d, 0xbd, -1, -1, -1, -1, 0xb9, -1, -1, -1, -1, 0x33, 0xc0, 0xf3, 0xab, 0x8b, 0xce, 0x89, 0x8d, -1, -1, -1, -1, 0x89, 0x55, 0xdc, 0x8b, 0x45, 0xdc, 0x3b, 0x05, -1, -1, -1, -1, 0x75, 0x0f };
	inline int UpdateMin = 0x2a000000;
	inline int UpdateMax = 0x50000000;
	using UpdateFn = void(__fastcall*)(void*, int) noexcept;
	inline UpdateFn UpdateOriginal = nullptr;
	void __fastcall Update(void* __1, int whoAmI) noexcept;

	inline std::vector<int> LightingEngineGetColorSig = { 0x55, 0x8b, 0xec, 0x57, 0x56, 0x53, 0x50, 0x8b, 0x5d, 0x0c, 0x8d, 0x71, 0x1c, 0x39, 0x1e, -1, -1, 0x8b, 0x06, 0x03, 0x46, 0x08, 0x3b, 0xc3, -1, -1, 0x8b, 0x46, 0x04 };
	inline int LightingEngineGetColorMin = 0x22000000;
	inline int LightingEngineGetColorMax = 0x28000000;
	/*
	using LightingEngineGetColorFn = int(__stdcall*)(int, int) noexcept;
	inline LightingEngineGetColorFn LightingEngineGetColorOriginal = nullptr;
	int __stdcall LightingEngineGetColor(int x, int y) noexcept;
	*/


	inline std::vector<int> Vector3GetOneSig = { 0x57, 0x56, 0x8b, 0xf9, 0x8b, 0x35, -1, -1, -1, -1, 0x83, 0xc6, 0x04, 0xf3, -1, -1, 0x06, 0x66, -1, -1, 0x07, 0x83, 0xc6, 0x08 };
	inline int Vector3GetOneMin = 0x20000000;
	inline int Vector3GetOneMax = 0x26000000;
	using Vector3GetOneFn = int(__stdcall*)() noexcept;
	inline Vector3GetOneFn Vector3GetOneOriginal = nullptr;
}
