#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTimeSpeed_Manager final : public CBase
{
    // 특수한 시간 배율 기믹같이,
    // 중요도가 높은 글로벌 변수를 제어하기 위한 매니저

    // 제어 요청을 받아 시간 경과에 따라 보간하여 제어되도록 의도
    // 사용 방식? : 플레이어 행동 시 force change, 이후 정상화 과정에는 req 사용..
private:
    CTimeSpeed_Manager() = default;
    virtual ~CTimeSpeed_Manager() = default;

public:
    void Update_TimeSpeed(_float fRawTimeDelta)
    {
        _float followSpeed = 3.0f; // 클수록 더 빨리 목표에 붙음
        _float factor = 1.0f - expf(-followSpeed * fRawTimeDelta);

        fTimeSpeed_Multiplier = fTimeSpeed_Multiplier * (1.0f - factor)
            + fTimeSpeed_ReqTarget * factor;

        if (fTimeSpeed_Multiplier < fTimeSpeed_Min) fTimeSpeed_Multiplier = fTimeSpeed_Min;
        if (fTimeSpeed_Multiplier > fTimeSpeed_Max) fTimeSpeed_Multiplier = fTimeSpeed_Max;
    };

    void Req_EditTimeSpeed(_float fEditValue)           {   fTimeSpeed_ReqTarget = fEditValue;  };
    void Set_EditTimeSpeed(_float fEditValue)           {   fTimeSpeed_ReqTarget = fEditValue; 
                                                            fTimeSpeed_Multiplier = fEditValue; };
    _float Get_TimeSpeed()                              {   return fTimeSpeed_Multiplier;   }

private:
    _float fTimeSpeed_Multiplier = 1.f;     // 현재 수치
    _float fTimeSpeed_ReqTarget = 1.f;      // 목표 수치

    const _float fTimeSpeed_Min = 0.01f;
    const _float fTimeSpeed_Max = 1.f;

public:
    static CTimeSpeed_Manager* Create()                 { return new CTimeSpeed_Manager(); }
    virtual void Free() override                        { __super::Free(); }
};

NS_END