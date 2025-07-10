#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	// CLight 컨테이너로부터 특정 인덱스의 LIGHT_DESC 를 가져옴.
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);

private:
	list<class CLight*>				m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

NS_END