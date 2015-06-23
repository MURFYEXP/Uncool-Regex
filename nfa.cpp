#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <deque>
#include <stack>
#include "nfa.h"
#define M 256
#define N 256
#define ERROR -1
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

struct Edge_q;
struct node_v
{
    int mun;
    vector<Node> *n;
    bool endState;
};

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
    p->q.mun = num;
    p->q.n = n;
    p->edges = 0;
    return p;
}

vector<Node> endNode;
void endState(ptr_Node_q p, vector<Node> *n)
{
    for (int i = 0; i < n->size(); ++i) {
        
        if (((*n)[i]->edges) == NULL) {
            p->q.endState = true;
        }
    }
}

vector< struct node_v > finalStatus;
vector< struct node_v > non_finalStatus;
vector<int> edge_Value;
void split_to_N_A(ptr_Node_q node)
{
    if (node->q.endState) {
        finalStatus.push_back(node->q);
    }
    else
    {
        non_finalStatus.push_back(node->q);
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
    split_to_N_A(p);
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


/*NFA转DFA*/
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
  //  endNode = NULL;
}

bool is_have(int c)
{
    for (int i = 0; i < edge_Value.size(); ++i) {
        if (edge_Value[i] == c) {
            return true;
        }
    }
    return false;
}

bool bSplit()
{
    ptr_Node_q node = head;
    int sym;
    bool Had;
    while (node != NULL) {
        ptr_Edge_q edge = node->edges;
		while (edge){
            sym = edge->c;
            Had = is_have(sym);
            if (!Had) {
                edge_Value.push_back(sym);
            }
            edge = edge->next;
		}
        node = node->next;
    }
    return false;
}


/*DFA最小化*/
ptr_Node_q search_Node(int value)
{
    ptr_Node_q node = head;
    while (node != NULL) {
        if (node->num == value) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}


vector< ptr_Node_q > final;
vector< ptr_Node_q > non_final;
void convert(vector<struct node_v> *s, vector< ptr_Node_q > *t)
{
    ptr_Node_q n;
    for (int i = 0; i < s->size(); ++i) {
        int value = (*s)[i].mun;
        n = search_Node(value);
        (*t).push_back(n);
    }
}


bool is_in(ptr_Node_q node, vector< ptr_Node_q > *s)
{
    for (int i = 0; i < s->size(); ++i) {
        if ((*s)[i] == node) {
            return true;
        }
    }
    return false;
}


vector< vector<ptr_Node_q> > all;
vector<ptr_Node_q> which(ptr_Node_q s)
{
    for (int i = 0; i < all.size(); ++i) {
        for (int j = 0; j < all[i].size(); ++j)
        if (s == all[i][j]) {
            return all[i];
        }
    }
    return all[0];   //怎么处理
}



vector<ptr_Node_q> new_split(vector< ptr_Node_q > *s, vector< ptr_Node_q > *t)
{
    vector< ptr_Node_q > vr_less;
    bool Had;
    for (int i = 0; i < s->size(); ++i) {
        Had = is_in((*s)[i], t);
        if (!Had) {
            vr_less.push_back((*s)[i]);
        }
    }
    return vr_less;
}


bool bExistequivalentClass = true;
vector<ptr_Node_q> split(vector< ptr_Node_q > *s)
{
    if ((*s).size() == 1) {
        bExistequivalentClass = false;
        all.push_back(*s);
        return *s;
    }
    ptr_Node_q node;
    ptr_Node_q to;
    bool Had;
    vector<ptr_Node_q> Q;
    vector<ptr_Node_q> Re;
    for (int i = 0; i < edge_Value.size(); ++i) {
        for (int j = 0; j < s->size(); ++j) {
            node = (*s)[j];
            if ( node->edges != NULL && (node->edges->c == edge_Value[i]) ) {
                to = node->edges->to;
                Had = is_in(to, s);  
                if (!Had) {
                    Q.push_back(node);
                }
            }
        }
    }
    if (Q.size() != 0) {
        all.push_back(Q);
        Re = new_split(s, &Q);
        return Re;
    }
    all.push_back(*s);
    return *s;
}


struct Edge_all;
struct Node_all
{
    int num;
    bool endState;
    vector<ptr_Node_q> *q;
    Edge_all *edges;  //为什么一定为指针形式
    Node_all *next;
};
typedef Node_all *ptr_Node_all;


struct Edge_all
{
    int c;
    Node_all *from;
    Node_all *to;
    Edge_all *next;
};
typedef Edge_all *ptr_Edge_all;


static ptr_Node_all miniNode_q_new (int num, vector<ptr_Node_q> *n)
{
    ptr_Node_all p;
    p = (ptr_Node_all)malloc(sizeof(*p));
    p->num = num;
    p->q = n;
    p->edges = 0;
    return p;
}


ptr_Node_all head_All = NULL;
int node_Num = 0;
ptr_Node_all miniDfa_lookupOrInsert (vector<ptr_Node_q> *n)
{
    
    ptr_Node_all node = head_All;
    while (node != NULL){
        if ((*(node->q)) == *n)
            return node;
        node = node->next;
    }
    ptr_Node_all p = miniNode_q_new(node_Num++, n);
    p->endState = false;
    p->next = head_All;
    head_All = p;
    return p;
}


static ptr_Edge_all miniEdge_q_new (ptr_Node_all from
                              , ptr_Node_all to
                              , int c)
{
    ptr_Edge_all p;
    p = (ptr_Edge_all)malloc(sizeof(*p));
    p->c = c;
    p->from = from;
    p->to = to;
    p->next = 0;
    return p;
}


vector<int> num_save;
vector<ptr_Edge_all> edge_save;
bool is_ha(ptr_Node_all f_node,
           ptr_Node_all t_node,
           int value)
{
    for (int i = 0; i < edge_save.size(); ++i) {
        if (edge_save[i]->c == value &&
            edge_save[i]->from == f_node &&
            edge_save[i]->to == t_node) {
            return true;
        }
    }
    return false;
}


void miniDfa_addEdge(vector<ptr_Node_q> *from,
                 vector<ptr_Node_q> *to,
                 int value)
{
    ptr_Node_all f_node = miniDfa_lookupOrInsert (from);
    ptr_Node_all t_node = miniDfa_lookupOrInsert (to);
    
    if (f_node->edges != NULL) {
        if (is_ha(f_node, t_node, value)) {
            return;
        }
    }
    ptr_Edge_all edge = miniEdge_q_new(f_node, t_node, value);
    edge_save.push_back(edge);
    edge->next = f_node->edges;
    f_node->edges = edge;
}


void change_Iden()
{
    ptr_Node_q node = head;
   // vector<ptr_Node_q> from;
   // vector<ptr_Node_q> to;
    while (node != NULL) {
        ptr_Edge_q edge = node->edges;
		while (edge){
            auto from = new vector<ptr_Node_q>;
            *from = which(edge->from);
            auto to = new vector<ptr_Node_q>;
            *to = which(edge->to);
            miniDfa_addEdge(from, to, edge->c);
            edge = edge->next;
		}
        node = node->next;
    }
}

bool is_Exist(vector<ptr_Node_q> Q, unsigned long size)
{
    if (Q.size() == size) {
        bExistequivalentClass = false;
    }
    else
    {
        bExistequivalentClass = true;
    }
    return bExistequivalentClass;
}

void minimization()
{
    unsigned long size;
    bSplit();
    convert(&finalStatus, &final);
    convert(&non_finalStatus, &non_final);
    vector<ptr_Node_q> q = final;
    vector<ptr_Node_q> t = non_final;
    
    while (bExistequivalentClass) {
        size = q.size();
        q = split(&q);
        is_Exist(q, size);
    }

    bExistequivalentClass = true;
    while (bExistequivalentClass) {
        size = t.size();
        t = split(&t);
        is_Exist(t, size);
    }
    change_Iden();
}

int max_state = 0;
int table[M][N];
vector<int> new_Final;
void create_StateTable()
{
    ptr_Node_all node = head_All;
    int node_um;
    int edge_um;
    
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
                table[i][j] = ERROR;
        }
    }
    
	while (node != NULL)
	{
		ptr_Edge_all edge = node->edges;
		while (edge){
            node_um = edge->from->num;
            edge_um = edge->c;
            table[node_um][edge_um] = edge->to->num;
            edge = edge->next;
		}
        if (node->num > max_state) {
            max_state = node->num;
        }
		node = node->next;
	}
}

