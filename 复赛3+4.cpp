//4+3
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <atomic>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
using namespace std;

#define MAXROW 2000100
#define MAXROW3 6000100
#define MAXOUTSIZE 20000000
#define HALFOUTSIZE 10000000
#define PATHSIZE 128

struct Path
{
	int point1;
	int point2;
	double money1;
	double money3;
};

struct edge
{
	int to;
	int money;
	inline bool operator == (const int &pos) {
		return (this->to == pos);
	}
};

inline bool pathsort(const Path a, const Path b)
{
	if (a.point2 != b.point2)return a.point2 < b.point2;
	return a.point1 < b.point1;
}

inline bool mycom(const edge a, const edge b)
{
	return a.to < b.to;
}

int pathsgn1[MAXROW];
int pathnum1[MAXROW] = { 0 };
int pathsgn2[MAXROW];
int pathnum2[MAXROW] = { 0 };
int pathsgn3[MAXROW];
int pathnum3[MAXROW] = { 0 };
int pathsgn4[MAXROW];
int pathnum4[MAXROW] = { 0 };

int total[MAXROW3];   //读取的所有数据，不包含第三列  /600W
int totallen;   //据总长
int newsetnum[MAXROW];
int newsetlen;
int stack[MAXROW];
int setnum[MAXROW];
char* anstr[50];
int gap[50];
int pos[50];
int setlen = 1;
unordered_map<int, int> edindex;
edge* pic[MAXROW];   //2E
edge* Npic[MAXROW];   //2E
edge realpic[MAXROW];
edge realNpic[MAXROW];
edge* pictmp;
edge* Npictmp;
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
int res71[7 * HALFOUTSIZE];
int res72[7 * HALFOUTSIZE];
int res73[7 * HALFOUTSIZE];
int res74[7 * HALFOUTSIZE];
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
			pic[temp1][picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			Npic[temp2][Npicnum[temp2]++] = egtmp;
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
			pic[temp1][picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			Npic[temp2][Npicnum[temp2]++] = egtmp;
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
			pic[temp1][picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			Npic[temp2][Npicnum[temp2]++] = egtmp;
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
			pic[temp1][picnum[temp1]++] = egtmp;
			egtmp.to = temp1;
			Npic[temp2][Npicnum[temp2]++] = egtmp;
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
	pic[1] = realpic;
	Npic[1] = realNpic;
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
	for (int i = 0; i < setlen; i++)
	{
		if (newNpicnum[i] == 0)stack[top++] = i;
	}
	top--;
	while (top > -1)
	{
		x = stack[top--];
		for (int i = 0; i < picnum[x]; i++)
		{
			if (pic[x][i].to >= setlen)continue;
			if (--newNpicnum[pic[x][i].to] == 0)stack[++top] = pic[x][i].to;
			find(Npic[pic[x][i].to], Npic[pic[x][i].to] + Npicnum[pic[x][i].to], x)->to = setlen;
		}
		newpicnum[x] = 0;
		newNpicnum[x] = 0;
		picnum[x] = 0;
		Npicnum[x] = 0;
	}
	top = 0;
	for (int i = 0; i < setlen; i++)
	{
		if (newpicnum[i] == 0)stack[top++] = i;
	}
	top--;
	while (top > -1)
	{
		int x = stack[top--];
		for (int i = 0; i < Npicnum[x]; i++)
		{
			if (Npic[x][i].to >= setlen)continue;
			if (--newpicnum[Npic[x][i].to] == 0)stack[++top] = Npic[x][i].to;
			find(pic[Npic[x][i].to], pic[Npic[x][i].to] + picnum[Npic[x][i].to], x)->to = setlen;
		}
		newpicnum[x] = 0;
		newNpicnum[x] = 0;
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
		sort(pic[k], pic[k] + picnum[k], mycom);
		sort(Npic[k], Npic[k] + Npicnum[k], mycom);
	}
}

void sort2()
{
	int k;
	for (int i = newsetlen / 4; i < newsetlen / 2; i++)
	{
		k = newsetnum[i];
		sort(pic[k], pic[k] + picnum[k], mycom);
		sort(Npic[k], Npic[k] + Npicnum[k], mycom);
	}
}

void sort3()
{
	int k;
	for (int i = newsetlen / 2; i < newsetlen / 4 * 3; i++)
	{
		k = newsetnum[i];
		sort(pic[k], pic[k] + picnum[k], mycom);
		sort(Npic[k], Npic[k] + Npicnum[k], mycom);
	}
}

void sort4()
{
	int k;
	for (int i = newsetlen / 4 * 3; i < newsetlen; i++)
	{
		k = newsetnum[i];
		sort(pic[k], pic[k] + picnum[k], mycom);
		sort(Npic[k], Npic[k] + Npicnum[k], mycom);
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
	Path* path1 = new Path[setlen*PATHSIZE];            //4400W*4
	Path ptmp1;
	int pathsgnnum1 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, flag1, flag2;
	int p0, p4, p5, p6, p7, ps;
	int64_t m1, m2, m3, m4, m5, m6, m7, pm1, pm3;
	int length1, length2, length3, length4, length5, length6;
	int i0 = 1;
	for (; i0 < newsetlen; i0 += 4)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = newsetnum[i0];
		res3p[i0] = res31 + res31len * 3;
		res4p[i0] = res41 + res41len * 4;
		res5p[i0] = res51 + res51len * 5;
		res6p[i0] = res61 + res61len * 6;
		res7p[i0] = res71 + res71len * 7;
		length1 = newNpicnum[k0];
		for (int i1 = 0; i1 < length1; i1++)   //newNpicnum[picdex[i0]]为i0节点的入度
		{
			k1 = Npic[k0][i1].to;                           //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			m1 = Npic[k0][i1].money;
			length2 = newNpicnum[k1];
			for (int i2 = 0; i2 < length2; i2++)
			{
				k2 = Npic[k1][i2].to;                           //k2为i2对应i1的出节点实值
				if (k2 <= k0)continue;
				m2 = Npic[k1][i2].money;
				if (m1 > 3 * m2 || m2 > 5 * m1)continue;
				length3 = newNpicnum[k2];
				for (int i3 = 0; i3 < length3; i3++)
				{
					k3 = Npic[k2][i3].to;                           //k2为i2对应i1的出节点实值
					if (k3 < k0 || k3 == k1)continue;
					m3 = Npic[k2][i3].money;
					if (m2 > 3 * m3 || m3 > 5 * m2)continue;                       //k2为i2对应i1的出节点实值
					ps = pathnum1[k3];
					if (!ps)pathsgn1[pathsgnnum1++] = k3;
					path1[k3*PATHSIZE + ps].point1 = k1;
					path1[k3*PATHSIZE + ps].point2 = k2;
					path1[k3*PATHSIZE + ps].money1 = m1;
					path1[k3*PATHSIZE + pathnum1[k3]++].money3 = m3;
				}
			}
		}
		for (int i = 0; i < pathsgnnum1; i++)
		{
			Path* temp = path1 + pathsgn1[i] * PATHSIZE;
			sort(temp, temp + pathnum1[pathsgn1[i]], pathsort);
		}
		p0 = pathnum1[k0];
		for (int j0 = 0; j0 < p0; j0++)
		{
			ptmp1 = path1[k0*PATHSIZE + j0];
			pm1 = ptmp1.money1;
			pm3 = ptmp1.money3;
			if (pm3 > 3 * pm1 || pm1 > 5 * pm3)continue;
			res31[res31len * 3] = k0;
			res31[res31len * 3 + 1] = ptmp1.point2;
			res31[res31len * 3 + 2] = ptmp1.point1;
			res31len++;
		}

		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)      //newpicnum为i0对应节点的出度
		{
			k4 = pic[k0][i4].to;
			if (k4 <= k0)continue;
			m4 = pic[k0][i4].money;
			p4 = pathnum1[k4];
			for (int j1 = 0; j1 < p4; j1++)
			{
				ptmp1 = path1[k4*PATHSIZE + j1];
				pm1 = ptmp1.money1;
				pm3 = ptmp1.money3;
				if (m4 > 3 * pm1 || pm3 > 3 * m4 || \
					pm1 > 5 * m4 || m4 > 5 * pm3)continue;
				res41[res41len * 4] = k0;
				res41[res41len * 4 + 1] = k4;
				res41[res41len * 4 + 2] = ptmp1.point2;
				res41[res41len * 4 + 3] = ptmp1.point1;
				res41len++;
			}
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				m5 = pic[k4][i5].money;
				k5 = pic[k4][i5].to;
				if (k5 <= k0 || m5 > 3 * m4 || m4 > 5 * m5)continue;
				p5 = pathnum1[k5];
				for (int j1 = 0; j1 < p5; j1++)
				{
					ptmp1 = path1[k5*PATHSIZE + j1];
					pm1 = ptmp1.money1;
					pm3 = ptmp1.money3;
					if (m4 > 3 * pm1 || pm3 > 3 * m5 || \
						pm1 > 5 * m4 || m5 > 5 * pm3)continue;
					flag1 = ptmp1.point1;
					flag2 = ptmp1.point2;
					if (k4 == flag1 || k4 == flag2)continue;
					res51[res51len * 5] = k0;
					res51[res51len * 5 + 1] = k4;
					res51[res51len * 5 + 2] = k5;
					res51[res51len * 5 + 3] = flag2;
					res51[res51len * 5 + 4] = flag1;
					res51len++;
				}
				length5 = newpicnum[k5];

				for (int i6 = 0; i6 < length5; i6++)
				{
					m6 = pic[k5][i6].money;

					k6 = pic[k5][i6].to;
					if (k6 <= k0 || m6 > 3 * m5 || m5 > 5 * m6 || k6 == k4)continue;
					p6 = pathnum1[k6];
					for (int j1 = 0; j1 < p6; j1++)
					{
						ptmp1 = path1[k6*PATHSIZE + j1];
						pm1 = ptmp1.money1;
						pm3 = ptmp1.money3;
						if (m4 > 3 * pm1 || pm3 > 3 * m6 || \
							pm1 > 5 * m4 || m6 > 5 * pm3)continue;
						flag1 = ptmp1.point1;
						flag2 = ptmp1.point2;
						if (k4 == flag1 || k4 == flag2 || \
							k5 == flag1 || k5 == flag2)continue;
						res61[res61len * 6] = k0;
						res61[res61len * 6 + 1] = k4;
						res61[res61len * 6 + 2] = k5;
						res61[res61len * 6 + 3] = k6;
						res61[res61len * 6 + 4] = flag2;
						res61[res61len * 6 + 5] = flag1;
						res61len++;
					}
					length6 = newpicnum[k6];

					for (int i7 = 0; i7 < length6; i7++)
					{
						m7 = pic[k6][i7].money;
						k7 = pic[k6][i7].to;
						if (k7 <= k0 || m7 > 3 * m6 || m6 > 5 * m7 || k7 == k4 || k7 == k5)continue;
						p7 = pathnum1[k7];
						for (int j1 = 0; j1 < p7; j1++)
						{
							ptmp1 = path1[k7*PATHSIZE + j1];
							pm1 = ptmp1.money1;
							pm3 = ptmp1.money3;
							if (m4 > 3 * pm1 || pm3 > 3 * m7 || \
								pm1 > 5 * m4 || m7 > 5 * pm3)continue;
							flag1 = ptmp1.point1;
							flag2 = ptmp1.point2;
							if (k4 == flag1 || k4 == flag2 || \
								k5 == flag1 || k5 == flag2 || \
								k6 == flag1 || k6 == flag2)continue;
							res71[res71len * 7] = k0;
							res71[res71len * 7 + 1] = k4;
							res71[res71len * 7 + 2] = k5;
							res71[res71len * 7 + 3] = k6;
							res71[res71len * 7 + 4] = k7;
							res71[res71len * 7 + 5] = flag2;
							res71[res71len * 7 + 6] = flag1;
							res71len++;
						}
					}
				}
			}
		}
		for (int i = 0; i < pathsgnnum1; i++)
		{
			pathnum1[pathsgn1[i]] = 0;
		}
		pathsgnnum1 = 0;
	}
	res3p[i0] = res31 + res31len * 3;
	res4p[i0] = res41 + res41len * 4;
	res5p[i0] = res51 + res51len * 5;
	res6p[i0] = res61 + res61len * 6;
	res7p[i0] = res71 + res71len * 7;
}

