#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include"HeadFile.h"
extern DWORD initTimer, startTimer, endTimer;
extern virtualTime curTime;
extern FILE* logF;
Position ePos;
void beginGuideSystem(Graph* graphPtr, Position pos, int fun) {
	int strategy = 0, start = 0 , dest = 0, i, ifContinue = 0;
	Logic logicFile[22];
	FILE* pSavedFile = NULL;

	for (printf("\n\n"), i = 0; i < 50; printf("="), i++);
	printf("\n\t******欢迎使用马大爷导航系统******\n");
	Time_Refreshing(&curTime, 1, 0);
	for (printf("\n"), i = 0; i < 50; printf("="), i++);

	printf("\n以下是学校内所有建筑物及其对应编号:\n");
	for (int i = 0,j = 0; i < VERTEX_NUM; i++) {
		if (i < BUILDING_NUM || i >= BUILDING_NUM1) {
			printf("[%s%s%d] %-16s ", i<100?"0":"",i<10?"0":"",i, graphPtr->v[i].name);
			if ((++j + 1) % 3 == 0) printf("\n");
		}	
	}
	printf("\n\n以及如下图的逻辑位置与建筑物的对照表：\n");
	Logical_creat(logicFile);
	if (fun == 0) {//fun=0时用户需手动输入起点位置
		printf("\n您是否需要从上次保存退出的地方作为起始点进行导航？输入1：需要；输入0：不需要，我想重新选择起始点\n");
		scanf("%d", &ifContinue);
		if (ifContinue) {
			pSavedFile = fopen("PositionSaved.txt", "r");
			if (pSavedFile != NULL) {
				fscanf(pSavedFile, "%s %s %d %d %d %d", pos.direToVtx1, pos.direToVtx2, &pos.distToVtx1, &pos.distToVtx2, &pos.vtx1, &pos.vtx2);
				fclose(pSavedFile);
			}
			else {
				printf("\n上次未保存有数据，请输入起点建筑物编号：");
				scanf("%d", &dest);
			}
		}
		else {
			printf("\n请输入起点建筑物对应的编号：");
			scanf("%d", &start);
			while (start >= BUILDING_NUM && start < BUILDING_NUM1) {
				printf("\n建筑物不存在，请重新输入起点编号：");
				scanf("%d", &start);
			}
			pos.vtx1 = start; pos.vtx2 = start;
			memcpy(pos.direToVtx1, "", 1); memcpy(pos.direToVtx2, "", 1);
			pos.distToVtx1 = 0; pos.distToVtx2 = 0;
		}
	}
	
	printf("\n请输入您目标终点所对应的建筑物编号:");
	scanf_s("%d", &dest);
	while (dest >= BUILDING_NUM && dest < BUILDING_NUM1) {
		printf("\n建筑物不存在，请重新输入终点编号：");
		scanf("%d", &dest);
	}
	printf("\n请选择导航策略：\n0-最短距离策略   1-最短时间策略   2-途径最短距离策略   3-骑自行车的最短时间策略\n");
	scanf_s("%d", &strategy);
	Time_Refreshing(&curTime, (endTimer = GetTickCount64() - initTimer), 2);
	fprintf(logF, "用户开始导航，从起始位置：\n");
	Position_OutputToLog(graphPtr, pos);
	fprintf(logF, "开始导航,目的地是：(%d号)%s\n", graphPtr->v[dest].num,graphPtr->v[dest].name);
	startTimer = initTimer;
	if (strategy == 0)
	{
		printf("==========最短距离策略==========\n\n");
		ShortestPathNgt(graphPtr, pos, dest, 0);
	}
	else if (strategy == 1)
	{
		printf("==========最短时间策略==========\n\n");
		ShortestTimeNgt(graphPtr, pos, dest);
	}
	else if (strategy == 2)
	{
		printf("==========途径最短距离策略==========\n\n");
		ShortestPathNgt_passBy(graphPtr, pos, dest);
	}
	else if (strategy == 3)
	{
		printf("==========骑自行车的最短时间策略==========\n\n");
		ShortestTimeNgt_Bike(graphPtr, pos, dest);
	}
	printf("到达目的地，导航结束，2s后自动退出导航系统；\n\n");
	Sleep(2000);
	fclose(logF);
	system("pause");
	exit(0);
}
void ShortestPathNgt(Graph* graphPtr, Position pos, int dest, int func) // 完成导航功能。其中pos为用户位置信息，dest为目的地编号
{
	int* path1, * path2, * shortestPath;
	int revisedPath[VERTEX_NUM] = {0};
	int length1, length2, start, m, n, i;

	path1 = Dijkstra(graphPtr, pos.vtx1, dest, DISTANCE_MIN);
	if (pos.vtx1 != pos.vtx2) {
		path2 = Dijkstra(graphPtr, pos.vtx2, dest, DISTANCE_MIN);
	
		length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, dest, DISTANCE_MIN) + pos.distToVtx1;
		length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, dest, DISTANCE_MIN) + pos.distToVtx2;
		// 用户选择边的两个端点之一作为起始点，比较哪个端点作为起始点算出的路径更优
		if (length1 <= length2)
		{
			shortestPath = path1;
			start = pos.vtx1;
		}
		else
		{
			shortestPath = path2;
			start = pos.vtx2;
		}
	}
	else {
		shortestPath = path1;
		start = pos.vtx1;
	}
	m = dest;
	while (m != start)
	{
		n = shortestPath[m];
		revisedPath[n] = m;
		m = n;
	}
	if (func == 0) {
		GuideWay_InOrder(graphPtr, revisedPath, pos, start, dest, 0);
	}
	else if (func == 1) {//计算用户中途查询路线并一次性输出
		GuideWay(graphPtr, revisedPath, pos, start, dest, 1);
	}
}

