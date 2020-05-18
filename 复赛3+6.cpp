#include <atomic>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
#include<iostream>
using namespace std;

#define MAXROW 2000100
#define MAXROW3 6000100
#define MAXOUTSIZE 20000000
#define HALFOUTSIZE 10000000

template <class T>
void measure(T&& func) {
	using namespace std::chrono;
	auto start = system_clock::now();
	func();
	duration<double> diff = system_clock::now() - start;
	cout << "elapsed: " << diff.count() << "seconds" << endl;
}

struct edge
{
	int to;
	int money;
	inline bool operator == (const int &pos) {
		return (this->to == pos);
	}
};

inline bool mycom(const edge a, const edge b)
{
	return a.to < b.to;
}

int edtotal[MAXROW3];
int total[MAXROW3];
int totallen;
int setnum[MAXROW];
int newsetnum[MAXROW];
int newsetlen;
int stack[MAXROW];
char* anstr[100];
char vis1[MAXROW] = { 0 };
int visgn1[MAXROW3];

char vis2[MAXROW] = { 0 };
int visgn2[MAXROW3];

char vis3[MAXROW] = { 0 };
int visgn3[MAXROW3];

char vis4[MAXROW] = { 0 };
int visgn4[MAXROW3];

int gap[100];
int pos[100];
int setlen = 1;
unordered_map<int, int> edindex;

int pic[MAXROW];
int Npic[MAXROW];
edge realpic[MAXROW];
edge realNpic[MAXROW];
atomic_int picnum[MAXROW];
atomic_int Npicnum[MAXROW];
int newpicnum[MAXROW];
int newNpicnum[MAXROW];
atomic_int picdegree[MAXROW];
atomic_int Npicdegree[MAXROW];

int res31[3 * HALFOUTSIZE];
int res32[3 * HALFOUTSIZE];
int res33[3 * HALFOUTSIZE];
int res34[3 * HALFOUTSIZE];
int res41[4 * HALFOUTSIZE];
int res42[4 * HALFOUTSIZE];
int res43[4 * HALFOUTSIZE];
int res44[4 * HALFOUTSIZE];
int res51[5 * HALFOUTSIZE];
int res52[5 * HALFOUTSIZE];
int res53[5 * HALFOUTSIZE];
int res54[5 * HALFOUTSIZE];
int res61[6 * HALFOUTSIZE];
int res62[6 * HALFOUTSIZE];
int res63[6 * HALFOUTSIZE];
int res64[6 * HALFOUTSIZE];
int res71[7 * MAXOUTSIZE];
int res72[7 * MAXOUTSIZE];
int res73[7 * MAXOUTSIZE];
int res74[7 * MAXOUTSIZE];
int* res3p[MAXROW];
int* res4p[MAXROW];
int* res5p[MAXROW];
int* res6p[MAXROW];
int* res7p[MAXROW];
int res31len = 0;
int res41len = 0;
int res51len = 0;
int res61len = 0;
int res71len = 0;
int res32len = 0;
int res42len = 0;
int res52len = 0;
int res62len = 0;
int res72len = 0;
int res33len = 0;
int res43len = 0;
int res53len = 0;
int res63len = 0;
int res73len = 0;
int res34len = 0;
int res44len = 0;
int res54len = 0;
int res64len = 0;
int res74len = 0;
int res3len;
int res4len;
int res5len;
int res6len;
int res7len;


void loaddata(char* loadpath)
{
	FILE* pFile = fopen(loadpath, "rb");
	fseek(pFile, 0, SEEK_END);
	int len = ftell(pFile);
	char* szBuf = new char[len];
	fseek(pFile, 0, SEEK_SET);
	int iRead = fread(szBuf, 1, len, pFile);
	fclose(pFile);
	int num = 0;
	totallen = 0;
	for (int i = 0; i < len; i++)
	{
		if (szBuf[i] >= '0' && szBuf[i] <= '9')
		{
			num = num * 10 + (szBuf[i] - '0');
			continue;
		}
		if (szBuf[i] == ',' || szBuf[i] == '\n')
		{
			if (!num&&totallen % 3 == 2)totallen -= 2;
			else total[totallen++] = num;
			num = 0;
		}
	}
	delete[] szBuf;
}

void mkdeg1()
{
	for (int i = 0; i < totallen / 12 * 3; i += 3)
	{
		picdegree[edindex[total[i]]]++;
		Npicdegree[edindex[total[i + 1]]]++;
	}
}

void mkdeg2()
{
	for (int i = totallen / 12 * 3; i < totallen / 6 * 3; i += 3)
	{
		picdegree[edindex[total[i]]]++;
		Npicdegree[edindex[total[i + 1]]]++;
	}
}

void mkdeg3()
{
	for (int i = totallen / 6 * 3; i < totallen / 4 * 3; i += 3)
	{
		picdegree[edindex[total[i]]]++;
		Npicdegree[edindex[total[i + 1]]]++;
	}
}

void mkdeg4()
{
	for (int i = totallen / 4 * 3; i < totallen; i += 3)
	{
		picdegree[edindex[total[i]]]++;
		Npicdegree[edindex[total[i + 1]]]++;
	}
}

