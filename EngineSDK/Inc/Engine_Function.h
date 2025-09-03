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

	

	struct Vec2
	{
		float start2DPoint, end2DPoint;
	};

	inline float CCW(const Vec2& a, const Vec2& b, const Vec2& c)
	{
		// >0: CCW, <0: CW, =0: 일직선
		return (b.start2DPoint - a.start2DPoint) * (c.end2DPoint - a.end2DPoint) - (b.end2DPoint - a.end2DPoint) * (c.start2DPoint - a.start2DPoint);
	}

	inline bool IsIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
	{
		const float EPS = 0.2f; // 오차 허용 범위
		float ab_c = CCW(A, B, C);
		float ab_d = CCW(A, B, D);
		float cd_a = CCW(C, D, A);
		float cd_b = CCW(C, D, B);

		if (fabs(ab_c) < EPS && fabs(ab_d) < EPS && fabs(cd_a) < EPS && fabs(cd_b) < EPS)
		{
			// 일직선 상의 경우 → 구간 겹침 판정 필요
			if (max(A.start2DPoint, B.start2DPoint) < min(C.start2DPoint, D.start2DPoint)) return false;
			if (max(C.start2DPoint, D.start2DPoint) < min(A.start2DPoint, B.start2DPoint)) return false;
			if (max(A.end2DPoint, B.end2DPoint) < min(C.end2DPoint, D.end2DPoint))     return false;
			if (max(C.end2DPoint, D.end2DPoint) < min(A.end2DPoint, B.end2DPoint))     return false;

			return true; // 구간이 겹침
		}

		return (ab_c * ab_d <= EPS) && (cd_a * cd_b <= EPS);
	}
}

#endif // Engine_Function_h__
