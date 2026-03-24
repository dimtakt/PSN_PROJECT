#pragma once
#include "EnemyState.h"

NS_BEGIN(Client)

class CEnemyAttackMeleeState final : public CEnemyState
{
private:
	explicit CEnemyAttackMeleeState() = default;
	virtual ~CEnemyAttackMeleeState() = default;

public:
	HRESULT Initialize() override;
	void Enter(CComponent* pOwner) override;
	void Update(CComponent* pOwner, _float fTimeDelta) override;
	void Exit(CComponent* pOwner) override;
	_bool Check_Transition(CComponent* pOwner) override;

public:
	static CEnemyAttackMeleeState* Create();
};

NS_END
