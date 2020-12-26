# 2020软件精英挑战赛（Huawei-Code-Craft）
本人所在团队为武长赛区hust_1037，队友有华科的hust_zjq与武汉理工大学的ddmm_whut。个人热身赛封榜前200名守门员，初赛赛区第二，复赛A榜赛区第6/7，B榜没分。个人比较菜，主要以学习性质来参加这个比赛，所以下面分享的思路也比较细，如果大家有什么问题需要讨论也非常希望与我联系，希望能结交各位大佬，Q1209365920。
以下我对几个方面介绍我这次参赛过程的一些看法：1. 初赛经验，2.复赛经验，3.参赛体会

## 1.	初赛经验
初赛赛题需要通过对资金流水本文文件的分析，求出所有的循环转账记录，包括总的转账次数，其中限制条件如下：
1.本端账号ID和对端账号ID为一个32位的正整数
2.转账金额为一个32位的正整数
3.转账记录最多为28万条
4.每个账号平均转账记录数< 10
4.账号A给账号B最多转账一次
5.循环转账的路径长度最小为3最大为7

赛题总体上可以分成五个部分，（1）数据读取，（2）建立转账图，（3）剪枝等前处理，（4）求取循环转账（5）结果转字符并输出文本。
总体思路如下：
### （1）	数据读取
这部分由于有了热身赛的经验写起来得心应手，此处本人采用fread对文件进行读取，性能相当良好，因此初赛过程中基本没有进行过改动。热身赛的实例代码中采用stringstream以及getline组合进行读取，实测相当的慢，此外mmap文件映射性能差异不大，没有进行使用。  
fread的过程又可以分为几步，首先fopen打开文件，rb模式为二进制读取，实测比r快，接着通过fseek文件指针定位到文件末读取长度，在将文件指针定位到开头，建立字符缓冲区存取fread读取的数据并关闭文件。  
接下来需要将读取到的字符转码为所需的int数据型数据，大部分方便的库函数都没有自己写的代码运行快，这里我们也手写一个简单的语句实现字符转码。分析数组格式，为 转入ID，转出ID，转账金额 的格式，我们只需要转入ID，以及转出ID，为尽可能减少冗余的判断，我们针对数据格式依次读取，先建立两个寻找’,’的循环存取数字，再建立寻找’\n’的循环跳过第三个数，一直寻找到文件末尾完成数据读取。以下为该部分代码：  
```cpp
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
	while(i<len)
	{
		while (*(szBuf + i++) != ',')
		{
			num = num * 10 + (szBuf[i-1] - '0');
		}
		total[totallen++] = num;
		num = 0;
		while (*(szBuf + i++) != ',')
		{
			num = num * 10 + (szBuf[i-1] - '0');
		}
		total[totallen++] = num;
		num = 0;
		while (*(szBuf + i++) != '\n');
	}
	delete[] szBuf;
}
```
### （2）	建立转账图
完成转账数据读取后需要建立数据结构用于后续的运算，主要可以采用邻接表或邻接图进行构架，而邻接图存储容量大，访问慢，很早就被pass了。邻接表是通过制定下标的数组存储出边信息。此处本人采用过纯数组构建前向星，map<int,set<int>>等各种结构，一直受限在0.6分以上，最终在大佬们的提示下了解到id数值最大不超过28W (此前一直以为不超过规则说的2^32-1) ，且最大出入度不超过50。那问题就相对简单了，可以把原先建立的map映射去除，节省很大一部分时间（stl的许多数据结构都比较耗时，还是最基础的数组跑的快）。  
  为了便于核心的找环部分算法双向搜索的要求，这里需要同时建立正向表以及反向表，具体实现相对简单，将每个出边放到以其入边为下标的pic（正向表）处，记录出边数，Npic（反向表）同理，值得注意的是只是建立这两个表则存在pic与Npic各存在28W个下标，检索难度大，提供一个接口为picdex记录pic存有边数据的下标（边下标），同时作为后期的访问起点，类似于邻接表中的head。存在于Npic中而不存在于pic中的边下标必定出度为0，因此无需考虑。
