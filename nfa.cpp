#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include "nfa.h"
using namespace std;
int num = 0;

struct Edge_t;
struct Node_t
{
  int num;
  int visited;
  Edge_t *edges;  //为什么一定为指针形式
  Node_t *next;
};
typedef Node_t *Node;

struct Edge_t
{
    int c;
    Node_t *from;
    Node_t *to;
    Edge_t *next;
};
typedef Edge_t *Edge;

struct node_v
{
    vector<Node> *n;
    bool endState;
};

struct Edge_q;
struct Node_q
{
    int num;
    node_v q;
    Edge_q *edges;  //为什么一定为指针形式
    Node_q *next;
};
typedef Node_q *ptr_Node_q;

struct Edge_q
{
    int c;
    Node_q *from;
    Node_q *to;
    Edge_q *next;
};
typedef Edge_q *ptr_Edge_q;


// list head insert
static Node Node_new (int num, Node n)
{
  Node p;
  p = (Node)malloc(sizeof(*p));
  p->num = num;
  p->edges = 0;
  p->next = n;
  return p;
}


static Edge Edge_new (Node from
                        , Node to
                        , int c
                        , Edge edge)
{
  Edge p;
  p = (Edge)malloc(sizeof(*p));
  p->c = c;
  p->from = from;
  p->to = to;
  p->next = edge;
  return p;
}


Nfa_t *Nfa_new ()
{
  Nfa_t *p;
  p = (Nfa)malloc(sizeof(*p));
  p->start = -1;
  p->accept = -1;
  p->nodes = 0;
  return p;
}


// lookup a node "num" in the nfa and return
// it; if that
// node does not exist, create a fresh one
// and return it.
Node Nfa_lookupOrInsert (Nfa nfa, int num)
{
  assert (nfa);
  Node nodes = nfa->nodes;
  while (nodes){
    if (nodes->num == num)
      return nodes;
    nodes = nodes->next;
  }
  Node p = Node_new(num, nfa->nodes);
  nfa->nodes = p;
  return p;
}


void Nfa_addEdge(Nfa nfa, int from, int to, int c)
{
  Node f = Nfa_lookupOrInsert (nfa, from);
  Node t = Nfa_lookupOrInsert (nfa, to);
  f->edges = Edge_new (f, t, c, f->edges);
  return;
}

ptr_Node_q head = NULL;
static ptr_Node_q Node_q_new (int num, vector<Node> *n)
{
    ptr_Node_q p;
    p = (ptr_Node_q)malloc(sizeof(*p));
    p->num = num;
    p->q.n = n;
    p->edges = 0;
    return p;
}

void endState(ptr_Node_q &p, vector<Node> *n)
{
    for (int i = 0; i < n->size(); ++i) {
        if ((*n)[i] == endNode) {
            p->q.endState = true;
        }
    }
}

bool needNewNode = false;
ptr_Node_q Dfa_lookupOrInsert (vector<Node> *n)
{
    
    ptr_Node_q node = head;
    while (node != NULL){
        if (*(node->q.n) == *n)
            return node;
        node = node->next;
    }
    ptr_Node_q p = Node_q_new(num++, n);
    needNewNode = true;
    p->q.endState = false;
    endState(p, n);
    p->next = head;
    head = p;
    return p;
}


static ptr_Edge_q Edge_q_new (ptr_Node_q from
                      , ptr_Node_q to
                      , int c)
{
    ptr_Edge_q p;
    p = (ptr_Edge_q)malloc(sizeof(*p));
    p->c = c;
    p->from = from;
    p->to = to;
    p->next = 0;
    return p;
}


void Dfa_addEdge(vector<Node> *from,
                 vector<Node> *to,
                 int value)
{
    ptr_Node_q f_node = Dfa_lookupOrInsert (from);
    ptr_Node_q t_node = Dfa_lookupOrInsert (to);
    ptr_Edge_q edge = Edge_q_new(f_node, t_node, value);
    edge->next = f_node->edges;
    f_node->edges = edge;
}


vector<Node> closure;
void esp_closure(Node x)
{
    if (x == NULL) {
        return;
    }
    closure.push_back(x);
    x->visited = 1;
    int value;
    Edge edge = x->edges;
    Node y;
    while (edge != NULL)
    {
        value = edge->c;
        y = edge->to;
        if (value == EPS)
        {
            if (y->visited == 0) {
                esp_closure(y);
            }
        }
        edge = edge->next;
    }
}


static void visited_Reset(Nfa nfa)
{
    Node node = nfa->nodes;
    if (nfa != NULL) {
        while (node != NULL) {
            node->visited = 0;
            node = node->next;
        }
    }
}


static Node first_Node(Nfa nfa)
{
    Node current = nfa->nodes;
    Node pre = current->next;
    if (nfa != NULL) {
        while (pre != NULL) {
            current = current->next;
            pre = pre->next;
        }
    }
    return current;
}


void assign(vector<Node> &q)
{
    for (int i = 0; i < closure.size(); ++i) {
        q.push_back(closure[i]);
    }
}


void nfa_To_dfa(Nfa nfa)
{
    vector<Node> q0;
    Node n0 = first_Node(nfa);
    esp_closure(n0);
    assign(q0);
    
    vector< vector<Node> > Q;
    deque< vector<Node> > workList;
    Q.push_back(q0);
    workList.push_back(q0);
  
   // vector<Node> t;
   // vector<Node> pop;
   // bool Had;
    Edge edge;
    while (!workList.empty())
    {
        auto pop = new vector<Node>(workList.front());
        workList.pop_front();
        for (int i = 0; i < pop->size(); ++i)
        {
            edge = (*pop)[i]->edges;
            if (edge != NULL && edge->c != EPS)
            {
                closure.clear();
                visited_Reset(nfa);
                esp_closure(edge->to);
                auto t = new vector < Node >;
                t->clear();
                assign(*t);
                Dfa_addEdge(pop, t, edge->c);
                
                if (needNewNode == true) {
                    workList.push_back(*t);
                    needNewNode = false;
                }
                /*
                Had = is_have(Q, *t);
                if (!Had)
                {
                    Q.push_back(*t);
                    workList.push_back(*t);
                }*/
            }
        }
    }
}


void Dfa_print()
{
	cout << "\nDFA:" << endl;
    ptr_Node_q node = head;
	while (node != NULL)
	{
		ptr_Edge_q edge = node->edges;
		while (edge){
            printf ("%d ----(%d)----> %d\n"
                    , edge->from->num
                    , edge->c
                    , edge->to->num);
            edge = edge->next;
		}
		node = node->next;
	}
}


void Nfa_print (Nfa nfa)
{
  assert (nfa);
  Node nodes = nfa->nodes;
  assert (nodes);
  while (nodes){
    Edge edges = nodes->edges;
    while (edges){
      printf ("%d ----(%d)----> %d\n"
              , edges->from->num
              , edges->c
              , edges->to->num);
      edges = edges->next;
    }
    nodes = nodes->next;
  }
  return;
}
