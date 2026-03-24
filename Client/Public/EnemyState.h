#pragma once
#include "State.h"


NS_BEGIN(Client)

class CEnemyAI;
class CEnemy;

class CEnemyState abstract : public CState
{
protected:
    explicit CEnemyState() = default;
    virtual ~CEnemyState() = default;

protected:
    static CEnemyAI* Get_EnemyAI(CComponent* pOwner);
    static CEnemy* Get_Enemy(CComponent* pOwner);
    static void Apply_StateTag(CEnemy* pEnemy, const _wstring& strStateTag);

public:
    virtual HRESULT Initialize() = 0;
    virtual void Enter(CComponent* pOwner) = 0;
    virtual void Update(CComponent* pOwner, _float fTimeDelta) = 0;
    virtual void Exit(CComponent* pOwner) = 0;
    virtual _bool Check_Transition(CComponent* pOwner) = 0;

public:
    virtual void Free();
};

NS_END

