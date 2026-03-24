#pragma once
#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CEnemyState;
class CEnemyAnyState;

class CEnemyAI final : public CComponent
{
public:
    typedef struct tagEnemyAIParam {

        _trigger    isGetDamaged_Upper      = false;
        _trigger    isGetDamaged_Lower      = false;

        _bool       isHaveWeapon            = false;
        _bool       isHaveGunWeapon         = false;
        _bool       isNearExistWeapon       = false;
        _bool       isGroggy                = false;

        _float      fDistToPlayer           = 0.f;


        void ResetTriggers()
        {
            isGetDamaged_Upper          = false;
            isGetDamaged_Lower          = false;
        }

    } ENEMY_AI_PARAM;

    typedef struct tagEnemyAIDesc {
        // Initialize 시 필요한 정보.

        CGameObject*        pOwner;

    } ENEMY_AI_DESC;

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
    const _wstring&     Get_CurrentStateTag() const { return m_strCurrentStateTag; }

private:
    _bool               Insert_State(const _wstring& strStateTag, CEnemyState* state);
    CEnemyState*        Find_State(const _wstring& strStateTag);
    
    _bool               Check_AnyState();

private:
    ENEMY_AI_PARAM      m_tAIParam = {};
    
    CEnemyAnyState*     m_pAnyState         = nullptr;
    CEnemyState*        m_pCurrentState     = nullptr;
    CGameObject*        m_pOwner            = nullptr;
    _wstring            m_strCurrentStateTag = L"";

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