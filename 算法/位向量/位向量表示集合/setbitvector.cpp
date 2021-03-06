// setbitvector.cpp: 定义控制台应用程序的入口点。
//
// 位向量来表示集合

#include "stdafx.h"

#define BITSPERWORD  32                   // 一个int型有32个位可以表示32个数
#define SHIFT        5                    // 32位就相当于移动5位
#define MASK         0x1F                 // 获取一个数的最后5位
#define N            10000000             // 共10000000个数
int a[1 + N/BITSPERWORD];                 // 一共需要1+N/BITSPERWORD个int来表示10000000个数

// i >> SHIFT相当于除以32取整， i & MASK相当于除以32后的余数
void  set(int i)  {        a[i >> SHIFT] |=       (1 << (i & MASK)); }           // 设置第i个数存在
void  clr(int i)  {        a[i >> SHIFT] &=      ~(1 << (i & MASK)); }           // 设置第i个数不存在
int   test(int i) { return a[i >> SHIFT] &        (1 << (i & MASK)); }           // 检验第i个数是否存在


int main()
{
	int i;
	for (i = 0; i < N; i++)
		clr(i);	
	while (scanf("%d", &i) != EOF)
		set(i);
	for (i = 0; i < N; i++)
		if (test(i))
			printf("%d\n", i);
    return 0;
}

