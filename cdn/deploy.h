#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

#define MAX(X,Y) (X) > (Y) ? (X) : (Y)

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

	

#endif
