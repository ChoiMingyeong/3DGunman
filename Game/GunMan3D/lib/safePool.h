#pragma once

#include <Windows.h>

template<class Type>
class element
{
public:
	element();
	element(const Type& target);
	~element();

public:
	Type&			operator= (const Type& target);			// = 연산자 오버로딩

	Type			operator++ (int);						// ++ 후위 연산자 오버로딩
	const Type&		operator++ ();							// ++ 전위 연산자 오버로딩
	Type			operator-- (int);						// -- 후위 연산자 오버로딩
	const Type&		operator--();							// -- 전위 연산자 오버로딩

	Type&			operator += (const Type& target);		// += 연산자 오버로딩(type)
	Type&			operator += (const element& target);	// += 연산자 오버로딩(element)
	Type&			operator -= (const Type& target);		// -= 연산자 오버로딩(type)
	Type&			operator -= (const element& target);	// -= 연산자 오버로딩(element)

	Type			operator+ (const Type& target);			// + 연산자 오버로딩(Type)
	Type			operator+ (const element& target);		// + 연산자 오버로딩(element)
	Type			operator- (const Type& target);			// - 연산자 오버로딩(Type)
	Type			operator- (const element& target);		// - 연산자 오버로딩(element)

private:
	void			Use();			// 사용 시작 ( 이미 사용중이면 대기 )
	void			Unuse();		// 사용 끝!

private:
	Type			value;			// 실제 값
	unsigned int	usLock;			// 스레드 세이프 잠금
};

template<class Type>
class safePool
{
public:
	safePool(int count);
	safePool(int count, const Type& target);
	~safePool();

public:
	element<Type>&	operator[] (int index);					// [] 연산자 오버로딩

public:
	int				count();		// 풀 개수 얻기

private:
	int				m_Count;		// 풀 개수
	char*			m_MemoryPool;	// 메모리 풀
	element<Type>*	m_Pool;			// 풀 배열
};

template<class Type>
inline safePool<Type>::safePool(int count)
	: m_Count(count)
{
	m_MemoryPool = new char[sizeof(element<Type>) * m_Count];
	m_Pool = reinterpret_cast<element<Type>*>(m_MemoryPool);		// 메모리 풀 할당

	for (int i = 0; i < m_Count; ++i)
	{
		new (m_MemoryPool + sizeof(element<Type>) * i) element<Type>();		// placement new로 할당된 메모리 공간에 변수 생성
	}
}

template<class Type>
inline safePool<Type>::safePool(int count, const Type& target)
	: safePool(count)
{
	m_MemoryPool = new char[sizeof(element<Type>) * m_Count];			// 메모리 풀 할당

	for (int i = 0; i < m_Count; ++i)
	{
		element<Type>* pMemory = (element<Type>*)(m_MemoryPool + (sizeof(element<Type>) * i));
		m_Pool[i] = new (pMemory) element<Type>(target);		// placement new로 할당된 메모리 공간에 변수 생성
	}
}

template<class Type>
inline safePool<Type>::~safePool()
{
	for (int i = 0; i < m_Count; ++i)
	{
		m_Pool[i].~element<Type>();
	}

	delete[] m_MemoryPool;
	m_Pool = nullptr;
}

template<class Type>
inline element<Type>& safePool<Type>::operator[](int index)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_Pool[index];
}

template<class Type>
inline int safePool<Type>::count()
{
	return m_Count;
}

template<class Type>
inline element<Type>::element()
{
	value = Type();
	usLock = 0;
}

template<class Type>
inline element<Type>::element(const Type& target)
{
	value = Type(target);
	usLock = 0;
}

template<class Type>
inline element<Type>::~element()
{

}

template<class Type>
inline Type& element<Type>::operator=(const Type& target)
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	value = target;
	Unuse();

	return value;
}

template<class Type>
inline Type element<Type>::operator++(int)
{
	Use();
	value++;
	Unuse();

	return value;
}

template<class Type>
inline const Type& element<Type>::operator++()
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	++value;
	Unuse();

	return value;
}

template<class Type>
inline Type element<Type>::operator--(int)
{
	Use();
	value--;
	Unuse();

	return value;
}

template<class Type>
inline const Type& element<Type>::operator--()
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	--value;
	Unuse();

	return value;
}

template<class Type>
inline Type& element<Type>::operator+=(const Type& target)
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	value = value + target;
	Unuse();

	return value;
}

template<class Type>
inline Type& element<Type>::operator+=(const element& target)
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	value = value + target.value;
	Unuse();

	return value;
}

template<class Type>
inline Type& element<Type>::operator-=(const Type& target)
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	value = value - target;
	Unuse();

	return value;
}

template<class Type>
inline Type& element<Type>::operator-=(const element& target)
{
	// TODO: 여기에 return 문을 삽입합니다.
	Use();
	value = value - target.value;
	Unuse();

	return value;
}

template<class Type>
inline Type element<Type>::operator+(const Type& target)
{
	Use();
	value += target;
	Unuse();

	return value;
}

template<class Type>
inline Type element<Type>::operator+(const element& target)
{
	Use();
	value += target.value;
	Unuse();

	return value;
}

template<class Type>
inline Type element<Type>::operator-(const Type& target)
{
	Use();
	value -= target;
	Unuse();

	return value;
}

template<class Type>
inline Type element<Type>::operator-(const element& target)
{
	Use();
	value -= target.value;
	Unuse();

	return value;
}

template<class Type>
inline void element<Type>::Use()
{
	while (1 == InterlockedExchange(&usLock, 1))
	{

	}
}

template<class Type>
inline void element<Type>::Unuse()
{
	InterlockedCompareExchange(&usLock, 0, 1);
}