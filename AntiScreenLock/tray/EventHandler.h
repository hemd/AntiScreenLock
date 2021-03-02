#pragma once

#include <functional>

typedef std::function<void()>						Delegate0;
typedef std::function<void(LPCTSTR)>				Delegate1_string;
typedef std::function<void(UINT)>					Delegate1_UINT;
typedef std::function<void(UINT, BOOL &)>			Delegate2_UINT_BOOL;

class EventHandler
{
public:
	EventHandler()
	{
	}
	EventHandler(Delegate0 &&func) : m_func_no_para(std::move(func))
	{
	}
	EventHandler(Delegate1_string &&func) : m_func_para_string(std::move(func))
	{
	}
	EventHandler(Delegate1_UINT &&func) : m_func_para_uint(std::move(func))
	{
	}
	EventHandler(Delegate2_UINT_BOOL &&func) : m_func_para_uint_bool(std::move(func))
	{
	}

	template<typename Fn, typename = typename std::enable_if<!std::is_same<typename std::decay<Fn>::type, EventHandler>::value>::type>
	EventHandler(Fn &&fn) : EventHandler(std::function<ComTraits::function_traits<typename std::decay<Fn>::type>::type>(fn))	// 用于支持 lambda
	{
	}

	void Invoke()
	{
		if(m_func_no_para)
		{
			m_func_no_para();
		}
	}


	void Invoke(LPCTSTR strPara) //字符串参数
	{
		if(m_func_para_string)
		{
			m_func_para_string(strPara);
		}
	}

	void Invoke(UINT nPara) //正整数参数
	{
		if(m_func_para_uint)
		{
			m_func_para_uint(nPara);
		}
	}

	void Invoke(UINT nPara, BOOL& bHandled)
	{
		if(m_func_para_uint_bool)
		{
			m_func_para_uint_bool(nPara, bHandled);
		}
	}



public:
	Delegate0 m_func_no_para;
	Delegate1_string m_func_para_string;
	Delegate1_UINT m_func_para_uint;
	Delegate2_UINT_BOOL m_func_para_uint_bool;
};
