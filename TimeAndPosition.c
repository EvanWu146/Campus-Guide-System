#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include"HeadFile.h"
extern virtualTime curTime;
extern FILE* logF;
void setTime(virtualTime* t) {
	time_t rawtime;
	struct tm* timeInfo;
	time(&rawtime);
	timeInfo = localtime(&rawtime);
	t->year = timeInfo->tm_year + 1900;
	t->month = timeInfo->tm_mon + 1;
	t->day = timeInfo->tm_mday;
	t->hour = timeInfo->tm_hour;
	t->min = timeInfo->tm_min;
}
void Time_Refreshing(virtualTime* t, DWORD ms, int fun) {
	unsigned long mins = ms / 5000;
	t->min += mins;
	if (t->min >= 60) {
		t->hour += (t->min / 60);
		t->min %= 60;
	}
	if (t->hour >= 24) {
		t->day += (t->hour / 24);
		t->hour %= 24;
	}
	if (fun == 1) {
		printf("\n[Current time: %s%0ld: %s%0ld]", t->hour<10?"0":"", t->hour, t->min<10?"0":"", t->min);
	}
	else if (fun == 0) {
		printf("\n[您进入系统的时间: %ld年 %s%ld月 %s%ld日 %s%ld: %s%ld]", t->year, t->month<10?"0":"" ,t->month, t->day<10?"0":"", t->day, t->hour<10?"0":"",t->hour, t->min<10?"0":"", t->min);
	}
	else if (fun == 2) {
		fprintf(logF, "\n[时间: %ld年 %s%ld月 %s%ld日 %s%0ld: %s%0ld]", t->year, t->month < 10 ? "0" : "", t->month, t->day < 10 ? "0" : "", t->day, t->hour < 10 ? "0" : "", t->hour, t->min < 10 ? "0" : "", t->min);
	}
	else if (fun == 3) {//fun为3时直接返回
		return;
	}
	
}
void Position_toString(Graph* g, Position p) {
	if (p.distToVtx1 == 0||p.distToVtx2 == 0) {
		if (p.distToVtx1 == 0) {
			printf("\n您正处于(%d号)%s%s旁边。\n", p.vtx1, g->v[p.vtx1].name, !g->v[p.vtx1].flag?"路口":"");
		}
		else if (p.distToVtx2 == 0) {
			printf("\n您正处于(%d号)%s%s旁边。\n", p.vtx2, g->v[p.vtx2].name, !g->v[p.vtx2].flag ? "路口" : "");
		}
	}
	else {
		if (g->v[p.vtx1].flag == 0) {
			printf("\n您现在的位置：向%s距离%d号路口%d米，", p.direToVtx1, p.vtx1, p.distToVtx1);
		}
		else {
			printf("\n您现在的位置：向%s距离(%d号)%s%d米，", p.direToVtx1, p.vtx1, g->v[p.vtx1].name, p.distToVtx1);
		}
		if (g->v[p.vtx2].flag == 0) {
			printf("向%s距离%d号路口%d米。\n", p.direToVtx2, p.vtx2, p.distToVtx2);
		}
		else {
			printf("向%s距离(%d号)%s%d米。\n", p.direToVtx2, p.vtx2, g->v[p.vtx2].name, p.distToVtx2);
		}
	}
	
}

void Position_Changing(Graph* g, Position* p, int mVtx, int nVtx, int func) {
	if (func == 0) {//仅交换起始点
		char temStr[3];
		int temDist;
		int temVtx;
		temVtx = p->vtx1;
		p->vtx1 = p->vtx2;
		p->vtx2 = temVtx;
		temDist = p->distToVtx1;
		p->distToVtx1 = p->distToVtx2;
		p->distToVtx2 = temDist;
		memcpy(temStr, p->direToVtx1, 3);
		memcpy(p->direToVtx1, p->direToVtx2, 3);
		memcpy(p->direToVtx2, temStr, 3);
	}
	else if (func == 1) {//更改位置（此时到达了导航路径上某一结点，需要将位置描述变更到下一条道路）
		p->vtx1 = mVtx;
		p->vtx2 = nVtx;
		p->distToVtx1 = 0;
		p->distToVtx2 = g->e[mVtx][nVtx]->length;
		memcpy(p->direToVtx2, g->e[mVtx][nVtx]->direction, 3);
		memcpy(p->direToVtx1, g->e[nVtx][mVtx]->direction, 3);
	}
	else if (func == 2) {//更改位置（此时到达导航终点vtx2）
		p->distToVtx1 = g->e[mVtx][nVtx]->length;
		p->distToVtx2 = 0;
	}
	else if (func == 3) {//从校区1抵达校区2，或从校区2抵达校区1
		if (p->vtx1 == 0) {//从校区1出发，路径0-143，抵达校区2的143-98边
			p->vtx1 = 143;
			p->vtx2 = 98;
		}
		else if (p->vtx1 == 143) {//从校区2出发，路径143-0，抵达校区1的0-35边
			p->vtx1 = 0;
			p->vtx2 = 35;
		}
		p->distToVtx1 = 0;
		p->distToVtx2 = 200;
		memcpy(p->direToVtx1, "南", 3);
		memcpy(p->direToVtx2, "北", 3);
	}
}

void Position_OutputToLog(Graph* g, Position p) {
	if (p.distToVtx1 == 0 || p.distToVtx2 == 0) {
		if (p.distToVtx1 == 0) {
			fprintf(logF, "[(%d号)%s%s旁边。]", p.vtx1, g->v[p.vtx1].name, !g->v[p.vtx1].flag ? "路口" : "");
		}
		else if (p.distToVtx2 == 0) {
			fprintf(logF, "[(%d号)%s%s旁边。]", p.vtx2, g->v[p.vtx2].name, !g->v[p.vtx2].flag ? "路口" : "");
		}
	}
	else {
		if (g->v[p.vtx1].flag == 0) {
			fprintf(logF, "[向%s距离%d号路口%d米，", p.direToVtx1, p.vtx1, p.distToVtx1);
		}
		else {
			fprintf(logF, "[向%s距离(%d号)%s%d米，", p.direToVtx1, p.vtx1, g->v[p.vtx1].name, p.distToVtx1);
		}
		if (g->v[p.vtx2].flag == 0) {
			fprintf(logF, "向%s距离%d号路口%d米。]", p.direToVtx2, p.vtx2, p.distToVtx2);
		}
		else {
			fprintf(logF, "向%s距离(%d号)%s%d米。]", p.direToVtx2, p.vtx2, g->v[p.vtx2].name, p.distToVtx2);
		}
	}
}
