#include <stdio.h>
#define BUFF_SIZE 300

int compare_files(){
        char filename_searched[] = "weight.txt";
    char filename_virtual[] = "weight_virtual.txt";
    FILE *fs, *fv;
    char buf_s[BUFF_SIZE]={0};    
    char buf_v[BUFF_SIZE]={0};
    fs = fopen(filename_searched, "r");
    fv = fopen(filename_virtual, "r");
    while( (fgets(buf_s, BUFF_SIZE, fs) != NULL) && (fgets(buf_v, BUFF_SIZE, fv) != NULL) ){
        static int cnt=0;
        
        for(int i=0; i < BUFF_SIZE; i++){
            if(buf_s[i] != EOF  && buf_v[i] != EOF){
                if(buf_s[i] == buf_v[i]){
                    // printf("%c, %c\r\n", buf_s[i], buf_v[i]);
                }
                else{
                    printf("i: %d\r\n",i);
                    fclose(fs);
                    fclose(fv);

                    return 0;
                } 
            }
            else{
                fclose(fs);
                fclose(fv);
                printf("ファイルの最後まで読み込みました\r\n");
                return 1;
            }

        }
        cnt++;
        printf("cnt: %d\r\n",cnt);
    }
    fclose(fs);
    fclose(fv);
    return 1;
}
int main(){
    if(compare_files() == 1){
        printf("一致しました\r\n");
    }
    else{
        printf("不一致です\r\n");
    }
    return 0;
}