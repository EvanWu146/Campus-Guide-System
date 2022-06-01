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
	printf("\n\t******��ӭʹ�����ү����ϵͳ******\n");
	Time_Refreshing(&curTime, 1, 0);
	for (printf("\n"), i = 0; i < 50; printf("="), i++);

	printf("\n������ѧУ�����н����Ｐ���Ӧ���:\n");
	for (int i = 0,j = 0; i < VERTEX_NUM; i++) {
		if (i < BUILDING_NUM || i >= BUILDING_NUM1) {
			printf("[%s%s%d] %-16s ", i<100?"0":"",i<10?"0":"",i, graphPtr->v[i].name);
			if ((++j + 1) % 3 == 0) printf("\n");
		}	
	}
	printf("\n\n�Լ�����ͼ���߼�λ���뽨����Ķ��ձ�\n");
	Logical_creat(logicFile);
	if (fun == 0) {//fun=0ʱ�û����ֶ��������λ��
		printf("\n���Ƿ���Ҫ���ϴα����˳��ĵط���Ϊ��ʼ����е���������1����Ҫ������0������Ҫ����������ѡ����ʼ��\n");
		scanf("%d", &ifContinue);
		if (ifContinue) {
			pSavedFile = fopen("PositionSaved.txt", "r");
			if (pSavedFile != NULL) {
				fscanf(pSavedFile, "%s %s %d %d %d %d", pos.direToVtx1, pos.direToVtx2, &pos.distToVtx1, &pos.distToVtx2, &pos.vtx1, &pos.vtx2);
				fclose(pSavedFile);
			}
			else {
				printf("\n�ϴ�δ���������ݣ���������㽨�����ţ�");
				scanf("%d", &dest);
			}
		}
		else {
			printf("\n��������㽨�����Ӧ�ı�ţ�");
			scanf("%d", &start);
			while (start >= BUILDING_NUM && start < BUILDING_NUM1) {
				printf("\n�����ﲻ���ڣ���������������ţ�");
				scanf("%d", &start);
			}
			pos.vtx1 = start; pos.vtx2 = start;
			memcpy(pos.direToVtx1, "", 1); memcpy(pos.direToVtx2, "", 1);
			pos.distToVtx1 = 0; pos.distToVtx2 = 0;
		}
	}
	
	printf("\n��������Ŀ���յ�����Ӧ�Ľ�������:");
	scanf_s("%d", &dest);
	while (dest >= BUILDING_NUM && dest < BUILDING_NUM1) {
		printf("\n�����ﲻ���ڣ������������յ��ţ�");
		scanf("%d", &dest);
	}
	printf("\n��ѡ�񵼺����ԣ�\n0-��̾������   1-���ʱ�����   2-;����̾������   3-�����г������ʱ�����\n");
	scanf_s("%d", &strategy);
	Time_Refreshing(&curTime, (endTimer = GetTickCount64() - initTimer), 2);
	fprintf(logF, "�û���ʼ����������ʼλ�ã�\n");
	Position_OutputToLog(graphPtr, pos);
	fprintf(logF, "��ʼ����,Ŀ�ĵ��ǣ�(%d��)%s\n", graphPtr->v[dest].num,graphPtr->v[dest].name);
	startTimer = initTimer;
	if (strategy == 0)
	{
		printf("==========��̾������==========\n\n");
		ShortestPathNgt(graphPtr, pos, dest, 0);
	}
	else if (strategy == 1)
	{
		printf("==========���ʱ�����==========\n\n");
		ShortestTimeNgt(graphPtr, pos, dest);
	}
	else if (strategy == 2)
	{
		printf("==========;����̾������==========\n\n");
		ShortestPathNgt_passBy(graphPtr, pos, dest);
	}
	else if (strategy == 3)
	{
		printf("==========�����г������ʱ�����==========\n\n");
		ShortestTimeNgt_Bike(graphPtr, pos, dest);
	}
	printf("����Ŀ�ĵأ�����������2s���Զ��˳�����ϵͳ��\n\n");
	Sleep(2000);
	fclose(logF);
	system("pause");
	exit(0);
}
void ShortestPathNgt(Graph* graphPtr, Position pos, int dest, int func) // ��ɵ������ܡ�����posΪ�û�λ����Ϣ��destΪĿ�ĵر��
{
	int* path1, * path2, * shortestPath;
	int revisedPath[VERTEX_NUM] = {0};
	int length1, length2, start, m, n, i;

	path1 = Dijkstra(graphPtr, pos.vtx1, dest, DISTANCE_MIN);
	if (pos.vtx1 != pos.vtx2) {
		path2 = Dijkstra(graphPtr, pos.vtx2, dest, DISTANCE_MIN);
	
		length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, dest, DISTANCE_MIN) + pos.distToVtx1;
		length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, dest, DISTANCE_MIN) + pos.distToVtx2;
		// �û�ѡ��ߵ������˵�֮һ��Ϊ��ʼ�㣬�Ƚ��ĸ��˵���Ϊ��ʼ�������·������
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
	else if (func == 1) {//�����û���;��ѯ·�߲�һ�������
		GuideWay(graphPtr, revisedPath, pos, start, dest, 1);
	}
}

void ShortestTimeNgt(Graph* graphPtr, Position pos, int dest) 
// ��ɵ������ܡ�����posΪ�û�λ����Ϣ��destΪĿ�ĵر��
{//funcΪ0����ʱ���������·�ߣ�Ϊ1��һ�������·��
	int* path1, * path2, * shortestPath;
	int revisedPath[VERTEX_NUM];
	int length1, length2, start, m, n;
	path1 = Dijkstra(graphPtr, pos.vtx1, dest, TIME_MIN);
	
	if (pos.vtx1 != pos.vtx2) {
		path2 = Dijkstra(graphPtr, pos.vtx2, dest, TIME_MIN);
		length1 = CalculatePathLength(graphPtr, path1, pos.vtx1, dest, TIME_MIN) + pos.distToVtx1 * (2520 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		length2 = CalculatePathLength(graphPtr, path2, pos.vtx2, dest, TIME_MIN) + pos.distToVtx2 * (2520 / graphPtr->e[pos.vtx1][pos.vtx2]->crowding_degree);
		// �û�ѡ��ߵ������˵�֮һ��Ϊ��ʼ�㣬�Ƚ��ĸ��˵���Ϊ��ʼ�������·������
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

void ShortestPathNgt_passBy(Graph* graphPtr, Position pos, int dest) // ��ɵ������ܡ�����posΪ�û�λ����Ϣ��destΪĿ�ĵر��
{
	int i,pointNum = 0, *passby;
	printf("����������Ҫ;���ĵص���Ŀ��\n");
	scanf_s("%d", &pointNum);
	passby = malloc(sizeof(int)*(pointNum+1));
	printf("�밴˳���������ǵı�ţ�\n");
	for (i = 0; i < pointNum; i++) {
		scanf("%d", &passby[i]);
	}
	passby[i] = dest;
	ShortestPathNgt(graphPtr, pos, passby[0], 0);
	for (i = 1; i <= pointNum; i++) {
		ShortestPathNgt(graphPtr, ePos, passby[i],0);
	}
}

void ShortestTimeNgt_Bike(Graph* graphPtr, Position pos, int dest) // ��ɵ������ܡ�����posΪ�û�λ����Ϣ��destΪĿ�ĵر��
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
		// �û�ѡ��ߵ������˵�֮һ��Ϊ��ʼ�㣬�Ƚ��ĸ��˵���Ϊ��ʼ�������·������
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
