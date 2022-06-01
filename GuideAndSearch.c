#include"HeadFile.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include<process.h>
#include<math.h>
extern DWORD initTimer, startTimer, endTimer;//程序计时器变量
HANDLE hMutex;
Graph eGraph;
extern Position ePos;
int endFlag, whichCampus;//whichCampus代表现在用户正在哪个校区，=1为主校区，=2为新校区
extern virtualTime curTime;
extern FILE* logF;


unsigned _stdcall run(void* pArguments) {
	char inputc = '\0';
	int func = 0;
	FILE* positionSavedFile = NULL;
	while (endFlag) {
		if (inputc = getch()) {
			WaitForSingleObject(hMutex, INFINITE);
			printf("当前导航已暂停：请输入下一步指令：\n[0]输入0则继续导航；\n[1]输入1查询当前位置；\n[2]输入2查询周边教学楼/宿舍/其他服务设施；\n[3]输入3更换导航地点；\n[4]输入4则保存当前位置信息，安全退出导航系统导航\n");
			scanf_s("%d", &func);
			if (func == 0) {
				system("pause");
			}
			else if (func == 1) {
				Position_toString(&eGraph, ePos);
				system("pause");
			}
			else if (func == 2) {
				SearchAround(&eGraph, ePos);
				system("pause");
			}
			else if (func == 3) {
				beginGuideSystem(&eGraph, ePos, 1);
			}
			else if (func == 4) {
				positionSavedFile = fopen("PositionSaved.txt", "w+");
				if (positionSavedFile != NULL) {
					fprintf(positionSavedFile, "%s %s %d %d %d %d", ePos.direToVtx1, ePos.direToVtx2, ePos.distToVtx1, ePos.distToVtx2, ePos.vtx1, ePos.vtx2);
					fclose(positionSavedFile);
				}
				else {
					printf("出错了，位置不能正常保存！");
					system("pause");
				}
				exit(0);
			}
			else {
				system("pause");
			}
			inputc = '\0';
			ReleaseMutex(hMutex);
		}
	}
	_endthreadex(0);
	return;
}
void GuideWay_InOrder(Graph* graphPtr, int path[VERTEX_NUM], Position pos, int start, int dest, int fun) 
{//fun为0是步行导航，fun为1是最短距离导航（考虑拥挤度），fun为2是骑行导航
	HANDLE hThread1;
	unsigned ThreadID1 = 1;
	th Th1 = { 1,1};
	hMutex = CreateMutex(NULL, FALSE, NULL);
	hThread1 = (HANDLE)_beginthreadex(NULL, 0, run, (void*)&Th1, 0, &ThreadID1);
	endFlag = 1;

	int m = 400, n = 0;
	int curPathFlag = 0, stateOfUser = 1, bAFlag = 1;
	//stateOfUser表示用户的状态，为0为在校园内道路上，为1为起始状态，为2表示在两个校区间（公交车或者校车）
	double journeyOfMin = 0.0, ratio = 1.0;
	
	Position_toString(graphPtr, pos);
	printf("Tips：您可以在导航的过程中按任意键以暂停导航；\n");
	eGraph = *graphPtr;
	Time_Refreshing(&curTime, ((endTimer = GetTickCount64()) - startTimer), 1);

	if (fun == 0 || fun == 1) {//将路线结果输入log文件中
		GuideWay(graphPtr, path, pos, start, dest, 0);
	}
	else GuideWay_bicycle(graphPtr, path, pos, start, dest);
	
	startTimer = GetTickCount64();
	while (m != dest) {//每分钟作为一次循环，计算用户在某一分钟内的导航路线，对应现实五秒钟
		WaitForSingleObject(hMutex, INFINITE);
		journeyOfMin = (fun == 2) ? 200 : 100;

		while (journeyOfMin != 0 && m != dest) {//每次循环遍历一条路（可能走完也可能没有走完），一共遍历n条，n条路长度和为journeyOfMin
			if (stateOfUser == 1) {//开始状态，准备从用户起始位置走到出发点
				if (pos.vtx1 == pos.vtx2) {//表示用户正处于某一个建筑物旁边
					m = start; n = path[m];
					Position_Changing(graphPtr, &pos, start, n, 1);
					stateOfUser = 0;
				}
				else {//用户正处于某条道路上
					if (fun == 1) {
						ratio = 10.0 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree;
						//printf("[%d,%lf]", graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree,ratio);
					}
					else if (fun == 2) {
						bAFlag = !graphPtr->e[pos.vtx1][pos.vtx2]->bicycleAllow;
						ratio = bAFlag ? 2.0 : 1.0;
					}//bAFlag为1时只允许步行，为0时允许骑行
				
					if (start == pos.vtx2) {
						Position_Changing(graphPtr, &pos, 0, 0, 0);
					}//将起点设至pos的vtx1处
					if (ratio * pos.distToVtx1 <= journeyOfMin) {//能在journeyOfMin距离内走到起始点
						printf("向%s%s %d 米到达", pos.direToVtx1,bAFlag?"步行":"骑行" , pos.distToVtx1);
						if (pos.vtx1 < BUILDING_NUM || n >= BUILDING_NUM1)		printf(" %s；", graphPtr->v[pos.vtx1].name);
						else		printf(" %d号路口；", pos.vtx1);
						m = start;
						n = path[m];
						journeyOfMin -= pos.distToVtx1 * ratio;
						Position_Changing(graphPtr, &pos, m, n, 1);//将用户的起始位置设置为起始点
						stateOfUser = 0;
					}
					else {//不能，则先走完journeyOfMin剩下的路程
						printf("向%s%s %.0lf 米；", pos.direToVtx1, bAFlag ? "步行" : "骑行", journeyOfMin / ratio);
						pos.distToVtx1 -= journeyOfMin / ratio;
						pos.distToVtx2 += journeyOfMin / ratio;
						journeyOfMin = 0.0;
					}
				}
				
			}
			else if (stateOfUser == 2) {
				//用户需要到达另外一个校区，公交车每15分组有一次，路程一小时；校内班车每一小时有一次，路程40分钟	
				if (curTime.min > 0 && curTime.min <= 40) {//此时选择坐公交车
					printf("请您耐心在校门旁等候公交车，预计需要等待%ld分钟；\n", 15 - curTime.min%15);
					printf("请您上13路公交车，预计需要乘坐1小时；\n");
					Position_Changing(graphPtr, &pos, 0, 0, 3);
					Time_Refreshing(&curTime, 60 * 5000 + ((15 - curTime.min % 15) * 5000), 3);
					printf("您已到达校区%d；\n", pos.vtx1 == 0 ? 1:2);
				}
				else {
					printf("请您耐心在校门旁等候校内班车，预计需要等待%ld分钟；\n", 60-curTime.min);
					printf("请您上校车，预计需要乘坐40分钟；\n");
					Position_Changing(graphPtr, &pos, 0, 0, 3);
					Time_Refreshing(&curTime, 40 * 5000 + ((60 - curTime.min) * 5000), 3);
					printf("您已到达校区%d；\n", pos.vtx1 == 0 ? 1 : 2);
				}
				m = n;
				n = path[m];
				stateOfUser = 0;
			}
			else {//在校园内的某条道路上
				if (fun == 1) {
					ratio = 10.0 / graphPtr->e[m][n]->crowding_degree;
					//printf("[%d,%lf]", graphPtr->e[m][n]->crowding_degree, ratio);
				}
				else if (fun == 2) {
					bAFlag = !graphPtr->e[m][n]->bicycleAllow;
					ratio = bAFlag ? 2.0 : 1.0;
				}
				if (curPathFlag) {//表示刚刚那条路还没走完，现在正在某条道路中间
					if (ratio * pos.distToVtx2 <= journeyOfMin) {//能在journeyOfMin距离内走到vtx2
						printf("向%s%s %d 米到达", graphPtr->e[m][n]->direction, bAFlag ? "步行" : "骑行",pos.distToVtx2);
						if (n < BUILDING_NUM || n >= BUILDING_NUM1)		printf(" %s;", graphPtr->v[n].name);
						else	printf(" %d号路口；", n);
						journeyOfMin -= pos.distToVtx2 * ratio;
						if (n != dest) {//没有到达终点，则将位置变更到下一条道路上
							m = n;
							n = path[m];
							Position_Changing(graphPtr, &pos, m, n, 1);
						}
						else {
							Position_Changing(graphPtr, &pos, m, n, 2);
							m = n;
						}
						curPathFlag = 0;
					}
					else {//走不到vtx2则先走完journeyOfMin的路程
						printf("向%s%s %.0lf 米；", graphPtr->e[m][n]->direction, bAFlag ? "步行" : "骑行", journeyOfMin / ratio);
						pos.distToVtx1 += journeyOfMin / ratio;
						pos.distToVtx2 -= journeyOfMin / ratio;
						journeyOfMin = 0.0;
					}
				}
				else {//表示刚刚那条路走完了，现在用户正处于某个路口/建筑物
					if (ratio * graphPtr->e[m][n]->length <=  journeyOfMin) {//能在journeyOfMin距离内从vtx1走到路尾，则使用户走到路尾的路口/建筑物
						printf("向%s%s %d 米到达", graphPtr->e[m][n]->direction, bAFlag ? "步行" : "骑行",graphPtr->e[m][n]->length);
						if (n < BUILDING_NUM || n >= BUILDING_NUM1)		printf(" %s;", graphPtr->v[n].name);
						else		printf(" %d号路口；", n);
						journeyOfMin -= graphPtr->e[m][n]->length * ratio;
						if (n != dest) {//没有到达终点，则将位置变更到下一条道路上
							m = n;
							n = path[m];
							Position_Changing(graphPtr, &pos, m, n, 1);
						}
						else {
							Position_Changing(graphPtr, &pos, m, n, 2);
							m = n;
						}
					}
					else {//走不到路尾则先走完journeyOfMin的路程
						printf("向%s%s %.0lf 米；", graphPtr->e[m][n]->direction, bAFlag ? "步行" : "骑行", journeyOfMin / ratio);
						pos.distToVtx1 += journeyOfMin / ratio;
						pos.distToVtx2 -= journeyOfMin / ratio;
						journeyOfMin = 0.0;
						curPathFlag = 1;//标记当前路径未走完
					}
				}
				if ((m == 0 && n == 143) || (n == 0 && m == 143)) stateOfUser = 2;//路径0-143表示用户正准备跨校区
			}
		}
		
		ePos = pos;
		endTimer = GetTickCount64();
		Time_Refreshing(&curTime, 0, 2);
		Position_OutputToLog(graphPtr, pos);
		printf("\n");
		if(stateOfUser == 0 || stateOfUser == 1)
			Sleep(5000);//校区内导航需要系统模拟等待时间
		Time_Refreshing(&curTime,((endTimer = GetTickCount64()) - startTimer), 1);
		startTimer = GetTickCount64();
		ReleaseMutex(hMutex);
	}
	CloseHandle(hThread1);
	endFlag = 0;
	return;
}
void GuideWay(Graph * graphPtr, int path[VERTEX_NUM], Position pos, int start, int dest, int fun) //指路，即打印路径信息
{
	int m, n;
	//将查询结果一次性输出到log文件中，仅在fun为0时生效
	fprintf(logF, "\n路线为：\n");
	if (fun == 0) {
		if (pos.vtx1 != pos.vtx2) {
			if (start == pos.vtx1)
			{
				fprintf(logF, "向%s步行 %d 米到达", pos.direToVtx1, pos.distToVtx1);
				if (pos.vtx1 < BUILDING_NUM || pos.vtx1 >= BUILDING_NUM1)
					fprintf(logF, " %s\n", graphPtr->v[pos.vtx1].name);
				else
					fprintf(logF, " %d号路口\n", pos.vtx1);
			}
			else if (start == pos.vtx2)
			{
				fprintf(logF, "向%s步行 %d 米到达", pos.direToVtx2, pos.distToVtx2);
				if (pos.vtx2 < BUILDING_NUM || pos.vtx2 >= BUILDING_NUM1)
					fprintf(logF, " %s\n", graphPtr->v[pos.vtx2].name);
				else
					fprintf(logF, " %d号路口\n", pos.vtx2);
			}
		}
		m = start;
		while (m != dest)
		{
			n = path[m];
			if ((m == 0 && n == 143) || (m == 143 && n == 0)) {
				fprintf(logF, "用户从%s校区乘坐公交车/校内班车,到达%s校区", m == 0 ? "主" : "新", m == 143 ? "主" : "新");
			}
			else {
				fprintf(logF, "向%s步行 %d 米到达", graphPtr->e[m][n]->direction, graphPtr->e[m][n]->length);
				if (n < BUILDING_NUM || n >= BUILDING_NUM1)
					fprintf(logF, " %s\n", graphPtr->v[n].name);
				else
					fprintf(logF, " %d号路口\n", n);
			}
			m = n;
		}
	}
	//将查询结果一次性输入到stdout上，仅在fun为1时生效
	if (fun == 1) {
		if (pos.vtx1 != pos.vtx2) {
			if (start == pos.vtx1)
			{
				printf("向%s步行 %d 米到达", pos.direToVtx1, pos.distToVtx1);
				if (pos.vtx1 < BUILDING_NUM || pos.vtx1 >= BUILDING_NUM1)
					printf(" %s\n", graphPtr->v[pos.vtx1].name);
				else
					printf(" %d号路口\n", pos.vtx1);
			}
			else if (start == pos.vtx2)
			{
				printf("向%s步行 %d 米到达", pos.direToVtx2, pos.distToVtx2);
				if (pos.vtx2 < BUILDING_NUM || pos.vtx2 >= BUILDING_NUM1)
					printf(" %s\n", graphPtr->v[pos.vtx2].name);
				else
					printf(" %d号路口\n", pos.vtx2);
			}
		}
		m = start;
		while (m != dest)
		{
			n = path[m];
			if ((m == 0 && n == 143)|| (m == 143 && n == 0)) {
				printf("用户从%s校区乘坐公交车/校内班车,到达%s校区", m == 0 ? "主" : "新", m == 143 ? "主" : "新");
			}
			else {
				printf("向%s步行 %d 米到达", graphPtr->e[m][n]->direction, graphPtr->e[m][n]->length);
				if (n < BUILDING_NUM || n >= BUILDING_NUM1)
					printf(" %s\n", graphPtr->v[n].name);
				else
					printf(" %d号路口\n", n);
			}
			m = n;
		}	
	}
}

