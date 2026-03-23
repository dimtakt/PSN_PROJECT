#pragma once
#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CEnemyState;

class CEnemyAI final : public CComponent
{
public:
    typedef struct tagEnemyAIParam {
        // 여기에 전용 파라미터 정의,
        // Enemy 에서는 이걸 Getter 통해 조작하는 식으로.
        // 이게 블랙보드 방식.

        _trigger    isGetDamaged_Upper      = false;
        _trigger    isGetDamaged_Lower      = false;
        
        _trigger    isGainWeapon            = false;

        _bool       isHaveWeapon            = false;
        _bool       isNearExistWeapon       = false;

        _float      fDistToPlayer           = 0.f;


        void ResetTriggers()
        {
            isGetDamaged_Upper          = false;
            isGetDamaged_Lower          = false;

            isGainWeapon                = false;
        }

    } ENEMY_AI_PARAM;

    typedef struct tagEnemyAI {
        // Initialize 시 필요한 정보.

        CGameObject*        pOwner;

    } ENEMY_AI;

private:
    CEnemyAI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CEnemyAI() = default;

public:
    virtual HRESULT     Initialize(void* pArg) override;
    void                Update(_float fTimeDelta); // 상태 로직 실행

    void                Change_State(CEnemyState* pNewState);
    void                Change_State(const _wstring& strStateTag);

    CGameObject*        Get_Owner()     { return m_pOwner; }
    ENEMY_AI_PARAM&     Get_AIParam()   { return m_tAIParam; }

private:
    _bool               Insert_State(const _wstring& strStateTag, CEnemyState* state);
    CEnemyState*        Find_State(const _wstring& strStateTag);
    
    _bool               Check_AnyState();

private:
    ENEMY_AI_PARAM      m_tAIParam = {};
    
    CEnemyAnyState*     m_pAnyState         = nullptr;
    CEnemyState*        m_pCurrentState     = nullptr;
    CGameObject*        m_pOwner            = nullptr;

    //CEnemyState*        m_pEntryState       = nullptr;
    //CEnemyAnyState*     m_pAnyState         = nullptr;
    
    // 내부에 EntryState / AnyState 통합
    unordered_map<_wstring, CEnemyState*>    m_umapStates = {};

public:
    static CEnemyAI*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent*     Clone(void* pArg) override;
    virtual void            Free() override;
};

NS_END