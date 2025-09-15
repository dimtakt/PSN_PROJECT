#include "Engine_Shader_Defines.hlsli"


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
texture2D g_Texture;

vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightPos;
float g_fRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/(w == 뷰스페이스상의 z) */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * vDepthDesc.y;
    /* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    /* 월드공간상의 좌표를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/(w == 뷰스페이스상의 z) */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * vDepthDesc.y;
    /* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    /* 월드공간상의 좌표를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fRange - fDistance) / g_fRange);
    
    float fShade = max(dot(vNormal * -1.f, normalize(vLightDir)), 0.f);
    
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    
   
    vector vLook = vWorldPos - g_vCamPosition;
    
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular * fAtt;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse * vShade + vSpecular;
    
    /* 내 픽셀의 광원 기준의 깊이 */ 
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/(w == 뷰스페이스상의 z) */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * vDepthDesc.y;
    /* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    /* 월드공간상의 좌표를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vPosition = mul(vWorldPos, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
    
    /* 광원기준으로 표현됐을때 그려져있어야할 위치에 이미 그려져있떤 누군가의 깊이 */
    float2 vTexcoord;
    
    /* -1 ~ 1 -> 0 ~ 1 */
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    
    /* 1 ~ -1 -> 0 ~ 1 */ 
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vLightDepth = g_LightDepthTexture.Sample(DefaultSampler, vTexcoord);
    float fViewZ = vLightDepth.x * 1000.0f;
    
    if (vPosition.w - 0.1f > fViewZ)
        Out.vColor = Out.vColor * 0.3f;
    
    return Out;
}





technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }



}
