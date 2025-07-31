#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;



struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;    
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
        
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
    float3 vTexcoord : TEXCOORD0;    

};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
    
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;    
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);    
    
    return Out;    
}



technique11 DefaultTechnique
{  
    pass Sky
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();   
        PixelShader = compile ps_5_0 PS_MAIN();

        
    }
}
