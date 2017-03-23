#include "deploy.h"
#include "maxflow.h"

#include <stdio.h>
#include <cstring>

using namespace std;

const int maxn = 1010;
const int INIT_ANS_LEN = 10010;
const int MAX_FLOW = 1000000000;

vector<int> edge[maxn];
vector<int> width[maxn];
vector<int> cost[maxn];

int N, Edge, Consumer, tot_consumes = 0;
int cons[maxn], _cons[maxn];
int snode[maxn], tnode[maxn], tneed[maxn];
char *ans_str;
int ans_str_len, cur_ans_len;
int server_price;
int serverFlag[maxn];   // serverFlag[i] == True: build a server at node i

void build_edge(int x, int y, int w, int c) {
	edge[x].push_back(y);
	width[x].push_back(w);
	cost[x].push_back(c);

	edge[y].push_back(x);
	width[y].push_back(w);
	cost[y].push_back(c);	
}

void add_to_answer(char *word) {
	int len = strlen(word);
	if (ans_str_len + len > cur_ans_len) {
		cur_ans_len *= 2;
		char *tmp = new char[cur_ans_len];
		memcpy(tmp, ans_str, sizeof(char) * ans_str_len);
		delete ans_str;
		ans_str = tmp;
	}
	strcat(ans_str, word);
	ans_str_len += len;
}

void calc_max_flow(int* snode, int slen, int* tnode, int tlen) {
	int S = N, T = N + 1;
	MaxFlow *maxflow = new MaxFlow(N + 2, N, N + 1);
	for (int i = 0; i < slen; i++) 
		maxflow->connect(S, snode[i], MAX_FLOW, 0);
	for (int i = 0; i < tlen; i++) 
		maxflow->connect(tnode[i], T, tneed[tnode[i]], 0);
	for (int i = 0; i < N; i++)
		for (unsigned int j = 0; j < edge[i].size(); j++)
			maxflow->connect(i, j, width[i][j], cost[i][j]);

	int ans_flow = 0, ans_cost = 0;
	maxflow->calc_result(ans_flow, ans_cost);
	maxflow->find_route();

	ans_str = new char[INIT_ANS_LEN];
	cur_ans_len = INIT_ANS_LEN;

	int route_cnt = maxflow->get_route_cnt();
	sprintf(ans_str, "%d\n\n", route_cnt);

	for (int i = 0; i < route_cnt; i++) {
		int *route = NULL;
		int route_len = 0;
		maxflow->get_route(i, route, route_len);
		for (int j = 1; j < route_len; j++) {
			char num[10];
			sprintf(num, "%d ", route[j]);
			add_to_answer(num);
		}
		char end[10];
		sprintf(end, "%d %d\n", _cons[route[route_len - 1]], route[0]);
		add_to_answer(end);
    }

}

void selectServer()
{
    memset(serverFlag, false, sizeof(serverFlag));
    for (int i = 0; i < Consumer; i ++) {
        int x_t = cons[i], len_t;
        int flowCnt_t = 0, costMax_t = 0;
        len_t = width[x_t].size();
        for (int j = 0; j < len_t; j ++) {
            flowCnt_t += width[x_t].at(j);
            if (costMax_t < cost[x_t].at(j))
                costMax_t = cost[x_t].at(j);
        }
        if (flowCnt_t < tneed[x_t] || tneed[x_t] * costMax_t > server_price) {
            serverFlag[x_t] = true;
        }
    }
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	sscanf(topo[0], "%d %d %d", &N, &Edge, &Consumer);
	sscanf(topo[2], "%d", &server_price);

	int next = 0;
	for (int i  = 4; i < line_num; i++) {
		int x, y, w, c;
		if (strcmp(topo[i], "\r\n") == 0) { next = i + 1; break; }
		sscanf(topo[i], "%d %d %d %d", &x, &y, &w, &c);
		build_edge(x, y, w, c);
		build_edge(y, x, w, c);
	}
	for (int i = next; i < line_num; i++) {
		int x, y, w;
		sscanf(topo[i], "%d %d %d", &x, &y, &w);
		cons[x] = y; _cons[y] = x;
		tneed[y] = w;
		tot_consumes += w;
	}

    selectServer();
    int serverCnt = 0;

    for (int i = 0; i < Consumer; i++) {
        tnode[i] = cons[i];
      //  snode[i] = cons[i];
	}

    for (int i = 0; i < N; i ++)
        if (serverFlag[i]) {
            snode[serverCnt ++] = i;
        }

    calc_max_flow(snode, serverCnt, tnode, Consumer);
	// 需要输出的内容
	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(ans_str, filename);

}
