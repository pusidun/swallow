// Copyright [2021] <Copyright pusidun@hotmail.com>

#include "swallow/base/exception.h"

class Foo {
public:
    void test() {
        auto bar = std::bind(&Foo::callback, this);
        bar();
        throw swallow::Exception("ooops!");
    }
private:
    void callback() {
        printf("Foo callback is running...");
    }
};

int main() {
    //lambda exception
    try{
        [](){
            throw swallow::Exception("lambda exception test");
        }();
    }catch(swallow::Exception& e){
        printf("catch lambda exception,cause:%s\n", e.what());
        printf("lambda exception backtrace:\n%s", e.printStack());
    }

    Foo foo;
    try{
        foo.test();
    }
    catch(swallow::Exception& e) {
        printf("catch class func exception,cause:%s\n", e.what());
        printf("class func backtrace:%s\n", e.printStack());
    }
    return 0;
}