```cpp
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
```
### （3）	剪枝等前处理
这个说是优化方法但应该大家都会做，通过删掉不可能成环的边，减少主体找环部分的计算量，具体而言就是讲出入度为0的点一一删除直到不存在出入度为0的点。本人采用拓扑排序的方法进行剪枝，基本参考以下博客：  
https://blog.csdn.net/insistgogo/article/details/6978718  
具体实现首先是通过检索picdex寻找到入度为0的点，建立栈进行存储，通过循环对入栈的点依次进行处理，如果由于删除点使得其出边的点入度为0，则该出边的点也入栈，依次进行最终可将所有入度为的点去除。入度为0的操作包括将删除该点，通过delpic进行记录，判断其出边的点入度是否为0，通过反向表找到被删除的点，并将其删除，删除的方案采用将其设置为大于所有id，并将出边记录-1，并在后续的排序工作中删除。出度为0的点按照同样的方法进行操作。
```cpp
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
由于赛题要求所求环中最小的点需要放在第一位，并且总体有序排列，通过前期排序可以有效的解决该问题，相对后期排序省去了内部排序以及更大工作量的结果排序的工作，并且可以有效的将剪枝部分置位的点删除。这里首先需要对头结点picdex进行的删除进行操作，与上述相同将删除的点置位为最大值30W，并将头结点计数-1。依次完成头结点的排序以及边排序后，即生成所需的数据结构。
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
```
### （4）	求取循环转账
求取有向环网上主流的是应用深度优先遍历（dfs）通过递归语句进行查找，这里由于限制条件设定只需搜索3-7层的有向环，因此这里可以应用for循环嵌套进行实现减少递归函数中函数访问。  
笔者最开始写的是8层嵌套，但实测相对较慢（第一版成绩249），后来经高人点播可以采用正反双向查找的方法降低时间复杂度：8层嵌套的复杂度为O8，而正反双向查找按照笔者最终采用的反向搜索2层，正向搜索5层，复杂度为（O3+O6）,能得到有效的提高（也就是这次改变从249分提升到3分），而很容易想到可以采用反向3层，正向4层的方案，复杂度变为（O4+O5）得到更多的提高，线下测试也得到了有效的提速，但在线上发生了负优化，分析原因可能主要有以下两点导致，第一，线上三领域点数量远大于二领域点，笔者构建反向路径path存储从起点到达某点的路径，2层是空间20即可，而3层是需要500。第二，由于反向检索4层时path需要存储的节点有两个，其排序顺序违背题意所述的从左往右，比较同位数大小，越小则排越前的原则，因此后期需要再进行一次排序。  
为了解决每个环的第一位数必须是其中最小的数，并且可能存在重复环的问题，这里采用过滤所有比起点小的元素的方法，保证每个起点找到的环都是以其开头的环。  
在反向检索过程中如上所述采用path对经过的路径进行记录，搜索3层达到的终点即为path的下标。接着进行正向的检索，从第2层开始查找是否有反向搜索到该点的路径，如果有的话则存入res3结果数据，以下均是以此类推完成，仅需注意避免与经过的节点重复，即可完成全部环的搜索。  
这部分由于采用2+5线下相当慢，所幸线上一直表现良好，一直到初赛完结。以下为多线程中的其中一份找环程序，另外3分均是复制粘贴代码量太大就不贴出来了。  
```cpp
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
```
在之后的优化过程中我们也尝试了将找环部分采用多线程以加快其执行速度，这里采用4线程将找环过程的起点进行划分分别查找，并采用多组4组res分别存取，以保证多线程处理后仍然有序，由于每个找环函数都只搜索起点往后的环，因此越往后其计算量越小，为保证任务量相对均衡，这里采用节点1/14,1/6,1/3,1进行划分。
```cpp
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
```
### （5）	结果转字符并输出文本
结果转字符笔者同样尝试过许多版本，包括to_string，fprintf等，还是上述说的，方便的库函数往往没有手工写的函数快。数字转字符主要问题是不知道数据的位数，笔者参考以下博客：  
https://blog.csdn.net/zhou_lost/article/details/85217366
由于id不大于28W，因此我们只需考虑1-6位数，为加快速度，采用多个if/else嵌套根据数字位数对字符进行填充。提取位数是通过对数字不断除十取余的方法得到每位数的数值并转码为字符数字。由于每个结果数组所转的字符需要添加在前个结果数组对应字符之后，如res4要在res3之后添加，因此设定返回值count为下一次任务的起点。
```cpp
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
```
考虑到环的总数是唯一一个可能>=100W的数，为减少判断将其单独拿出，另写一个函数，仅增加一层if（a>=100W），程序相似，就不贴出来了。  
在实际测试中这块也是比较耗时的一部分，并且显然可以并行，同样采用多线程进行处理。将res3-7测定时间发现，res7的时间大约是res3-6之和的8倍左右，因此这里将转字符的工作分为9分，一份处理3-6层，另外8层均摊7层，形成多线程任务分配。以下为3-6层的转字符函数：
```cpp
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
```
由于转字符任务完成后就可以直接写入文本，因此按顺序开设多线程，并依次阻塞，完成指定部分任务就开始写入文本，因此形成的多线程总函数如下：
```cpp
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
```
## 2.复赛经验
复赛A榜对于初赛的改动主要在于，增加了数据量至200W，环数上限至2kW，以及增加了转账的权值。个人原本以为赛题改动不大，在初赛的基础上简单改改复赛也就过去了，但后来发现我太天真了，初赛由于线上数据比较简单导致对于程序的部分性能考察不明显，没能暴露出问题，到复赛后才发觉和前排的差距相当大。之后就开启了对成就漫长的优化拉锯战。
复赛A榜相较于初赛产生的主要问题包括：
1.	由于存在AB榜，针对线上数据的trick不再有效，尤其是数据必须做映射（因为可能的id上限为2^32-1），以及最大出入度未知；
2.	对算法的性能考察更强，以及数据量扩展对3+4或2+5双向搜索路径存储空间的影响。

