#pragma once
#include <Windows.h>

template <class T>
class safe_pool
{
#pragma region element
private:
	// Ǯ���� ����ϴ� ���� ��� Ŭ���� ( �� �κа� �� �κ��� �ְ� �̸� ����ϱ� ���� ������ �����ǰ� �ִ�. )
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

#pragma region ������ �����ε�
		// = ������ �����ε�
		T& operator= (const T& target)
		{
			Use();

			m_Value = target;

			Unuse();

			return m_Value;
		}

		// ++ ���� ������ �����ε�
		T operator++ (int)
		{
			Use();

			m_Value++;

			Unuse();

			return m_Value;
		}

		// ++ ���� ������ �����ε�
		const T& operator++ ()
		{
			Use();

			++m_Value;

			Unuse();

			return m_Value;
		}

		// -- ���� ������ �����ε�
		T operator-- (int)
		{
			Use();

			m_Value--;

			Unuse();

			return m_Value;
		}

		// -- ���� ������ �����ε�
		const T& operator-- ()
		{
			Use();

			--m_Value;

			Unuse();

			return m_Value;
		}

		// += ������ �����ε�
		T& operator += (const T& target)
		{
			Use();

			m_Value = m_Value + target;

			Unuse();

			return m_Value;
		}

		// += ������ �����ε�(element)
		T& operator += (const element& target)
		{
			Use();

			m_Value = m_Value + target.m_Value;

			Unuse();

			return m_Value;
		}

		// -= ������ �����ε�
		T& operator -= (const T& target)
		{
			Use();

			m_Value = m_Value - target;

			Unuse();

			return m_Value;
		}

		// -= ������ �����ε�(element)
		T& operator -= (const element& target)
		{
			Use();

			m_Value = m_Value - target.m_Value;

			Unuse();

			return m_Value;
		}

		// + ������ �����ε�
		T operator+(const T target)
		{
			Use();

			m_Value += target;

			Unuse();

			return m_Value;
		}

		// + ������ �����ε�(element)
		T operator+(const element target)
		{
			Use();

			m_Value += target.m_Value;

			Unuse();

			return m_Value;
		}

		// - ������ �����ε�
		T operator-(const T target)
		{
			Use();

			m_Value -= target;

			Unuse();

			return m_Value;
		}

		// - ������ �����ε�(element)
		T operator-(const element target)
		{
			Use();

			m_Value -= target.m_Value;

			Unuse();

			return m_Value;
		}

#pragma endregion

	private:
		// ��� ����
		void Use()
		{
			while (LOCK == InterlockedExchange(&m_Lock, LOCK))
			{

			}
		}
		// ���� ��� ��
		void Unuse()
		{
			InterlockedCompareExchange(&m_Lock, UNLOCK, LOCK);
		}

	private:
		T				m_Value;		// ���� ��
		unsigned int	m_Lock;			// ������ ������ ���
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
	int			m_Count;		// Ǯ ����
	element* m_ElementArray;	// Ǯ �迭

};

