#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define paraNum 759498
#define modelSize 9600
#define nums 26


//Declare struct

struct csvTable
{
    char bit[48];
};

typedef struct{
    int *current;    // no place for x and n in readable code :-)
    int *record;
    size_t size;
}Model;

typedef struct {
    int list[3];
    char stat[10];
}stat_list;


//Declare function
int search(Model *arr, size_t n, long long int target)
{
    // 1st comparison
    if (arr->record[n - 1] >= target)
        return 1;
  
    long long int backup = arr->record[n - 1];
    arr->record[n - 1] = target;
  
    // no termination condition and thus
    // no comparison
    for (long long int i = 0;; i++) {
        // this would be executed at-most n times
        // and therefore at-most n comparisons
        if (arr->record[i] == target) {
            // replace arr[n-1] with its actual element
            // as in original 'arr[]'
            arr->record[n - 1] = backup;
  
            // if 'x' is found before the '(n-1)th'
            // index, then it is present in the array
            // final comparison
            if (i < n - 1)
                return 1;
  
            // else not present in the array
            return 0;
        }
    }
}
Model *newVector (size_t sz); 

stat_list *wom_first(int epoch, int para, int begin, int end);
stat_list *wom_second(Model *Wom, int count,int epoch, int para, int begin, int end, int cycle);
stat_list *app_10_first(int epoch, int para, int begin, int end);
stat_list *app_10_second(Model *app, int epoch, int para, int begin, int end, int cycle, int count);
stat_list *app_01_first(int epoch, int para, int begin, int end);
stat_list *app_01_second(Model *app, int epoch, int para, int begin, int end, int cycle, int count);


// Declare table
struct csvTable table[nums][paraNum];
stat_list *retVal;

void init(){
    retVal= malloc (sizeof(stat_list));
}