本人在复赛之后对各部分基本都进行了一定的调整，其中主要包括四个方面：
1.	建图方面为适应数据量的变化改为类似前向星的建图方式；
2.	算法方面由于一段时间的上分瓶颈又把各类方法都写了一遍，最后有3+6反向剪枝算法以及3+4双向搜索算法两种；
3.	对负载均衡的方法进行的一定的探索，由于初赛次数多且线上数据集不变，因此盲猜线程负载比上分，而复赛却因此分数上遇到了瓶颈。
4.	对转字符负载均衡方法进行了一定探索，由于线上环数分配情况不明，转字符的负载也存在均衡问题。

以下我对个人的程序进行一定解析：
读取部分基本没变化就不多赘述了，唯一增加了一个对转账金额为0的数据的筛除。
### （1）.建图部分
由于数据量比较大，前期大家都在讨论用数组还是vector，我也都写了，不过vector可能用的不是很好，速度相比纯数组下降很多，至今还没搞通大佬们怎么把vector用到比数组还快的。  
首先这里需要重新捡起初赛抛弃的数据映射，而unordered_map与map的对比中在win上测试相差不多，但在鲲鹏中map会慢几倍，因此这里提取第一列的无重复数集，通过提前排序以适应unordered_map排序，其中提取无重复数集的过程中不知道为什么memcpy更慢，后来一直采用遍历的方式。  
为了解决出入度过大的问题，尤其是后期换了菊花图，在vector写法中可以直接push_back，而在数组中可以模拟这个push_back的过程，每当数组达到一定的大小就开设新的空间并将原来的数据memcpy过去，比起vector有一定提升，但还是慢，后来采用类似前向星的数据密排方式，参考下述的链接：  
https://blog.csdn.net/acdreamers/article/details/16902023   
首先统计数据出入度，由此定位每个id对应的出边/入边在一个200W大小的数组中的位置。然后之后的过程就和原先一样了，不过由于数据的访问多了一层数组嵌套，访问速度肯定还是比初赛的要低些。另外建图部分有些计算较慢开设了多线程，并用原子变量atomic解决的数据冲突的问题，相较mutex线程锁效率更高。。  
以下为相关程序:
```cpp
void mkdeg1()
{
	for (int i = 0; i < totallen / 12 * 3; i += 3)
	{
		picdegree[edindex[total[i]]]++;
		Npicdegree[edindex[total[i + 1]]]++;
	}
}
void makestar()
{
	for (int i = 0; i < totallen; i += 3)
	{
		setnum[setlen++] = total[i];     //提取第一列数据
	}
	sort(setnum + 1, setnum + setlen);  //排序 
	setlen = unique(setnum + 1, setnum + setlen) - setnum;  //去重
	for (int i = 1; i < setlen; i++)
	{
		edindex[setnum[i]] = i;   //映射
	}
	thread mg1(mkdeg1);
	thread mg2(mkdeg2);
	thread mg3(mkdeg3);
	thread mg4(mkdeg4);
	mg1.join();
	mg2.join();
	mg3.join();
	mg4.join();  //求出入度
	pic[1] = 0;
	Npic[1] = 0;
	for (int i = 2; i < setlen; i++)
	{
		pic[i] = pic[i - 1] + picdegree[i - 1];
		Npic[i] = Npic[i - 1] + Npicdegree[i - 1];
	}       //建立前向星框架
	thread mp1(mkpic1);
	thread mp2(mkpic2);
	thread mp3(mkpic3);
	thread mp4(mkpic4);
	mp1.join();
	mp2.join();
	mp3.join();
	mp4.join();     //添加数据建图
}
```
在前处理部分还有一点的变化在于，后期增加了对前向星头结点的遍历，因此在完成剪枝后建立了去除被删除头结点的新数集newsetnum。并在边排序中开设了多线程（上分无门就各种开线程hh）。
```cpp
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
```
### （2）算法部分
在初赛部分已经介绍过3+4双向搜索以及2+5双向搜索，总体变化不大，主要是路径存储的静态空间不太开设的下，因此改为动态空间，而三领域点听说不多，开设了128,（初赛开设到300还挂了，复赛竟然更低）。  
复赛主要采用了另一种3+6反向剪枝的算法，通过反向遍历3层，标记所有经过的点，再正向搜索的过程中根据其删除不可能满足3-7层环的点以此提高正向搜索的效率。并且反向搜索附加的读写负担较少，导致实测民间的200W数据集比3+4方法要快，其中比较值得注意的点就是条件判断是算法部分优化的一个关键，其中||对于前面已经判断为true的后面不会继续进行判断，因此概率越大越要放在前面，并且能进行continue的就尽早进行continue判断，避免无用的计算。  
均衡负载方面采用了按起点轮询的方式，开设4线程则相邻的4个头结点分别分配给四个线程实现尽可能的负载均衡。但这样之后结果存储就不连续了，为此增加了指针resxp指向结果数组用于转字符时的提取。  
以下为相关程序，同样多线程只列出一个：
```cpp
void checkcircle1()
{
	int visgnum1 = 0;     //反向检索的点数量记录

	int k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;      //记录出入边减少访问
	int64_t m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;   //记录转账金额减少访问

	int length0, length1, length2, length3, length4, length5, length6, length7, length8; //记录出入边数减少访问
	int i0 = 1;
	for (; i0 < newsetlen; i0 += 4)
	{
		k0 = newsetnum[i0];
		res3p[i0] = res31 + res31len * 3;
		res4p[i0] = res41 + res41len * 4;
		res5p[i0] = res51 + res51len * 5;
		res6p[i0] = res61 + res61len * 6;
		res7p[i0] = res71 + res71len * 7;        //标记结果搜索到的位置
		length0 = newNpicnum[k0];
		for (int i1 = 0; i1 < length0; i1++)
		{
			k1 = realNpic[Npic[k0] + i1].to;
			if (k1 <= k0)continue;        //排除比起点小的点
			m1 = realNpic[Npic[k0] + i1].money;
			vis1[k1] = 3;     //反向检索第1层记录为3
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
					vis1[k2] = 2;     //反向检索第2层记录为2
					visgn1[visgnum1++] = k2;
				}
				length2 = newNpicnum[k2];
				for (int i3 = 0; i3 < length2; i3++)
				{
					k3 = realNpic[Npic[k2] + i3].to;
					if (k3 <= k0 || vis1[k3])continue;   //层数低的覆盖层数高的
					m3 = realNpic[Npic[k2] + i3].money;
					if (m2 > 3 * m3 || 5 * m2 < m3)continue;
					vis1[k3] = 1;   //反向检索第3层记录为1
					visgn1[visgnum1++] = k3;
				}
			}
		}
		vis1[k0] = 3;
		visgn1[visgnum1++] = k0;   //把起点也设为1层到达的节点，简化之后的判断
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
			vis1[visgn1[i]] = 0;         //通过记录的位置用于vis的数据进行删除
		}
		visgnum1 = 0;
	}
	res3p[i0] = res31 + res31len * 3;
	res4p[i0] = res41 + res41len * 4;
	res5p[i0] = res51 + res51len * 5;
	res6p[i0] = res61 + res61len * 6;
	res7p[i0] = res71 + res71len * 7;
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
```
### （3）转字符及输出部分
由于上述工作导致的结果数组乱序因此在转字符部分需要通过头结点访问此前resxp并间接访问存储的结果，一定程序上损失一点速度，不过相对均衡不在提升的速度而言不值一提。由于长度为7的环数量，远远大于长度为3-6的环，因此其转字符的工作量也远大于后者之和，因此此前一直采用3-6的环一个线程，7的环7个线程的分配方法，当后来意识到负载均衡的重要性后对7层环的线程数分配采用的工作量进行划分的方式，并将7层环的线程之间根据环的总数进行等分。以下为相关程序：
```cpp
void to_str1()
{
	anstr[0] = new char[res3len * 33 + res4len * 44 + res5len * 55 + res6len * 66];
	int countnum = res3len + res4len + res5len + res6len + res7len;   //总环数
	pos[0] = to_str0(countnum, anstr[0]);  
	for (int i = 1; i < newsetlen; i++)
	{
		pos[0] = to_str(res3p[i], (res3p[i + 4] - res3p[i]) / 3, 3, pos[0], anstr[0]);    //三层环转字符
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
	int mul = mu >= 2 ? mu - 1 : 1;   //为防止7层环过少
	int total = 0, i = 1, temp;
	gap[0] = 1;
	for (int j = 1; j < mul; j++)
	{
		temp = res7len / mul * j;     //总长mul等分用于转字符均衡负载
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
```
### （4）转字符及输出部分
到这里初赛复赛全部内容就结束了，但还有个东西不参与判题但大家应该也都会去写，也就是检测函数，难度不大，这里也简单介绍一下，具体方法与第一部分数据读取相似，将结果文本与答案文本的每个数字依次读取出来逐个对比，发现不同则输出错误数据，最终输出正确率。
相对来说不难，这里也就不做过多介绍了，直接贴出代码：
```cpp
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
		totalcheck++;
	}
	cout << right * 100.0 / totalcheck << '%' << endl;
	delete[] ans;
	delete[] sav;
}
```
对了再贴个笔者计时比较喜欢用的语句吧：
```cpp
cout << "DFS_time:" << double(clock()) / CLOCKS_PER_SEC << endl;
```
方便快捷，但是在多线程中会将多核的计时累加导致时间不准，因此本人还采用了gettimeofday()以及一个计时函数。
```cpp
template <class T>
void measure(T&& func) {
	using namespace std::chrono;
	auto start = system_clock::now();
	func();
	duration<double> diff = system_clock::now() - start;
	cout << "elapsed: " << diff.count() << "seconds" << endl;
}
```
使用方法：
```cpp
	measure([]() {
		checkcircle();
	});
```
## 3.参赛感受：
先讲场外吧，就聊聊个人体会比较深的几点：
1. 在心态要崩的时候多挺一挺，可能挺过了就是坦途。本人c++水平一般，很多都是边打比赛边学，热身赛单打当了200名守门员领了个娃娃。初赛有两个队友但因为一些原因前期基本单打，在连续几天的RE之后收获的第一个成绩喜提武长倒一。在即将退赛的边缘得到大佬的指点开始走上正轨跟大佬一起上分，附上笔者前期的成绩单。而复赛中前期也一直和前排大佬有着鸿沟级的差距，压抑了几天中与才能看到大佬们的尾气。
![排名](https://github.com/trybesthbk/HuaweiCodeCraft2020/blob/master/%E5%9B%BE%E7%89%87/20200519102708594.png)
2. 多水群，多认识一些大佬。大佬们总能在群里透露一些重要的信息，就比如本次知乎的开源baseline，测试数据集，还有各种上分技巧。多抱些大腿，不仅是比赛过程中重要的资源，也是专业发展过程中重要的资源。（说是这么说，但笔者也没抱上多少大腿啦hh）。  
3. 个人感觉对于像参赛的同学，不一定要在参赛前就找好队友抱大腿，尤其今年增加了热身赛制，在比赛过程中你就有机会结交到各路大佬，并且单打更能激发去认识大佬的热情。  

接着聊聊场内吧。  
由于本次比赛过程中榜单主要考验运行速度，因此需要尽一切可能削减各部分的运行速度，笔者对这方面没经验，吃了很多亏，给大家分享一下。
1. 不要嫌指针数组难用，它的运行速度快的很，而stl提供的各种数据结构，能不用就不用，笔者热身赛刚从python转c++的时候简直对数组操作的局限性深恶痛绝，用了vector，最后因为奇慢无比终于重新写起了数组。
2. C++里提供许多库函数，有些感觉很好用，不要被诱惑到了，自己手写的程序往往能运行更快，比较代表性就就是数字字符转化过程中的stringstream，getline，toa等等，不过有些像排序，查找，批量赋值还是可以用用sort，find，memset之类的，笔者之前尝试用快排，冒泡排序进行排序，比sort慢的多。
3. 之前一直没用好多线程总感觉速度一直没有什么提升，后来才知道多线程提速的关键在于均衡负载，因此两者的思考应该相互捆绑。此外变量冲突时往往原子变量atomic的损耗臂mutex要小。
4. 需要多次使用并且需通过索引计算等得到的变量应新建变量寄存，如下面代码中的l3，k4等。
```cpp
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
```
5.	当算法运行时间占大头时，前处理过程应极尽所能为其提供就方便读写的数据结构，能够有效提升速度。并且在多层循环中合理的处理判断分支语句能减少很多不必要的计算，有效提高效率。  
最后再聊一聊个人的一些小感受吧，前几天刚经历了复赛B榜事件，很多人也对此感到非常失望，毕竟从热身赛吐槽到复赛hh，相比而言个人可能并没那么遗憾吧，一个是本人主要借这次参赛机会学习到了很多东西，并且在复赛A榜的竞争中感觉相对大佬们完全力不从心，也深深感受到了知识浅薄，回去还是要好好学习hh。  
