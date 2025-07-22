#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }, m_pContext{pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);
    return S_OK;
}

void CPicking::Update()
{
    // 역으로 하나씩 소거해가며 최종 위치를 계산.

    POINT ptMouse = {};

    /* 1. 뷰포트 상(윈도우 공간)의 마우스 위치를 구하자 */
    GetCursorPos(&ptMouse);
    ScreenToClient(m_hWnd, &ptMouse);

    /* 2. 투영 스페이스로 옮기자 (NDC 좌표계로 변환. 즉 "가로와 세로 모두 -1 ~ 1 크기로..") */
    /* [로컬위치 * 월드행렬 * 뷰행렬 * "투영행렬 * 1/w"] */
    _float4 vPosition = {};
    vPosition.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
    vPosition.y = ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
    vPosition.z = 0.0f;
    vPosition.w = 1.f;

    /* 3. 투영 → 뷰 공간으로 옮기자 */
    /* [로컬위치 * 월드행렬 * "뷰행렬"] */
    _matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ);
    _vector vViewSpacePos = XMVector4Transform(XMLoadFloat4(&vPosition), ProjMatrix);
    XMStoreFloat4(&vPosition, vViewSpacePos);


    /* 레이 시작점 (카메라 위치)와 방향 설정 */
    m_vMousePos = _float3(0.f, 0.f, 0.f);
    m_vMouseRay = _float3(vPosition.x, vPosition.y, vPosition.z);

    /* 4. 뷰 공간 → 월드 공간으로 옮기자 */
    /* [로컬위치 * "월드행렬"] */
    _matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);

    _vector vWorldOrigin = XMVector3TransformCoord(XMLoadFloat3(&m_vMousePos), ViewMatrix);
    _vector vWorldDir = XMVector3TransformNormal(XMLoadFloat3(&m_vMouseRay), ViewMatrix);
    vWorldDir = XMVector3Normalize(vWorldDir);

    XMStoreFloat3(&m_vMousePos, vWorldOrigin);
    XMStoreFloat3(&m_vMouseRay, vWorldDir);
}

_bool CPicking::Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
    _vector vA = XMLoadFloat3(&vPointA);
    _vector vB = XMLoadFloat3(&vPointB);
    _vector vC = XMLoadFloat3(&vPointC);

    _vector rayOrigin = XMLoadFloat3(&m_vMousePos);
    _vector rayDir = XMLoadFloat3(&m_vMouseRay);

    float fDist = 0.f;
    bool isPicked = TriangleTests::Intersects(rayOrigin, rayDir, vA, vB, vC, fDist);

    if (isPicked)
    {
        _vector vHitPos = XMVectorAdd(rayOrigin, XMVectorScale(rayDir, fDist));
        XMStoreFloat3(&vPickedPos, vHitPos);
    }

    return isPicked;
}

_bool CPicking::Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
    _vector vA = XMLoadFloat3(&vPointA);
    _vector vB = XMLoadFloat3(&vPointB);
    _vector vC = XMLoadFloat3(&vPointC);

    _vector rayOrigin = XMLoadFloat3(&m_vLocalMousePos);
    _vector rayDir = XMVector3Normalize(XMLoadFloat3(&m_vLocalMouseRay));

    float fDist = 0.f;  
    bool isPicked = TriangleTests::Intersects(rayOrigin, rayDir, vA, vB, vC, fDist);

    if (isPicked)
    {
        _vector vHitPos = XMVectorAdd(rayOrigin, XMVectorScale(rayDir, fDist));
        XMStoreFloat3(&vPickedPos, vHitPos);
    }

    return isPicked;
}

void CPicking::Transform_ToLocalSpace(CTransform* pTransformCom)
{
    _matrix matWorldInv = pTransformCom->Get_WorldMatrix_Inverse();

    _vector vLocalPos = XMVector3TransformCoord(XMLoadFloat3(&m_vMousePos), matWorldInv);
    _vector vLocalRay = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_vMouseRay), matWorldInv));

    XMStoreFloat3(&m_vLocalMousePos, vLocalPos);
    XMStoreFloat3(&m_vLocalMouseRay, vLocalRay); 
}


CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX(TEXT("Failed to Created : CPicking"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
