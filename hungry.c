#include <stdio.h>
#include <stdlib.h>

int main(){
    
    int i=1;
    int mem =0 ;
    while(1){
        void *ptr = malloc(1000);
        printf("%d kb : malloced at %p\n",i,ptr);
        // printf("Allocated %d mb memory\n",i);
        i++;
        // sleep(0.01);

        if(i > 120000){
            break;
        }
    }
    return 0;
}