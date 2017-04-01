#ifndef __MAXFLOW_H__
#define __MAXFLOW_H__
#endif

#include <cstring>
#include <queue>

using namespace std;

class MaxFlow {

public:
	MaxFlow(int n, int s, int t) {
		N = n;
		S = s;
		T = t;
	}
	~MaxFlow() {
		for (int i = 0; i < N; i++) {
			edge[i].clear();
			flow[i].clear();
			cost[i].clear();
			oppo[i].clear();
			edge_forward[i].clear();
			edge_backward[i].clear();
		}
	}

	void connect(int x, int y, int f, int c) {

		edge[x].push_back(y);
		flow[x].push_back(f);
		cost[x].push_back(c);

		edge[y].push_back(x);
		flow[y].push_back(0);
		cost[y].push_back(-c);

		oppo[x].push_back(edge[y].size() - 1);
		oppo[y].push_back(edge[x].size() - 1);

		edge_forward[x].push_back(y);
		edge_backward[x].push_back(edge[y].size() - 1);
	}

	void calc_result(int &ans_f, int &ans_c) {
		ans_flow = 0;
		ans_cost = 0;
		while (find()) add_flow();
		ans_f = ans_flow;
		ans_c = ans_cost;
	}

	void find_route() {
		route_cnt = 0;
		stack[0] = S;
		memset(use, 0, sizeof(use));
 		use[S] = true;
		DFS(S, 1, 100000000);
	}

	int get_route_cnt() {
		return route_cnt;
	}

	void get_route(int index, int * &route, int &len) {
		route = route_head[index];
		len = route_len[index];
	}

private:
	static const int INF = 2147483647;
	static const int maxn = 2010;
	static const int max_route_cnt = 50010;

	int N, S, T;
	int ans_cost, ans_flow;
	int route_cnt = 0;

	vector<int> edge_forward[maxn];
	vector<int> edge_backward[maxn];
	vector<int> edge[maxn];
	vector<int> flow[maxn];
	vector<int> cost[maxn];
	vector<int> oppo[maxn];
	
	bool use[maxn];
	int pre[maxn];
	int dis[maxn], frm[maxn];

	int stack[maxn];
	int *route_head[max_route_cnt];
	int route_len[max_route_cnt];

	bool find() {
		memset(pre, 0, sizeof(pre));
		memset(use, 0, sizeof(use));
		queue<int> Queue;
		Queue.push(S);
		for (int i = 0; i < N; i++) 
			dis[i] = INF;
		dis[S] = 0;
		while (!Queue.empty()) {
			int x = Queue.front();
			Queue.pop();
			for (unsigned int i = 0; i < edge[x].size(); i++) {
				int y = edge[x][i];
				if (flow[x][i] == 0) continue;
				if (cost[x][i] + dis[x] < dis[y]) {
					dis[y] = cost[x][i] + dis[x];
					pre[y] = x; frm[y] = i;
					if (!use[y]) {
						use[y] = true;
						Queue.push(y);
					}
				}
			}
			use[x] = false;
		}
		return (dis[T] != INF);
	}

	void add_flow() {
		int x = T, y, min = INF, i;
		while (x != S) {
			y = pre[x];
			i = frm[x];
			if (flow[y][i] < min) min = flow[y][i];
			x = y;
		}

		x = T;
		while (x != S) {
			y = pre[x];
			i = frm[x];
			flow[y][i] -= min;
			flow[x][oppo[y][i]] += min;
			x = y;
		}
		ans_flow += min;
		ans_cost += min * dis[T];
	}

	int miner(int a, int b) {
		if (a < b) return a;
		return b;
	}

	int DFS(int x, int len, int rest) {
		if (x == T) {
			if (rest == 0) return rest;
			int *route = new int[len];
			for (int i = 0; i < len - 1; i++) 
				route[i] = stack[i];
			route[0] = rest;
			route_head[route_cnt ++] = route;
			route_len[route_cnt - 1] = len - 1;
			return rest;
		}
		int used = 0;
		for (unsigned int i = 0; i < edge_forward[x].size(); i++) 
			if (edge_forward[x][i] >= 0) {
				int y = edge_forward[x][i];
				int j = edge_backward[x][i];
				if (flow[y][j] == 0) continue;
				if (use[y]) continue;
				stack[len] = y;
				use[y] = true;
				int ret = DFS(y, len + 1, miner(rest, flow[y][j])); 
				use[y] = false;
				flow[x][i] += ret;
				flow[y][j] -= ret;
				rest -= ret;
				used += ret;
			}
		return used;

	}

};

