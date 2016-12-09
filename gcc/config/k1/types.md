(define_attr "type"
 "all,\
  tiny,\
  tiny_x,\
  lite, \
  lite_x, \
  alud_tiny,\
  alud_tiny_x,\
  alud_lite, \
  alud_lite_x, \
  alu,\
  alu0,\
  alu_x,\
  alud,\
  alud_x,\
  alud_y,\
  alud_z,\
  bcu,\
  bcu_get,\
  mau,\
  mau_fpu,\
  mau_x,\
  mau_lsu,\
  mau_lsu_x,\
  mau_lsu_fpu,\
  lsu_atomic,\
  lsu_atomic_x,\

  lsu_load,\
  lsu_load_x,\
  lsu_store,\
  lsu_store_x,\
  lsu,\
  lsu_x, \

  lsu_atomic_uncached,\
  lsu_atomic_uncached_x,\
  lsu_load_uncached,\
  lsu_load_uncached_x,\
  lsu_store_uncached,\
  lsu_store_uncached_x,\

  alu_full_odd, \
  alud_full_odd, \
  alud_full_odd_x, \
  mau_acc, \
  mau_acc_x, \
  mau_acc_odd, \
  and_Ilh, \
  extfz, \
  extfs, \
  insf, \
  abs, \
  clz, \
  ctz, \
  cbs, \
  stsu, \
  sat, \
  adds, \
  cmove, \
  fmuld, \
  faddd, \
  fsbfd, \
  fmulnrn, \
  fmulnd, \
  cmove_x, \
  addhp, \
  sbfhp, \
  abdhp, \
  landhp, \
  addshp, \
  cmovehp, \
  comphp, \
  minuhp, \
  maxhp, \
  minhp, \
  maxuhp, \
  abd, \
  abd_x, \
  bwluhp, \
  shift32"

  (const_string "all"))
