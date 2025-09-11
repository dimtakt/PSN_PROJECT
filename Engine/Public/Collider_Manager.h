#pragma once
#include "Base.h"

// https://chatgpt.com/c/68c28f00-ae5c-832f-a29a-474bf997149c
// 답변에 기반하여 제작
// 1. 기존 타입과는 별도로, 충돌 검사만을 위한 [레이어 enum class] 새로 작성
// 2. 충돌 검사 시에 객체 콜라이더별 [마스크] 에 저장된 대상과 검사를 실행
// 3. 검사 실행 시 충돌 판정이 뜨면, 해당 객체에게 [충돌 대상]과 함께 OnCollision 을 호출..
// 4. OnCollision 은 콜라이더 단위가 아닌 객체에서 만들어 줘야하나, 그러면 콜라이더에 오너를 저장해줘야됨?

//NS_BEGIN(Engine)
//
//class CCollider_Manager final : public CBase
//{
//private:
//	CObject_Manager();
//	virtual ~CObject_Manager() = default;
//
//public:
//	class CComponent* Get_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
//	class CGameObject* Get_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
//
//public:
//	HRESULT Initialize(_uint iNumLevels);
//	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg);
//	HRESULT Remove_GameObject_FromLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, class CGameObject* pObject);
//	void Priority_Update(_float fTimeDelta);
//	void Update(_float fTimeDelta);
//	void Late_Update(_float fTimeDelta);
//	void Clear(_uint iLevelIndex);
//
//public:
//	class CGameObject* Get_LastGameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag);
//
//
//private:
//	class CGameInstance* m_pGameInstance = { nullptr };
//	_uint									m_iNumLevels = {};
//	map<const _wstring, class CLayer*>* m_pLayers = {};
//
//private:
//	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);
//
//public:
//	static CObject_Manager* Create(_uint iNumLevels);
//	virtual void Free() override;
//};
//
//NS_END