int main()
{
    // Load file
    for(int i = 0; i < nums; i++){
        printf("%d\n",i);
        char filename[100];
        sprintf(filename,"weight_D_32/weight_32_%d.csv",i);
        FILE* fp = fopen(filename, "r");
        if (fp == NULL){
            printf("Couldn't open file\n");
            return 0;
        }
        for (int counter = 0; counter < paraNum; counter++){
            char entry[100];
            fgets(entry, 100, fp);

            char *temp = strtok(entry, ",");

            if (temp != NULL)
                strcpy(table[i][counter].bit, temp);

            else
                strcpy(table[i][counter].bit, "\0");

        }
        fclose(fp);
    }
    
    // initial model
    Model *baseline, *preSet, *preReset, *Wom, *App_wom;;
    baseline =  newVector(modelSize);
    preSet   =  newVector(modelSize);
    preReset =  newVector(modelSize);
    Wom      =  newVector(modelSize);
    App_wom  =  newVector(modelSize);
    printf("Data be prepared\n");

    for(long long int i = 0; i < modelSize; i++){
        baseline->current[i] = 1;
        baseline->record[i] = 0;
        preSet->current[i] = 1;
        preSet->record[i] = 0;
        preReset->current[i] = 0;
        preReset->record[i] = 0;
        Wom->current[i] = 1;
        Wom->record[i] = 0;
        App_wom->current[i] = 1;
        App_wom->record[i] = 0;
    }

    long long int writes[5];
    long long int SET[5];
    long long int RESET[5];
    long long int SET_pre[5];
    long long int RESET_pre[5];
    long long int SET_lat[5];
    long long int RESET_lat[5];
    long long int epoch[5];
    long long int para[5];

    

    
    //baseline
    int check = 0;
    int cycle = 0;
    long long int write_in = 0;
    long long int set_num = 0;
    long long int reset_num = 0;
    long long int set_pre = 0;
    long long int reset_pre = 0;
    long long int set_lat = 0;
    long long int reset_lat = 0;
    int i,j;
    int round = 0;
    for(i = 0; i < nums; i++){
        //printf("%d\n",i);
        for(j = 0; j< paraNum; j++){
            int set_c = 0;
            int rst_c = 0;
            for(int x = 0; x < 32; x++){
                if( baseline->current[x + 32*cycle] != table[i][j].bit[x] - '0'){
                    baseline->record[x + 32*cycle] += 1;
                    if( table[i][j].bit[x] == '0'){
                        baseline->current[x+32*cycle] = 0;
                        reset_num += 1;
                        rst_c += 1;
                    }
                    else{
                        baseline->current[x+32*cycle] = 1;
                        set_num += 1;
                        set_c += 1;
                    }
                }
            }
            if( set_c > 0)
                set_lat += 1;
            else
                reset_lat += 1;
            write_in += 1;
            cycle += 1;
            cycle %= (modelSize/32);
        }

    }

    printf("Baseline finished.\n");
    sleep(1);
    epoch[0]    =  round-1;
    para[0]      = j;
    writes[0]    = write_in;
    SET[0]       = set_num;
    RESET[0]     = reset_num;
    SET_pre[0]   = set_pre; 
    RESET_pre[0] = reset_pre;
    SET_lat[0]   = set_lat;
    RESET_lat[0] = reset_lat;
    
    //preSet
    set_pre = 0;
    reset_pre = 0;
    write_in  = 0;
    set_num   = 0;
    reset_num = 0;
    check = 0;
    cycle = 0;
    set_lat = 0;
    reset_lat = 0;
    round = 0;
    for(i = 0; i < nums; i++){
            //printf("%d\n",i);
        for(j = 0; j< paraNum; j++){
            
            // Check if need preSET
            int set_c = 0;
            int rst_c = 0;
            int check_pre = 0;
            for(int x = 0; x < 32; x++){
                if(preSet->current[x + 32*cycle] != 1){
                    preSet->record[x + 32*cycle] += 1;
                    check_pre = 1;
                    set_num += 1;
                    set_pre += 1;
                    preSet->current[x + 32*cycle] = 1;
                }
            }
            if(check_pre == 1){
                write_in += 1;
            }

            for(int x = 0; x < 32; x++){
                if( preSet->current[x + 32*cycle] != table[i][j].bit[x] - '0'){
                    preSet->record[x + 32*cycle] += 1;
                    if( table[i][j].bit[x] == '0'){
                        preSet->current[x+32*cycle] = 0;
                        reset_num += 1;
                        rst_c += 1;
                    }
                    else{
                        preSet->current[x+32*cycle] = 1;
                        set_num += 1;
                        set_c += 1;
                    }
                }
            }
            if(set_c > 0)
                set_lat += 1;
            else
                reset_lat += 1;
            write_in += 1;
            cycle += 1;
            cycle %= (modelSize/32);

        }

    }

    printf("preSet finished.\n");
    sleep(1);
    epoch[1]     = round -1;
    para[1]      = j;
    writes[1]    = write_in;
    SET[1]       = set_num;
    RESET[1]     = reset_num;
    SET_pre[1]   = set_pre; 
    RESET_pre[1] = reset_pre;
    SET_lat[1]   = set_lat;
    RESET_lat[1] = reset_lat;

    //preReset
    write_in  = 0;
    set_num   = 0;
    reset_num = 0;
    set_pre = 0;
    reset_pre = 0;
    check = 0;
    cycle = 0;
    set_lat = 0;
    reset_lat = 0;
    round = 0;


    for(i = 0; i < nums; i++){
        //printf("%d\n",i);
        for(j = 0; j< paraNum; j++){
            
            // Check if need preRESET
            int set_c = 0;
            int rst_c = 0;
            int check_pre = 0;
            for(int x = 0; x < 32; x++){
                if(preReset->current[x + 32*cycle] != 0){
                    preReset->record[x + 32*cycle] += 1;
                    check_pre = 1;
                    reset_num += 1;
                    reset_pre += 1;
                    preReset->current[x + 32*cycle] = 0;
                }
            }
            if(check_pre == 1)
                write_in += 1;


            for(int x = 0; x < 32; x++){
                if( preReset->current[x + 32*cycle] != table[i][j].bit[x] - '0'){
                    preReset->record[x + 32*cycle] += 1;
                    if( table[i][j].bit[x] == '0'){
                        preReset->current[x+32*cycle] = 0;
                        reset_num += 1;
                        rst_c += 1;
                    }
                    else{
                        preReset->current[x+32*cycle] = 1;
                        set_num += 1;
                        set_c += 1;
                    }
                }
            }
            write_in += 1;
            cycle += 1;
            cycle %= (modelSize/32);
            if(set_c > 0)
                set_lat += 1;
            else
                reset_lat += 1;

        }

    }

    printf("preReset finished.\n");
    sleep(1);
    epoch[2]  = round - 1;
    para[2]   = j;
    writes[2] = write_in;
    SET[2]    = set_num;
    RESET[2]  = reset_num;
    SET_pre[2]   = set_pre; 
    RESET_pre[2] = reset_pre;
    SET_lat[2]   = set_lat;
    RESET_lat[2] = reset_lat;
    
    // Wom 
    init();
    write_in  = 0;
    set_num   = 0;
    reset_num = 0;
    set_pre = 0;
    reset_pre = 0;
    check = 0;
    cycle = 0;
    reset_lat = 0;
    set_lat = 0;
    round = 0;
    char **stat = malloc((modelSize/3) * sizeof(char *)); // Allocate row pointers
    for(i = 0; i < modelSize/3 ; i++)
        stat[i] = malloc(10 * sizeof(char));  // Allocate each row separately
    for(i = 0; i < (modelSize/3); i++)
        strcpy(stat[i],"U");
 
    for(i = 0; i < nums; i++){
        //printf("%d\n",i);
        for(j = 0; j< paraNum; j++){
            
            int set_c = 0;
            int rst_c = 0;
            int check_wom = 0;
            for(int x = 0+16*cycle; x < 16+16*cycle; x++)
                if(strcmp(stat[x],"WOM_S") == 0)
                    check_wom = 1;
            if(check_wom == 1){        
                for(int x = 0; x < 48; x++){
                    if(Wom->current[x + 48*cycle] != 1){
                        Wom->record[x + 48*cycle] += 1;
                        set_num += 1;
                        set_pre += 1;
                        Wom->current[x + 48*cycle] = 1;
                    }
                }
                for(int x = 0+16*cycle; x < 16+16*cycle; x++)
                    strcpy(stat[x],"U");
                write_in += 1;
                
            }

            int count = 0;
            char current[16][10];
            int new_list[48];
            stat_list *temp;
            for(int x = 0+16*cycle; x < 16+16*cycle; x++){
                if(strcmp(stat[x],"U") == 0 || strcmp(stat[x],"WOM_U") == 0){
                    temp = wom_first(i,j,count*2,count*2+1);
                    new_list[count*3]   = temp -> list[0];
                    new_list[count*3+1] = temp -> list[1];
                    new_list[count*3+2] = temp -> list[2];
                    strcpy(current[count],temp -> stat);
                }

                else if(strcmp(stat[x],"WOM_F") == 0){
                    temp = wom_second(Wom,count,i,j,count*2,count*2+1,cycle);
                    new_list[count*3]   = temp -> list[0];
                    new_list[count*3+1] = temp -> list[1];
                    new_list[count*3+2] = temp -> list[2];
                    strcpy(current[count],temp -> stat);
                }
                count += 1;
            }

            for(int x = 0; x < 48; x++){
                if( Wom->current[x + 48*cycle] != new_list[x]){
                    Wom->record[x + 48*cycle] += 1;
                    if( new_list[x] == 0){
                        Wom->current[x+48*cycle] = 0;
                        reset_num += 1;
                        rst_c += 1;
                    }
                    else{
                        Wom->current[x+48*cycle] = 1;
                        set_num += 1;
                        set_c += 1;
                    }
                }
            }

            if(set_c > 0)
                set_lat += 1;
            else
                reset_lat += 1;
            write_in += 1;
            count = 0;
            for(int x = 0+16*cycle; x < 16+16*cycle; x++){
                strcpy(stat[x],current[count]);
                count += 1;
            }
            
            cycle += 1;
            cycle %= (modelSize/48);

        }

    }

    printf("Wom finished.\n");
    sleep(1);
    epoch[3]  = round - 1;
    para[3]   = j;
    writes[3] = write_in;
    SET[3]    = set_num;
    RESET[3]  = reset_num;
    SET_pre[3]   = set_pre; 
    RESET_pre[3] = reset_pre;
    SET_lat[3]   = set_lat;
    RESET_lat[3] = reset_lat;
    
    //App-wom
    write_in  = 0;
    set_num   = 0;
    reset_num = 0;
    set_pre = 0;
    reset_pre = 0;
    check = 0;
    cycle = 0;
    set_lat = 0;
    reset_lat = 0;
    round = 0;
    int wom_order[5][2] = { {0,9}, {1,2}, {3,4}, {5,6}, {7,8} };
    for(i = 0; i < (modelSize/3); i++)
        strcpy(stat[i],"U");

    for(i = 0; i < nums; i++){
        //printf("%d\n",i);
        for(j = 0; j< paraNum; j++){
        
            int set_c = 0;
            int rst_c = 0;
            int check_wom = 0;
            int check_10  = 0;
            int check_01  = 0;
                
            for(int x = 0+16*cycle; x < 16+16*cycle; x++){
                if(strcmp(stat[x],"WOM_S") == 0)
                    check_wom = 1;
                else if(strcmp(stat[x],"APP_10_S") == 0)
                    check_10 = 1;
                else if(strcmp(stat[x],"APP_01_S") == 0)
                    check_01 = 1;
            }

            if(check_10 == 1 || check_01 == 1){
                if(check_10 == 1){       
                    for(int x = 16; x < 48; x++){
                        if(App_wom->current[x + 48*cycle] != 0){
                            App_wom->record[x + 48*cycle] += 1;
                            reset_num += 1;
                            reset_pre += 1;
                            
                            App_wom->current[x + 48*cycle] = 0;
                        }
                    }
                    for(int x = 5+16*cycle; x < 16+16*cycle; x++)
                        strcpy(stat[x],"APP_01_U");
                    write_in += 1;
                }
                else if(check_01 == 1){
                    for(int x = 16; x < 48; x++){
                        if(App_wom->current[x + 48*cycle] != 1){
                            App_wom->record[x + 48*cycle] += 1;
                            set_num += 1;
                            set_pre += 1;
                        
                            App_wom->current[x + 48*cycle] = 1;
                        }
                    }
                    for(int x = 5+16*cycle; x < 16+16*cycle; x++)
                        strcpy(stat[x],"APP_10_U");
                    write_in += 1;
                }
            }

            if(check_wom == 1){
                        
                for(int x = 0; x < 16; x++){
                    if(App_wom->current[x + 48*cycle] != 1){
                        App_wom->record[x + 48*cycle] += 1;
                        set_num += 1;
                        set_pre += 1;
                        App_wom->current[x + 48*cycle] = 1;
                    }
                }
                for(int x = 0+16*cycle; x < 5+16*cycle; x++)
                    strcpy(stat[x],"U");
                
                write_in += 1;
            }

            
                


            int count = 0;
            char current[16][10];
            int new_list[48];
            stat_list *temp;
            for(int x = 0+16*cycle; x < 16+16*cycle; x++){
                if(strcmp(stat[x],"U") == 0 || strcmp(stat[x],"WOM_U") == 0 || strcmp(stat[x],"APP_10_U") == 0 || strcmp(stat[x],"APP_01_U") == 0 ){
                    if( count < 5 ){
                        temp = wom_first(i,j,wom_order[count][0],wom_order[count][1]);
                        new_list[count*3]   = temp -> list[0];
                        new_list[count*3+1] = temp -> list[1];
                        new_list[count*3+2] = temp -> list[2];
                        strcpy(current[count],temp -> stat);
                    }
                    else{

                        if(strcmp(stat[x],"U") == 0 || strcmp(stat[x],"APP_10_U") == 0 ){
                            temp = app_10_first(i,j,count*2-count+5,count*2-count+16);
                            new_list[count*3]   = temp -> list[0];
                            new_list[count*3+1] = temp -> list[1];
                            new_list[count*3+2] = temp -> list[2];
                            strcpy(current[count],temp -> stat);
                        }

                        else{
                            temp = app_01_first(i,j,count*2-count+5,count*2-count+16);
                            new_list[count*3]   = temp -> list[0];
                            new_list[count*3+1] = temp -> list[1];
                            new_list[count*3+2] = temp -> list[2];
                            strcpy(current[count],temp -> stat);

                        }
                    }
                }

                else if(strcmp(stat[x],"WOM_F") == 0){
                    temp = wom_second(App_wom,count,i,j,wom_order[count][0],wom_order[count][1],cycle);
                    new_list[count*3]   = temp -> list[0];
                    new_list[count*3+1] = temp -> list[1];
                    new_list[count*3+2] = temp -> list[2];
                    strcpy(current[count],temp -> stat);
                }

                else if(strcmp(stat[x],"APP_10_F") == 0){
                    temp = app_10_second(App_wom,i,j,count*2-count+5,count*2-count+16,cycle,count);
                    new_list[count*3]   = temp -> list[0];
                    new_list[count*3+1] = temp -> list[1];
                    new_list[count*3+2] = temp -> list[2];
                    strcpy(current[count],temp -> stat);
                } 

                else if(strcmp(stat[x],"APP_01_F") == 0){
                    temp = app_01_second(App_wom,i,j,count*2-count+5,count*2-count+16,cycle,count);
                    new_list[count*3]   = temp -> list[0];
                    new_list[count*3+1] = temp -> list[1];
                    new_list[count*3+2] = temp -> list[2];
                    strcpy(current[count],temp -> stat);
                } 

                count += 1;
            }

            for(int x = 0; x < 48; x++){
                if( App_wom->current[x + 48*cycle] != new_list[x]){
                    App_wom->record[x + 48*cycle] += 1;
                    if( new_list[x] == 0){
                        App_wom->current[x+48*cycle] = 0;
                        reset_num += 1;
                        rst_c += 1;
                    }
                    else{
                        App_wom->current[x+48*cycle] = 1;
                        set_num += 1;
                        set_c += 1;
                    }
                }
            }
            write_in += 1;
            count = 0;
            for(int x = 0+16*cycle; x < 16+16*cycle; x++){
                strcpy(stat[x],current[count]);
                count += 1;
            }
            
            if(set_c > 0)
                set_lat += 1;
            else
                reset_lat += 1;
            cycle += 1;
            cycle %= (modelSize/48);
        }
    }

    printf("App-wom finished.\n");
    sleep(1);
    epoch[4]  = round - 1;
    para[4]   = j;
    writes[4] = write_in;
    SET[4]    = set_num;
    RESET[4]  = reset_num;
    SET_pre[4]   = set_pre; 
    RESET_pre[4] = reset_pre;
    SET_lat[4] = set_lat;
    RESET_lat[4] = reset_lat;
    
    long long int iteration[5];
    for(i = 0; i < 5; i++)
        iteration[i] = epoch[i] * paraNum + para[i];

    FILE *fp;
    fp = fopen("ans.txt","a+");
    fprintf(fp,"\nIteration:     ");
    for(i = 0; i<5; i++)
        fprintf(fp,"%8lld ,", iteration[i]);
    fprintf(fp,"\nWrites:    ");
    for(i = 0; i<5; i++)
        fprintf(fp,"%8lld ,", writes[i]);
    fprintf(fp,"\nSET:       ");
    for(i = 0; i<5; i++)
        fprintf(fp,"%8lld ,", SET[i]);
    fprintf(fp,"\nRESET:     ");
    for(i = 0; i<5; i++)
        fprintf(fp,"%8lld ,", RESET[i]);
    fprintf(fp,"\nSET pre:   ");
    /*for(i = 0; i<5; i++)
        printf("%8lld ", SET_pre[i]);
    printf("\nRESET pre: ");
    for(i = 0; i<5; i++)
        printf("%8lld ", RESET_pre[i]);*/
    fprintf(fp,"\nSET lat:   ");
    for(i = 0; i<5; i++)
        fprintf(fp,"%8lld ,", SET_lat[i]);
    fprintf(fp,"\nRESET lat: ");
    for(i = 0; i<5; i++)
        fprintf(fp,"%8lld ,", RESET_lat[i]);
    fprintf(fp,"\n");
    
    long long int wom_count[16];
    long long int app_count[16];
    for(i = 0; i < 16; i++){
        wom_count[i] = 0;
        app_count[i] = 0;
    }
    for(i = 0; i < (modelSize/3); i++){
        int a = i % 16;
        wom_count[a] += Wom->record[3*i] + Wom -> record[3*i+1] + Wom -> record[3*i+2];
        app_count[a] += App_wom->record[3*i] + App_wom -> record[3*i+1] + App_wom -> record[3*i+2];
    } 

    fprintf(fp,"Wom: ");
    for(i = 0; i<16; i++)
        fprintf(fp,"%lld ,",wom_count[i]);
    fprintf(fp,"\nApp: ");
    for(i = 0; i<16; i++)
        fprintf(fp,"%lld ,",app_count[i]);
    fprintf(fp,"\n");
    fclose(fp);

    return 0;
}

