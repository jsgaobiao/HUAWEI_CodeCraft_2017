#include <ctime>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <sstream>

using namespace std;

const int maxn = 1010;
const int INF = 214748364;
const int INIT_ANS_LEN = 10010;
const int MAX_FLOW = 1000000000;

int edge[maxn][maxn];
int flow[maxn][maxn];
int cost[maxn][maxn];

int N, Edge, Consumer, tot_consumes = 0;
int cons[maxn], _cons[maxn], tneed_index[maxn];
int snode[maxn], tnode[maxn], tneed[maxn];
int cover[maxn], cover_index[maxn];

char *ans_str;
int ans_str_len, cur_ans_len;
int server_price;
int serverFlag[maxn];   // serverFlag[i] == True: build a server at node i
int serverAns[maxn];


//你要完成的功能总入口
int main(int argc, char *argv[]) {

	FILE *input = fopen(argv[1], "r");
	FILE *answer = fopen(argv[2], "r");

	fscanf(input, "%d %d %d", &N, &Edge, &Consumer);
	fscanf(input, "%d", &server_price);

	int next = 0;
	for (int i  = 0; i < Edge; i++) {
		int x, y, w, c;
		fscanf(input, "%d %d %d %d", &x, &y, &w, &c);
		//printf("%d %d %d %d\n", x, y, w, c);
		flow[x][y] += w;
		flow[y][x] += w;
		cost[x][y] = c;
		cost[y][x] = c;
	}

	for (int i = 0; i < Consumer; i++) {
		int x, y, w;
		fscanf(input, "%d %d %d", &x, &y, &w);
		cons[x] = y; _cons[y] = x;
		tneed[y] = w;
		tot_consumes += w;
	}
	fclose(input);

	int route_cnt = 0, tot_money = 0;
	fscanf(answer, "%d\n\n", &route_cnt);
	memset(cover, 0, sizeof(cover));
	for (int i = 0; i < route_cnt; i ++) {
		char sen[100];
		fgets(sen, 100, answer);
		stringstream s(sen);
		int x, y, route[100], len = 0; 
		while (s >> x) { route[len ++] = x;}
		int need = route[len - 1], money = 0;
		
		if (!cover[route[0]]) {
			tot_money += server_price;
			cover[route[0]] = true;
		}

		for (int j = 0; j < len - 3; j++) {
			x = route[j];
			y = route[j + 1];
			if (flow[x][y] < need) {
				printf("over flow at route %d, from %d to %d, flowboud is %d\n", i, x, y, flow[x][y]);
				exit(0);
			}
			else flow[x][y] -= need;
			money += cost[x][y] * need;
		}
		tot_money += money;
		tneed[route[len - 3]] -= need;
	}
	for (int i = 0; i < N; i++)
		if (tneed[i] > 0) {
			printf("flow didn't meet the need at point %d\n", i);
			exit(0);
		}
	printf("successful! cost = %d\n", tot_money);
	return 0;

}
