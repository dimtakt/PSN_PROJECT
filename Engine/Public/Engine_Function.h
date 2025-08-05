#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();
		return dwRefCnt;
	}




	// _wstring 을 _string 으로
	inline std::string WStringToString(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// const _char* 을 _wstring 으로
	inline std::wstring ConvertCharToWString(const char* pszStr)
	{
		if (pszStr == nullptr)
			return L"";

		int iLength = MultiByteToWideChar(CP_ACP, 0, pszStr, -1, nullptr, 0);
		std::wstring wstr(iLength, 0);

		MultiByteToWideChar(CP_ACP, 0, pszStr, -1, &wstr[0], iLength);

		// 문자열 끝의 '\0' 제거
		if (!wstr.empty() && wstr.back() == L'\0')
			wstr.pop_back();

		return wstr;
	}

	// _wstring 을 const _char 으로
	inline char* WStringToChar(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (size_needed == 0) return nullptr;

		char* result = new char[size_needed];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, result, size_needed, nullptr, nullptr);
		return result;
	}
}

#endif // Engine_Function_h__