void checkcircle2()
{
	Path* path2 = new Path[setlen*PATHSIZE];            //4400W*4
	Path ptmp1;
	int pathsgnnum2 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, flag1, flag2;
	int p0, p4, p5, p6, p7, ps;
	int64_t m1, m2, m3, m4, m5, m6, m7, pm1, pm3;
	int length1, length2, length3, length4, length5, length6;
	int i0 = 2;
	for (; i0 < newsetlen; i0 += 4)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = newsetnum[i0];
		res3p[i0] = res32 + res32len * 3;
		res4p[i0] = res42 + res42len * 4;
		res5p[i0] = res52 + res52len * 5;
		res6p[i0] = res62 + res62len * 6;
		res7p[i0] = res72 + res72len * 7;
		length1 = newNpicnum[k0];
		for (int i1 = 0; i1 < length1; i1++)   //newNpicnum[picdex[k0]]为k0节点的入度
		{
			k1 = Npic[k0][i1].to;                           //k1为i1对应k0的出节点实值
			if (k1 <= k0)continue;
			m1 = Npic[k0][i1].money;
			length2 = newNpicnum[k1];
			for (int i2 = 0; i2 < length2; i2++)
			{
				k2 = Npic[k1][i2].to;                           //k2为i2对应i1的出节点实值
				if (k2 <= k0)continue;
				m2 = Npic[k1][i2].money;
				if (m1 > 3 * m2 || m2 > 5 * m1)continue;
				length3 = newNpicnum[k2];
				for (int i3 = 0; i3 < length3; i3++)
				{
					k3 = Npic[k2][i3].to;                           //k2为i2对应i1的出节点实值
					if (k3 < k0 || k3 == k1)continue;
					m3 = Npic[k2][i3].money;
					if (m2 > 3 * m3 || m3 > 5 * m2)continue;                       //k2为i2对应i1的出节点实值
					ps = pathnum2[k3];
					if (!ps)pathsgn2[pathsgnnum2++] = k3;
					path2[k3*PATHSIZE + ps].point1 = k1;
					path2[k3*PATHSIZE + ps].point2 = k2;
					path2[k3*PATHSIZE + ps].money1 = m1;
					path2[k3*PATHSIZE + pathnum2[k3]++].money3 = m3;
				}
			}
		}
		for (int i = 0; i < pathsgnnum2; i++)
		{
			Path* temp = path2 + pathsgn2[i] * PATHSIZE;
			sort(temp, temp + pathnum2[pathsgn2[i]], pathsort);
		}
		p0 = pathnum2[k0];
		for (int j0 = 0; j0 < p0; j0++)
		{
			ptmp1 = path2[k0*PATHSIZE + j0];
			pm1 = ptmp1.money1;
			pm3 = ptmp1.money3;
			if (pm3 > 3 * pm1 || pm1 > 5 * pm3)continue;
			res32[res32len * 3] = k0;
			res32[res32len * 3 + 1] = ptmp1.point2;
			res32[res32len * 3 + 2] = ptmp1.point1;
			res32len++;
		}
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)      //newpicnum为k0对应节点的出度
		{
			k4 = pic[k0][i4].to;
			if (k4 <= k0)continue;
			m4 = pic[k0][i4].money;
			p4 = pathnum2[k4];
			for (int j1 = 0; j1 < p4; j1++)
			{
				ptmp1 = path2[k4*PATHSIZE + j1];
				pm1 = ptmp1.money1;
				pm3 = ptmp1.money3;
				if (m4 > 3 * pm1 || pm3 > 3 * m4 || \
					pm1 > 5 * m4 || m4 > 5 * pm3)continue;
				res42[res42len * 4] = k0;
				res42[res42len * 4 + 1] = k4;
				res42[res42len * 4 + 2] = ptmp1.point2;
				res42[res42len * 4 + 3] = ptmp1.point1;
				res42len++;
			}
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				m5 = pic[k4][i5].money;
				k5 = pic[k4][i5].to;
				if (k5 <= k0 || m5 > 3 * m4 || m4 > 5 * m5)continue;
				p5 = pathnum2[k5];
				for (int j1 = 0; j1 < p5; j1++)
				{
					ptmp1 = path2[k5*PATHSIZE + j1];
					pm1 = ptmp1.money1;
					pm3 = ptmp1.money3;
					if (m4 > 3 * pm1 || pm3 > 3 * m5 || \
						pm1 > 5 * m4 || m5 > 5 * pm3)continue;
					flag1 = ptmp1.point1;
					flag2 = ptmp1.point2;
					if (k4 == flag1 || k4 == flag2)continue;
					res52[res52len * 5] = k0;
					res52[res52len * 5 + 1] = k4;
					res52[res52len * 5 + 2] = k5;
					res52[res52len * 5 + 3] = flag2;
					res52[res52len * 5 + 4] = flag1;
					res52len++;
				}
				length5 = newpicnum[k5];

				for (int i6 = 0; i6 < length5; i6++)
				{
					m6 = pic[k5][i6].money;

					k6 = pic[k5][i6].to;
					if (k6 <= k0 || m6 > 3 * m5 || m5 > 5 * m6 || k6 == k4)continue;
					p6 = pathnum2[k6];
					for (int j1 = 0; j1 < p6; j1++)
					{
						ptmp1 = path2[k6*PATHSIZE + j1];
						pm1 = ptmp1.money1;
						pm3 = ptmp1.money3;
						if (m4 > 3 * pm1 || pm3 > 3 * m6 || \
							pm1 > 5 * m4 || m6 > 5 * pm3)continue;
						flag1 = ptmp1.point1;
						flag2 = ptmp1.point2;
						if (k4 == flag1 || k4 == flag2 || \
							k5 == flag1 || k5 == flag2)continue;
						res62[res62len * 6] = k0;
						res62[res62len * 6 + 1] = k4;
						res62[res62len * 6 + 2] = k5;
						res62[res62len * 6 + 3] = k6;
						res62[res62len * 6 + 4] = flag2;
						res62[res62len * 6 + 5] = flag1;
						res62len++;
					}
					length6 = newpicnum[k6];

					for (int i7 = 0; i7 < length6; i7++)
					{
						m7 = pic[k6][i7].money;
						k7 = pic[k6][i7].to;
						if (k7 <= k0 || m7 > 3 * m6 || m6 > 5 * m7 || k7 == k4 || k7 == k5)continue;
						p7 = pathnum2[k7];
						for (int j1 = 0; j1 < p7; j1++)
						{
							ptmp1 = path2[k7*PATHSIZE + j1];
							pm1 = ptmp1.money1;
							pm3 = ptmp1.money3;
							if (m4 > 3 * pm1 || pm3 > 3 * m7 || \
								pm1 > 5 * m4 || m7 > 5 * pm3)continue;
							flag1 = ptmp1.point1;
							flag2 = ptmp1.point2;
							if (k4 == flag1 || k4 == flag2 || \
								k5 == flag1 || k5 == flag2 || \
								k6 == flag1 || k6 == flag2)continue;
							res72[res72len * 7] = k0;
							res72[res72len * 7 + 1] = k4;
							res72[res72len * 7 + 2] = k5;
							res72[res72len * 7 + 3] = k6;
							res72[res72len * 7 + 4] = k7;
							res72[res72len * 7 + 5] = flag2;
							res72[res72len * 7 + 6] = flag1;
							res72len++;
						}
					}
				}
			}
		}
		for (int i = 0; i < pathsgnnum2; i++)
		{
			pathnum2[pathsgn2[i]] = 0;
		}
		pathsgnnum2 = 0;
	}
	res3p[i0] = res32 + res32len * 3;
	res4p[i0] = res42 + res42len * 4;
	res5p[i0] = res52 + res52len * 5;
	res6p[i0] = res62 + res62len * 6;
	res7p[i0] = res72 + res72len * 7;
}

