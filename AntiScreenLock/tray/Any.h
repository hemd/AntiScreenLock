#pragma once
#include "ComTraits.h"

////////////////////////////////////////////////////////////////////////////////
// 可以存储任意类型，如果是 LPTSTR 等类型，会自动 _tcsdup、free，这个要注意！
// Cast<T> 前可以先用 Is<T> 判断类型是否模糊匹配，不匹配会抛出异常 std::bad_cast
// 模糊匹配规则如下：
// 如果 T 为 const volatile unsigned U &，先移除 &、const、volatile、unsigned 等得到 U，如果为 long 再替换为 int，再进行比较
// 如果是指针类型比如 const volatile U * const &，则根据上面规则得到 U *，多级指针也一样：const volatile U * const * => U * *
// Cast<T> 返回值：如果 T 是标量类型，返回的是值，否则返回的是引用，比如 Cast<string>() 返回的是 string &
// 最好使用引用防止对象拷贝：string &str = any.Cast<string>() 或 auto &str = any.Cast<const string &>()
class CAny
{
	// 判断是否使用 placement new
	template<typename T>
	struct IsPlacement : std::integral_constant<bool, sizeof(T) <= 12 && std::is_trivially_copyable<T>::value || std::is_reference<T>::value>
	{
	};

	template<typename T, typename C>
	struct IsPlacement<CStringT<T, C>> : std::true_type
	{
	};

	template<typename T>
	struct IsPlacement<std::shared_ptr<T>> : std::true_type
	{
	};

	template<>
	struct IsPlacement<std::tuple<>> : std::true_type
	{
	};

	template<typename T, typename... Rest>
	struct IsPlacement<std::tuple<T, Rest...>> : std::integral_constant<bool, sizeof(std::tuple<T, Rest...>) <= 12 && IsPlacement<T>::value && IsPlacement<std::tuple<Rest...>>::value>
	{
	};

	template<typename T, typename U>
	struct IsPlacement<std::pair<T, U>> : IsPlacement<std::tuple<T, U>>
	{
	};

	// 判断是否返回值而非引用
	template<typename T>
	struct IsScalarEx : std::is_scalar<T>
	{
	};

	template<typename T, typename C>
	struct IsScalarEx<CStringT<T, C>> : std::true_type
	{
	};

	template<typename T>
	struct IsScalarEx<std::shared_ptr<T>> : std::true_type
	{
	};

	// 判断是否常量引用
	template<typename T>
	struct IsConstRef : std::false_type
	{
	};

	template<typename T>
	struct IsConstRef<const T &> : std::true_type
	{
	};

public:
	CAny() : m_pVtbl(NULL), m_pData(NULL)
	{
	}

	~CAny()
	{
		Clear();
	}

	template<typename T, typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, CAny>::value>::type>
	CAny(T &&value) : CAny()
	{
		Assign<T>(std::forward<T>(value));
	}

	CAny::CAny(const CAny &_Right) : CAny()
	{
		*this = _Right;
	}

	CAny::CAny(CAny &&_Right) : CAny()
	{
		*this = std::move(_Right);
	}