void GuideWay_bicycle(Graph* graphPtr, int path[VERTEX_NUM], Position pos, int start, int dest) 
//将自行车导航路线输出至log文件中
{
	int m, n;
	fprintf(logF, "路线为：\n");
	if (pos.vtx1 != pos.vtx2) {
		if (start == pos.vtx1)
		{
			if (graphPtr->e[pos.vtx1][pos.vtx2]->bicycleAllow == 0)
				fprintf(logF, "向%s步行 %d 米到达", pos.direToVtx1, pos.distToVtx1);
			else
				fprintf(logF, "向%s骑行 %d 米到达", pos.direToVtx1, pos.distToVtx1);
			if (pos.vtx1 < BUILDING_NUM || pos.vtx1 >= BUILDING_NUM1)
				fprintf(logF, " %s\n", graphPtr->v[pos.vtx1].name);
			else
				fprintf(logF, " %d号路口\n", pos.vtx1);
		}
		else if (start == pos.vtx2)
		{
			if (graphPtr->e[pos.vtx1][pos.vtx2]->bicycleAllow == 0)
				fprintf(logF, "向%s步行 %d 米到达", pos.direToVtx2, pos.distToVtx2);
			else
				fprintf(logF, "向%s骑行 %d 米到达", pos.direToVtx2, pos.distToVtx2);
			if (pos.vtx2 < BUILDING_NUM || pos.vtx2 >= BUILDING_NUM1)
				fprintf(logF, " %s\n", graphPtr->v[pos.vtx2].name);
			else
				fprintf(logF, " %d号路口\n", pos.vtx2);
		}
	}
	m = start;
	while (m != dest)
	{
		n = path[m];
		if ((m == 0 && n == 143) || (m == 143 && n == 0)) {
			fprintf(logF, "用户从%s校区乘坐公交车/校内班车,到达%s校区", m == 0 ? "主" : "新", m == 143 ? "主" : "新");
		}
		else {
			if (graphPtr->e[m][n]->bicycleAllow == 0)
				fprintf(logF, "向%s步行 %d 米到达", graphPtr->e[m][n]->direction, graphPtr->e[m][n]->length);
			else
				fprintf(logF, "向%s骑行 %d 米到达", graphPtr->e[m][n]->direction, graphPtr->e[m][n]->length);
			if (n < BUILDING_NUM || n >= BUILDING_NUM1)
				fprintf(logF, " %s\n", graphPtr->v[n].name);
			else
				fprintf(logF, " %d号路口\n", n);
		}
		
		m = n;
	}
}


