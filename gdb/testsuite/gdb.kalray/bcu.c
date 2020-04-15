#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// tests
extern int no_bcu_test;
extern int call_test, ra_call_test;
extern int goto_test;
extern int cb_taken_test;
extern int cb_skip_test;
extern int icall_test, ra_icall_test;
extern int igoto_test;
extern int ret_test, ra_ret_test;
extern int loopdo_test;
extern int get_test;
extern int iget_test;
extern int pcrel_no_bcu_test;
extern int pcrel_bcu_test;

// bcu
extern int bpbcu_call_test;
extern int bpbcu_cb_skip_test;
extern int bpbcu_cb_taken_test;
extern int bpbcu_getpc_test;
extern int bpbcu_getra_test;
extern int bpbcu_goto_test;
extern int bpbcu_icall_test;
extern int bpbcu_igetpc_test;
extern int bpbcu_igetra_test;
extern int bpbcu_igoto_test;
extern int bpbcu_loopdo_test_1;
extern int bpbcu_loopdo_test_2;
extern int bpbcu_no_bcu_test;
extern int bpbcu_ret_test;
extern int bpbcu_uret_test;
extern int bpbcu_pcrel_no_bcu_test;
extern int bpbcu_pcrel_bcu_test;

#define MAX_TEST_BCUS 3
#define DECL_BCU(NAME) \
{ \
  "bpbcu_" #NAME, \
  (void *) &bpbcu_ ## NAME \
}

struct bcu_s
{
  const char *name;
  void *addr;
};

struct ret_s
{
  const char *name;
  intptr_t val;
};

struct test_s
{
  const char *name;
  void *addr;
  int nret;
  struct ret_s ret[2];
  struct bcu_s bcu[MAX_TEST_BCUS];
};

uint64_t ret_val[2] = {1,};
uint64_t saved_regs[4] = {1,};

struct test_s tests[] =
{
  {"no bcu", &no_bcu_test, 0, {}, {DECL_BCU(no_bcu_test)}},
  {"get", &get_test, 2, {{"ra", 0}, {"pc", (intptr_t) &bpbcu_getpc_test}},
    {DECL_BCU(getra_test), DECL_BCU(getpc_test)}},
  {"iget", &iget_test, 2, {{"ra", 0}, {"pc", (intptr_t) &bpbcu_igetpc_test}},
    {DECL_BCU(igetra_test), DECL_BCU(igetpc_test)}},
  {"call", &call_test, 1, {{"ra", (intptr_t) &ra_call_test}}, {DECL_BCU(call_test)}},
  {"goto", &goto_test, 0, {}, {DECL_BCU(goto_test)}},
  {"cb taken", &cb_taken_test, 0, {}, {DECL_BCU(cb_taken_test)}},
  {"cb skip", &cb_skip_test, 0, {}, {DECL_BCU(cb_skip_test)}},
  {"icall", &icall_test, 1, {{"ra", (intptr_t) &ra_icall_test}}, {DECL_BCU(icall_test)}},
  {"igoto", &igoto_test, 0, {}, {DECL_BCU(igoto_test)}},
  {"ret", &ret_test, 1, {{"ra", (intptr_t) &ra_ret_test}}, {DECL_BCU(ret_test)}},
  {"loopdo", &loopdo_test, 1, {{"inc", 30}}, {DECL_BCU(loopdo_test_1), DECL_BCU(loopdo_test_2)}},
  {"pcrel no bcu", &pcrel_no_bcu_test, 1, {"pc_plus_0x20", (intptr_t) &bpbcu_pcrel_no_bcu_test + 0x20},
    {DECL_BCU(pcrel_no_bcu_test)}},
  {"pcrel bcu", &pcrel_bcu_test, 2,
    {{"pc_plus_0x30", (intptr_t) &bpbcu_pcrel_bcu_test + 0x30}, {"pc", (intptr_t) &bpbcu_pcrel_bcu_test}},
    {DECL_BCU(pcrel_bcu_test)}},
};
int ntests = sizeof (tests) / sizeof (tests[0]);

const char *msg_change[] = {"invalid value", "reg r0 changed", "reg r1 changed", "reg r12 changed", "reg ra changed"};
extern intptr_t do_test (void *addr_test);
intptr_t utests_r12 = 0;

int main (int argc, char **argv)
{
  int idx_text, idx_ret, result = 0;
  intptr_t ret, ret1;
  struct test_s *test = tests;

  for (idx_text = 0, ret = 0; idx_text < ntests; idx_text++)
  {
    struct test_s *t = &test[idx_text];

    printf ("%s (address %p", t->name, t->addr);
    if (t->nret)
     printf (", expected return");
    for (idx_ret = 0; idx_ret < t->nret; idx_ret++)
      printf (" %s=0x%lx", t->ret[idx_ret].name, t->ret[idx_ret].val);
    printf (") ...\n");

    ret = do_test (t->addr);
    ret1 = 0;
    for (idx_ret = 0; idx_ret < t->nret; idx_ret++)
    {
      if (t->ret[idx_ret].val != ret_val[idx_ret])
      {
        ret1 = 1;
        printf ("%s returned %s=0x%llx ", t->name, t->ret[idx_ret].name, (unsigned long long) ret_val[idx_ret]);
      }
      else if (!strcmp (t->ret[0].name, "ra"))
        ret = 0;
    }

    if (ret >= 1 && ret <= 4)
      printf ("%s %s ", t->name, msg_change[ret]);
    if (ret || ret1)
    {
      printf ("%s\n", "FAILED");
      ret = 1;
      result++;
    }
    else
      printf ("%s\n", "PASSED");
    fflush (stdout);

    if (idx_text == 0)
      test[1].ret[0].val = test[2].ret[0].val = ret_val[0];
  }

  if (!result)
    printf ("\ngdb bcu tests PASSED\n");
  else
    printf ("\ngdb bcu FAILED with code %d\n", result);

  return result;
}

