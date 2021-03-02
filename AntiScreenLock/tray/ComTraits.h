#pragma once

#include <memory>

namespace ComTraits
{

////////////////////////////////////////////////////////////////////////////////
// 模糊类型
template<typename T>
struct FuzzyType
{
	typedef T type;
};

template<typename T>
struct FuzzyType<std::shared_ptr<T>>
{
	typedef std::shared_ptr<typename FuzzyType<T>::type> type;
};

template<typename T>
struct FuzzyType<std::unique_ptr<T>>
{
	typedef std::unique_ptr<typename FuzzyType<T>::type> type;
};

template<typename T>
struct FuzzyType<std::vector<T>>
{
	typedef std::vector<typename FuzzyType<T>::type> type;
};

#define _FUZZY_TYPE(dst, src) \
template<> \
struct FuzzyType<src> \
{ \
	typedef dst type; \
};

_FUZZY_TYPE(char, signed char)
_FUZZY_TYPE(char, unsigned char)
_FUZZY_TYPE(short, unsigned short)
_FUZZY_TYPE(int, unsigned int)
_FUZZY_TYPE(int, long)
_FUZZY_TYPE(int, unsigned long)
_FUZZY_TYPE(__int64, unsigned __int64)
#undef _FUZZY_TYPE

// 移除多级指针的 CV
#define _FUZZY_TYPE(CV_OPT) \
template<typename T> \
struct FuzzyType<CV_OPT T *> \
{ \
	typedef typename FuzzyType<T>::type *type; \
};

_CLASS_DEFINE_CV(_FUZZY_TYPE)
#undef _FUZZY_TYPE

// 多维数组转为多级指针
template<typename T>
struct ExtentToPointer
{
	typedef T type;
};

template<typename T, size_t N>
struct ExtentToPointer<T[N]>
{
	typedef typename ExtentToPointer<T>::type *type;
};

template<typename T>
struct ExtentToPointer<T[]>
{
	typedef typename ExtentToPointer<T>::type *type;
};

// 超级 decay 并模糊类型
template<typename T>
struct DecayEx
{
private:
	typedef typename std::remove_reference<T>::type T1;
	typedef typename std::conditional<std::is_array<T1>::value, typename ExtentToPointer<T1>::type, typename std::remove_cv<T1>::type>::type T2;

public:
	typedef typename FuzzyType<T2>::type type;
};

////////////////////////////////////////////////////////////////////////////////
// 封装 LPSTR、LPWSTR
template<typename T>
class CWrapStr
{
public:
	template<typename U, typename = typename std::enable_if<std::is_convertible<U, const T *>::value>::type>
	CWrapStr(U &&str) : m_ptr(Strdup((const T *)str))
	{
	}

	~CWrapStr()
	{
		if (m_ptr)
			free(m_ptr);
	}

	CWrapStr(const CWrapStr &_Right) : CWrapStr(_Right.m_ptr)
	{
	}

	CWrapStr(CWrapStr &&_Right) : m_ptr(_Right.m_ptr)
	{
		_Right.m_ptr = NULL;
	}

	operator const T *() const
	{
		return m_ptr;
	}

private:
	static char *Strdup(const char *lpStr)
	{
		return lpStr ? _strdup(lpStr) : NULL;
	}

	static wchar_t *Strdup(const wchar_t *lpStr)
	{
		return lpStr ? _wcsdup(lpStr) : NULL;
	}

	T *m_ptr;

	CWrapStr &operator=(const CWrapStr &_Right) = delete;
};

// 是否封装 T
template<typename T>
struct WrapType
{
	typedef T type;
};

#define _WRAP_TYPE(dst, src) \
template<> \
struct WrapType<src> \
{ \
	typedef dst type; \
};

_WRAP_TYPE(CWrapStr<char>, char *)
_WRAP_TYPE(CWrapStr<char>, const char *)
_WRAP_TYPE(CWrapStr<wchar_t>, wchar_t *)
_WRAP_TYPE(CWrapStr<wchar_t>, const wchar_t *)
#undef _WRAP_TYPE

// 使用时封装类型
template<typename T>
using DecayWrap = typename WrapType<typename std::decay<T>::type>::type;

// 匹配时模糊类型
template<typename T>
inline const std::type_info &FuzzyTypeId()
{
	return typeid(typename WrapType<typename DecayEx<T>::type>::type);
}

////////////////////////////////////////////////////////////////////////////////
// MakeDelegate
template<typename Ret, typename F, typename T>
class CDelegate
{
public:
	CDelegate(F f, T t) : m_f(f), m_t(t) {}

	template<typename... Args>
	Ret operator()(Args &&... args) const
	{
		return (m_t->*m_f)(std::forward<Args>(args)...);
	}

private:
	F m_f;
	T m_t;
};

template<typename Ret, typename C, typename... Args, typename T>
inline CDelegate<Ret, Ret(C::*)(Args...), T *> MakeDelegate(Ret(C::*f)(Args...), T *t)
{
	return CDelegate<Ret, Ret(C::*)(Args...), T *>(f, t);
}

template<typename Ret, typename C, typename... Args, typename T>
inline CDelegate<Ret, Ret(C::*)(Args...) const, const T *> MakeDelegate(Ret(C::*f)(Args...) const, const T *t)
{
	return CDelegate<Ret, Ret(C::*)(Args...) const, const T *>(f, t);
}

////////////////////////////////////////////////////////////////////////////////
// function_traits
template<typename T>
struct function_traits;

template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
	typedef Ret type(Args...);
	typedef Ret ret_type;
};

template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)>
{
};

template<typename Ret, typename F, typename T>
struct function_traits<CDelegate<Ret, F, T>> : function_traits<F>
{
};

#define _FUNCTION_TRAITS(CALL_OPT, X1, CV_OPT) \
template<typename Ret, typename T, typename... Args> \
struct function_traits<Ret(CALL_OPT T::*)(Args...) CV_OPT> : function_traits<Ret(Args...)> \
{ \
};

_MEMBER_CALL_CV(_FUNCTION_TRAITS )
#undef _FUNCTION_TRAITS

template<typename T>
struct function_traits : function_traits<decltype(&T::operator())>
{
};

////////////////////////////////////////////////////////////////////////////////
// GetFn
template<typename Fn>
inline auto GetFn(Fn &&fn) -> decltype(std::forward<Fn>(fn))
{
	return std::forward<Fn>(fn);
}

template<typename Fn>
inline typename std::add_lvalue_reference<Fn>::type GetFn(std::shared_ptr<Fn> &pfn)
{
	return *pfn;
}

template<typename Fn>
inline typename std::add_lvalue_reference<Fn>::type GetFn(const std::shared_ptr<Fn> &pfn)
{
	return *pfn;
}

template<typename Fn>
inline typename std::add_lvalue_reference<Fn>::type GetFn(std::unique_ptr<Fn> &pfn)
{
	return *pfn;
}

template<typename Fn>
inline typename std::add_lvalue_reference<Fn>::type GetFn(const std::unique_ptr<Fn> &pfn)
{
	return *pfn;
}

}	// namespace ComTraits

#define BIND(c, f) ComTraits::MakeDelegate(&f, &c)
