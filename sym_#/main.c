#include <stdio.h>
#define STR(x) #x

#define fput(_list) printf("%s\n",".u_boot_list_2_"#_list"_1")

int main() {
    //int are = 10;
    printf("%s\n", "you "STR(are)"my sunshn");  // 打印输出："num"
	
	fput(123);
    return 0;
}
