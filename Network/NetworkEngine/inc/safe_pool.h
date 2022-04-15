#pragma once
#include <Windows.h>

template <class T>
class safe_pool
{
#pragma region element
private:
	// 풀에서 사용하는 실제 요소 클래스 ( 값 부분과 락 부분이 있고 이를 사용하기 위한 연산자 재정의가 있다. )
	class element
	{
		const unsigned int LOCK = 1;
		const unsigned int UNLOCK = 0;

	public:
		element()
		{
			m_Value = T();
			m_Lock = UNLOCK;
		}
		~element()
		{

		}

	public:
		T& value()
		{
			return m_Value;
		}

#pragma region 연산자 오버로딩
		// = 연산자 오버로딩
		T& operator= (const T& target)
		{
			Use();

			m_Value = target;

			Unuse();

			return m_Value;
		}

		// ++ 후위 연산자 오버로딩
		T operator++ (int)
		{
			Use();

			m_Value++;

			Unuse();

			return m_Value;
		}

		// ++ 전위 연산자 오버로딩
		const T& operator++ ()
		{
			Use();

			++m_Value;

			Unuse();

			return m_Value;
		}

		// -- 후위 연산자 오버로딩
		T operator-- (int)
		{
			Use();

			m_Value--;

			Unuse();

			return m_Value;
		}

		// -- 전위 연산자 오버로딩
		const T& operator-- ()
		{
			Use();

			--m_Value;

			Unuse();

			return m_Value;
		}

		// += 연산자 오버로딩
		T& operator += (const T& target)
		{
			Use();

			m_Value = m_Value + target;

			Unuse();

			return m_Value;
		}

		// += 연산자 오버로딩(element)
		T& operator += (const element& target)
		{
			Use();

			m_Value = m_Value + target.m_Value;

			Unuse();

			return m_Value;
		}

		// -= 연산자 오버로딩
		T& operator -= (const T& target)
		{
			Use();

			m_Value = m_Value - target;

			Unuse();

			return m_Value;
		}

		// -= 연산자 오버로딩(element)
		T& operator -= (const element& target)
		{
			Use();

			m_Value = m_Value - target.m_Value;

			Unuse();

			return m_Value;
		}

		// + 연산자 오버로딩
		T operator+(const T target)
		{
			Use();

			m_Value += target;

			Unuse();

			return m_Value;
		}

		// + 연산자 오버로딩(element)
		T operator+(const element target)
		{
			Use();

			m_Value += target.m_Value;

			Unuse();

			return m_Value;
		}

		// - 연산자 오버로딩
		T operator-(const T target)
		{
			Use();

			m_Value -= target;

			Unuse();

			return m_Value;
		}

		// - 연산자 오버로딩(element)
		T operator-(const element target)
		{
			Use();

			m_Value -= target.m_Value;

			Unuse();

			return m_Value;
		}

#pragma endregion

	private:
		// 사용 시작
		void Use()
		{
			while (LOCK == InterlockedExchange(&m_Lock, LOCK))
			{

			}
		}
		// 변수 사용 끝
		void Unuse()
		{
			InterlockedCompareExchange(&m_Lock, UNLOCK, LOCK);
		}

	private:
		T				m_Value;		// 실제 값
		unsigned int	m_Lock;			// 스레드 세이프 잠금
	};
#pragma endregion

public:
	safe_pool(int count)
		:m_Count(count)
	{
		m_ElementArray = new element[m_Count];
	}
	~safe_pool()
	{
		delete[] m_ElementArray;
		m_ElementArray = nullptr;
	}

public:
	int count()
	{
		return m_Count;
	}

	element& operator[] (int index)
	{
		return m_ElementArray[index];
	}

private:
	int			m_Count;		// 풀 개수
	element* m_ElementArray;	// 풀 배열

};

