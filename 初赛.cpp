#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
using namespace std;

int total[560000];   //读取的所有数据，不包含第三列
int totallen;   //据总长
char* anstr1;
char* anstr2;
char* anstr3;
char* anstr4;
char* anstr5;
char* anstr6;
char* anstr7;
char* anstr8;
char* anstr9;
int pos1;
int pos2;
int pos3;
int pos4;
int pos5;
int pos6;
int pos7;
int pos8;
int pos9;
int pic[280000][50];
int Npic[280000][50];
int picdex[280000];
int picdexid[280000];
int Npicdex[280000];
bool delpic[280000] = { 0 };
int picdexnum = 0;
int newpicdexnum = 0;
int Npicdexnum = 0;
int picnum[280000] = { 0 };
int Npicnum[280000] = { 0 };
int newpicnum[280000];
int newNpicnum[280000];
int res31[3 * 500000];
int res32[3 * 500000];
int res33[3 * 500000];
int res34[3 * 500000];
int res41[4 * 500000];
int res42[4 * 500000];
int res43[4 * 500000];
int res44[4 * 500000];
int res51[5 * 1000000];
int res52[5 * 1000000];
int res53[5 * 1000000];
int res54[5 * 1000000];
int res61[6 * 1000000];
int res62[6 * 2000000];
int res63[6 * 2000000];
int res64[6 * 2000000];
int res71[7 * 3000000];
int res72[7 * 3000000];
int res73[7 * 3000000];
int res74[7 * 3000000];

int pathsgn1[280000];
int path1[5600000] = { 0 };
int pathnum1[280000] = { 0 };

int pathsgn2[280000];
int path2[5600000] = { 0 };
int pathnum2[280000] = { 0 };

int pathsgn3[280000];
int path3[5600000] = { 0 };
int pathnum3[280000] = { 0 };

int pathsgn4[280000];
int path4[5600000] = { 0 };
int pathnum4[280000] = { 0 };


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
	int i = 0;
	while (i < len)
	{
		while (*(szBuf + i++) != ',')
		{
			num = num * 10 + (szBuf[i - 1] - '0');
		}
		total[totallen++] = num;
		num = 0;
		while (*(szBuf + i++) != ',')
		{
			num = num * 10 + (szBuf[i - 1] - '0');
		}
		total[totallen++] = num;
		num = 0;
		while (*(szBuf + i++) != '\n');
	}
	delete[] szBuf;
}

void makestar()
{
	int u, v;
	for (int i = 0; i < totallen; i += 2)
	{
		u = total[i];
		v = total[i + 1];
		if (picnum[u] == 0)
		{
			picdex[++picdexnum] = u;//存不重复第一位节点
		}
		pic[u][picnum[u]++] = v;//画正向图，picnum为各节点出度
		Npic[v][Npicnum[v]++] = u;//画反向图,Npicnum为各节点入度
	}
}

void jianzhi()
{
	newpicdexnum = ++picdexnum;
	memcpy(newpicnum, picnum, 280000 * sizeof(int));
	memcpy(newNpicnum, Npicnum, 280000 * sizeof(int));
	int top = 0;
	int x, temp;
	//int stack[280000];
	int* stack = new int[totallen / 2];
	for (int i = 1; i < picdexnum; i++)
	{
		temp = picdex[i];
		if (newNpicnum[temp] == 0) {//判定入度为0
			stack[top++] = temp;//存入节点实值
			delpic[temp] = 1;
		}
	}
	top--;
	while (top > -1)
	{
		x = stack[top--];
		for (int i = 0; i < picnum[x]; i++)
		{
			temp = pic[x][i];
			if (temp >= 300000)continue;
			if (--newNpicnum[temp] == 0) {
				stack[++top] = temp;
				delpic[temp] = 1;
			}
			*find(Npic[temp], Npic[temp] + Npicnum[temp], x) = 300000;//在反向图里也删除x
		}
	}
	top = 0;
	for (int i = 1; i < picdexnum + 1; i++)
	{
		temp = picdex[i];
		if (newpicnum[temp] == 0) {
			stack[top++] = temp;
			delpic[temp] = 1;
		}
	}
	top--;
	while (top > -1)
	{
		x = stack[top--];
		for (int i = 0; i < newNpicnum[x]; i++)
		{
			temp = Npic[x][i];
			if (temp >= 300000)continue;
			if (--newpicnum[temp] == 0) {
				stack[++top] = temp;
				delpic[temp] = 1;
			}
			*find(pic[temp], pic[temp] + picnum[temp], x) = 300000;//在正向图里也删除x
		}
	}
}

