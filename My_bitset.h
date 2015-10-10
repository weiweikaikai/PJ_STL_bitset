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
				Pbs->set(off, bool(Bs));//C++版的强制类型转换和C语言的(bool)Bs一样
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
		My_reference(My_bitset<N>&X, size_t P) :Pbs(&X), off(P)//也称作工厂函数
		{}//存在的拷贝构造函数不仅将参数为这种的拷贝构造设置为私有而且将构造函数设为私有
		//也即是系统不会在提供默认的构造函数，但是因为这种的参数与默认的拷贝构造函数不同，所以系统还会提供默认的拷贝构造
		My_reference(My_reference &);

		My_bitset<N> *Pbs;
		size_t off;
	};
	STCONS(size_t, bitset_size, N);
	bool at(size_t P)const//返回P位置的比特位 因为存在重载只有常对象可以访问
	{                   //有越界检查
		if (N <= P)
			My_Xran();
		return test(P);
	}//
	My_reference at(size_t P)//支持 at[i]=at[j]
	{
		if (N <= P)
			My_Xran();
		return (My_reference(*this, P));
	}
	bool operator[](size_t P)const//和at功能相同，但是没有越界检查
	{            //因为存在重载只有常对象可以访问
		return test(P);
	}
	My_reference operator[](size_t P)//支持 b[i] = b[j]
	{
		return (My_reference(*this, P));
	}
	My_bitset()
	{
		My_Tidy();
	}
	My_bitset(unsigned long X)
	{
		My_Tidy();//用户给的所有位全部置零
		for (size_t P = 0; X != 0 && P < N; X >>= 1, ++P)
		{
			if (X & 1)
			{
				set(P);//P位置置为1   (0--N-1)
			}
		}
	}
	explicit My_bitset(const string &S, size_t P = 0, size_t L = (size_t)(-1))
	{
		if (S.size() < P)//如果用户自己定义长度单数实际输入长度不够就会抛出异常
			My_Xran();
		if (S.size() - P < L)
			L = S.size() - P;//将长度设为实际的输入长度
		if (N < L)
			L = N;
		My_Tidy();//全部置0
		P += L;
		for (size_t i = 0; i < L; ++i)
		{
			if (S[--P] == '1')
				set(i);//是1就置1
			else
			{
				if (S[P] != '0')//不是1还不是0就会抛异常
					My_Xinv();
			}
		}

	}
	My_bitset<N>& set()//将用户要使用的所有的位置1
	{
		My_Tidy(~(T)0);
	}
	My_bitset<N>& set(size_t P, bool X = true)//指定的P位置置零或者置1
	{
		if (N <= P)   //位置0---N-1合法
			My_Xran();
		if (X)//置1
			A[P / Nb] |= (T)1 << P % Nb;   //哪个数组元素中的哪一个比特位置1
		else//置0
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
		if (D != 0)//把是字长整数倍的部分先移动
		{
			for (int i = Nw; 0 <= i; --i)
			{
				A[i] = (D <= i) ? A[i - D] : 0;
			}
			if ((P %= Nb) != 0)//其余的零头部分进行移动
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
			if ((P %= Nb) != 0)//其余的零头部分进行移动
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
	bool test(size_t P)const//判断P位置的二进制位是不是1
	{
		if (N <= P)
			My_Xran();//越界操作
		return (A[P / Nb] & ((T)1 << (P % Nb))) != 0;
	}
	size_t count()const//计算1的个数
	{
		size_t count = 0;
		for (int i = Nw; i >= 0; --i)
		{
			for (int value = A[i]; value != 0; value <<= 4)//以4个bit为单位进行计数
			{
				count += "\0\1\1\2\1\2\2\3"
					"\1\2\2\3\2\3\3\4"[value & 0xF];
			}
		}
		return count;
	}
	size_t count_zero()const//计算0的个数
	{
		return (N - count());//用户角度所得到的值
		//return (Allocator_Sum_bit() - count());//真正内存中0的个数
	}
	size_t Allocator_Sum_bit()const//内存中实际分配了多少个比特位
	{
		return ((Nw + 1)*Nb);
	}
	size_t My_count()const //计算1的个数
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
	size_t My_count_zero()const //计算0的个数
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
	My_bitset<N>& reset()//重置0
	{
		My_Tidy();
		return (*this;)
	}
	My_bitset<N>&reset(size_t)//部分bit位重置0
	{
		return (set(P, 0));
	}
	My_bitset<N>&flip()//用户知道的全部bit位置反
	{
		for (int i = Nw; 0 <= i; --i)
		{
			A[i] = ~A[i];
		}
		My_Trim();//把不是类型bit整数倍的零头部分用户实际可以看到的bit位不变，把实际分配的其他用户看不到的bit为重新设置为0
		return (*this);
	}
	My_bitset<N>& flip(size_t P)//指定位置置反
	{
		if (N <= P)
			My_Xran();
		A[P / Nb] ^= (T)1 << P % Nb;
		return (*this);
	}
	unsigned long to_ulong()const//二进制转unsigned long
	{
		int flag = sizeof(unsigned long);
		if (flag <((Nw+1)*sizeof(T)) && A[(flag%sizeof(T))+1] != 0)//超出to_ulong所能接受的范围就是溢出
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
				R.set(i);//是1就置1
				++i;
			}
			else
			{
				if (tmp >=0 && str[tmp] != '1' && str[tmp] != '0' && str[tmp] != 1 && str[tmp] != 0)//不是1还不是0就会抛异常
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
	 My_bitset(const My_bitset<N>&);//直接不允许拷贝构造
	enum{ 
		  Nb = CHAR_BIT *sizeof(T),//unsigned long 总共多少位
		  Nw = (N == 0) ? 0 : (N-1)/Nb//减一的目的是为了保证Nw每次都是数组最高位的下标(需要的unsigned long 个数)
	     };
	void My_Tidy(T x = 0)//整理
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
	void My_Trim()//修剪
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