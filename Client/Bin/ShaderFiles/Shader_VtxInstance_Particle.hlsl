#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
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
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
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
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.3f)
        discard;
    
    float fColor = saturate(In.vLifeTime.y - In.vLifeTime.x);
        
    Out.vColor = vector(fColor, 0.f, 0.f, 1.f);
    
    Out.vColor.a = fColor;
    
    return Out;
}



PS_OUT PS_MAIN_TRIPARTICLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.3f)
        discard;
    
    //float fColor = saturate(In.vLifeTime.y - In.vLifeTime.x);
    //float fAlphaBase = saturate(In.vLifeTime.y - In.vLifeTime.x);                  // y가 1 아래라면 알파값이 0 ~ y 가 됨
    float fAlphaBase = saturate((In.vLifeTime.y - In.vLifeTime.x) / In.vLifeTime.y); // 알파값 0 ~ 1
    
    float fAlpha = saturate(-(pow(fAlphaBase - 1.f, 2.f)) + 1.f); // y = -(x-1)^2 + 1  ( x = fAlphaBase, y = fAlpha )
        
    Out.vColor = vector(0.8f, 1.f, 1.f, 1.f);           // 색상 값은 고정으로 두고, 알파값만 바뀌도록 해서 까매지며 없어지지 않도록 수정
    
    Out.vColor.a = fAlpha;
    
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
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlend_TriParticle
    {
        SetRasterizerState(RS_Cull_None);               // 반대에서도 보이도록 cull none으로 바꿈
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TRIPARTICLE();
    }
}
