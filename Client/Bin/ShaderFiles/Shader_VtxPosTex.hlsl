


/* 
VTXPOSTEX
LPCSTR SemanticName;
UINT SemanticIndex;
DXGI_FORMAT Format;
UINT InputSlot;
UINT AlignedByteOffset;
D3D11_INPUT_CLASSIFICATION InputSlotClass;
UINT InstanceDataStepRate;
*/ 

/* 
D3D11_INPUT_ELEMENT_DESC Elements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
*/

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;



sampler DefaultSampler = sampler_state 
{     
    filter = min_mag_mip_linear;
    
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
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
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;    
}




technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass
    {
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
