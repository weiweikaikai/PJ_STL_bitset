#pragma once
#include<iostream>
using namespace std;

#define CHAR_BIT 8
#define THROW(x, y)	throw x(y)
#define STCONS(ty, name, val)	enum {name = val}
#define My_assert(exp) (void)( !!(exp) || (_assert(#exp, __FILE__, __LINE__), 0) )

template<size_t N>
class My_bitset
{
	typedef unsigned long T;
public:
	class My_reference
	{
		friend class My_bitset<N>;
	public:
		My_reference& operator=(bool X)
		{
			Pbs->set(off, X);
		}
		My_reference&operator=(const My_reference&Bs)
		{
			if (this != &Bs)
			{
				Pbs->set(off, bool(Bs));//C++���ǿ������ת����C���Ե�(bool)Bsһ��
			}
			return (*this);
		}
		My_reference& flip()
		{
			Pbs->flip(off);
		}
		bool operator~() const
		{
			return (!Pbs->test(off));
		}
		operator bool() const
		{
			return (Pbs->test(off));
		}
	private:
		My_reference(My_bitset<N>&X, size_t P) :Pbs(&X), off(P)//Ҳ������������
		{}//���ڵĿ������캯������������Ϊ���ֵĿ�����������Ϊ˽�ж��ҽ����캯����Ϊ˽��
		//Ҳ����ϵͳ�������ṩĬ�ϵĹ��캯����������Ϊ���ֵĲ�����Ĭ�ϵĿ������캯����ͬ������ϵͳ�����ṩĬ�ϵĿ�������
		My_reference(My_reference &);

