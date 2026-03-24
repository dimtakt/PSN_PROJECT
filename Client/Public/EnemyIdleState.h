#pragma once
#include "EnemyState.h"

NS_BEGIN(Client)

class CEnemyIdleState final : public CEnemyState
{
private:
	explicit CEnemyIdleState() = default;
	virtual ~CEnemyIdleState() = default;

public:
	HRESULT Initialize() override;
	void Enter(CComponent* pOwner) override;
	void Update(CComponent* pOwner, _float fTimeDelta) override;
	void Exit(CComponent* pOwner) override;
	_bool Check_Transition(CComponent* pOwner) override;

public:
	static CEnemyIdleState* Create();
};

NS_END
