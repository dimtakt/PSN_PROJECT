#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTimeSpeed_Manager final : public CBase
{
    // 특수한 시간 배율 기믹같이,
    // 중요도가 높은 글로벌 변수를 제어하기 위한 매니저

    // 제어 요청을 받아 시간 경과에 따라 보간하여 제어되도록 의도
private:
    CTimeSpeed_Manager() = default;
    virtual ~CTimeSpeed_Manager() = default;

public:
    void Update_TimeSpeed(_float fRawTimeDelta)
    {
        // 요청을 받은 시점부터 보간 진행
        if (fTimeSpeed_Multiplier != fTimeSpeed_ReqTarget)
        {
            _float fLeftTime = fTimeSpeed_LerpTransition - fElapsedTime_forLerp;
            _float fLerpRatio = fRawTimeDelta / fLeftTime;
            if (fLerpRatio > 1) fLerpRatio = 1;

            fTimeSpeed_Multiplier = (fTimeSpeed_Multiplier * (1 - fLerpRatio)) + (fTimeSpeed_ReqTarget * fLerpRatio);
            if (fTimeSpeed_Multiplier < fTimeSpeed_Min) fTimeSpeed_Multiplier = fTimeSpeed_Min;
            if (fTimeSpeed_Multiplier > fTimeSpeed_Max) fTimeSpeed_Multiplier = fTimeSpeed_Max;

            fElapsedTime_forLerp += fRawTimeDelta;
        }
        else
        {
            fElapsedTime_forLerp = 0.f;
        }
    };

    void Req_EditTimeSpeed(_float fEditValue)           {   fTimeSpeed_ReqTarget = fEditValue;  };
    void Set_EditTimeSpeed(_float fEditValue)           {   fTimeSpeed_Multiplier = fEditValue; };
    _float Get_TimeSpeed()                              {   return fTimeSpeed_Multiplier;   }


    void Set_LerpTransitionTime(_float fTransValue)     {   fTimeSpeed_LerpTransition = fTransValue;    }
    _float Get_LerpTransitionTime()                     {   return fTimeSpeed_LerpTransition;   }


private:
    _float fTimeSpeed_Multiplier = 1.f;
    _float fTimeSpeed_ReqTarget = 1.f;

    _float fTimeSpeed_LerpTransition = 0.2f;
    _float fElapsedTime_forLerp = 0.f;

    _float fTimeSpeed_Min = 0.1f;
    _float fTimeSpeed_Max = 1.f;

public:
    static CTimeSpeed_Manager* Create()                  { return new CTimeSpeed_Manager(); }
    virtual void Free() override                        { __super::Free(); }
};

NS_END