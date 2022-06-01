#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include"HeadFile.h"
DWORD initTimer, startTimer, endTimer;//程序计时器变量
virtualTime curTime = { 0,0,0,0,0 };
FILE* logF = NULL;
int main(void)
{
	setTime(&curTime);
	logF = fopen("logFile.txt", "a+");
	Position pos = {0,0,"","",0,0};
	Graph graph;
	graph_creat(&graph);
	initTimer = GetTickCount64();
	beginGuideSystem(&graph, pos, 0);
}