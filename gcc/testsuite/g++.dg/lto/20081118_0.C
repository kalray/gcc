/* { dg-lto-do link } */
/* { dg-require-effective-target fpic } */
/* { dg-lto-options {{-fPIC -flto -flto-partition=1to1 -r -nostdlib}} } */
/* { dg-skip-if "PLT not supported yet" { k1-*-* } } */

/* We used to ICE because of dangling pointers.  */

class object
{
public:
  virtual ~object() {}
};

class foo : public object
{
  virtual int method(void);
};

int
foo::method(void)
{
}