	template<typename T, typename... Args>
	void Emplace(Args &&... args)
	{
		Clear();
		Assign<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename = typename std::enable_if<!std::is_same<typename std::decay<T>::type, CAny>::value>::type>
	CAny &operator=(T &&value)
	{
		Clear();
		Assign<T>(std::forward<T>(value));
		return *this;
	}

	CAny &CAny::operator=(const CAny &_Right)
	{
		Clear();

		if (_Right.m_pVtbl)
		{
			((CData *)&_Right.m_pVtbl)->Copy(&m_pVtbl);
		}
		else if (_Right.m_pData)
		{
			m_pData = _Right.m_pData->Copy(NULL);
		}

		return *this;
	}

	CAny &CAny::operator=(CAny &&_Right)
	{
		Clear();

		if (_Right.m_pVtbl)
		{
			((CData *)&_Right.m_pVtbl)->Move(&m_pVtbl);
			_Right.Clear();
		}
		else if (_Right.m_pData)
		{
			std::swap(m_pData, _Right.m_pData);
		}

		return *this;
	}

	template<typename T>
	bool Is() const
	{
		CData *ptr = GetPtr();
		return ptr && ptr->IsType(ComTraits::FuzzyTypeId<T>());
	}

	template<>
	bool Is<void>() const
	{
		return m_pVtbl == NULL && m_pData == NULL;
	}

	template<typename T>
	typename std::enable_if<IsScalarEx<T>::value, T>::type Cast() const
	{
		if (!Is<T>())
			return T();

		return ((CDataV<T> *)GetPtr())->m_value;
	}

	template<typename T>
	typename std::enable_if<IsConstRef<T>::value, T>::type Cast() const
	{
		if (!Is<T>())
			throw std::bad_cast();

		return ((CDataV<T> *)GetPtr())->m_value;
	}

	template<typename T>
	typename std::enable_if<!IsScalarEx<T>::value && !IsConstRef<T>::value, typename std::add_lvalue_reference<T>::type>::type Cast()
	{
		static_assert(!std::is_rvalue_reference<T>::value, "is_rvalue_reference");

		if (!Is<T>())
			throw std::bad_cast();

		return ((CDataV<T> *)GetPtr())->m_value;
	}

private:
	template<typename T, typename... Args>
	typename std::enable_if<IsPlacement<typename std::decay<T>::type>::value>::type Assign(Args &&... args)
	{
		new (&m_pVtbl) CDataV<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	typename std::enable_if<!IsPlacement<typename std::decay<T>::type>::value>::type Assign(Args &&... args)
	{
		m_pData = new CDataV<T>(std::forward<Args>(args)...);
	}

	template<>
	void Assign<nullptr_t>(nullptr_t &&)
	{
	}

	void Clear()
	{
		if (m_pVtbl)
		{
			((CData *)&m_pVtbl)->~CData();
		}
		else if (m_pData)
		{
			delete m_pData;
		}

		m_pVtbl = NULL;
		m_pData = NULL;
	}

	// 将 T 封装为 CDataT，擦除类型
	class CData
	{
	public:
		virtual ~CData() = default;
		virtual CData *Copy(void *) = 0;
		virtual CData *Move(void *) = 0;
		virtual bool IsType(const std::type_info &) const = 0;
	};

	template<typename T>
	class CDataT : public CData
	{
	public:
		template<typename... Args>
		CDataT(Args &&... args) : m_value(std::forward<Args>(args)...)
		{
		}

		CData *Copy(void *lpData) override
		{
			return lpData ? new (lpData) CDataT(m_value) : new CDataT(m_value);
		}

		CData *Move(void *lpData) override
		{
			return new (lpData) CDataT(std::move(m_value));
		}

		bool IsType(const std::type_info &type) const override
		{
			return type == ComTraits::FuzzyTypeId<T>();
		}

		T m_value;
	};

	template<typename T>
	using CDataV = CDataT<ComTraits::DecayWrap<T>>;

	CData *GetPtr() const
	{
		return m_pVtbl ? (CData *)&m_pVtbl : m_pData;
	}

	// 这3个变量顺序很重要
	PVOID    m_pVtbl;
	CData   *m_pData;
	__int64  m_space;
};

template<typename T, typename... Args>
CAny MakeAny(Args &&... args)
{
	CAny any;
	any.Emplace<T>(std::forward<Args>(args)...);
	return any;
}

template<typename... Args>
CAny MakeAnyTuple(Args &&... args)
{
	CAny any;
	any.Emplace<std::tuple<typename std::decay<Args>::type...>>(std::forward<Args>(args)...);
	return any;
}

// 伪 CAny 指针，为了禁用左值拷贝、赋值
class CAnyPtr
{
public:
	CAnyPtr(CAny &&any) : m_any(std::move(any))
	{
	}

	CAnyPtr(CAnyPtr &&_Right) : m_any(std::move(_Right.m_any))
	{
	}

	CAnyPtr &operator=(CAnyPtr &&_Right)
	{
		m_any = std::move(_Right.m_any);
		return *this;
	}

	CAny *operator->()
	{
		return &m_any;
	}

private:
	CAny m_any;

	CAnyPtr(const CAnyPtr &) = delete;
	CAnyPtr &operator=(const CAnyPtr &) = delete;
};
