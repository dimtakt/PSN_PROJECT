#include "Engine_Shader_Defines.hlsli"

// ==============================
// || 글로벌 변수들
// ==============================

// 글로벌 변수의 경우 초기값을 지정해줄 시,
// 외부로부터 받은 값이 있다면 해당 값을 이용, 아니면 기본값을 이용함

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);       // Light 방향
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);    // Light 색상 및 밝기의 세기
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f); // 환경광. 을 가장한 최소 밝기 제한으로써 사용.
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);   // 반사광.

vector g_vCamPosition;                                  // 카메라 위치

// 마테리얼용 재질 텍스쳐
texture2D g_DiffuseTexture;

// 마테리얼(재질)의 환경광, 반사광 초기화
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


float g_fTiling = 50.f;



// ==============================
// || 이하 셰이딩
// ==============================





struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}









/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;

};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
};

/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord * g_fTiling); // ksta : 터레인 타일링
    
    // 빛의 반대 벡터와, 표면의 법선 벡터가 얼마나 일치하는지를 내적 계산을 통해 담음.
    // 비슷할 수록 큰 값이 담김으로써 빛의 세기가 더 커지도록 반영.
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
    
    // 슬라이딩 얘기하심
    
    // reflect() : 2번째 인수로 반사면의 법선(데칼코마니 기준선이라고 생각해도됨) 벡터를 받아 1번째 인수 벡터의 반사벡터를 구함
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    // 카메라로부터 픽셀이 상대적으로 어느 위치에 있는지 계산
    vector vLook = In.vWorldPos - g_vCamPosition;  
    
    // 계산 구조는 위의 fShade 와 동일
    // 셰이더 값은 0~1이므로 거듭제곱을 하여도 0~1 사이로 최소 최댓값은 동일함
    // 이를 이용하여 거듭제곱을 이용 셰이더의 민감도를 조정하는 식으로 활용.
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
    
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
                    (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;   // 반사 추가로 이 줄 추가
    
    return Out;
}








technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);

        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    ///* 모델의 상황에 따라 다른 쉐이딩 기법 세트(블렌딩 + 디스토션  )를 먹여주기위해서 */
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

    ///* 정점의 정보에 따라 쉐이더 파일을 작성한다. */
    ///* 정점의 정보가 같지만 완전히 다른 취급을 하느 ㄴ객체나 모델을 그리는 방식 -> 렌더링방식에 차이가 생길 수 있다. */ 
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

}
