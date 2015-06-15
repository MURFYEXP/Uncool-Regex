#include <assert.h>
#include <stdio.h>
#include "re.h"
#include "todo.h"


Re re_Eps_new ()
{
  Re_Eps p;
  p = (Re_Eps)malloc(sizeof(*p));
  p->kind = RE_KIND_EPS;
  return (Re)p;
}

Re re_Char_new (int c)
{
  Re_Char p;
  p = (Re_Char)malloc(sizeof(*p));
  p->kind = RE_KIND_CHAR;
  p->c = c;
  return (Re)p;
}

Re re_Concat_new (Re left, Re right)
{
  Re_Concat p;
  p = (Re_Concat)malloc(sizeof(*p));
  p->kind = RE_KIND_CONCAT;
  p->left = left;
  p->right = right;
  return (Re)p;
}

Re re_Alt_new (Re left, Re right)
{
  Re_Alt p;
  p = (Re_Alt)malloc(sizeof(*p));
  p->kind = RE_KIND_ALT;
  p->left = left;
  p->right = right;
  return (Re)p;
}

Re re_Closure_new (Re exp)
{
  Re_Closure p;
  p = (Re_Closure)malloc(sizeof(*p));
  p->kind = RE_KIND_CLOSURE;
  p->exp = exp;
  return (Re)p;
}

void Re_print (Re e)
{
  assert (e);
  switch (e->kind){
  case RE_KIND_EPS:
    printf ("\\eps");
    break;
  case RE_KIND_CHAR:{
    Re_Char p = (Re_Char)e;
    printf ("%d", p->c);
    break;
  }
  case RE_KIND_ALT:{
    Re_Alt p = (Re_Alt)e;
    printf ("(");
    Re_print (p->left);
    printf (") | (");
    Re_print (p->right);
    printf (")");
    break;
  }
  case RE_KIND_CONCAT:{
    Re_Concat p = (Re_Concat)e;
    printf ("(");
    Re_print (p->left);
    printf (")(");
    Re_print (p->right);
    printf (")");
    break;
  }
  case RE_KIND_CLOSURE:{
    Re_Closure p = (Re_Closure)e;
    printf ("(");
    Re_print (p->exp);
    printf (")*");
    break;
  }
  default:
    break;
  }
  return;
}

static int counter = 0;
static int nextNum ()
{
  return counter++;
}

static Nfa Re_thompsonDoit (Nfa nfa,Re e)
{
  assert (e);
  switch (e->kind){
  case RE_KIND_EPS:{
    int from = nextNum();
    int to = nextNum();
    Nfa_addEdge (nfa, from, to, EPS);
    nfa->start = from;
    nfa->accept = to;
    break;
  }
  case RE_KIND_CHAR:{
    Re_Char p = (Re_Char)e;
    int from = nextNum();
    int to = nextNum();
    Nfa_addEdge (nfa, from, to, p->c);
    nfa->start = from;
    nfa->accept = to;
    break;
  }
  case RE_KIND_ALT:{
      Re_Alt p = (Re_Alt)e;
      int first = nextNum();
      Re_thompsonDoit (nfa, p->left);
      int oldStart = nfa->start;
      int oldAccept = nfa->accept;
      Re_thompsonDoit (nfa, p->right);
      int end = nextNum();
      
      Nfa_addEdge(nfa, first, oldStart, EPS);
      Nfa_addEdge(nfa, first, nfa->start, EPS);
      Nfa_addEdge(nfa, nfa->accept, end, EPS);
      Nfa_addEdge(nfa, oldAccept, end, EPS);
      
      nfa->start = first;
      nfa->accept = end;
    break;
  }
  case RE_KIND_CONCAT:{
    Re_Concat p = (Re_Concat)e;
    Re_thompsonDoit (nfa, p->left);
    int oldStart = nfa->start;
    int oldAccept = nfa->accept;
    Re_thompsonDoit (nfa, p->right);
    Nfa_addEdge(nfa, oldAccept, nfa->start,
                EPS);
    nfa->start = oldStart;
    break;
  }
  case RE_KIND_CLOSURE:{
    //TODO();
      Re_Closure p = (Re_Closure)e;
      int first = nextNum();
      Re_thompsonDoit (nfa, p->exp);
      int end = nextNum();
      Nfa_addEdge(nfa, first, nfa->start, EPS);
      Nfa_addEdge(nfa, first, end, EPS);
      Nfa_addEdge(nfa, nfa->accept, nfa->start, EPS);
      Nfa_addEdge(nfa, nfa->accept, end, EPS);
      nfa->start = first;
    break;
  }
  default:
    break;
  }
    return nfa;
}

Node endNode = NULL;
Nfa Re_thompson (Re e)
{
  Nfa nfa = Nfa_new ();
  counter = 0;
  Re_thompsonDoit (nfa, e);
  endNode = nfa->nodes;
  return nfa;
}


