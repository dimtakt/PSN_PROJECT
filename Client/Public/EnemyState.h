#pragma once
#include "State.h"


NS_BEGIN(Client)

class CEnemyAI;

class CEnemyState abstract : public CState
{
protected:
    explicit CEnemyState() = default;
    virtual ~CEnemyState() = default;

public:
    virtual HRESULT Initialize() = 0;
    virtual void Enter(CComponent* pOwner) = 0;
    virtual void Update(CComponent* pOwner, _float fTimeDelta) = 0;
    virtual void Exit(CComponent* pOwner) = 0;

public:
    virtual void Free();
};

NS_END