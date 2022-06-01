#include"HeadFile.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>

void graph_creat(Graph* graphPtr) //将地图存入graphPtr指向的Graph类型结构体中
{
	int i, flag1;
	FILE* fPtr = fopen("VertexAndEdge.txt", "r");
	char name1[20];
	int length1, crowding_degree1, vertex_head1, vertex_last1, bicycleAllow1;
	char direction1[3];
	if (fPtr == NULL)
	{
		printf("File could not be opened\n");
	}
	else
	{
		for (i = 0; i < VERTEX_NUM; i++)
		{
			if (i < BUILDING_NUM || i >= BUILDING_NUM1)
			{
				fscanf(fPtr, "%s %d", name1, &flag1);
				graphPtr->v[i].flag = flag1;
				graphPtr->v[i].num = i;
				strcpy(graphPtr->v[i].name, name1);
			}
			else
			{
				graphPtr->v[i].flag = 0;
				graphPtr->v[i].num = i;
				graphPtr->v[i].name[0] = '\0';
			}
		}
		for (i = 0; i < EDGE_NUM; i++) {
			for (int j = 0; j < EDGE_NUM; j++) {
				graphPtr->e[i][j] = NULL;
			}
		}
		for (i = 0; i < 2 * EDGE_NUM; i++)
		{
			length1 = 0; vertex_head1 = 0; vertex_last1 = 0; crowding_degree1 = 0; bicycleAllow1 = 0;
			fscanf(fPtr, "%d %d %d %d %d %s", &vertex_head1, &vertex_last1, &length1, &crowding_degree1, &bicycleAllow1, direction1);
			graphPtr->e[vertex_head1][vertex_last1] = (Edge*)malloc(sizeof(Edge));
			graphPtr->e[vertex_head1][vertex_last1]->length = length1;
			graphPtr->e[vertex_head1][vertex_last1]->vertex_head = vertex_head1;
			graphPtr->e[vertex_head1][vertex_last1]->vertex_last = vertex_last1;
			graphPtr->e[vertex_head1][vertex_last1]->crowding_degree = crowding_degree1;
			graphPtr->e[vertex_head1][vertex_last1]->bicycleAllow = bicycleAllow1;
			strcpy(graphPtr->e[vertex_head1][vertex_last1]->direction, direction1);
		}
		fclose(fPtr);
	}
}
void Logical_creat(Logic* logicPtr)
{
	FILE* fPtr = fopen("logic.txt", "r");
	char name1[40], location1[30];
	int i = 0, number, location_number, j;
	if (fPtr == NULL)
	{
		printf("File could not be opened\n");
	}
	else
	{
		fscanf(fPtr, "%s %s %d", name1, location1, &location_number);
		while (!feof(fPtr))
		{
			strcpy((logicPtr + i)->name, name1);
			strcpy((logicPtr + i)->location, location1);
			(logicPtr + i)->location_num = location_number;
			(logicPtr + i)->num = i;
			i++;
			fscanf(fPtr, "%s %s %d", name1, location1, &location_number);
		}
		for (j = 0; j < i; j++)
		{
			printf("[编号 %s%s%d]: %s\n", (logicPtr + j)->location_num < 100 ? "0" : "", (logicPtr + j)->location_num < 10 ? "0":"" , (logicPtr + j)->location_num, (logicPtr + j)->name);
		}
		fclose(fPtr);
	}
	
}