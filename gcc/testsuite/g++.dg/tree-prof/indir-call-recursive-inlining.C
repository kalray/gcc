/* { dg-options "-O2 " } */

#ifndef __KVX__
#define N 10
#else
#define N 5
#endif

class Parent
{
public:
  Parent *object;

  Parent()
  {
       object = this;
  }

  virtual void recurse (int t) = 0;
};

class Child : public Parent
{

  Parent *
  get_object ()
  {
     return this;
  }

public:
  virtual void
  recurse (int t)
  {
    if (t != N)
      for (int i = 0; i < 5; ++i)
        get_object()->recurse(t + 1);
  };
};

Parent *
create_object ()
{
  Child *mod = new Child;
  return mod;
}

int
main (int argc, char **argv)
{
  Parent *parent = create_object ();

  for (int i = 0; i < 5; ++i)
    {
	  parent->recurse (0);
    }

  return 0;
}

