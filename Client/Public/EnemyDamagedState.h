#pragma once
#include "EnemyState.h"

NS_BEGIN(Client)

class CEnemyDamagedState final : public CEnemyState
{
private:
	explicit CEnemyDamagedState() = default;
	virtual ~CEnemyDamagedState() = default;

public:
	HRESULT Initialize() override;
	void Enter(CComponent* pOwner) override;
	void Update(CComponent* pOwner, _float fTimeDelta) override;
	void Exit(CComponent* pOwner) override;
	_bool Check_Transition(CComponent* pOwner) override;

public:
	static CEnemyDamagedState* Create();
};

NS_END
