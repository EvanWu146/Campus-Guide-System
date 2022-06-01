#define VERTEX_NUM 144//��������У���Ķ�����
#define BUILDING_NUM 30
#define BUILDING_NUM1 114
#define EDGE_NUM 200//��������У���ı���
#define DISTANCE_MIN 0
#define TIME_MIN 1
#define PASS_THROUGH 2
#define BICYCLE 3
#define TIME_MAX 32
#define FILE_BUFFER_LENGTH 30000
typedef struct {
	int length, crowding_degree;//���ȣ�ӵ����
	int vertex_head;//���
	int vertex_last;//����
	int bicycleAllow;//�Ƿ��������г�ͨ��
	char direction[3];//����
}Edge, * EdgePtr;//��
typedef struct {
	int flag;//0����·�ڣ�1�����ѧ¥������¥�ͷ�����ʩ��2����칫¥����������
	int num;//���
	char name[20];//���������֣�·��û��
}Vertex;//�㣬����������·�ں�ѧУ����
typedef struct {
	int vtx1;
	int vtx2;
	char direToVtx1[3];
	char direToVtx2[3];
	int distToVtx1;
	int distToVtx2;
}Position;//�û���ǰ��λ��
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
	char name[40];//�߼���ַ�� 
	int num;//�߼���ַ��� 
	char location[30];//��Ӧ����λ�� 
	int location_num;//��������λ�ý������ 
}Logic;
void setTime(virtualTime*);
void beginGuideSystem(Graph*, Position, int);//��ʼ����
void graph_creat(Graph*); //����ͼ����graphPtrָ���Graph���ͽṹ����
void Logical_creat(Logic*);
void ShortestPathNgt(Graph*, Position, int, int);//���·������
void ShortestTimeNgt(Graph*, Position, int);//���ʱ�����
void ShortestPathNgt_passBy(Graph*, Position, int);//;�����·������
void ShortestTimeNgt_Bike(Graph*, Position, int);//���г����ʱ�����
int* Dijkstra(Graph*, int, int, int);//Dijkstra�㷨
int CalculatePathLength(Graph*, int*, int, int, int);//����·���ĳ���
void GuideWay_InOrder(Graph*, int[], Position, int, int, int);//����ʱ�����
void GuideWay(Graph*, int[], Position, int, int,int);//����һ�������·��
void GuideWay_bicycle(Graph*, int[], Position, int, int);//������������г���
void SearchAround(Graph*, Position);//��ѯ��Χ������ʩ��Ϣ
void Position_toString(Graph*, Position);//���λ����Ϣ
void Position_Changing(Graph*, Position*, int, int, int);//λ��״̬�ĸı�
void Time_Refreshing(virtualTime*, unsigned long, int);//ʱ��ˢ�²����
void Position_OutputToLog(Graph* , Position );//λ��������ļ�
unsigned _stdcall run(void* pArguments);//�̺߳���


