#pragma once
#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CComponent;
NS_END

NS_BEGIN(Client)

class CState abstract: public CBase
{
protected:
    explicit CState() = default;
    virtual ~CState() = default;

public:
    virtual HRESULT Initialize() = 0;
    virtual void Enter(CComponent* pOwner) = 0;
    virtual void Update(CComponent* pOwner, _float fTimeDelta) = 0;
    virtual void Exit(CComponent* pOwner) = 0;

public:
    virtual void Free() { __super::Free(); };
};

NS_END