#pragma once

#include "global.h"
#include "MyUtility.h"




enum class KEY_STATE
{
	TAP,     // 막 눌림
	PRESSED, // TAP 이후에도 계속 눌려있음
	AWAY,    // 막 뗀 상황
	NONE,    // 안눌린 상태
};

struct tKeyInfo
{
	KEY_STATE eState;     // 현재 키 상태
	bool      bPrevCheck; // 이전 프레임에서 키가 눌렸는지 체크
};

#define KEY_TAP(key)     KEY_STATE::TAP == Key::GetInst()->GetKeyState(key)
#define KEY_PRESSED(key) KEY_STATE::PRESSED == Key::GetInst()->GetKeyState(key)
#define KEY_AWAY(key)    KEY_STATE::AWAY == Key::GetInst()->GetKeyState(key)

class Key
{
public:
	Key();
	~Key();

private:
	// SingleTon
	static Key* m_pInst;
public:
	static Key* GetInst() {
		if (nullptr == m_pInst) m_pInst = new Key;
		return m_pInst;
	}
	static void Destroy() {
		if (nullptr != m_pInst) {
			delete m_pInst;
			m_pInst = nullptr;
		}
	}

private:
	std::vector<tKeyInfo> m_vecKey;
	XMFLOAT2             m_vMousePos;
	XMFLOAT2             m_vMousePrevPos;
	XMFLOAT2             m_vMouseDir;

	std::bitset<(UINT)KEY::KEY_END> m_arrEnabled;

public:
	void init();
	void update();

	void SetEnableInput(KEY _key, bool _enable);
	void EnableAll(bool _enable);

public:
	KEY_STATE GetKeyState(KEY _eKey) const
	{
		if (m_arrEnabled[(UINT)_eKey])
		{
			return m_vecKey[(UINT)_eKey].eState;
		}
		return KEY_STATE::NONE;
	}

	XMFLOAT2 GetMousePos() const { return m_vMousePos; }
	XMFLOAT2 GetMouseDir() const { return m_vMouseDir; }

	XMFLOAT2 GetMouseDelta() const
	{
		XMFLOAT2 vDelta = XMFLOAT2(m_vMousePos.x - m_vMousePrevPos.x, m_vMousePos.y - m_vMousePrevPos.y);
		return vDelta;
	}

};

