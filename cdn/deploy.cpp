#include "deploy.h"
#include "maxflow.h"

#include <ctime>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>

using namespace std;

const int maxn = 1010;
const int INF = 2147483647;
const int INIT_ANS_LEN = 30010;
const int MAX_FLOW = 1000000000;

vector<int> edge[maxn];
vector<int> width[maxn];
vector<int> cost[maxn];

int N, Edge, Consumer, tot_consumes = 0;
int cons[maxn], _cons[maxn], tneed_index[maxn];
int snode[maxn], tnode[maxn], tneed[maxn];
int cover[maxn], cover_index[maxn];

char *ans_str;
int ans_str_len, cur_ans_len;
int server_price;
int serverFlag[maxn];   // serverFlag[i] == True: build a server at node i
int serverAns[maxn];

bool cmp_need(int x, int y) {
	return tneed[x] > tneed[y];
}

bool cmp_cover(int x, int y) {
	return cover[x] > cover[y];
}

void build_edge(int x, int y, int w, int c) {
	edge[x].push_back(y);
	width[x].push_back(w);
	cost[x].push_back(c);
}

void add_to_answer(char *word) {
	int len = strlen(word);
	if (ans_str_len + len >= cur_ans_len) {
		cur_ans_len *= 2;
		char *tmp = new char[cur_ans_len];
		memset(tmp, 0, sizeof(char) * cur_ans_len);
		memcpy(tmp, ans_str, sizeof(char) * ans_str_len);
		delete ans_str;
		ans_str = tmp;
		//ans_str[ans_str_len] = '\0';
	}
	strcat(ans_str, word);
	ans_str_len += len;
	//ans_str[ans_str_len] = '\0';
}

int calc_max_flow(int* snode, int slen, int* tnode, int tlen, bool findRouteFlag) {
	int S = N, T = N + 1;
	MaxFlow *maxflow = new MaxFlow(N + 2, N, N + 1);
	for (int i = 0; i < slen; i++) 
		maxflow->connect(S, snode[i], MAX_FLOW, 0);
	for (int i = 0; i < tlen; i++) 
		maxflow->connect(tnode[i], T, tneed[tnode[i]], 0);
	for (int i = 0; i < N; i++)
		for (unsigned int j = 0; j < edge[i].size(); j++)
			maxflow->connect(i, edge[i][j], width[i][j], cost[i][j]);

	int ans_flow = 0, ans_cost = 0;
    int tot_cost = 0;
	maxflow->calc_result(ans_flow, ans_cost);
    if (ans_flow < tot_consumes)
        return INF;
    tot_cost = ans_cost + slen * server_price;
    if (!findRouteFlag) {
    	delete maxflow;
    	return tot_cost;
    }
        
    printf("ans_flow=%d/%d, ans_cost=%d\n", ans_flow, tot_consumes, ans_cost);
	maxflow->find_route();

	ans_str = new char[INIT_ANS_LEN];
	cur_ans_len = INIT_ANS_LEN;

	int route_cnt = maxflow->get_route_cnt();
	sprintf(ans_str, "%d\n\n", route_cnt);
	ans_str_len = strlen(ans_str);
	int find_flow = 0;
	for (int i = 0; i < route_cnt; i++) {
		int *route = NULL;
		int route_len = 0;
		maxflow->get_route(i, route, route_len);
		for (int j = 1; j < route_len; j++) {
			char num[20];
			memset(num, 0, sizeof(num));
			sprintf(num, "%d ", route[j]);
			add_to_answer(num);
		}
		char end[20];
		memset(end, 0, sizeof(end));
		sprintf(end, "%d %d\n", _cons[route[route_len - 1]], route[0]);
		add_to_answer(end);
		find_flow += route[0];
    }
    printf("find_flow=%d\n", find_flow);
    delete maxflow;
    return tot_cost;
}

int selectServer(int kth)
{
    int minCost = INF, loc = -1;
    for (int i = 0; i < N; i++) {
    	if (cover[cover_index[i]] == 0) break;
    	if (!serverFlag[cover_index[i]]) {
    		snode[kth] = cover_index[i];
    		int cost = calc_max_flow(snode, kth + 1, tnode, Consumer, false);
    		if (cost < minCost) { minCost = cost; loc = cover_index[i]; } 
    	}
    }
    	
    if (minCost == INF) {
    	snode[kth] = tneed_index[kth];
    	serverFlag[tneed_index[kth]] = true;
    }
    else {
    	snode[kth] = loc;
    	serverFlag[loc] = true;
    }
    return minCost;
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	time_t start_time, end_time;
	time(&start_time);

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


	for (int i = 0; i < N; i++)
		tneed_index[i] = i;

	sort(tneed_index, tneed_index + N, cmp_need);

    for (int i = 0; i < Consumer; i++) {
        tnode[i] = cons[i];
      //  snode[i] = cons[i];
    }

    int minCost = INF, start = 0;
    
    memset(serverFlag, false, sizeof(serverFlag));
    if (Consumer > 300) 
    	start = Consumer / 2;
    else if (Consumer > 100)
    	start = Consumer / 3;
    if (Consumer > 100) {
    	for (int i = 0; i < start; i++) {
    		snode[i] = tneed_index[i];
    		serverFlag[tneed_index[i]] = true;
    	}
    }

	memset(cover, 0, sizeof(cover));
	for (int i = start; i < Consumer; i++) {
		int cur = tneed_index[i];
		for (int j = 0; j < edge[cur].size(); j++)
			cover[edge[cur][j]] ++;
	}
	for (int i = 0; i < N; i++) {
		cover_index[i] = i;
	}
	sort(cover_index, cover_index + N, cmp_cover);

    for (int o = start; o < Consumer; o ++) {
        int cost = selectServer(o);
        printf("Cost: %d\n", cost);

        if (cost < minCost) {
        	minCost = cost;
        	memcpy(serverAns, serverFlag, sizeof(serverFlag));
        }
        else if (cost > minCost) break;

        time(&end_time);
        double cost_time = difftime(end_time, start_time);
        if (cost_time > 50) break;
    }

    int serverCnt = 0;
    for (int i = 0; i < N; i ++)
        if (serverAns[i]) {
            snode[serverCnt ++] = i;
        }
    int cost = calc_max_flow(snode, serverCnt, tnode, Consumer, true);
    printf("tot_cost: %d\n", cost);

	// 需要输出的内容
	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(ans_str, filename);

}
