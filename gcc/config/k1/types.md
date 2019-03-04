(define_attr "type"
 "all,\
  tiny,\
  tiny_x,\
  tiny_y,\
  lite, \
  lite_x, \
  lite_y, \
  alu,\
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
  lsu_load_auxw,\
  lsu_load_auxw_x,\
  lsu_load_auxw_y,\
  lsu_store,\
  lsu_store_x,\
  lsu_store_auxr,\
  lsu_store_auxr_x,\
  lsu_store_auxr_y,\
  lsu,\
  lsu_auxr_auxw,\
  lsu_auxr_auxw_x,\
  lsu_auxr_auxw_y,\
  lsu_x, \
  lsu_atomic_uncached,\
  lsu_atomic_uncached_x,\
  lsu_load_uncached,\
  lsu_load_uncached_x,\
  lsu_load_auxw_uncached,\
  lsu_load_auxw_uncached_x,\
  lsu_load_auxw_uncached_y,\
  mau_acc, \
  mau_acc_x, \
  mau_acc_y"

  (const_string "all"))

(define_attr "class" "other,branch,jump,call,link,return" (const_string "other"))

