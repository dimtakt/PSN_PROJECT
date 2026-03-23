#pragma once
#include "EnemyState.h"

NS_BEGIN(Client)

class CEnemyAnyState final : public CEnemyState
{
private:
    explicit CEnemyAnyState() = default;
    virtual ~CEnemyAnyState() = default;

public:
    virtual HRESULT Initialize();
    virtual void Enter(CComponent* pOwner);
    virtual void Update(CComponent* pOwner, _float fTimeDelta);
    virtual void Exit(CComponent* pOwner);

    _bool Check_Transition(CComponent* pOwner);

public:
    static CEnemyAnyState* Create();
    virtual void Free();
};


NS_END