void mkpic1()
{
	edge egtmp;
	int temp1, temp2, temp3, temp4;
	for (int i = 0; i < totallen / 12 * 3; i += 3)
	{
		temp4 = total[i + 1];
		if (edindex[temp4])
		{
			temp2 = edindex[temp4];
			temp1 = edindex[total[i]];
			temp3 = total[i + 2];
			egtmp.to = temp2; egtmp.money = temp3;
			realpic[pic[temp1] + picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			realNpic[Npic[temp2] + Npicnum[temp2]++] = egtmp;

		}
	}
}

void mkpic2()
{
	edge egtmp;
	int temp1, temp2, temp3, temp4;
	for (int i = totallen / 12 * 3; i < totallen / 6 * 3; i += 3)
	{
		temp4 = total[i + 1];
		if (edindex[temp4])
		{
			temp2 = edindex[temp4];
			temp1 = edindex[total[i]];
			temp3 = total[i + 2];
			egtmp.to = temp2; egtmp.money = temp3;
			realpic[pic[temp1] + picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			realNpic[Npic[temp2] + Npicnum[temp2]++] = egtmp;

		}
	}
}

void mkpic3()
{
	edge egtmp;
	int temp1, temp2, temp3, temp4;
	for (int i = totallen / 6 * 3; i < totallen / 4 * 3; i += 3)
	{
		temp4 = total[i + 1];
		if (edindex[temp4])
		{
			temp2 = edindex[temp4];
			temp1 = edindex[total[i]];
			temp3 = total[i + 2];
			egtmp.to = temp2; egtmp.money = temp3;
			realpic[pic[temp1] + picnum[temp1]++] = egtmp;

			egtmp.to = temp1;
			realNpic[Npic[temp2] + Npicnum[temp2]++] = egtmp;

		}
	}
}

void mkpic4()
{
	edge egtmp;
	int temp1, temp2, temp3, temp4;
	for (int i = totallen / 4 * 3; i < totallen; i += 3)
	{
		temp4 = total[i + 1];
		if (edindex[temp4])
		{
			temp2 = edindex[temp4];
			temp1 = edindex[total[i]];
			temp3 = total[i + 2];
			egtmp.to = temp2; egtmp.money = temp3;
			realpic[pic[temp1] + picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			realNpic[Npic[temp2] + Npicnum[temp2]++] = egtmp;

		}
	}
}

void makestar()
{
	for (int i = 0; i < totallen; i += 3)
	{
		setnum[setlen++] = total[i];
	}
	sort(setnum + 1, setnum + setlen);
	setlen = unique(setnum + 1, setnum + setlen) - setnum;
	for (int i = 1; i < setlen; i++)
	{
		edindex[setnum[i]] = i;
	}
	thread mg1(mkdeg1);
	thread mg2(mkdeg2);
	thread mg3(mkdeg3);
	thread mg4(mkdeg4);
	mg1.join();
	mg2.join();
	mg3.join();
	mg4.join();
	pic[1] = 0;
	Npic[1] = 0;
	for (int i = 2; i < setlen; i++)
	{
		pic[i] = pic[i - 1] + picdegree[i - 1];
		Npic[i] = Npic[i - 1] + Npicdegree[i - 1];
	}
	thread mp1(mkpic1);
	thread mp2(mkpic2);
	thread mp3(mkpic3);
	thread mp4(mkpic4);
	mp1.join();
	mp2.join();
	mp3.join();
	mp4.join();
}
void jianzhi()
{
	memcpy(newpicnum, picnum, setlen * sizeof(int));
	memcpy(newNpicnum, Npicnum, setlen * sizeof(int));
	int top = 0;
	int x;
	for (int i = 1; i < setlen; i++)
	{
		if (newNpicnum[i] == 0)stack[top++] = i;
	}
	top--;
	while (top > -1)
	{
		x = stack[top--];
		for (int i = 0; i < picnum[x]; i++)
		{
			int temp = realpic[pic[x] + i].to;
			if (temp >= setlen)continue;
			if (--newNpicnum[temp] == 0)stack[++top] = temp;
			find(&realNpic[Npic[temp]], &realNpic[Npic[temp] + Npicnum[temp]], x)->to = setlen;
		}
		newNpicnum[x] = -2;
		picnum[x] = 0;
		Npicnum[x] = 0;
	}
	top = 0;
	for (int i = 1; i < setlen; i++)
	{
		if (newpicnum[i] == 0)stack[top++] = i;
	}
	top--;
	while (top > -1)
	{
		int x = stack[top--];
		for (int i = 0; i < Npicnum[x]; i++)
		{
			int temp = realNpic[Npic[x] + i].to;
			if (temp >= setlen)continue;
			if (--newpicnum[temp] == 0)stack[++top] = temp;
			find(&realpic[pic[temp]], &realpic[pic[temp] + picnum[temp]], x)->to = setlen;
		}
		newNpicnum[x] = -2;
		picnum[x] = 0;
		Npicnum[x] = 0;
	}
}

void sort1()
{
	int k;
	for (int i = 1; i < newsetlen / 4; i++)
	{
		k = newsetnum[i];
		sort(&realpic[pic[k]], &realpic[pic[k] + picnum[k]], mycom);
		sort(&realNpic[Npic[k]], &realNpic[Npic[k] + Npicnum[k]], mycom);
	}
}

void sort2()
{
	int k;
	for (int i = newsetlen / 4; i < newsetlen / 2; i++)
	{
		k = newsetnum[i];
		sort(&realpic[pic[k]], &realpic[pic[k] + picnum[k]], mycom);
		sort(&realNpic[Npic[k]], &realNpic[Npic[k] + Npicnum[k]], mycom);
	}
}

void sort3()
{
	int k;
	for (int i = newsetlen / 2; i < newsetlen / 4 * 3; i++)
	{
		k = newsetnum[i];
		sort(&realpic[pic[k]], &realpic[pic[k] + picnum[k]], mycom);
		sort(&realNpic[Npic[k]], &realNpic[Npic[k] + Npicnum[k]], mycom);
	}
}

void sort4()
{
	int k;
	for (int i = newsetlen / 4 * 3; i < newsetlen; i++)
	{
		k = newsetnum[i];
		sort(&realpic[pic[k]], &realpic[pic[k] + picnum[k]], mycom);
		sort(&realNpic[Npic[k]], &realNpic[Npic[k] + Npicnum[k]], mycom);
	}
}
void toeasy()
{
	int fw = 0;
	for (int i = 1; i < setlen; i++)
	{
		newsetnum[i - fw] = i;
		if (!picnum[i])fw++;
	}
	newsetlen = setlen - fw;
	thread st1(sort1);
	thread st2(sort2);
	thread st3(sort3);
	thread st4(sort4);
	st1.join();
	st2.join();
	st3.join();
	st4.join();
}

void checkcircle1()
{
	int visgnum1 = 0;

	int k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;
	int64_t m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;

	int length0, length1, length2, length3, length4, length5, length6, length7, length8;
	int i0 = 1;
	for (; i0 < newsetlen; i0 += 4)
	{
		k0 = newsetnum[i0];
		res3p[i0] = res31 + res31len * 3;
		res4p[i0] = res41 + res41len * 4;
		res5p[i0] = res51 + res51len * 5;
		res6p[i0] = res61 + res61len * 6;
		res7p[i0] = res71 + res71len * 7;
		length0 = newNpicnum[k0];
		for (int i1 = 0; i1 < length0; i1++)
		{
			k1 = realNpic[Npic[k0] + i1].to;
			if (k1 <= k0)continue;
			m1 = realNpic[Npic[k0] + i1].money;
			vis1[k1] = 3;
			visgn1[visgnum1++] = k1;
			length1 = newNpicnum[k1];
			for (int i2 = 0; i2 < length1; i2++)
			{
				k2 = realNpic[Npic[k1] + i2].to;
				if (k2 <= k0)continue;
				m2 = realNpic[Npic[k1] + i2].money;
				if (m1 > 3 * m2 || 5 * m1 < m2)continue;
				if (vis1[k2] <= 1)
				{
					vis1[k2] = 2;
					visgn1[visgnum1++] = k2;
				}
				length2 = newNpicnum[k2];
				for (int i3 = 0; i3 < length2; i3++)
				{
					k3 = realNpic[Npic[k2] + i3].to;
					if (k3 <= k0 || vis1[k3])continue;
					m3 = realNpic[Npic[k2] + i3].money;
					if (m2 > 3 * m3 || 5 * m2 < m3)continue;
					vis1[k3] = 1;
					visgn1[visgnum1++] = k3;
				}
			}
		}
		vis1[k0] = 3;
		visgn1[visgnum1++] = k0;
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)
		{
			k4 = realpic[pic[k0] + i4].to;
			if (k4 <= k0)continue;
			m4 = realpic[pic[k0] + i4].money;
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				k5 = realpic[pic[k4] + i5].to;
				if (k5 <= k0)continue;
				m5 = realpic[pic[k4] + i5].money;
				if (m5 > 3 * m4 || 5 * m5 < m4)continue;
				length5 = newpicnum[k5];
				for (int i6 = 0; i6 < length5; i6++)
				{
					k6 = realpic[pic[k5] + i6].to;
					if (k6 < k0 || k6 == k4)continue;
					m6 = realpic[pic[k5] + i6].money;
					if (m6 > 3 * m5 || 5 * m6 < m5)continue;
					if (k6 == k0) {
						if (m4 > 3 * m6 || 5 * m4 < m6)continue;
						res31[res31len * 3] = k0;
						res31[res31len * 3 + 1] = k4;
						res31[res31len * 3 + 2] = k5;
						res31len++;
						continue;
					}
					length6 = newpicnum[k6];
					for (int i7 = 0; i7 < length6; i7++)
					{
						k7 = realpic[pic[k6] + i7].to;
						if (!vis1[k7] || k7 == k4 || k7 == k5)continue;
						m7 = realpic[pic[k6] + i7].money;
						if (m7 > 3 * m6 || 5 * m7 < m6)continue;
						if (k7 == k0) {
							if (m4 > 3 * m7 || 5 * m4 < m7)continue;
							res41[res41len * 4] = k0;
							res41[res41len * 4 + 1] = k4;
							res41[res41len * 4 + 2] = k5;
							res41[res41len * 4 + 3] = k6;
							res41len++;
							continue;
						}
						length7 = newpicnum[k7];
						for (int i8 = 0; i8 < length7; i8++)
						{
							k8 = realpic[pic[k7] + i8].to;
							if (vis1[k8] <= 1 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							m8 = realpic[pic[k7] + i8].money;
							if (m8 > 3 * m7 || 5 * m8 < m7)continue;
							if (k8 == k0) {
								if (m4 > 3 * m8 || 5 * m4 < m8)continue;
								res51[res51len * 5] = k0;
								res51[res51len * 5 + 1] = k4;
								res51[res51len * 5 + 2] = k5;
								res51[res51len * 5 + 3] = k6;
								res51[res51len * 5 + 4] = k7;
								res51len++;
								continue;
							}
							length8 = newpicnum[k8];
							for (int i9 = 0; i9 < length8; i9++)
							{
								k9 = realpic[pic[k8] + i9].to;
								if (vis1[k9] <= 2 || k9 == k4 || k9 == k5 || k9 == k6 || k9 == k7)continue;
								m9 = realpic[pic[k8] + i9].money;
								if (m9 > 3 * m8 || 5 * m9 < m8)continue;
								if (k9 == k0) {
									if (m4 > 3 * m9 || 5 * m4 < m9)continue;
									res61[res61len * 6] = k0;
									res61[res61len * 6 + 1] = k4;
									res61[res61len * 6 + 2] = k5;
									res61[res61len * 6 + 3] = k6;
									res61[res61len * 6 + 4] = k7;
									res61[res61len * 6 + 5] = k8;
									res61len++;
									continue;
								}
								m10 = find(&realpic[pic[k9]], &realpic[pic[k9] + newpicnum[k9]], k0)->money;
								if (m10 > 3 * m9 || m4 > 3 * m10 || 5 * m10 < m9 || 5 * m4 < m10)continue;
								res71[res71len * 7] = k0;
								res71[res71len * 7 + 1] = k4;
								res71[res71len * 7 + 2] = k5;
								res71[res71len * 7 + 3] = k6;
								res71[res71len * 7 + 4] = k7;
								res71[res71len * 7 + 5] = k8;
								res71[res71len * 7 + 6] = k9;
								res71len++;
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < visgnum1; i++)
		{
			vis1[visgn1[i]] = 0;
		}
		visgnum1 = 0;
	}
	res3p[i0] = res31 + res31len * 3;
	res4p[i0] = res41 + res41len * 4;
	res5p[i0] = res51 + res51len * 5;
	res6p[i0] = res61 + res61len * 6;
	res7p[i0] = res71 + res71len * 7;
}

void checkcircle2()
{
	int visgnum2 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;
	int64_t m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;
	int length0, length1, length2, length3, length4, length5, length6, length7, length8;
	int i0 = 2;
	for (; i0 < newsetlen; i0 += 4)
	{
		k0 = newsetnum[i0];
		res3p[i0] = res32 + res32len * 3;
		res4p[i0] = res42 + res42len * 4;
		res5p[i0] = res52 + res52len * 5;
		res6p[i0] = res62 + res62len * 6;
		res7p[i0] = res72 + res72len * 7;
		length0 = newNpicnum[k0];
		for (int i1 = 0; i1 < length0; i1++)
		{
			k1 = realNpic[Npic[k0] + i1].to;
			if (k1 <= k0)continue;
			m1 = realNpic[Npic[k0] + i1].money;
			vis2[k1] = 3;
			visgn2[visgnum2++] = k1;
			length1 = newNpicnum[k1];
			for (int i2 = 0; i2 < length1; i2++)
			{
				k2 = realNpic[Npic[k1] + i2].to;
				if (k2 <= k0)continue;
				m2 = realNpic[Npic[k1] + i2].money;
				if (m1 > 3 * m2 || 5 * m1 < m2)continue;
				if (vis2[k2] <= 1)
				{
					vis2[k2] = 2;
					visgn2[visgnum2++] = k2;
				}
				length2 = newNpicnum[k2];
				for (int i3 = 0; i3 < length2; i3++)
				{
					k3 = realNpic[Npic[k2] + i3].to;
					if (k3 <= k0 || vis2[k3])continue;
					m3 = realNpic[Npic[k2] + i3].money;
					if (m2 > 3 * m3 || 5 * m2 < m3)continue;
					vis2[k3] = 1;
					visgn2[visgnum2++] = k3;
				}
			}
		}
		vis2[k0] = 3;
		visgn2[visgnum2++] = k0;
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)
		{
			k4 = realpic[pic[k0] + i4].to;
			if (k4 <= k0)continue;
			m4 = realpic[pic[k0] + i4].money;
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				k5 = realpic[pic[k4] + i5].to;
				if (k5 <= k0)continue;
				m5 = realpic[pic[k4] + i5].money;
				if (m5 > 3 * m4 || 5 * m5 < m4)continue;
				length5 = newpicnum[k5];
				for (int i6 = 0; i6 < length5; i6++)
				{
					k6 = realpic[pic[k5] + i6].to;
					if (k6 < k0 || k6 == k4)continue;
					m6 = realpic[pic[k5] + i6].money;
					if (m6 > 3 * m5 || 5 * m6 < m5)continue;
					if (k6 == k0) {
						if (m4 > 3 * m6 || 5 * m4 < m6)continue;
						res32[res32len * 3] = k0;
						res32[res32len * 3 + 1] = k4;
						res32[res32len * 3 + 2] = k5;
						res32len++;
						continue;
					}
					length6 = newpicnum[k6];
					for (int i7 = 0; i7 < length6; i7++)
					{
						k7 = realpic[pic[k6] + i7].to;
						if (!vis2[k7] || k7 == k4 || k7 == k5)continue;
						m7 = realpic[pic[k6] + i7].money;
						if (m7 > 3 * m6 || 5 * m7 < m6)continue;
						if (k7 == k0) {
							if (m4 > 3 * m7 || 5 * m4 < m7)continue;
							res42[res42len * 4] = k0;
							res42[res42len * 4 + 1] = k4;
							res42[res42len * 4 + 2] = k5;
							res42[res42len * 4 + 3] = k6;
							res42len++;
							continue;
						}
						length7 = newpicnum[k7];
						for (int i8 = 0; i8 < length7; i8++)
						{
							k8 = realpic[pic[k7] + i8].to;
							if (vis2[k8] <= 1 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							m8 = realpic[pic[k7] + i8].money;
							if (m8 > 3 * m7 || 5 * m8 < m7)continue;
							if (k8 == k0) {
								if (m4 > 3 * m8 || 5 * m4 < m8)continue;
								res52[res52len * 5] = k0;
								res52[res52len * 5 + 1] = k4;
								res52[res52len * 5 + 2] = k5;
								res52[res52len * 5 + 3] = k6;
								res52[res52len * 5 + 4] = k7;
								res52len++;
								continue;
							}
							length8 = newpicnum[k8];
							for (int i9 = 0; i9 < length8; i9++)
							{
								k9 = realpic[pic[k8] + i9].to;
								if (vis2[k9] <= 2 || k9 == k4 || k9 == k5 || k9 == k6 || k9 == k7)continue;
								m9 = realpic[pic[k8] + i9].money;
								if (m9 > 3 * m8 || 5 * m9 < m8)continue;
								if (k9 == k0) {
									if (m4 > 3 * m9 || 5 * m4 < m9)continue;
									res62[res62len * 6] = k0;
									res62[res62len * 6 + 1] = k4;
									res62[res62len * 6 + 2] = k5;
									res62[res62len * 6 + 3] = k6;
									res62[res62len * 6 + 4] = k7;
									res62[res62len * 6 + 5] = k8;
									res62len++;
									continue;
								}
								m10 = find(&realpic[pic[k9]], &realpic[pic[k9] + newpicnum[k9]], k0)->money;
								if (m10 > 3 * m9 || m4 > 3 * m10 || 5 * m10 < m9 || 5 * m4 < m10)continue;
								res72[res72len * 7] = k0;
								res72[res72len * 7 + 1] = k4;
								res72[res72len * 7 + 2] = k5;
								res72[res72len * 7 + 3] = k6;
								res72[res72len * 7 + 4] = k7;
								res72[res72len * 7 + 5] = k8;
								res72[res72len * 7 + 6] = k9;
								res72len++;
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < visgnum2; i++)
		{
			vis2[visgn2[i]] = 0;
		}
		visgnum2 = 0;
	}
	res3p[i0] = res32 + res32len * 3;
	res4p[i0] = res42 + res42len * 4;
	res5p[i0] = res52 + res52len * 5;
	res6p[i0] = res62 + res62len * 6;
	res7p[i0] = res72 + res72len * 7;
}

void checkcircle3()
{
	int visgnum3 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;
	int64_t m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;
	int length0, length1, length2, length3, length4, length5, length6, length7, length8;
	int i0 = 3;
	for (; i0 < newsetlen; i0 += 4)
	{
		k0 = newsetnum[i0];
		res3p[i0] = res33 + res33len * 3;
		res4p[i0] = res43 + res43len * 4;
		res5p[i0] = res53 + res53len * 5;
		res6p[i0] = res63 + res63len * 6;
		res7p[i0] = res73 + res73len * 7;
		length0 = newNpicnum[k0];
		for (int i1 = 0; i1 < length0; i1++)
		{
			k1 = realNpic[Npic[k0] + i1].to;
			if (k1 <= k0)continue;
			m1 = realNpic[Npic[k0] + i1].money;
			vis3[k1] = 3;
			visgn3[visgnum3++] = k1;
			length1 = newNpicnum[k1];
			for (int i2 = 0; i2 < length1; i2++)
			{
				k2 = realNpic[Npic[k1] + i2].to;
				if (k2 <= k0)continue;
				m2 = realNpic[Npic[k1] + i2].money;
				if (m1 > 3 * m2 || 5 * m1 < m2)continue;
				if (vis3[k2] <= 1)
				{
					vis3[k2] = 2;
					visgn3[visgnum3++] = k2;
				}
				length2 = newNpicnum[k2];
				for (int i3 = 0; i3 < length2; i3++)
				{
					k3 = realNpic[Npic[k2] + i3].to;
					if (k3 <= k0 || vis3[k3])continue;
					m3 = realNpic[Npic[k2] + i3].money;
					if (m2 > 3 * m3 || 5 * m2 < m3)continue;
					vis3[k3] = 1;
					visgn3[visgnum3++] = k3;
				}
			}
		}
		vis3[k0] = 3;
		visgn3[visgnum3++] = k0;
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)
		{
			k4 = realpic[pic[k0] + i4].to;
			if (k4 <= k0)continue;
			m4 = realpic[pic[k0] + i4].money;
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				k5 = realpic[pic[k4] + i5].to;
				if (k5 <= k0)continue;
				m5 = realpic[pic[k4] + i5].money;
				if (m5 > 3 * m4 || 5 * m5 < m4)continue;
				length5 = newpicnum[k5];
				for (int i6 = 0; i6 < length5; i6++)
				{
					k6 = realpic[pic[k5] + i6].to;
					if (k6 < k0 || k6 == k4)continue;
					m6 = realpic[pic[k5] + i6].money;
					if (m6 > 3 * m5 || 5 * m6 < m5)continue;
					if (k6 == k0) {
						if (m4 > 3 * m6 || 5 * m4 < m6)continue;
						res33[res33len * 3] = k0;
						res33[res33len * 3 + 1] = k4;
						res33[res33len * 3 + 2] = k5;
						res33len++;
						continue;
					}
					length6 = newpicnum[k6];
					for (int i7 = 0; i7 < length6; i7++)
					{
						k7 = realpic[pic[k6] + i7].to;
						if (!vis3[k7] || k7 == k4 || k7 == k5)continue;
						m7 = realpic[pic[k6] + i7].money;
						if (m7 > 3 * m6 || 5 * m7 < m6)continue;
						if (k7 == k0) {
							if (m4 > 3 * m7 || 5 * m4 < m7)continue;
							res43[res43len * 4] = k0;
							res43[res43len * 4 + 1] = k4;
							res43[res43len * 4 + 2] = k5;
							res43[res43len * 4 + 3] = k6;
							res43len++;
							continue;
						}
						length7 = newpicnum[k7];
						for (int i8 = 0; i8 < length7; i8++)
						{
							k8 = realpic[pic[k7] + i8].to;
							if (vis3[k8] <= 1 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							m8 = realpic[pic[k7] + i8].money;
							if (m8 > 3 * m7 || 5 * m8 < m7)continue;
							if (k8 == k0) {
								if (m4 > 3 * m8 || 5 * m4 < m8)continue;
								res53[res53len * 5] = k0;
								res53[res53len * 5 + 1] = k4;
								res53[res53len * 5 + 2] = k5;
								res53[res53len * 5 + 3] = k6;
								res53[res53len * 5 + 4] = k7;
								res53len++;
								continue;
							}
							length8 = newpicnum[k8];
							for (int i9 = 0; i9 < length8; i9++)
							{
								k9 = realpic[pic[k8] + i9].to;
								if (vis3[k9] <= 2 || k9 == k4 || k9 == k5 || k9 == k6 || k9 == k7)continue;
								m9 = realpic[pic[k8] + i9].money;
								if (m9 > 3 * m8 || 5 * m9 < m8)continue;
								if (k9 == k0) {
									if (m4 > 3 * m9 || 5 * m4 < m9)continue;
									res63[res63len * 6] = k0;
									res63[res63len * 6 + 1] = k4;
									res63[res63len * 6 + 2] = k5;
									res63[res63len * 6 + 3] = k6;
									res63[res63len * 6 + 4] = k7;
									res63[res63len * 6 + 5] = k8;
									res63len++;
									continue;
								}
								m10 = find(&realpic[pic[k9]], &realpic[pic[k9] + newpicnum[k9]], k0)->money;
								if (m10 > 3 * m9 || m4 > 3 * m10 || 5 * m10 < m9 || 5 * m4 < m10)continue;
								res73[res73len * 7] = k0;
								res73[res73len * 7 + 1] = k4;
								res73[res73len * 7 + 2] = k5;
								res73[res73len * 7 + 3] = k6;
								res73[res73len * 7 + 4] = k7;
								res73[res73len * 7 + 5] = k8;
								res73[res73len * 7 + 6] = k9;
								res73len++;
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < visgnum3; i++)
		{
			vis3[visgn3[i]] = 0;
		}
		visgnum3 = 0;
	}
	res3p[i0] = res33 + res33len * 3;
	res4p[i0] = res43 + res43len * 4;
	res5p[i0] = res53 + res53len * 5;
	res6p[i0] = res63 + res63len * 6;
	res7p[i0] = res73 + res73len * 7;
}

void checkcircle4()
{
	int visgnum4 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;
	int64_t m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;
	int length0, length1, length2, length3, length4, length5, length6, length7, length8;
	int i0 = 4;
	for (; i0 < newsetlen; i0 += 4)
	{
		k0 = newsetnum[i0];
		res3p[i0] = res34 + res34len * 3;
		res4p[i0] = res44 + res44len * 4;
		res5p[i0] = res54 + res54len * 5;
		res6p[i0] = res64 + res64len * 6;
		res7p[i0] = res74 + res74len * 7;
		length0 = newNpicnum[k0];
		for (int i1 = 0; i1 < length0; i1++)
		{
			k1 = realNpic[Npic[k0] + i1].to;
			if (k1 <= k0)continue;
			m1 = realNpic[Npic[k0] + i1].money;
			vis4[k1] = 3;
			visgn4[visgnum4++] = k1;
			length1 = newNpicnum[k1];
			for (int i2 = 0; i2 < length1; i2++)
			{
				k2 = realNpic[Npic[k1] + i2].to;
				if (k2 <= k0)continue;
				m2 = realNpic[Npic[k1] + i2].money;
				if (m1 > 3 * m2 || 5 * m1 < m2)continue;
				if (vis4[k2] <= 1)
				{
					vis4[k2] = 2;
					visgn4[visgnum4++] = k2;
				}
				length2 = newNpicnum[k2];
				for (int i3 = 0; i3 < length2; i3++)
				{
					k3 = realNpic[Npic[k2] + i3].to;
					if (k3 <= k0 || vis4[k3])continue;
					m3 = realNpic[Npic[k2] + i3].money;
					if (m2 > 3 * m3 || 5 * m2 < m3)continue;
					vis4[k3] = 1;
					visgn4[visgnum4++] = k3;
				}
			}
		}
		vis4[k0] = 3;
		visgn4[visgnum4++] = k0;
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)
		{
			k4 = realpic[pic[k0] + i4].to;
			if (k4 <= k0)continue;
			m4 = realpic[pic[k0] + i4].money;
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				k5 = realpic[pic[k4] + i5].to;
				if (k5 <= k0)continue;
				m5 = realpic[pic[k4] + i5].money;
				if (m5 > 3 * m4 || 5 * m5 < m4)continue;
				length5 = newpicnum[k5];
				for (int i6 = 0; i6 < length5; i6++)
				{
					k6 = realpic[pic[k5] + i6].to;
					if (k6 < k0 || k6 == k4)continue;
					m6 = realpic[pic[k5] + i6].money;
					if (m6 > 3 * m5 || 5 * m6 < m5)continue;
					if (k6 == k0) {
						if (m4 > 3 * m6 || 5 * m4 < m6)continue;
						res34[res34len * 3] = k0;
						res34[res34len * 3 + 1] = k4;
						res34[res34len * 3 + 2] = k5;
						res34len++;
						continue;
					}
					length6 = newpicnum[k6];
					for (int i7 = 0; i7 < length6; i7++)
					{
						k7 = realpic[pic[k6] + i7].to;
						if (!vis4[k7] || k7 == k4 || k7 == k5)continue;
						m7 = realpic[pic[k6] + i7].money;
						if (m7 > 3 * m6 || m7 < 0.2*m6)continue;
						if (k7 == k0) {
							if (m4 > 3 * m7 || 5 * m4 < m7)continue;
							res44[res44len * 4] = k0;
							res44[res44len * 4 + 1] = k4;
							res44[res44len * 4 + 2] = k5;
							res44[res44len * 4 + 3] = k6;
							res44len++;
							continue;
						}
						length7 = newpicnum[k7];
						for (int i8 = 0; i8 < length7; i8++)
						{
							k8 = realpic[pic[k7] + i8].to;
							if (vis4[k8] <= 1 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							m8 = realpic[pic[k7] + i8].money;
							if (m8 > 3 * m7 || 5 * m8 < m7)continue;
							if (k8 == k0) {
								if (m4 > 3 * m8 || 5 * m4 < m8)continue;
								res54[res54len * 5] = k0;
								res54[res54len * 5 + 1] = k4;
								res54[res54len * 5 + 2] = k5;
								res54[res54len * 5 + 3] = k6;
								res54[res54len * 5 + 4] = k7;
								res54len++;
								continue;
							}
							length8 = newpicnum[k8];
							for (int i9 = 0; i9 < length8; i9++)
							{
								k9 = realpic[pic[k8] + i9].to;
								if (vis4[k9] <= 2 || k9 == k4 || k9 == k5 || k9 == k6 || k9 == k7)continue;
								m9 = realpic[pic[k8] + i9].money;
								if (m9 > 3 * m8 || 5 * m9 < m8)continue;
								if (k9 == k0) {
									if (m4 > 3 * m9 || 5 * m4 < m9)continue;
									res64[res64len * 6] = k0;
									res64[res64len * 6 + 1] = k4;
									res64[res64len * 6 + 2] = k5;
									res64[res64len * 6 + 3] = k6;
									res64[res64len * 6 + 4] = k7;
									res64[res64len * 6 + 5] = k8;
									res64len++;
									continue;
								}
								m10 = find(&realpic[pic[k9]], &realpic[pic[k9] + newpicnum[k9]], k0)->money;
								if (m10 > 3 * m9 || m4 > 3 * m10 || 5 * m10 < m9 || 5 * m4 < m10)continue;
								res74[res74len * 7] = k0;
								res74[res74len * 7 + 1] = k4;
								res74[res74len * 7 + 2] = k5;
								res74[res74len * 7 + 3] = k6;
								res74[res74len * 7 + 4] = k7;
								res74[res74len * 7 + 5] = k8;
								res74[res74len * 7 + 6] = k9;
								res74len++;
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < visgnum4; i++)
		{
			vis4[visgn4[i]] = 0;
		}
		visgnum4 = 0;
	}
	res3p[i0] = res34 + res34len * 3;
	res4p[i0] = res44 + res44len * 4;
	res5p[i0] = res54 + res54len * 5;
	res6p[i0] = res64 + res64len * 6;
	res7p[i0] = res74 + res74len * 7;
}

void checkcircle()
{
	thread ck1(&checkcircle1);
	thread ck2(&checkcircle2);
	thread ck3(&checkcircle3);
	thread ck4(&checkcircle4);
	ck4.join();
	ck3.join();
	ck2.join();
	ck1.join();
}

int to_str0(int intput, char* output)
{
	if (intput >= 10000000)
	{
		output[8] = '\n';
		output[7] = '0' + intput % 10;
		intput /= 10;
		output[6] = '0' + intput % 10;
		intput /= 10;
		output[5] = '0' + intput % 10;
		intput /= 10;
		output[4] = '0' + intput % 10;
		intput /= 10;
		output[3] = '0' + intput % 10;
		intput /= 10;
		output[2] = '0' + intput % 10;
		intput /= 10;
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 9;
	}
	else if (intput >= 1000000)
	{
		output[7] = '\n';
		output[6] = '0' + intput % 10;
		intput /= 10;
		output[5] = '0' + intput % 10;
		intput /= 10;
		output[4] = '0' + intput % 10;
		intput /= 10;
		output[3] = '0' + intput % 10;
		intput /= 10;
		output[2] = '0' + intput % 10;
		intput /= 10;
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 8;
	}
	else if (intput >= 100000)
	{
		output[6] = '\n';
		output[5] = '0' + intput % 10;
		intput /= 10;
		output[4] = '0' + intput % 10;
		intput /= 10;
		output[3] = '0' + intput % 10;
		intput /= 10;
		output[2] = '0' + intput % 10;
		intput /= 10;
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 7;
	}
	else if (intput >= 10000)
	{
		output[5] = '\n';
		output[4] = '0' + intput % 10;
		intput /= 10;
		output[3] = '0' + intput % 10;
		intput /= 10;
		output[2] = '0' + intput % 10;
		intput /= 10;
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 6;
	}
	else if (intput >= 1000)
	{
		output[4] = '\n';
		output[3] = '0' + intput % 10;
		intput /= 10;
		output[2] = '0' + intput % 10;
		intput /= 10;
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 5;
	}
	else if (intput >= 100)
	{
		output[3] = '\n';
		output[2] = '0' + intput % 10;
		intput /= 10;
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 4;
	}
	else if (intput >= 10)
	{
		output[2] = '\n';
		output[1] = '0' + intput % 10;
		intput /= 10;
		output[0] = '0' + intput % 10;
		return 3;
	}
	else
	{
		output[1] = '\n';
		output[0] = '0' + intput % 10;
		return 2;
	}
}

int to_str(int* intput, int size, int batch, int count, char* output)
{
	int b;
	for (int j = 0; j < size; j++)
	{
		for (int i = 0; i < batch - 1; i++)
		{
			b = setnum[intput[j * batch + i]];
			if (b < 1000000)
			{
				if (b >= 100000)
				{
					output[count + 5] = '0' + b % 10;
					b /= 10;
					output[count + 4] = '0' + b % 10;
					b /= 10;
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 6;
				}
				else if (b >= 10000)
				{
					output[count + 4] = '0' + b % 10;
					b /= 10;
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 5;
				}
				else if (b >= 1000)
				{
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 4;
				}
				else if (b >= 100)
				{
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count + 0] = '0' + b % 10;
					count += 3;
				}
				else if (b >= 10)
				{
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 2;
				}
				else
				{
					output[count] = '0' + b % 10;
					count += 1;
				}
				output[count++] = ',';
			}
			else {
				if (b < 10000000)
				{
					output[count + 6] = '0' + b % 10;
					b /= 10;
					output[count + 5] = '0' + b % 10;
					b /= 10;
					output[count + 4] = '0' + b % 10;
					b /= 10;
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 7;
				}
				else if (b < 100000000)
				{
					output[count + 7] = '0' + b % 10;
					b /= 10;
					output[count + 6] = '0' + b % 10;
					b /= 10;
					output[count + 5] = '0' + b % 10;
					b /= 10;
					output[count + 4] = '0' + b % 10;
					b /= 10;
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 8;
				}
				else if (b < 1000000000)
				{
					output[count + 8] = '0' + b % 10;
					b /= 10;
					output[count + 7] = '0' + b % 10;
					b /= 10;
					output[count + 6] = '0' + b % 10;
					b /= 10;
					output[count + 5] = '0' + b % 10;
					b /= 10;
					output[count + 4] = '0' + b % 10;
					b /= 10;
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 9;
				}
				else
				{
					output[count + 9] = '0' + b % 10;
					b /= 10;
					output[count + 8] = '0' + b % 10;
					b /= 10;
					output[count + 7] = '0' + b % 10;
					b /= 10;
					output[count + 6] = '0' + b % 10;
					b /= 10;
					output[count + 5] = '0' + b % 10;
					b /= 10;
					output[count + 4] = '0' + b % 10;
					b /= 10;
					output[count + 3] = '0' + b % 10;
					b /= 10;
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count] = '0' + b % 10;
					count += 10;
				}
				output[count++] = ',';
			}
		}
		b = setnum[intput[j * batch + batch-1]];
		if (b < 1000000)
		{
			if (b >= 100000)
			{
				output[count + 5] = '0' + b % 10;
				b /= 10;
				output[count + 4] = '0' + b % 10;
				b /= 10;
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 6;
			}
			else if (b >= 10000)
			{
				output[count + 4] = '0' + b % 10;
				b /= 10;
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 5;
			}
			else if (b >= 1000)
			{
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 4;
			}
			else if (b >= 100)
			{
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count + 0] = '0' + b % 10;
				count += 3;
			}
			else if (b >= 10)
			{
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 2;
			}
			else
			{
				output[count] = '0' + b % 10;
				count += 1;
			}
			output[count++] = '\n';
		}
		else {
			if (b < 10000000)
			{
				output[count + 6] = '0' + b % 10;
				b /= 10;
				output[count + 5] = '0' + b % 10;
				b /= 10;
				output[count + 4] = '0' + b % 10;
				b /= 10;
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 7;
			}
			else if (b < 100000000)
			{
				output[count + 7] = '0' + b % 10;
				b /= 10;
				output[count + 6] = '0' + b % 10;
				b /= 10;
				output[count + 5] = '0' + b % 10;
				b /= 10;
				output[count + 4] = '0' + b % 10;
				b /= 10;
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 8;
			}
			else if (b < 1000000000)
			{
				output[count + 8] = '0' + b % 10;
				b /= 10;
				output[count + 7] = '0' + b % 10;
				b /= 10;
				output[count + 6] = '0' + b % 10;
				b /= 10;
				output[count + 5] = '0' + b % 10;
				b /= 10;
				output[count + 4] = '0' + b % 10;
				b /= 10;
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 9;
			}
			else
			{
				output[count + 9] = '0' + b % 10;
				b /= 10;
				output[count + 8] = '0' + b % 10;
				b /= 10;
				output[count + 7] = '0' + b % 10;
				b /= 10;
				output[count + 6] = '0' + b % 10;
				b /= 10;
				output[count + 5] = '0' + b % 10;
				b /= 10;
				output[count + 4] = '0' + b % 10;
				b /= 10;
				output[count + 3] = '0' + b % 10;
				b /= 10;
				output[count + 2] = '0' + b % 10;
				b /= 10;
				output[count + 1] = '0' + b % 10;
				b /= 10;
				output[count] = '0' + b % 10;
				count += 10;
			}
			output[count++] = '\n';
		}
	}
	return count;
}
void to_str1()
{
	anstr[0] = new char[res3len * 33 + res4len * 44 + res5len * 55 + res6len * 66];
	int countnum = res3len + res4len + res5len + res6len + res7len;
	pos[0] = to_str0(countnum, anstr[0]);
	for (int i = 1; i < newsetlen; i++)
	{
		pos[0] = to_str(res3p[i], (res3p[i + 4] - res3p[i]) / 3, 3, pos[0], anstr[0]);
	}
	for (int i = 1; i < newsetlen; i++)
	{
		pos[0] = to_str(res4p[i], (res4p[i + 4] - res4p[i]) >> 2, 4, pos[0], anstr[0]);
	}
	for (int i = 1; i < newsetlen; i++)
	{
		pos[0] = to_str(res5p[i], (res5p[i + 4] - res5p[i]) / 5, 5, pos[0], anstr[0]);
	}
	for (int i = 1; i < newsetlen; i++)
	{
		pos[0] = to_str(res6p[i], (res6p[i + 4] - res6p[i]) / 6, 6, pos[0], anstr[0]);
	}
}

void to_str2(int id)
{
	anstr[id] = new char[res7len * 70];
	int temp = gap[id];
	for (int i = gap[id - 1]; i < temp; i++)
	{
		pos[id] = to_str(res7p[i], (res7p[i + 4] - res7p[i]) / 7, 7, pos[id], anstr[id]);
	}
}


void savetostr()
{
	char savepath[30] = "/projects/student/result.txt";
	res3len = res31len + res32len + res33len + res34len;
	res4len = res41len + res42len + res43len + res44len;
	res5len = res51len + res52len + res53len + res54len;
	res6len = res61len + res62len + res63len + res64len;
	res7len = res71len + res72len + res73len + res74len;
	int mu = res7len * 7 / (res3len * 3 + res4len * 4 + res5len * 5 + res6len * 6);
	int mul = mu >= 2 ? mu - 1 : 1;
	int total = 0, i = 1, temp;
	gap[0] = 1;
	for (int j = 1; j < mul; j++)
	{
		temp = res7len / mul * j;
		for (; i < newsetlen; i++)
		{
			total += (res7p[i + 4] - res7p[i]) / 7;
			if (total > temp) {
				gap[j] = i++;
				break;
			}
		}
	}
	gap[mul] = newsetlen;
	thread t[100];
	t[0] = thread(to_str1);
	for (int i = 1; i <= mul; i++)
	{
		t[i] = thread(to_str2, i);
	}
	t[0].join();
	FILE* fp = fopen(savepath, "wb");
	fwrite(anstr[0], sizeof(char), pos[0], fp);
	for (int i = 1; i <= mul; i++)
	{
		t[i].join();
		fwrite(anstr[i], sizeof(char), pos[i], fp);
	}
	fclose(fp);
}

int main()
{
	char loadpath[30] = "/data/1004812/test_data.txt";

	loaddata(loadpath);
	measure([]() {
		makestar();
	});

	jianzhi();
	toeasy();
	measure([]() {
		checkcircle();
	});
	measure([]() {
		savetostr();
	});

	exit(0);
}