/*跳进了两次的坑啊！！！！*/
void clear(stack<int> &stack)
{
    while (!(stack).empty()) {
        (stack).pop();
    }
}

char *str;
char getChar()
{
    char c;
    c = *(str++);
    return c;
}

void rollback()
{
    str--;
}


bool num_End(int state)
{
    ptr_Node_all node = head_All;
    while (node != NULL) {
        if (node->num == state) {
            break;
        }
        node = node->next;
    }
    
    if (node == NULL) {
        return false;
    }
    ptr_Node_q q;
    for (int i = 0; i < (node->q)->size(); ++i) {
        for (int j = 0; j < final.size(); ++j) {
            q = (*(node->q))[i];
            if (final[j]->num == q->num ) {
                return true;
            }
        }
    }
    return false;
}

char *input_Str()
{
    char input_Str[256];
    scanf("%s", input_Str);
    char *first = input_Str;
    str = input_Str;
    return first;
}


void  nextToken()
{
    
    char input_Str[256];
    scanf("%s", input_Str);
    char *first = input_Str;
    str = input_Str;
    
    char c;
    int b;
    int state = max_state;
    stack<int> stack;
    
    while (state != ERROR)
    {
        c = getChar();
        if (c == '\0') {
            break;
        }
        if (num_End(state))
        {
            clear(stack);
        }
        stack.push(state);
        b = c;
        state = table[state][b];
    }
    
    while(!num_End(state))
    {
        state = stack.top();
        rollback();
        if (str < first) {
            cout << "Matching is failure";
            break;
        }
    }
    
    while (first < str) {
        cout << *first;
        first++;
    }
}

void miniDfa_print()
{
    create_StateTable();
    
    nextToken();
	cout << "\n\nminiDFA:" << endl;
    ptr_Node_all node = head_All;
	while (node != NULL)
	{
		ptr_Edge_all edge = node->edges;
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