void toeasy()
{
	for (int j = 0; j < picdexnum; j++)
	{
		if (delpic[picdex[j]]) {
			picdex[j] = 300000;
			newpicdexnum--;
		}
	}
		sort(picdex + 1, picdex + picdexnum );//对正向第一位节点进行排序 30w的剪去节点都到最后了
		for (int i = 1; i < newpicdexnum; i++)
		{
			sort(pic[picdex[i]], pic[picdex[i]] + picnum[picdex[i]]);//对出度节点按大小进行排序
		};
		for (int i = 1; i < newpicdexnum; i++)
		{
			sort(Npic[picdex[i]], Npic[picdex[i]] + Npicnum[picdex[i]]);//对入度节点按大小排序
		};
}

void checkcircle1()
{
	int pathsgnnum1 = 0;
	int k0, k1, k2, k4, k5, k6, k7, k8, index, flag;
	int length1, length2, length3, length4, length5, length6, length7;
	for (int i0 = 1; i0 < newpicdexnum / 14; i0++)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = picdex[i0];                                      //i0对应节点实值
		length1 = newNpicnum[picdex[i0]];
		for (int i1 = 0; i1 < length1; i1++)   //newNpicnum[picdex[i0]]为i0节点的入度
		{
			k1 = Npic[k0][i1];                                  //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			length2 = newNpicnum[Npic[k0][i1]];
			for (int i2 = 0; i2 < length2; i2++)
			{
				k2 = Npic[k1][i2];                                //k2为i2对应i1的出节点实值
				if (k2 >= k0) {
					path1[k2 * 20 + pathnum1[k2]++] = k1;             //记录k1值
					pathsgn1[pathsgnnum1++] = k2;                 //pathsgnnum为标记的距离k0长度为2的节点个数
				}
			}
		}
		length3 = newpicnum[picdex[i0]];
		for (int i4 = 0; i4 < length3; i4++)      //newpicnum为i0对应节点的出度
		{
			k4 = pic[k0][i4];
			if (k4 <= k0)continue;
			for (int j1 = 0; j1 < pathnum1[k4]; j1++)
			{
				flag = path1[k4 * 20 + j1];
				index = res31len * 3;
				res31[index++] = k0;
				res31[index++] = k4;
				res31[index++] = flag;
				res31len++;
			}
			length4 = newpicnum[pic[k0][i4]];
			for (int i5 = 0; i5 < length4; i5++)
			{
				k5 = pic[k4][i5];
				if (k5 <= k0)continue;
				for (int j1 = 0; j1 < pathnum1[k5]; j1++)
				{
					flag = path1[k5 * 20 + j1];
					if (k4 == flag)continue;
					index = res41len * 4;
					res41[index++] = k0;
					res41[index++] = k4;
					res41[index++] = k5;
					res41[index++] = flag;
					res41len++;
				}
				length5 = newpicnum[pic[k4][i5]];
				for (int i6 = 0; i6 < length5; i6++)
				{
					k6 = pic[k5][i6];
					if (k6 <= k0 || k6 == k4)continue;
					for (int j1 = 0; j1 < pathnum1[k6]; j1++)
					{
						flag = path1[k6 * 20 + j1];
						if (k4 == flag || \
							k5 == flag)continue;
						index = res51len * 5;
						res51[index++] = k0;
						res51[index++] = k4;
						res51[index++] = k5;
						res51[index++] = k6;
						res51[index++] = flag;
						res51len++;
					}
					length6 = newpicnum[pic[k5][i6]];
					for (int i7 = 0; i7 < length6; i7++)
					{
						k7 = pic[k6][i7];
						if (k7 <= k0 || k7 == k4 || k7 == k5)continue;
						for (int j1 = 0; j1 < pathnum1[k7]; j1++)
						{
							flag = path1[k7 * 20 + j1];
							if (k4 == flag || \
								k5 == flag || \
								k6 == flag)continue;
							index = res61len * 6;
							res61[index++] = k0;
							res61[index++] = k4;
							res61[index++] = k5;
							res61[index++] = k6;
							res61[index++] = k7;
							res61[index++] = flag;
							res61len++;
						}
						length7 = newpicnum[pic[k6][i7]];
						for (int i8 = 0; i8 < length7; i8++)
						{
							k8 = pic[k7][i8];
							if (k8 <= k0 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							for (int j1 = 0; j1 < pathnum1[k8]; j1++)
							{
								flag = path1[k8 * 20 + j1];
								if (k4 == flag || \
									k5 == flag || \
									k6 == flag || \
									k7 == flag)continue;
								index = res71len * 7;
								res71[index++] = k0;
								res71[index++] = k4;
								res71[index++] = k5;
								res71[index++] = k6;
								res71[index++] = k7;
								res71[index++] = k8;
								res71[index++] = flag;
								res71len++;
							}
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
}

void checkcircle2()
{
	int pathsgnnum2 = 0;
	int k0, k1, k2, k4, k5, k6, k7, k8, index, flag;
	int l1, l2, l3, l4, l5, l6, l7;
	for (int i0 = newpicdexnum / 14; i0 < newpicdexnum / 6; i0++)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = picdex[i0];                                      //i0对应节点实值
		l1 = newNpicnum[picdex[i0]];
		for (int i1 = 0; i1 < l1; i1++)   //newNpicnum[picdex[i0]]为i0节点的入度
		{
			k1 = Npic[k0][i1];                                  //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			l2 = newNpicnum[Npic[k0][i1]];
			for (int i2 = 0; i2 < l2; i2++)
			{
				k2 = Npic[k1][i2];                                //k2为i2对应i1的出节点实值
				if (k2 >= k0) {
					path2[k2 * 20 + pathnum2[k2]++] = k1;             //记录k1值
					pathsgn2[pathsgnnum2++] = k2;                 //pathsgnnum为标记的距离k0长度为2的节点个数
				}
			}
		}
		l3 = newpicnum[picdex[i0]];
		for (int i4 = 0; i4 < l3; i4++)      //newpicnum为i0对应节点的出度
		{
			k4 = pic[k0][i4];
			if (k4 <= k0)continue;
			for (int j1 = 0; j1 < pathnum2[k4]; j1++)
			{
				flag = path2[k4 * 20 + j1];
				index = res32len * 3;
				res32[index++] = k0;
				res32[index++] = k4;
				res32[index++] = flag;
				res32len++;
			}
			l4 = newpicnum[pic[k0][i4]];
			for (int i5 = 0; i5 < l4; i5++)
			{
				k5 = pic[k4][i5];
				if (k5 <= k0)continue;
				for (int j1 = 0; j1 < pathnum2[k5]; j1++)
				{
					flag = path2[k5 * 20 + j1];
					if (k4 == flag)continue;
					index = res42len * 4;
					res42[index++] = k0;
					res42[index++] = k4;
					res42[index++] = k5;
					res42[index++] = flag;
					res42len++;
				}
				l5 = newpicnum[pic[k4][i5]];
				for (int i6 = 0; i6 < l5; i6++)
				{
					k6 = pic[k5][i6];
					if (k6 <= k0 || k6 == k4)continue;
					for (int j1 = 0; j1 < pathnum2[k6]; j1++)
					{
						flag = path2[k6 * 20 + j1];
						if (k4 == flag || \
							k5 == flag)continue;
						index = res52len * 5;
						res52[index++] = k0;
						res52[index++] = k4;
						res52[index++] = k5;
						res52[index++] = k6;
						res52[index++] = flag;
						res52len++;
					}
					l6 = newpicnum[pic[k5][i6]];
					for (int i7 = 0; i7 < l6; i7++)
					{
						k7 = pic[k6][i7];
						if (k7 <= k0 || k7 == k4 || k7 == k5)continue;
						for (int j1 = 0; j1 < pathnum2[k7]; j1++)
						{
							flag = path2[k7 * 20 + j1];
							if (k4 == flag || \
								k5 == flag || \
								k6 == flag)continue;
							index = res62len * 6;
							res62[index++] = k0;
							res62[index++] = k4;
							res62[index++] = k5;
							res62[index++] = k6;
							res62[index++] = k7;
							res62[index++] = flag;
							res62len++;
						}
						l7 = newpicnum[pic[k6][i7]];
						for (int i8 = 0; i8 < l7; i8++)
						{
							k8 = pic[k7][i8];
							if (k8 <= k0 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							for (int j1 = 0; j1 < pathnum2[k8]; j1++)
							{
								flag = path2[k8 * 20 + j1];
								if (k4 == flag || \
									k5 == flag || \
									k6 == flag || \
									k7 == flag)continue;
								index = res72len * 7;
								res72[index++] = k0;
								res72[index++] = k4;
								res72[index++] = k5;
								res72[index++] = k6;
								res72[index++] = k7;
								res72[index++] = k8;
								res72[index++] = flag;
								res72len++;
							}
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
}

void checkcircle3()
{
	int pathsgnnum3 = 0;
	int k0, k1, k2, k4, k5, k6, k7, k8, index, flag;
	int l1, l2, l3, l4, l5, l6, l7;
	for (int i0 = newpicdexnum / 6; i0 < newpicdexnum / 3; i0++)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = picdex[i0];                                      //i0对应节点实值
		l1 = newNpicnum[picdex[i0]];
		for (int i1 = 0; i1 < l1; i1++)   //newNpicnum[picdex[i0]]为i0节点的入度
		{
			k1 = Npic[k0][i1];                                  //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			l2 = newNpicnum[Npic[k0][i1]];
			for (int i2 = 0; i2 < l2; i2++)
			{
				k2 = Npic[k1][i2];                                //k2为i2对应i1的出节点实值
				if (k2 >= k0) {
					path3[k2 * 20 + pathnum3[k2]++] = k1;             //记录k1值
					pathsgn3[pathsgnnum3++] = k2;                 //pathsgnnum为标记的距离k0长度为2的节点个数
				}
			}
		}
		l3 = newpicnum[picdex[i0]];
		for (int i4 = 0; i4 < l3; i4++)      //newpicnum为i0对应节点的出度
		{
			k4 = pic[k0][i4];
			if (k4 <= k0)continue;
			for (int j1 = 0; j1 < pathnum3[k4]; j1++)
			{
				index = res33len * 3;
				res33[index++] = k0;
				res33[index++] = k4;
				res33[index++] = path3[k4 * 20 + j1];
				res33len++;
			}
			l4 = newpicnum[pic[k0][i4]];
			for (int i5 = 0; i5 < l4; i5++)
			{
				k5 = pic[k4][i5];
				if (k5 <= k0)continue;
				for (int j1 = 0; j1 < pathnum3[k5]; j1++)
				{
					flag = path3[k5 * 20 + j1];
					if (k4 == flag)continue;
					index = res43len * 4;
					res43[index++] = k0;
					res43[index++] = k4;
					res43[index++] = k5;
					res43[index++] = flag;
					res43len++;
				}
				l5 = newpicnum[pic[k4][i5]];
				for (int i6 = 0; i6 < l5; i6++)
				{
					k6 = pic[k5][i6];
					if (k6 <= k0 || k6 == k4)continue;
					for (int j1 = 0; j1 < pathnum3[k6]; j1++)
					{
						flag = path3[k6 * 20 + j1];
						if (k4 == flag || \
							k5 == flag)continue;
						index = res53len * 5;
						res53[index++] = k0;
						res53[index++] = k4;
						res53[index++] = k5;
						res53[index++] = k6;
						res53[index++] = flag;
						res53len++;
					}
					l6 = newpicnum[pic[k5][i6]];
					for (int i7 = 0; i7 < l6; i7++)
					{
						k7 = pic[k6][i7];
						if (k7 <= k0 || k7 == k4 || k7 == k5)continue;
						for (int j1 = 0; j1 < pathnum3[k7]; j1++)
						{
							flag = path3[k7 * 20 + j1];
							if (k4 == flag || \
								k5 == flag || \
								k6 == flag)continue;
							index = res63len * 6;
							res63[index++] = k0;
							res63[index++] = k4;
							res63[index++] = k5;
							res63[index++] = k6;
							res63[index++] = k7;
							res63[index++] = flag;
							res63len++;
						}
						l7 = newpicnum[pic[k6][i7]];
						for (int i8 = 0; i8 < l7; i8++)
						{
							k8 = pic[k7][i8];
							if (k8 <= k0 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							for (int j1 = 0; j1 < pathnum3[k8]; j1++)
							{
								flag = path3[k8 * 20 + j1];
								if (k4 == flag || \
									k5 == flag || \
									k6 == flag || \
									k7 == flag)continue;
								index = res73len * 7;
								res73[index++] = k0;
								res73[index++] = k4;
								res73[index++] = k5;
								res73[index++] = k6;
								res73[index++] = k7;
								res73[index++] = k8;
								res73[index++] = flag;
								res73len++;
							}
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
}

void checkcircle4()
{
	int pathsgnnum4 = 0;

	int k0, k1, k2, k4, k5, k6, k7, k8, index, flag;
	int l1, l2, l3, l4, l5, l6, l7;
	for (int i0 = newpicdexnum / 3; i0 < newpicdexnum; i0++)                //newpicdexnum为剪枝后的第一位不重复节点个数
	{
		k0 = picdex[i0];                                      //i0对应节点实值
		l1 = newNpicnum[picdex[i0]];
		for (int i1 = 0; i1 < l1; i1++)   //newNpicnum[picdex[i0]]为i0节点的入度
		{
			k1 = Npic[k0][i1];                                  //k1为i1对应i0的出节点实值
			if (k1 <= k0)continue;
			l2 = newNpicnum[Npic[k0][i1]];
			for (int i2 = 0; i2 < l2; i2++)
			{
				k2 = Npic[k1][i2];                                //k2为i2对应i1的出节点实值
				if (k2 >= k0) {
					path4[k2 * 20 + pathnum4[k2]++] = k1;             //记录k1值
					pathsgn4[pathsgnnum4++] = k2;                 //pathsgnnum为标记的距离k0长度为2的节点个数
				}
			}
		}
		l3 = newpicnum[picdex[i0]];
		for (int i4 = 0; i4 < l3; i4++)      //newpicnum为i0对应节点的出度
		{
			k4 = pic[k0][i4];
			if (k4 <= k0)continue;
			for (int j1 = 0; j1 < pathnum4[k4]; j1++)
			{
				flag = path4[k4 * 20 + j1];
				index = res34len * 3;
				res34[index++] = k0;
				res34[index++] = k4;
				res34[index++] = flag;
				res34len++;
			}
			l4 = newpicnum[pic[k0][i4]];
			for (int i5 = 0; i5 < l4; i5++)
			{
				k5 = pic[k4][i5];
				if (k5 <= k0)continue;
				for (int j1 = 0; j1 < pathnum4[k5]; j1++)
				{
					flag = path4[k5 * 20 + j1];
					if (k4 == flag)continue;
					index = res44len * 4;
					res44[index++] = k0;
					res44[index++] = k4;
					res44[index++] = k5;
					res44[index++] = flag;
					res44len++;
				}
				l5 = newpicnum[pic[k4][i5]];
				for (int i6 = 0; i6 < l5; i6++)
				{
					k6 = pic[k5][i6];
					if (k6 <= k0 || k6 == k4)continue;
					for (int j1 = 0; j1 < pathnum4[k6]; j1++)
					{
						flag = path4[k6 * 20 + j1];
						if (k4 == flag || \
							k5 == flag)continue;
						index = res54len * 5;
						res54[index++] = k0;
						res54[index++] = k4;
						res54[index++] = k5;
						res54[index++] = k6;
						res54[index++] = flag;
						res54len++;
					}
					l6 = newpicnum[pic[k5][i6]];
					for (int i7 = 0; i7 < l6; i7++)
					{
						k7 = pic[k6][i7];
						if (k7 <= k0 || k7 == k4 || k7 == k5)continue;
						for (int j1 = 0; j1 < pathnum4[k7]; j1++)
						{
							flag = path4[k7 * 20 + j1];
							if (k4 == flag || \
								k5 == flag || \
								k6 == flag)continue;
							index = res64len * 6;
							res64[index++] = k0;
							res64[index++] = k4;
							res64[index++] = k5;
							res64[index++] = k6;
							res64[index++] = k7;
							res64[index++] = flag;
							res64len++;
						}
						l7 = newpicnum[pic[k6][i7]];
						for (int i8 = 0; i8 < l7; i8++)
						{
							k8 = pic[k7][i8];
							if (k8 <= k0 || k8 == k4 || k8 == k5 || k8 == k6)continue;
							for (int j1 = 0; j1 < pathnum4[k8]; j1++)
							{
								flag = path4[k8 * 20 + j1];
								if (k4 == flag || \
									k5 == flag || \
									k6 == flag || \
									k7 == flag)continue;
								index = res74len * 7;
								res74[index++] = k0;
								res74[index++] = k4;
								res74[index++] = k5;
								res74[index++] = k6;
								res74[index++] = k7;
								res74[index++] = k8;
								res74[index++] = flag;
								res74len++;
							}
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
}

void checkcircle()
{
	thread ck4(&checkcircle1);
	thread ck1(&checkcircle4);
	thread ck3(&checkcircle2);
	thread ck2(&checkcircle3);
	ck4.join();
	ck1.join();
	ck3.join();
	ck2.join();
}

int to_str0(int intput,char* output)
{
	if (intput >= 1000000)
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
	int a;
	for (int j = 0; j < size; j++)
	{
		for (int i = 0; i < batch; i++)
		{
			a = intput[j * batch + i];
			if (a >= 100000)
			{
				output[count + 5] = '0' + a % 10;
				a /= 10;
				output[count + 4] = '0' + a % 10;
				a /= 10;
				output[count + 3] = '0' + a % 10;
				a /= 10;
				output[count + 2] = '0' + a % 10;
				a /= 10;
				output[count + 1] = '0' + a % 10;
				a /= 10;
				output[count] = '0' + a % 10;
				count += 6;
			}
			else if (a >= 10000)
			{
				output[count + 4] = '0' + a % 10;
				a /= 10;
				output[count + 3] = '0' + a % 10;
				a /= 10;
				output[count + 2] = '0' + a % 10;
				a /= 10;
				output[count + 1] = '0' + a % 10;
				a /= 10;
				output[count] = '0' + a % 10;
				count += 5;
			}
			else if (a >= 1000)
			{
				output[count + 3] = '0' + a % 10;
				a /= 10;
				output[count + 2] = '0' + a % 10;
				a /= 10;
				output[count + 1] = '0' + a % 10;
				a /= 10;
				output[count] = '0' + a % 10;
				count += 4;
			}
			else if (a >= 100)
			{
				output[count + 2] = '0' + a % 10;
				a /= 10;
				output[count + 1] = '0' + a % 10;
				a /= 10;
				output[count] = '0' + a % 10;
				count += 3;
			}
			else if (a >= 10)
			{
				output[count + 1] = '0' + a % 10;
				a /= 10;
				output[count] = '0' + a % 10;
				count += 2;
			}
			else
			{
				output[count] = '0' + a % 10;
				count += 1;
			}
			output[count++] = ',';
		}
		output[--count] = '\n';
		count ++;
	}
	return count;
}

void to_str1()
{
	anstr1 = new char[res3len * 30 + res4len * 40 + res5len * 50 + res6len * 60];
	int countnum = res3len + res4len + res5len + res6len + res7len;
	pos1 = to_str0(countnum, anstr1);
	pos1 = to_str(res31, res31len, 3, pos1, anstr1);
	pos1 = to_str(res32, res32len, 3, pos1, anstr1);
	pos1 = to_str(res33, res33len, 3, pos1, anstr1);
	pos1 = to_str(res34, res34len, 3, pos1, anstr1);
	pos1 = to_str(res41, res41len, 4, pos1, anstr1);
	pos1 = to_str(res42, res42len, 4, pos1, anstr1);
	pos1 = to_str(res43, res43len, 4, pos1, anstr1);
	pos1 = to_str(res44, res44len, 4, pos1, anstr1);
	pos1 = to_str(res51, res51len, 5, pos1, anstr1);
	pos1 = to_str(res52, res52len, 5, pos1, anstr1);
	pos1 = to_str(res53, res53len, 5, pos1, anstr1);
	pos1 = to_str(res54, res54len, 5, pos1, anstr1);
	pos1 = to_str(res61, res61len, 6, pos1, anstr1);
	pos1 = to_str(res62, res62len, 6, pos1, anstr1);
	pos1 = to_str(res63, res63len, 6, pos1, anstr1);
	pos1 = to_str(res64, res64len, 6, pos1, anstr1);

}

void to_str2()
{
	anstr2 = new char[res71len * 70 / 2];
	pos2 = to_str(res71, res71len / 2, 7, 0, anstr2);
}

void to_str3()
{
	anstr3 = new char[res71len * 70 / 2];
	pos3 = to_str(res71 + res71len / 2 * 7, res71len - res71len / 2, 7, 0, anstr3);
}

void to_str4()
{
	anstr4 = new char[res72len * 70 / 2];
	pos4 = to_str(res72, res72len / 2, 7, 0, anstr4);
}

void to_str5()
{
	anstr5 = new char[res72len * 70 / 2];
	pos5 = to_str(res72 + res72len / 2 * 7, res72len - res72len / 2, 7, 0, anstr5);
}

void to_str6()
{
	anstr6 = new char[res73len * 70 / 2];
	pos6 = to_str(res73, res73len / 2, 7, 0, anstr6);
}

void to_str7()
{
	anstr7 = new char[res73len * 70 / 2];
	pos7 = to_str(res73 + res73len / 2 * 7, res73len - res73len / 2, 7, 0, anstr7);
}

void to_str8()
{
	anstr8 = new char[res74len * 70 / 2];
	pos8 = to_str(res74, res74len / 2, 7, 0, anstr8);
}

void to_str9()
{
	anstr9 = new char[res74len * 70 / 2];
	pos9 = to_str(res74 + res74len / 2 * 7, res74len - res74len / 2, 7, 0, anstr9);
}

void savetostr(char* savepath)
{
	res3len = res31len + res32len + res33len + res34len;
	res4len = res41len + res42len + res43len + res44len;
	res5len = res51len + res52len + res53len + res54len;
	res6len = res61len + res62len + res63len + res64len;
	res7len = res71len + res72len + res73len + res74len;

	thread t1(&to_str1);
	thread t2(&to_str2);
	thread t3(&to_str3);
	thread t4(&to_str4);
	thread t5(&to_str5);
	thread t6(&to_str6);
	thread t7(&to_str7);
	thread t8(&to_str8);
	thread t9(&to_str9);
	FILE* fp = fopen(savepath, "wb");
	t1.join();
	fwrite(anstr1, sizeof(char), pos1, fp);
	t2.join();
	fwrite(anstr2, sizeof(char), pos2, fp);
	t3.join();
	fwrite(anstr3, sizeof(char), pos3, fp);
	t4.join();
	fwrite(anstr4, sizeof(char), pos4, fp);
	t5.join();
	fwrite(anstr5, sizeof(char), pos5, fp);
	t6.join();
	fwrite(anstr6, sizeof(char), pos6, fp);
	t7.join();
	fwrite(anstr7, sizeof(char), pos7, fp);
	t8.join();
	fwrite(anstr8, sizeof(char), pos8, fp);
	t9.join();
	fwrite(anstr9, sizeof(char), pos9, fp);
	fclose(fp);
}

void checkans(char* checkpath, char* savepath)
{
	FILE* pFile = fopen(checkpath, "rb");
	fseek(pFile, 0, SEEK_END);
	int len = ftell(pFile);
	char* szBuf = new char[len];
	fseek(pFile, 0, SEEK_SET);
	int iRead = fread(szBuf, 1, len, pFile);
	fclose(pFile);
	int num = 0;
	int* ans = new int[len];
	int anslen = 0;
	for (int i = 0; i < len; i++)
	{
		if (*(szBuf + i) >= '0' && *(szBuf + i) <= '9')
		{
			num = num * 10 + (szBuf[i] - '0');
			continue;
		}
		if (*(szBuf + i) == ',' || *(szBuf + i) == '\n')
		{
			*(ans + anslen) = num;
			num = 0;
			anslen++;
		}
	}

	FILE* pFile2 = fopen(savepath, "rb");
	fseek(pFile2, 0, SEEK_END);
	len = ftell(pFile2);
	fseek(pFile2, 0, SEEK_SET);
	int iRead2 = fread(szBuf, 1, len, pFile2);
	fclose(pFile2);
	num = 0;
	int* sav = new int[len];
	int savlen = 0;
	for (int i = 0; i < len; i++)
	{
		if (*(szBuf + i) >= '0' && *(szBuf + i) <= '9')
		{
			num = num * 10 + (szBuf[i] - '0');
			continue;
		}
		if (*(szBuf + i) == ',' || *(szBuf + i) == '\n')
		{
			*(sav + savlen) = num;
			num = 0;
			savlen++;
		}
	}
	delete[] szBuf;

	int right = 0;
	int totalcheck = 0;
	if (savlen != anslen) {
		cout << "错误！！" << endl;
		return;
	}
	for (int i = 0; i < savlen; i++)
	{
		if (ans[i] == sav[i])right++;
		else {
			cout << sav[i] << endl;
		}
		totalcheck++;
	}
	cout << right * 100.0 / totalcheck << '%' << endl;
	delete[] ans;
	delete[] sav;
}

int main()
{
	//_CrtSetBreakAlloc(5552);
	char loadpath[30] = "/data/1004812/test_data.txt";
	char checkpath[30] = "/data/1004812/result.txt";
	char savepath[30] = "/projects/student/result.txt";
	loaddata(loadpath);           //导入数据并转化为数组total
	makestar();         //配置邻接表
	jianzhi();
	toeasy();
	checkcircle();
	savetostr(savepath);           //int转string
	checkans(checkpath, savepath);
	system("pause");
	exit(0);
}
