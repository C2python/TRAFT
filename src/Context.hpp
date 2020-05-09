#include <string>
#include <iostream>

class UPContext{
    UPContext(const UPContext& other)=default;
    UPContext& operator=(const UPContext& other)=default;
protected:
  virtual void finish(int r) = 0;

  // variant of finish that is safe to call "synchronously."  override should
  // return true.
  virtual bool sync_finish(int r) {
    return false;
  }

 public:
  UPContext() {}
  virtual ~UPContext() {}  
  /*
  * delete this：在定时器中，已经运行的UPContext，定时器并未显示的删除他们，需要UPContext自杀
  */
  virtual void complete(int r) {
    finish(r);
    delete this;
  }
  virtual bool sync_complete(int r) {
    if (sync_finish(r)) {
      delete this;
      return true;
    }
    return false;
  }
};

class TestContext:public UPContext{
    std::string name;
public:
    TestContext(std::string name):name(name){}
    void finish(int r) override{
        dout(0)<<"Context Name: "<<name<<dendl;
    }
};