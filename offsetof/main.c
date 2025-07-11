#include <stddef.h>
#include <stdio.h>
struct MyStruct {
    int member1;
    char member2;
    double member3;
    int a;
};

int main() {
    size_t offset = offsetof(struct MyStruct, a);
    printf("Hello World %d\n", ((struct MyStruct *)0)->member3);

    return 0;
}
