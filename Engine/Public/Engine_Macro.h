#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{


	#ifndef			MSG_BOX
	#define			MSG_BOX(_message)			MessageBox(NULL, _message, L"System Message", MB_OK)
	#endif

	#define			ENUM_CLASS(ENUM)				static_cast<unsigned int>(ENUM)

	#define			NS_BEGIN(NAMESPACE)		namespace NAMESPACE {
	#define			NS_END						}
	
	#define			USING(NAMESPACE)	using namespace NAMESPACE;

	#ifdef	ENGINE_EXPORTS
	#define ENGINE_DLL		_declspec(dllexport)
	#else
	#define ENGINE_DLL		_declspec(dllimport)
	#endif


	#define NULL_CHECK( _ptr)	\
		{if( _ptr == 0){ return;}}
	
	#define NULL_CHECK_RETURN( _ptr, _return)	\
		{if( _ptr == 0){return _return;}}
	
	#define NULL_CHECK_MSG( _ptr, _message )		\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);}}
	
	#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);return _return;}}
	
	#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return E_FAIL;}
	
	#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return _return;}
	
	#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return E_FAIL;}
	
	#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return _return;}
	
	
	
	#define NO_COPY(CLASSNAME)										\
			private:												\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		
	
	#define DECLARE_SINGLETON(CLASSNAME)							\
			NO_COPY(CLASSNAME)										\
			private:												\
			static CLASSNAME*	m_pInstance;						\
			public:													\
			static CLASSNAME*	GetInstance( void );				\
			static void DestroyInstance( void );			
	
	#define IMPLEMENT_SINGLETON(CLASSNAME)							\
			CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
				if(nullptr == m_pInstance) {						\
					m_pInstance = new CLASSNAME;					\
				}													\
				return m_pInstance;									\
			}														\
			void CLASSNAME::DestroyInstance( void ) {				\
				if(nullptr != m_pInstance)	{						\
					delete m_pInstance;								\
					m_pInstance = nullptr;							\
				}													\
			}
}


// ==============================
// ||	Custom Macros
// START=========================

	#define			TO_RAD(DEGREE)									XMConvertToRadians(DEGREE)
	#define			TO_DEG(RADIAN)									XMConvertToDegrees(RADIAN)

	#define			IS_BETWEEN(condition, minValue, maxValue)		(((minValue) <= (condition)) && ((condition) < (maxValue)))	// 이상 and 미만

	#define			FAILED_CHECK_NOMSG(_hr)							if( ((HRESULT)(_hr)) < 0 )	\
																		{ return E_FAIL;}

	#define			ROT_TO_QUAT(ROT_X, ROT_Y, ROT_Z)				XMQuaternionRotationRollPitchYaw(ROT_X, ROT_Y, ROT_Z)

	#define			MAT_TO_ROT(FLOAT4X4)							_float3{TO_DEG(asin(-FLOAT4X4._32)), TO_DEG(atan2(FLOAT4X4._31, FLOAT4X4._33)), TO_DEG(atan2(FLOAT4X4._12, FLOAT4X4._22))}

	#define			QUAT_TO_MAT(QUAT)								XMMatrixRotationQuaternion(QUAT)


// END===========================

#endif // Engine_Macro_h__
