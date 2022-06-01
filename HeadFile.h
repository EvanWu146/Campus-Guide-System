#define VERTEX_NUM 144//新增了新校区的顶点数
#define BUILDING_NUM 30
#define BUILDING_NUM1 114
#define EDGE_NUM 200//新增了新校区的边数
#define DISTANCE_MIN 0
#define TIME_MIN 1
#define PASS_THROUGH 2
#define BICYCLE 3
#define TIME_MAX 32
#define FILE_BUFFER_LENGTH 30000
typedef struct {
	int length, crowding_degree;//长度，拥挤度
	int vertex_head;//入点
	int vertex_last;//出点
	int bicycleAllow;//是否允许自行车通过
	char direction[3];//方向
}Edge, * EdgePtr;//边
typedef struct {
	int flag;//0代表路口，1代表教学楼，宿舍楼和服务设施，2代表办公楼等其他建筑
	int num;//序号
	char name[20];//建筑的名字，路口没有
}Vertex;//点，包括建筑、路口和学校大门
typedef struct {
	int vtx1;
	int vtx2;
	char direToVtx1[3];
	char direToVtx2[3];
	int distToVtx1;
	int distToVtx2;
}Position;//用户当前的位置
typedef struct {
	Vertex v[VERTEX_NUM];
	EdgePtr e[EDGE_NUM][EDGE_NUM];
}Graph;//

typedef struct threadArgument {
	int ID_thread;
	int Address_thread;
}th;
typedef struct {
	unsigned long year;
	unsigned long month;
	unsigned long day;
	unsigned long hour;
	unsigned long min;
}virtualTime;
typedef struct {
	char name[40];//逻辑地址名 
	int num;//逻辑地址序号 
	char location[30];//对应物理位置 
	int location_num;//对于物理位置建筑编号 
}Logic;
void setTime(virtualTime*);
void beginGuideSystem(Graph*, Position, int);//开始导航
void graph_creat(Graph*); //将地图存入graphPtr指向的Graph类型结构体中
void Logical_creat(Logic*);
void ShortestPathNgt(Graph*, Position, int, int);//最短路径策略
void ShortestTimeNgt(Graph*, Position, int);//最短时间策略
void ShortestPathNgt_passBy(Graph*, Position, int);//途径最短路径策略
void ShortestTimeNgt_Bike(Graph*, Position, int);//自行车最短时间策略
int* Dijkstra(Graph*, int, int, int);//Dijkstra算法
int CalculatePathLength(Graph*, int*, int, int, int);//计算路径的长度
void GuideWay_InOrder(Graph*, int[], Position, int, int, int);//导航时序输出
void GuideWay(Graph*, int[], Position, int, int,int);//导航一次性输出路线
void GuideWay_bicycle(Graph*, int[], Position, int, int);//导航输出（自行车）
void SearchAround(Graph*, Position);//查询周围服务设施信息
void Position_toString(Graph*, Position);//输出位置信息
void Position_Changing(Graph*, Position*, int, int, int);//位置状态的改变
void Time_Refreshing(virtualTime*, unsigned long, int);//时间刷新并输出
void Position_OutputToLog(Graph* , Position );//位置输出到文件
unsigned _stdcall run(void* pArguments);//线程函数


