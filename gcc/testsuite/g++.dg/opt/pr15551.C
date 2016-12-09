// PR target/15551
// This used to crash on pentium4-pc-cygwin due to an alloca problem.
// Testcase submitted by Hans Horn to mingw bug tracker
//
// { dg-do run }
// { dg-options "-O3" }

#include <cstring>
#include <fstream>
#include <cstdio>
using namespace std;

ostream* logfile;

int main () {

  logfile = new ofstream("bar", ios::out);

#ifdef STACK_SIZE
  char expList[STACK_SIZE/2]
#else
  char expList[20000];
#endif
  strcpy(expList, "foo");

  delete logfile;
  remove ("bar");

  return 0;
}
