#ifndef NFA_H
#define NFA_H

#define EPS (-2)

struct Node_t;
typedef Node_t *Node;
extern Node endNode;

struct Nfa_t
{
  int start;     // start node of an NFA
  int accept;    // accept node of an NFA
  Node_t *nodes;  // a list of all nodes
};
typedef Nfa_t *Nfa;


Nfa Nfa_new ();

void Nfa_addEdge(Nfa nfa, int from, int to, int c);
void Nfa_print (Nfa nfa);

#endif
