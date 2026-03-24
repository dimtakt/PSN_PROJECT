#pragma once
#include "EnemyState.h"

NS_BEGIN(Client)

class CEnemyTrackWeaponState final : public CEnemyState
{
private:
	explicit CEnemyTrackWeaponState() = default;
	virtual ~CEnemyTrackWeaponState() = default;

public:
	HRESULT Initialize() override;
	void Enter(CComponent* pOwner) override;
	void Update(CComponent* pOwner, _float fTimeDelta) override;
	void Exit(CComponent* pOwner) override;
	_bool Check_Transition(CComponent* pOwner) override;

public:
	static CEnemyTrackWeaponState* Create();
};

NS_END
