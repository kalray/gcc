(define_attr "type"
 "all,\
  alu_tiny,\
  alu_tiny_x,\
  alu_tiny_y,\
  alu_lite, \
  alu_lite_x, \
  alu_lite_y, \
  alu_full,\
  alu_full_x,\
  alu_full_y,\
  alu_full_copro,\
  alu_lite_x2,\
  alu_tiny_x2,\
  alu_tiny_x2_x,\
  alu_tiny_x2_y,\
  bcu,\
  bcu_get,\
  mau,\
  mau_x,\
  mau_y,\
  mau_auxr, \
  mau_auxr_x, \
  mau_auxr_y, \
  mau_fpu,\
  mau_auxr_fpu,\
  lsu_auxw_atomic,\
  lsu_auxw_atomic_x,\
  lsu_auxw_atomic_y,\
  lsu_auxr_auxw_atomic,\
  lsu_auxr_auxw_atomic_x,\
  lsu_auxr_auxw_atomic_y,\
  lsu_auxw_load,\
  lsu_auxw_load_x,\
  lsu_auxw_load_y,\
  lsu_auxr_store,\
  lsu_auxr_store_x,\
  lsu_auxr_store_y,\
  lsu,\
  lsu_auxr_auxw,\
  lsu_auxr_auxw_x,\
  lsu_auxr_auxw_y,\
  lsu_x, \
  lsu_y, \
  lsu_auxw_load_uncached,\
  lsu_auxw_load_uncached_x,\
  lsu_auxw_load_uncached_y,\
  tca"
  (const_string "all"))

(define_attr "class" "other,branch,jump,call,link,return" (const_string "other"))