void checkcircle3()
{
	Path* path3 = new Path[setlen*PATHSIZE];            //4400W*4
	Path ptmp1;
	int pathsgnnum3 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, flag1, flag2;
	int p0, p4, p5, p6, p7, ps;
	int64_t m1, m2, m3, m4, m5, m6, m7, pm1, pm3;
	int length1, length2, length3, length4, length5, length6;
	int i0 = 3;
	for (; i0 < newsetlen; i0 += 4)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = newsetnum[i0];
		res3p[i0] = res33 + res33len * 3;
		res4p[i0] = res43 + res43len * 4;
		res5p[i0] = res53 + res53len * 5;
		res6p[i0] = res63 + res63len * 6;
		res7p[i0] = res73 + res73len * 7;
		length1 = newNpicnum[k0];
		for (int i1 = 0; i1 < length1; i1++)   //newNpicnum[picdex[k0]]为i0节点的入度
		{
			k1 = Npic[k0][i1].to;                           //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			m1 = Npic[k0][i1].money;
			length2 = newNpicnum[k1];
			for (int i2 = 0; i2 < length2; i2++)
			{
				k2 = Npic[k1][i2].to;                           //k2为i2对应i1的出节点实值
				if (k2 <= k0)continue;
				m2 = Npic[k1][i2].money;
				if (m1 > 3 * m2 || m2 > 5 * m1)continue;
				length3 = newNpicnum[k2];
				for (int i3 = 0; i3 < length3; i3++)
				{
					k3 = Npic[k2][i3].to;                           //k2为i2对应i1的出节点实值
					if (k3 < k0 || k3 == k1)continue;
					m3 = Npic[k2][i3].money;
					if (m2 > 3 * m3 || m3 > 5 * m2)continue;                       //k2为i2对应i1的出节点实值
					ps = pathnum3[k3];
					if (!ps)pathsgn3[pathsgnnum3++] = k3;
					path3[k3*PATHSIZE + ps].point1 = k1;
					path3[k3*PATHSIZE + ps].point2 = k2;
					path3[k3*PATHSIZE + ps].money1 = m1;
					path3[k3*PATHSIZE + pathnum3[k3]++].money3 = m3;
				}
			}
		}
		for (int i = 0; i < pathsgnnum3; i++)
		{
			Path* temp = path3 + pathsgn3[i] * PATHSIZE;
			sort(temp, temp + pathnum3[pathsgn3[i]], pathsort);
		}
		p0 = pathnum3[k0];
		for (int j0 = 0; j0 < p0; j0++)
		{
			ptmp1 = path3[k0*PATHSIZE + j0];
			pm1 = ptmp1.money1;
			pm3 = ptmp1.money3;
			if (pm3 > 3 * pm1 || pm1 > 5 * pm3)continue;
			res33[res33len * 3] = k0;
			res33[res33len * 3 + 1] = ptmp1.point2;
			res33[res33len * 3 + 2] = ptmp1.point1;
			res33len++;
		}
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)      //newpicnum为k0对应节点的出度
		{
			k4 = pic[k0][i4].to;
			if (k4 <= k0)continue;
			m4 = pic[k0][i4].money;
			p4 = pathnum3[k4];
			for (int j1 = 0; j1 < p4; j1++)
			{
				ptmp1 = path3[k4*PATHSIZE + j1];
				pm1 = ptmp1.money1;
				pm3 = ptmp1.money3;
				if (m4 > 3 * pm1 || pm3 > 3 * m4 || \
					pm1 > 5 * m4 || m4 > 5 * pm3)continue;
				res43[res43len * 4] = k0;
				res43[res43len * 4 + 1] = k4;
				res43[res43len * 4 + 2] = ptmp1.point2;
				res43[res43len * 4 + 3] = ptmp1.point1;
				res43len++;
			}
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				m5 = pic[k4][i5].money;
				k5 = pic[k4][i5].to;
				if (k5 <= k0 || m5 > 3 * m4 || m4 > 5 * m5)continue;
				p5 = pathnum3[k5];
				for (int j1 = 0; j1 < p5; j1++)
				{
					ptmp1 = path3[k5*PATHSIZE + j1];
					pm1 = ptmp1.money1;
					pm3 = ptmp1.money3;
					if (m4 > 3 * pm1 || pm3 > 3 * m5 || \
						pm1 > 5 * m4 || m5 > 5 * pm3)continue;
					flag1 = ptmp1.point1;
					flag2 = ptmp1.point2;
					if (k4 == flag1 || k4 == flag2)continue;
					res53[res53len * 5] = k0;
					res53[res53len * 5 + 1] = k4;
					res53[res53len * 5 + 2] = k5;
					res53[res53len * 5 + 3] = flag2;
					res53[res53len * 5 + 4] = flag1;
					res53len++;
				}
				length5 = newpicnum[k5];



				for (int i6 = 0; i6 < length5; i6++)
				{
					m6 = pic[k5][i6].money;
					k6 = pic[k5][i6].to;
					if (k6 <= k0 || m6 > 3 * m5 || m5 > 5 * m6 || k6 == k4)continue;
					p6 = pathnum3[k6];
					for (int j1 = 0; j1 < p6; j1++)
					{
						ptmp1 = path3[k6*PATHSIZE + j1];
						pm1 = ptmp1.money1;
						pm3 = ptmp1.money3;
						if (m4 > 3 * pm1 || pm3 > 3 * m6 || \
							pm1 > 5 * m4 || m6 > 5 * pm3)continue;
						flag1 = ptmp1.point1;
						flag2 = ptmp1.point2;
						if (k4 == flag1 || k4 == flag2 || \
							k5 == flag1 || k5 == flag2)continue;
						res63[res63len * 6] = k0;
						res63[res63len * 6 + 1] = k4;
						res63[res63len * 6 + 2] = k5;
						res63[res63len * 6 + 3] = k6;
						res63[res63len * 6 + 4] = flag2;
						res63[res63len * 6 + 5] = flag1;
						res63len++;
					}
					length6 = newpicnum[k6];

					for (int i7 = 0; i7 < length6; i7++)
					{
						m7 = pic[k6][i7].money;
						k7 = pic[k6][i7].to;
						if (k7 <= k0 || m7 > 3 * m6 || m6 > 5 * m7 || k7 == k4 || k7 == k5)continue;
						p7 = pathnum3[k7];
						for (int j1 = 0; j1 < p7; j1++)
						{
							ptmp1 = path3[k7*PATHSIZE + j1];
							pm1 = ptmp1.money1;
							pm3 = ptmp1.money3;
							if (m4 > 3 * pm1 || pm3 > 3 * m7 || \
								pm1 > 5 * m4 || m7 > 5 * pm3)continue;
							flag1 = ptmp1.point1;
							flag2 = ptmp1.point2;
							if (k4 == flag1 || k4 == flag2 || \
								k5 == flag1 || k5 == flag2 || \
								k6 == flag1 || k6 == flag2)continue;
							res73[res73len * 7] = k0;
							res73[res73len * 7 + 1] = k4;
							res73[res73len * 7 + 2] = k5;
							res73[res73len * 7 + 3] = k6;
							res73[res73len * 7 + 4] = k7;
							res73[res73len * 7 + 5] = flag2;
							res73[res73len * 7 + 6] = flag1;
							res73len++;
						}
					}
				}
			}
		}
		for (int i = 0; i < pathsgnnum3; i++)
		{
			pathnum3[pathsgn3[i]] = 0;
		}
		pathsgnnum3 = 0;
	}
	res3p[i0] = res33 + res33len * 3;
	res4p[i0] = res43 + res43len * 4;
	res5p[i0] = res53 + res53len * 5;
	res6p[i0] = res63 + res63len * 6;
	res7p[i0] = res73 + res73len * 7;
}