Model *newVector (size_t sz){
    // Try to allocate vector structure.

    Model *retVal2 = malloc (sizeof (Model));
    if (retVal2 == NULL)
        return NULL;

    // Try to allocate vector data, free structure if fail.

    retVal2->current = malloc (sz * sizeof (int));
    retVal2->record = malloc (sz * sizeof (int));
    if (retVal2->current == NULL) {
        free (retVal2);
        return NULL;
    }
    if (retVal2->record == NULL) {
        free (retVal2);
        return NULL;
    }
    // Set size and return.

    retVal2->size = sz;
    return retVal2;
}

stat_list *wom_first(int epoch, int para, int begin, int end){

    //stat_list *retVal = malloc (sizeof(stat_list));
    if( table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 1;
        retVal->list[1] = 1;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"WOM_U");
    }

    else if(table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 1;
        retVal->list[1] = 1;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"WOM_F");
    }

    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 1;
        retVal->list[1] = 0;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"WOM_F");
    }
    
    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 0;
        retVal->list[1] = 1;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"WOM_F");
    }

    return retVal;
}

stat_list *wom_second(Model *Wom, int count, int epoch, int para, int begin, int end, int cycle){


    if( table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 0;
        retVal->list[1] = 0;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"WOM_S");
    }

    else if(table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end]  == '1'){
        if(Wom -> current[ 3*count + 48*cycle] == 1 && Wom -> current[ 3*count + 1 + 48*cycle] == 1 && Wom -> current[ 3*count + 2 + 48*cycle] == 0 ){
            retVal->list[0] = 1;
            retVal->list[1] = 1;
            retVal->list[2] = 0;
            strcpy(retVal->stat,"WOM_F");
        }
        else{
            retVal->list[0] = 0;
            retVal->list[1] = 0;
            retVal->list[2] = 1;
            strcpy(retVal->stat,"WOM_S");
        }
    }

    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '0'){
        if(Wom -> current[ 3*count + 48*cycle] == 1 && Wom -> current[ 3*count + 1 + 48*cycle] == 0 && Wom -> current[ 3*count + 2 + 48*cycle] == 1 ){
            retVal->list[0] = 1;    
            retVal->list[1] = 0;
            retVal->list[2] = 1;
            strcpy(retVal->stat,"WOM_F");
        }
        else{
            retVal->list[0] = 0;
            retVal->list[1] = 1;
            retVal->list[2] = 0;
            strcpy(retVal->stat,"WOM_S");
        }
    }
    
    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '1'){
        if(Wom -> current[ 3*count + 48*cycle] == 0 && Wom -> current[ 3*count + 1 + 48*cycle] == 1 && Wom -> current[ 3*count + 2 + 48*cycle] == 1 ){
            retVal->list[0] = 0;
            retVal->list[1] = 1;
            retVal->list[2] = 1;
            strcpy(retVal->stat,"WOM_F");
        }
        else{
            retVal->list[0] = 1;
            retVal->list[1] = 0;
            retVal->list[2] = 0;
            strcpy(retVal->stat,"WOM_S");
        }
    }

    return retVal;
}

