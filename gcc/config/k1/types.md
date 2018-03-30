(define_attr "type"
 "all,\
  tiny,\
  tiny_x,\
  tiny_y,\
  lite, \
  lite_x, \
  lite_y, \
  alu,\
  alu0,\
  alu_x,\
  alu_y,\
  bcu,\
  bcu_get,\
  mau,\
  mau_fpu,\
  mau_x,\
  mau_y,\
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
  mau_acc, \
  mau_acc_x, \
  mau_acc_y, \
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

(define_attr "class" "other,branch,jump,call,link,return" (const_string "other"))