void checkcircle4()
{
	Path* path4 = new Path[setlen*PATHSIZE];            //4400W*4
	Path ptmp1;
	int pathsgnnum4 = 0;
	int k0, k1, k2, k3, k4, k5, k6, k7, flag1, flag2;
	int p0, p4, p5, p6, p7, ps;
	int64_t m1, m2, m3, m4, m5, m6, m7, pm1, pm3;
	int length1, length2, length3, length4, length5, length6;
	int i0 = 4;
	for (; i0 < newsetlen; i0 += 4)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = newsetnum[i0];
		res3p[i0] = res34 + res34len * 3;
		res4p[i0] = res44 + res44len * 4;
		res5p[i0] = res54 + res54len * 5;
		res6p[i0] = res64 + res64len * 6;
		res7p[i0] = res74 + res74len * 7;
		length1 = newNpicnum[k0];
		for (int i1 = 0; i1 < length1; i1++)   //newNpicnum[picdex[i0]]为i0节点的入度
		{
			k1 = Npic[k0][i1].to;                           //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			m1 = Npic[k0][i1].money;
			length2 = newNpicnum[k1];
			for (int i2 = 0; i2 < length2; i2++)
			{
				k2 = Npic[k1][i2].to;                           //k2为i2对应i1的出节点实值
				if (k2 <= k0)continue;
				m2 = Npic[k1][i2].money;
				if (m1 > 3 * m2 || m2 > 5 * m1)continue;
				length3 = newNpicnum[k2];
				for (int i3 = 0; i3 < length3; i3++)
				{
					k3 = Npic[k2][i3].to;                           //k2为i2对应i1的出节点实值
					if (k3 < k0 || k3 == k1)continue;
					m3 = Npic[k2][i3].money;
					if (m2 > 3 * m3 || m3 > 5 * m2)continue;                       //k2为i2对应i1的出节点实值
					ps = pathnum4[k3];
					if (!ps)pathsgn4[pathsgnnum4++] = k3;
					path4[k3*PATHSIZE + ps].point1 = k1;
					path4[k3*PATHSIZE + ps].point2 = k2;
					path4[k3*PATHSIZE + ps].money1 = m1;
					path4[k3*PATHSIZE + pathnum4[k3]++].money3 = m3;
				}
			}
		}
		for (int i = 0; i < pathsgnnum4; i++)
		{
			Path* temp = path4 + pathsgn4[i] * PATHSIZE;
			sort(temp, temp + pathnum4[pathsgn4[i]], pathsort);
		}
		p0 = pathnum4[k0];
		for (int j0 = 0; j0 < p0; j0++)
		{
			ptmp1 = path4[k0*PATHSIZE + j0];
			pm1 = ptmp1.money1;
			pm3 = ptmp1.money3;
			if (pm3 > 3 * pm1 || pm1 > 5 * pm3)continue;
			res34[res34len * 3] = k0;
			res34[res34len * 3 + 1] = ptmp1.point2;
			res34[res34len * 3 + 2] = ptmp1.point1;
			res34len++;
		}
		length3 = newpicnum[k0];
		for (int i4 = 0; i4 < length3; i4++)      //newpicnum为k0对应节点的出度
		{
			k4 = pic[k0][i4].to;
			if (k4 <= k0)continue;
			m4 = pic[k0][i4].money;
			p4 = pathnum4[k4];
			for (int j1 = 0; j1 < p4; j1++)
			{
				ptmp1 = path4[k4*PATHSIZE + j1];
				pm1 = ptmp1.money1;
				pm3 = ptmp1.money3;
				if (m4 > 3 * pm1 || pm3 > 3 * m4 || \
					pm1 > 5 * m4 || m4 > 5 * pm3)continue;
				res44[res44len * 4] = k0;
				res44[res44len * 4 + 1] = k4;
				res44[res44len * 4 + 2] = ptmp1.point2;
				res44[res44len * 4 + 3] = ptmp1.point1;
				res44len++;
			}
			length4 = newpicnum[k4];
			for (int i5 = 0; i5 < length4; i5++)
			{
				m5 = pic[k4][i5].money;
				k5 = pic[k4][i5].to;
				if (k5 <= k0 || m5 > 3 * m4 || m4 > 5 * m5)continue;
				p5 = pathnum4[k5];
				for (int j1 = 0; j1 < p5; j1++)
				{
					ptmp1 = path4[k5*PATHSIZE + j1];
					pm1 = ptmp1.money1;
					pm3 = ptmp1.money3;
					if (m4 > 3 * pm1 || pm3 > 3 * m5 || \
						pm1 > 5 * m4 || m5 > 5 * pm3)continue;
					flag1 = ptmp1.point1;
					flag2 = ptmp1.point2;
					if (k4 == flag1 || k4 == flag2)continue;
					res54[res54len * 5] = k0;
					res54[res54len * 5 + 1] = k4;
					res54[res54len * 5 + 2] = k5;
					res54[res54len * 5 + 3] = flag2;
					res54[res54len * 5 + 4] = flag1;
					res54len++;
				}
				length5 = newpicnum[k5];

				for (int i6 = 0; i6 < length5; i6++)
				{
					m6 = pic[k5][i6].money;
					k6 = pic[k5][i6].to;
					if (k6 <= k0 || m6 > 3 * m5 || m5 > 5 * m6 || k6 == k4)continue;
					p6 = pathnum4[k6];
					for (int j1 = 0; j1 < p6; j1++)
					{
						ptmp1 = path4[k6*PATHSIZE + j1];
						pm1 = ptmp1.money1;
						pm3 = ptmp1.money3;
						if (m4 > 3 * pm1 || pm3 > 3 * m6 || \
							pm1 > 5 * m4 || m6 > 5 * pm3)continue;
						flag1 = ptmp1.point1;
						flag2 = ptmp1.point2;
						if (k4 == flag1 || k4 == flag2 || \
							k5 == flag1 || k5 == flag2)continue;
						res64[res64len * 6] = k0;
						res64[res64len * 6 + 1] = k4;
						res64[res64len * 6 + 2] = k5;
						res64[res64len * 6 + 3] = k6;
						res64[res64len * 6 + 4] = flag2;
						res64[res64len * 6 + 5] = flag1;
						res64len++;
					}
					length6 = newpicnum[k6];

					for (int i7 = 0; i7 < length6; i7++)
					{
						m7 = pic[k6][i7].money;
						k7 = pic[k6][i7].to;
						if (k7 <= k0 || m7 > 3 * m6 || m6 > 5 * m7 || k7 == k4 || k7 == k5)continue;
						p7 = pathnum4[k7];
						for (int j1 = 0; j1 < p7; j1++)
						{
							ptmp1 = path4[k7*PATHSIZE + j1];
							pm1 = ptmp1.money1;
							pm3 = ptmp1.money3;
							if (m4 > 3 * pm1 || pm3 > 3 * m7 || \
								pm1 > 5 * m4 || m7 > 5 * pm3)continue;
							flag1 = ptmp1.point1;
							flag2 = ptmp1.point2;
							if (k4 == flag1 || k4 == flag2 || \
								k5 == flag1 || k5 == flag2 || \
								k6 == flag1 || k6 == flag2)continue;
							res74[res74len * 7] = k0;
							res74[res74len * 7 + 1] = k4;
							res74[res74len * 7 + 2] = k5;
							res74[res74len * 7 + 3] = k6;
							res74[res74len * 7 + 4] = k7;
							res74[res74len * 7 + 5] = flag2;
							res74[res74len * 7 + 6] = flag1;
							res74len++;
						}
					}
				}
			}
		}
		for (int i = 0; i < pathsgnnum4; i++)
		{
			pathnum4[pathsgn4[i]] = 0;
		}
		pathsgnnum4 = 0;
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
	ck1.join();
	ck2.join();
	ck3.join();
	ck4.join();
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
	int a, b;
	for (int j = 0; j < size; j++)
	{
		for (int i = 0; i < batch; i++)
		{
			b = a = setnum[intput[j * batch + i]];
			if (a < 10000000)
			{
				if (a >= 1000000)
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
				else if (a >= 100000)
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
				else if (a >= 10000)
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
				else if (a >= 1000)
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
				else if (a >= 100)
				{
					output[count + 2] = '0' + b % 10;
					b /= 10;
					output[count + 1] = '0' + b % 10;
					b /= 10;
					output[count + 0] = '0' + b % 10;
					count += 3;
				}
				else if (a >= 10)
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
			else if (a >= 1000000000)
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
				output[count++] = ',';
			}
			else if (a >= 100000000)
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
				output[count++] = ',';
			}
			else
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
				output[count++] = ',';
			}
		}
		output[--count] = '\n';
		count += 1;
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
	anstr[id] = new char[res7len * 50];
	int temp = gap[id];
	for (int i = gap[id - 1]; i < temp; i++)
	{
		pos[id] = to_str(res7p[i], (res7p[i + 4] - res7p[i]) / 7, 7, pos[id], anstr[id]);
	}
}


void savetostr(char* savepath)
{
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
	char loadpath[30] = "/data/test_data.txt";
	char savepath[30] = "/projects/student/result.txt";
	loaddata(loadpath);
	makestar();
	jianzhi();
	toeasy();
	checkcircle();
	savetostr(savepath);
	exit(0);
}