stat_list *app_10_first(int epoch, int para, int begin, int end){

    //stat_list *retVal = malloc (sizeof(stat_list));
    if( table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 1;
        retVal->list[1] = 1;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"APP_10_U");
    }

    else if(table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 1;
        retVal->list[1] = 1;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"APP_10_U");
    }

    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 1;
        retVal->list[1] = 0;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"APP_10_F");
    }
    
    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 0;
        retVal->list[1] = 1;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"APP_10_F");
    }

    return retVal;
}

stat_list *app_10_second(Model *app, int epoch, int para, int begin, int end, int cycle, int count){

    //stat_list *retVal = malloc (sizeof(stat_list));
    if( table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 0;
        retVal->list[1] = 0;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"APP_10_S");
    }

    else if(table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 0;
        retVal->list[1] = 0;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"APP_10_S");
    }

    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = app -> current[count*3 + 48*cycle];
        retVal->list[1] = app -> current[count*3 + 2 + 48*cycle] ;
        retVal->list[2] = app -> current[count*3 + 1 + 48*cycle] ;
        strcpy(retVal->stat,"APP_10_F");
    }
    
    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = app -> current[count*3 + 48*cycle];
        retVal->list[1] = app -> current[count*3 + 2 + 48*cycle] ;
        retVal->list[2] = app -> current[count*3 + 1 + 48*cycle] ;
        strcpy(retVal->stat,"APP_10_F");
    }

    return retVal;

}