void ShortestTimeNgt(Graph* graphPtr, Position pos, int dest) 
// 完成导航功能。其中pos为用户位置信息，dest为目的地编号
{//func为0，则按时序输出导航路线；为1则一次性输出路线
	int* path1, * path2, * shortestPath;
	int revisedPath[VERTEX_NUM];
	int length1, length2, start, m, n;
	path1 = Dijkstra(graphPtr, pos.vtx1, dest, TIME_MIN);
	
	if (pos.vtx1 != pos.vtx2) {
		path2 = Dijkstra(graphPtr, pos.vtx2, dest, TIME_MIN);
		length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, dest, TIME_MIN) + pos.distToVtx1 * (2520 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, dest, TIME_MIN) + pos.distToVtx2 * (2520 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		// 用户选择边的两个端点之一作为起始点，比较哪个端点作为起始点算出的路径更优
		if (length1 <= length2)
		{
			shortestPath = path1;
			start = pos.vtx1;
		}
		else
		{
			shortestPath = path2;
			start = pos.vtx2;
		}
	}
	else {
		shortestPath = path1;
		start = pos.vtx1;
	}
	
	m = dest;
	while (m != start)
	{
		n = shortestPath[m];
		revisedPath[n] = m;
		m = n;
	}
	GuideWay_InOrder(graphPtr, revisedPath, pos, start, dest, 1);
	
}

void ShortestPathNgt_passBy(Graph* graphPtr, Position pos, int dest) // 完成导航功能。其中pos为用户位置信息，dest为目的地编号
{
	int i,pointNum = 0, *passby;
	printf("请输入你需要途经的地点数目：\n");
	scanf_s("%d", &pointNum);
	passby = malloc(sizeof(int)*(pointNum+1));
	printf("请按顺序输入它们的编号：\n");
	for (i = 0; i < pointNum; i++) {
		scanf("%d", &passby[i]);
	}
	passby[i] = dest;
	ShortestPathNgt(graphPtr, pos, passby[0], 0);
	for (i = 1; i <= pointNum; i++) {
		ShortestPathNgt(graphPtr, ePos, passby[i],0);
	}
}

void ShortestTimeNgt_Bike(Graph* graphPtr, Position pos, int dest) // 完成导航功能。其中pos为用户位置信息，dest为目的地编号
{
	int* path1, * path2, * shortestPath;
	int revisedPath[VERTEX_NUM];
	int length1, length2, start, m, n;
	path1 = Dijkstra(graphPtr, pos.vtx1, dest, BICYCLE);
	path2 = Dijkstra(graphPtr, pos.vtx2, dest, BICYCLE);
	if (pos.vtx2 == pos.vtx1) {
		shortestPath = path1;
		start = pos.vtx1;
	}
	else {
		if (graphPtr->e[pos.vtx1][pos.vtx2]->bicycleAllow == 1)
			length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, dest, BICYCLE) + pos.distToVtx1 * (2520 / 2 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		else
			length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, dest, BICYCLE) + pos.distToVtx1 * (2520 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		if (graphPtr->e[pos.vtx1][pos.vtx2]->bicycleAllow == 1)
			length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, dest, BICYCLE) + pos.distToVtx2 * (2520 / 2 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		else
			length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, dest, BICYCLE) + pos.distToVtx2 * (2520 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		// 用户选择边的两个端点之一作为起始点，比较哪个端点作为起始点算出的路径更优
		if (length1 <= length2)
		{
			shortestPath = path1;
			start = pos.vtx1;
		}
		else
		{
			shortestPath = path2;
			start = pos.vtx2;
		}
	}
	
	m = dest;
	while (m != start)
	{
		n = shortestPath[m];
		revisedPath[n] = m;
		m = n;
	}
	GuideWay_InOrder(graphPtr, revisedPath, pos, start, dest, 2);
}