void SearchAround(Graph* graphPtr, Position pos)
{//搜寻周围服务设施
	int i, dest;
	int* path1, * path2, * path;
	int length1, length2;
	int hasFacilities = 0;
	int end;
	if (pos.vtx1 <= 72 && pos.vtx2 <= 72) {
		i = 0;
		end = BUILDING_NUM;
	}
	else {
		i = BUILDING_NUM1;
		end = VERTEX_NUM;
	}
	for (; i < end; i++)
	{
		if ((i < BUILDING_NUM || i > BUILDING_NUM1) && graphPtr->v[i].flag == 1)
		{
			path1 = Dijkstra(graphPtr, pos.vtx1, graphPtr->v[i].num, DISTANCE_MIN);
			path2 = Dijkstra(graphPtr, pos.vtx2, graphPtr->v[i].num, DISTANCE_MIN);
			length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, graphPtr->v[i].num, DISTANCE_MIN) + pos.distToVtx1;
			length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, graphPtr->v[i].num, DISTANCE_MIN) + pos.distToVtx2;
			if (length1 <= 500 || length2 <= 500)
			{
				if (length1 < length2)
				{
					printf("%s 编号:%d 距离:%dm\n", graphPtr->v[i].name, graphPtr->v[i].num, length1);
					hasFacilities = 1;
				}
				else
				{
					printf("%s 编号:%d 距离:%dm\n", graphPtr->v[i].name, graphPtr->v[i].num, length2);
					hasFacilities = 1;
				}
			}
		}
	}
	if (!hasFacilities) {
		printf("很抱歉，您的周围没有教学楼、学生宿舍和其他服务设施。\n");
	}
	else {
		printf("请输入选择的建筑的的编号，若无则输入-1 系统继续进行中的导航\n");
		scanf_s("%d", &dest);
		if (dest != -1)
		{
			printf("最短路径如下：\n");
			ShortestPathNgt(graphPtr, pos, dest, 1);
		}
	}
	
}