stat_list *app_01_first(int epoch, int para, int begin, int end){

    //stat_list *retVal = malloc (sizeof(stat_list));
    if( table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 0;
        retVal->list[1] = 0;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"APP_01_U");
    }

    else if(table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 0;
        retVal->list[1] = 0;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"APP_01_U");
    }

    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 0;
        retVal->list[1] = 1;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"APP_01_F");
    }
    
    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 1;
        retVal->list[1] = 0;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"APP_01_F");
    }

    return retVal;
}

stat_list *app_01_second(Model *app, int epoch, int para, int begin, int end, int cycle, int count){

    //stat_list *retVal = malloc (sizeof(stat_list));
    if( table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = 1;
        retVal->list[1] = 1;
        retVal->list[2] = 1;
        strcpy(retVal->stat,"APP_01_S");
    }

    else if(table[epoch][para].bit[begin] == '0' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = 1;
        retVal->list[1] = 1;
        retVal->list[2] = 0;
        strcpy(retVal->stat,"APP_01_S");
    }

    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '0'){
        retVal->list[0] = app -> current[count*3 + 48*cycle];
        retVal->list[1] = app -> current[count*3 + 2 + 48*cycle] ;
        retVal->list[2] = app -> current[count*3 + 1 + 48*cycle] ;
        strcpy(retVal->stat,"APP_01_F");
    }
    
    else if(table[epoch][para].bit[begin] == '1' && table[epoch][para].bit[end] == '1'){
        retVal->list[0] = app -> current[count*3 + 48*cycle];
        retVal->list[1] = app -> current[count*3 + 2 + 48*cycle] ;
        retVal->list[2] = app -> current[count*3 + 1 + 48*cycle] ;
        strcpy(retVal->stat,"APP_01_F");
    }

    return retVal;

}








