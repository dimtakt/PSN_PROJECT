#pragma once
#include "EnemyState.h"

NS_BEGIN(Client)

class CEnemyTrackPlayerState final : public CEnemyState
{
private:
	explicit CEnemyTrackPlayerState() = default;
	virtual ~CEnemyTrackPlayerState() = default;

public:
	HRESULT Initialize() override;
	void Enter(CComponent* pOwner) override;
	void Update(CComponent* pOwner, _float fTimeDelta) override;
	void Exit(CComponent* pOwner) override;
	_bool Check_Transition(CComponent* pOwner) override;

public:
	static CEnemyTrackPlayerState* Create();
};

NS_END
