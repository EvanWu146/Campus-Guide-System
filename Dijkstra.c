#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include"HeadFile.h"
int* Dijkstra(Graph* graphPtr, int start, int dest, int function) //迪杰斯特拉算法求最短路径，并将路径信息保存到数组path中，具体不再赘述
{
	int shortestOrNot[VERTEX_NUM];
	int distance[VERTEX_NUM];
	int* path = NULL;
	int i, min, minID;
	path = malloc(sizeof(int) * VERTEX_NUM);
	
	for (i = 0; i < VERTEX_NUM; i++)
	{
		shortestOrNot[i] = 0;
		if (graphPtr->e[start][i])
		{
			if (function == TIME_MIN)
				distance[i] = graphPtr->e[start][i]->length * (2520 / graphPtr->e[start][i]->crowding_degree);
			else if (function == DISTANCE_MIN)
				distance[i] = graphPtr->e[start][i]->length;
			else if (function == BICYCLE)
			{
				if (graphPtr->e[start][i]->bicycleAllow == 1)
					distance[i] = graphPtr->e[start][i]->length * (2520 / 2 / graphPtr->e[start][i]->crowding_degree);
				else
					distance[i] = graphPtr->e[start][i]->length * (2520 / graphPtr->e[start][i]->crowding_degree);
			}
			path[i] = start;
		}
		else
		{
			distance[i] = -1;
			path[i] = -1;
		}
	}
	shortestOrNot[start] = 1;
	distance[start] = 0;
	while (shortestOrNot[dest] == 0)
	{
		min = -1;
		for (i = 0; i < VERTEX_NUM; i++)
		{
			if ((shortestOrNot[i] == 0) && (distance[i] != -1))
			{
				if (distance[i] < min || min == -1)
				{
					min = distance[i];
					minID = i;
				}
			}
		}
		if (min == -1)
		{
			printf("error1!");
			break;
		}
		else
		{
			shortestOrNot[minID] = 1;
		}
		for (i = 0; i < VERTEX_NUM; i++)
		{
			if (function == TIME_MIN && shortestOrNot[i] == 0 && graphPtr->e[minID][i])
			{
				if (distance[i] > distance[minID] + graphPtr->e[minID][i]->length * (2520 / graphPtr->e[minID][i]->crowding_degree) || distance[i] == -1)
				{
					distance[i] = distance[minID] + graphPtr->e[minID][i]->length * (2520 / graphPtr->e[minID][i]->crowding_degree);
					path[i] = minID;
				}
			}
			else if (function == DISTANCE_MIN && shortestOrNot[i] == 0 && graphPtr->e[minID][i])
			{
				if (distance[i] > distance[minID] + graphPtr->e[minID][i]->length || distance[i] == -1)
				{
					distance[i] = distance[minID] + graphPtr->e[minID][i]->length;
					path[i] = minID;
				}
			}
			else if (function == BICYCLE && shortestOrNot[i] == 0 && graphPtr->e[minID][i])
			{
				if (graphPtr->e[minID][i]->bicycleAllow == 1)
				{
					if (distance[i] > distance[minID] + graphPtr->e[minID][i]->length * (2520 / 2 / graphPtr->e[minID][i]->crowding_degree) || distance[i] == -1)
					{
						distance[i] = distance[minID] + graphPtr->e[minID][i]->length * (2520 / 2 / graphPtr->e[minID][i]->crowding_degree);
						path[i] = minID;
					}
				}
				else if (graphPtr->e[minID][i]->bicycleAllow == 0)
				{
					if (distance[i] > distance[minID] + graphPtr->e[minID][i]->length * (2520 / graphPtr->e[minID][i]->crowding_degree) || distance[i] == -1)
					{
						distance[i] = distance[minID] + graphPtr->e[minID][i]->length * (2520 / graphPtr->e[minID][i]->crowding_degree);
						path[i] = minID;
					}
				}
			}
		}
	}
	return path;
}

int CalculatePathLength(Graph* graphPtr, int* path, int start, int dest, int function) //计算路径长度
{
	int length = 0;
	int m = dest;
	int n;
	while (m != start)
	{
		n = path[m];
		if (function == TIME_MIN)
			length += graphPtr->e[m][n]->length * (2520 / graphPtr->e[m][n]->crowding_degree);
		else if (function == DISTANCE_MIN)
			length += graphPtr->e[m][n]->length;
		else if (function == BICYCLE)
		{
			if (graphPtr->e[m][n]->bicycleAllow == 1)
				length += graphPtr->e[m][n]->length * (2520 / 2 / graphPtr->e[m][n]->crowding_degree);
			else
				length += graphPtr->e[m][n]->length * (2520 / graphPtr->e[m][n]->crowding_degree);
		}
		m = n;
	}
	return length;
}