		My_bitset<N> *Pbs;
		size_t off;
	};
	STCONS(size_t, bitset_size, N);
	bool at(size_t P)const//����Pλ�õı���λ ��Ϊ��������ֻ�г�������Է���
	{                   //��Խ����
		if (N <= P)
			My_Xran();
		return test(P);
	}//
	My_reference at(size_t P)//֧�� at[i]=at[j]
	{
		if (N <= P)
			My_Xran();
		return (My_reference(*this, P));
	}
	bool operator[](size_t P)const//��at������ͬ������û��Խ����
	{            //��Ϊ��������ֻ�г�������Է���
		return test(P);
	}
	My_reference operator[](size_t P)//֧�� b[i] = b[j]
	{
		return (My_reference(*this, P));
	}
	My_bitset()
	{
		My_Tidy();
	}
	My_bitset(unsigned long X)
	{
		My_Tidy();//�û���������λȫ������
		for (size_t P = 0; X != 0 && P < N; X >>= 1, ++P)
		{
			if (X & 1)
			{
				set(P);//Pλ����Ϊ1   (0--N-1)
			}
		}
	}
	explicit My_bitset(const string &S, size_t P = 0, size_t L = (size_t)(-1))
	{
		if (S.size() < P)//����û��Լ����峤�ȵ���ʵ�����볤�Ȳ����ͻ��׳��쳣
			My_Xran();
		if (S.size() - P < L)
			L = S.size() - P;//��������Ϊʵ�ʵ����볤��
		if (N < L)
			L = N;
		My_Tidy();//ȫ����0
		P += L;
		for (size_t i = 0; i < L; ++i)
		{
			if (S[--P] == '1')
				set(i);//��1����1
			else
			{
				if (S[P] != '0')//����1������0�ͻ����쳣
					My_Xinv();
			}
		}

	}
	My_bitset<N>& set()//���û�Ҫʹ�õ����е�λ��1
	{
		My_Tidy(~(T)0);
	}
	My_bitset<N>& set(size_t P, bool X = true)//ָ����Pλ�����������1
	{
		if (N <= P)   //λ��0---N-1�Ϸ�
			My_Xran();
		if (X)//��1
			A[P / Nb] |= (T)1 << P % Nb;   //�ĸ�����Ԫ���е���һ������λ��1
		else//��0
			A[P / Nb] &= ~((T)1 << P % Nb);
		return (*this);
	}
	My_bitset<N>& operator&=(const My_bitset<N>&R)
	{
		if (&R != this)
		{
			for (int i = Nw; 0 <= i; --i)
			{
				A[i] &= R.W(i);
			}
		}
		return *this;
	}
	My_bitset<N>&operator|=(const My_bitset<N>&R)
	{
		if (&R != this)
		{
			for (int i = Nw; 0 <= i; --i)
			{
				A[i] |= R.W(i);
			}
		}
		return *this;
	}
	My_bitset<N>&operator^=(const My_bitset<N>&R)
	{
		if (&R != this)
		{
			for (int i = Nw; 0 <= i; --i)
			{
				A[i] ^= R.W(i);
			}
		}
		return *this;
	}
	My_bitset<N>&operator<<=(int P)
	{
		if (P < 0)
		{
			return(*this >>= P);
		}
		const int D = P / Nb;
		if (D != 0)//�����ֳ��������Ĳ������ƶ�
		{
			for (int i = Nw; 0 <= i; --i)
			{
				A[i] = (D <= i) ? A[i - D] : 0;
			}
			if ((P %= Nb) != 0)//�������ͷ���ֽ����ƶ�
			{
				A[Nw] <<= P;
			}
		}
		else
		{
			A[0] <<= P;
		}

		return *this;
	}
	My_bitset<N>&operator>>=(int P)
	{
		if (P < 0)
		{
			return (*this <<= P);
		}
		const int D = P / Nb;
		if (D != 0)
		{
			for (int i = 0; i <= Nw; ++i)
			{
				A[i] = (D <= Nw - i) ? A[i + D] : 0;
			}
			if ((P %= Nb) != 0)//�������ͷ���ֽ����ƶ�
			{
				A[Nw] >>= P;
			}
		}
		else
		{
			A[0] >>= P;
		}
		return *this;
	}
	bool test(size_t P)const//�ж�Pλ�õĶ�����λ�ǲ���1
	{
		if (N <= P)
			My_Xran();//Խ�����
		return (A[P / Nb] & ((T)1 << (P % Nb))) != 0;
	}
	size_t count()const//����1�ĸ���
	{
		size_t count = 0;
		for (int i = Nw; i >= 0; --i)
		{
			for (int value = A[i]; value != 0; value <<= 4)//��4��bitΪ��λ���м���
			{
				count += "\0\1\1\2\1\2\2\3"
					"\1\2\2\3\2\3\3\4"[value & 0xF];
			}
		}
		return count;
	}
	size_t count_zero()const//����0�ĸ���
	{
		return (N - count());//�û��Ƕ����õ���ֵ
		//return (Allocator_Sum_bit() - count());//�����ڴ���0�ĸ���
	}
	size_t Allocator_Sum_bit()const//�ڴ���ʵ�ʷ����˶��ٸ�����λ
	{
		return ((Nw + 1)*Nb);
	}
	size_t My_count()const //����1�ĸ���
	{
		size_t count = 0;
		for (int i = 0; i <= Nw; ++i)
		{
			size_t tmp = A[i];
			while (tmp)
			{
				tmp &= (tmp - 1);
				++count;
			}
		}
		return count;
	}
	size_t My_count_zero()const //����0�ĸ���
	{
		size_t count = 0;
		for (int i = 0; i <= Nw; ++i)
		{
			size_t tmp = A[i];
			while (tmp + 1)
			{
				tmp |= (tmp + 1);
				++count;
			}
		}
		return count;
	}
	My_bitset<N>& reset()//����0
	{
		My_Tidy();
		return (*this;)
	}
	My_bitset<N>&reset(size_t)//����bitλ����0
	{
		return (set(P, 0));
	}
	My_bitset<N>&flip()//�û�֪����ȫ��bitλ�÷�
	{
		for (int i = Nw; 0 <= i; --i)
		{
			A[i] = ~A[i];
		}
		My_Trim();//�Ѳ�������bit����������ͷ�����û�ʵ�ʿ��Կ�����bitλ���䣬��ʵ�ʷ���������û���������bitΪ��������Ϊ0
		return (*this);
	}
	My_bitset<N>& flip(size_t P)//ָ��λ���÷�
	{
		if (N <= P)
			My_Xran();
		A[P / Nb] ^= (T)1 << P % Nb;
		return (*this);
	}
	unsigned long to_ulong()const//������תunsigned long
	{
		int flag = sizeof(unsigned long);
		if (flag <((Nw+1)*sizeof(T)) && A[(flag%sizeof(T))+1] != 0)//����to_ulong���ܽ��ܵķ�Χ�������
		{
			My_Xoflo();
		}
		unsigned long ret = 0;
		for (int i = Nw; i >= 0; --i)
		{
			ret += A[i];
		}
		return ret;
	}
	string to_string()const
	{
		string S;
		S.reserve(N);
		for (size_t P = N; 0 < P;)
			S += test(--P) ? '1' : '0';
		return (S);
	}
	size_t size()const
	{
		return (N);
	}
	bool operator==(const My_bitset<N>& R) const
	{
		for (int i = Nw; 0 <= i; --)
		{
			if (A[i] != R.W(i))
				return (false);
		}
		return (true);
	}
	bool operator!=(const My_bitset<N>& R) const
	{
		return (!(*this == R));
	}
	bool any() const
	{
		for (int i = Nw; 0 <= i; --i)
		if (_A[i] != 0)
			return (true);
		return (false);
	}
	bool none() const
	{
		return (!any());
	}
	My_bitset<N> operator<<(size_t R) const
	{
		return (My_bitset<N>(*this) <<= R);
	}
	My_bitset<N> operator>>(size_t R) const
	{
		return (My_bitset<N>(*this) >>= R);
	}
	friend My_bitset<N> operator&(const My_bitset<N>& L,const My_bitset<N>& R)
	{
		return (My_bitset<N>(L) &= R);
	}
	friend My_bitset<N> operator|(const My_bitset<N>& L, const My_bitset<N>& R)
	{
		return (My_bitset<N>(L) |= R);
	}
	friend My_bitset<N> operator^(const My_bitset<N>& L, const My_bitset<N>& R)
	{
		return (My_bitset<N>(L) ^= R);
	}
	friend ostream& operator<<(ostream& Out, const My_bitset<N>& R)
	{
		for (size_t P = N; 0 < P;)
		{
			Out << (R.test(--P) ? '1' : '0');
		}
		return (Out);
	}
	friend istream& operator>>(istream& in,  My_bitset<N>& R)
	{
		char*str = new char[N];
		for (size_t j = 0; j < N; ++j)
		{
			str[j] = '0';
		}
		in >>str;
		int tmp = (int)N;
		for (size_t i =0; i<N && tmp>=0;)
		{
			if (str[--tmp] == '1')
			{
				R.set(i);//��1����1
				++i;
			}
			else
			{
				if (tmp >=0 && str[tmp] != '1' && str[tmp] != '0' && str[tmp] != 1 && str[tmp] != 0)//����1������0�ͻ����쳣
				{
					R.My_Xinv();
				}
			}
			
		}
		return in;
	}
	T  W(size_t i) const
	{
		return (A[i]);
	}
private:
	 My_bitset(const My_bitset<N>&);//ֱ�Ӳ�����������
	enum{ 
		  Nb = CHAR_BIT *sizeof(T),//unsigned long �ܹ�����λ
		  Nw = (N == 0) ? 0 : (N-1)/Nb//��һ��Ŀ����Ϊ�˱�֤Nwÿ�ζ����������λ���±�(��Ҫ��unsigned long ����)
	     };
	void My_Tidy(T x = 0)//����
	{
		for (int I = Nw; 0 <= I; --I)
		{
			A[I] = x;
		}
		if (x != 0)
		{
			My_Trim();
		}
	}
	void My_Trim()//�޼�
	{
		if (N % Nb != 0)
		{
			A[Nw] &= ((T)1 << N % Nb) - 1;
		}
	}

	void My_Xinv() const
	{
		THROW(invalid_argument, "invalid bitset<N> char");
	}
	void My_Xoflo() const
	{
		THROW(overflow_error,"bitset<N> conversion overflow");
	}
	void My_Xran() const
	{
		THROW(out_of_range, "invalid bitset<N> position");
	}


	T A[Nw + 1];
};