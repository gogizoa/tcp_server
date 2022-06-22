#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio_ext.h>

struct Process {
    int p_id;
    int arrivetime; //파일에서 입력받는값
    int bursttime; //파일에서 입력받는값
    int waittime;
    int remaintime;
    int turnaroundtime;
    int responsetime;
};

int input_data(struct Process p[]){
    // 텍스트 파일을 읽어 main의 프로세스 구조체 포인터 배열에 저장하는 함수
    FILE *fd = fopen("process.txt", "r");
    char data[255];
    char *pdata;
    int i = 1;
    int arrivetime, bursttime;

    while (!feof(fd)) {
       if( fgets(data, sizeof(data), fd) == NULL) break;
        pdata = strtok(data, " ");
        for(int j = 0; j < 2; j++){
            if(j == 0){
                arrivetime = atoi(pdata);
            }
            else{
                bursttime = atoi(pdata);
            }
            pdata = strtok(NULL, " ");
        }
        p[i-1].p_id = i;
        p[i-1].arrivetime = arrivetime;
        p[i-1].bursttime = bursttime;
        p[i-1].remaintime = p[i-1].bursttime;
        i++;
    }
    fclose(fd);
    return i-1; // i개의 프로세스 (id : 1 ~ i)
}

void remove_process(struct Process *p, int pos) {//ready배열 대기수 하나 줄이기 
    p[pos].p_id = 0;  
    p[pos].arrivetime = 0;
    p[pos].bursttime = 0;
    p[pos].remaintime = 0;
    p[pos].waittime = 0;
    p[pos].responsetime = 0;
    p[pos].turnaroundtime = 0;
}

void clear_process(struct Process *p, int process_num) {//배열 초기화
    for(int i=0;i<process_num;i++){
        p[i].waittime = 0;
        p[i].responsetime = 0;
        p[i].turnaroundtime = 0;
    }
}

void SJFscheduler(struct Process *p, int num_process) {
    int origin_numprocess = num_process;
    int currenttime = 0;
    int excute[1000] = {0,};
    struct Process ready[100]; //ready배열
    clear_process(p,num_process);
    int readynum = 0; //ready배열에 있는 프로세스 수
    int processlist[101] = {0,};

    FILE *fd = fopen("pytxt.txt", "w");
    fprintf(fd,"%d\n",num_process);

    while(num_process > 0){
        for(int i=0;i<origin_numprocess;i++){ // 들어온 process를 ready 배열에 넣어준다.
            if(p[i].arrivetime == currenttime){
                memcpy(&ready[readynum], &p[i], sizeof(struct Process));
                readynum++;
                for(int i=0;i<readynum-1;i++){ // ready 배열에 추가되면 정렬을 해준다.
                     for(int j=i+1;j<readynum;j++){
                        if(ready[i].remaintime > ready[j].remaintime){
                            struct Process temp = ready[i];
                            ready[i] = ready[j];
                            ready[j] = temp;
                        }
                    }
                }
            }
        }
        if(readynum > 0) { // ready배열이 하나이상 차있을때
            if(processlist[ready[0].p_id] == 0){ // 실행했던 프로세스 리스트
                processlist[ready[0].p_id] = 1;
                p[ready[0].p_id - 1].waittime = currenttime - p[ready[0].p_id - 1].arrivetime;
                p[ready[0].p_id - 1].responsetime = currenttime;
            }
            excute[currenttime] = ready[0].p_id;
            ready[0].remaintime--;
            fprintf(fd,"%d %d %d\n",ready[0].p_id, currenttime, 1);
            if(ready[0].remaintime == 0){
                p[ready[0].p_id - 1].turnaroundtime = currenttime - p[ready[0].p_id - 1].arrivetime + 1;
                p[ready[0].p_id - 1].responsetime = currenttime - p[ready[0].p_id - 1].responsetime + 1;
                for(int i=0; i<readynum-1; i++) {
                    memcpy(&ready[i],&ready[i+1],sizeof(struct Process));
                }
                remove_process(ready, readynum-1);
                readynum--;
                num_process--;
            }
        } else {
            excute[currenttime] = 0;
        }

        currenttime++;
    }

    fclose(fd);
    int idle_time=0;
    for(int i=0;i<currenttime;i++){
        if(excute[i] == 0 )idle_time++;
    }

    float utilization = (1- ((float)idle_time/(currenttime-1))) * 100;

    printf("\n");
    float aver_response = 0, aver_turnaround = 0, aver_waiting = 0;
    printf("\
+-------+--------+----------+-------+\n\
|PROCESS|RESPONSE|TURNAROUND|WAITING|\n\
|   ID  |  TIME  |   TIME   | TIME  |\n\
+-------+--------+----------+-------+\n");
    for(int i = 0; i < origin_numprocess; i++){
        printf("|  %3d  |", p[i].p_id);
        printf("  %5d |", p[i].responsetime);
        printf("    %5d |", p[i].turnaroundtime);
        printf(" %5d |\n", p[i].waittime);

        aver_response += p[i].responsetime;
        aver_turnaround += p[i].turnaroundtime;
        aver_waiting += p[i].waittime;
    }
    printf("+-------+--------+----------+-------+\n\n");

    aver_response /= origin_numprocess;
    aver_turnaround /= origin_numprocess;
    aver_waiting /= origin_numprocess;

    printf("Total %d Processes\n", origin_numprocess);
    printf("Total Runtime           : %d\n", currenttime-1);
    printf("Utilization             : %.3lf%%\n", utilization);
    printf("Idle Time               : %d\n", idle_time);
    printf("Average Waiting Time    : %.3f\n", aver_waiting);
    printf("Average Turnaround Time : %.3f\n", aver_turnaround);
    printf("Average Response Time   : %.3f\n", aver_response);
    printf("\n");

}

void RRscheduler(struct Process *p, int num_process) {
    // struct Process p[100]; //프로세스 리스트
    // int num_process = input_data(&p); // 파일에서 입력받기 입력받은 프로세스 의 수

    FILE *fd = fopen("pytxt.txt", "w");
    fprintf(fd,"%d\n",num_process);

    int origin_numprocess = num_process;
    int currenttime = 0;
    int timequantum;
    printf("timequantum input\n");
    scanf("%d", &timequantum);
    int excute[1000] = {0,};
    struct Process ready[100]; //ready배열
    clear_process(p,num_process);
    int readynum = 0; //ready배열에 있는 프로세스 수
    int now_excute = 0; // 실행중인지 확인
    int process_excutetime = 0; //각 프로세스에서 실행중인 시간
    int processlist[101] = {0,}; //프로세스 목록

    while(num_process > 0){
        if(readynum>1){
            for(int i=1;i<readynum;i++){
                p[ready[i].p_id - 1].waittime++;
            }
        }

        for(int i=0;i<origin_numprocess;i++){ // 들어온 process를 ready 배열에 넣어준다.
            if(p[i].arrivetime == currenttime){
                memcpy(&ready[readynum], &p[i], sizeof(struct Process));
                readynum++;
            }
        }

        if(now_excute == 0){ //실행중인 프로세스 없음
            if(ready[0].p_id != 0){ //ready에 있는 첫번째 프로세서 실행
                now_excute = 1; //프로세스 실행중으로 바꾸기
                if(processlist[ready[0].p_id] == 0){
                    processlist[ready[0].p_id] = 1;
                    p[ready[0].p_id - 1].responsetime = currenttime;
                }
            } else {
                excute[currenttime] = 0;
                process_excutetime = 0;
            }
        }

        if(now_excute == 1){ //실행중인 프로세스 있을때
            excute[currenttime] = ready[0].p_id; 
            ready[0].remaintime--;
            process_excutetime++;
            if(ready[0].remaintime == 0){ //ready배열에 첫번째 프로세스의 남은시간이 없을때
                p[ready[0].p_id - 1].turnaroundtime = currenttime - p[ready[0].p_id - 1].arrivetime + 1;
                p[ready[0].p_id - 1].responsetime = currenttime - p[ready[0].p_id - 1].responsetime + 1;
                fprintf(fd,"%d %d %d\n",ready[0].p_id, currenttime-process_excutetime+1 , process_excutetime);
                if(readynum > 1){ //ready배열에 대기중인 프로세스가 있을때
                    for(int i=0;i<readynum-1;i++){
                    memcpy(&ready[i],&ready[i+1],sizeof(struct Process)); // ready배열 한칸씩 앞으로 당기기
                    }
                    remove_process(ready,readynum-1);
                } else { //ready배열에 대기중인 프로세스가 없을때
                    remove_process(ready,0);
                    now_excute = 0;
                }
                readynum--;
                num_process--;
                process_excutetime = 0;
            }

            if(timequantum == process_excutetime){ // 실행중인 프로세스가 타임슬라이스만큼 실행했을때
                fprintf(fd,"%d %d %d\n",ready[0].p_id, currenttime-process_excutetime+1 , process_excutetime);
                if(readynum > 1){ //ready배열에 대기 중인 프로세스가 있을때
                    struct Process temp = ready[0];
                    for(int i=0;i<readynum-1;i++){
                        ready[i] = ready[i+1]; //ready배열 앞으로 한칸씩 당기기
                    }
                    ready[readynum-1] = temp; //실행하던 프로세스를 ready배열 맨마지막으로 보내기
                }
                process_excutetime = 0;
                if(processlist[ready[0].p_id] == 0){
                    processlist[ready[0].p_id] = 1;
                    p[ready[0].p_id - 1].responsetime = currenttime+1;
                }
            }
        }
        currenttime++;
    }

    int idle_time=0;
    for(int i=0;i<currenttime;i++){
        if(excute[i] == 0 )idle_time++;
    }

    float utilization = (1- ((float)idle_time/(currenttime-1))) * 100;
    fprintf(fd,"%d",currenttime);
    fclose(fd);

    printf("\n");
    float aver_response = 0, aver_turnaround = 0, aver_waiting = 0;
    printf("\
+-------+--------+----------+-------+\n\
|PROCESS|RESPONSE|TURNAROUND|WAITING|\n\
|   ID  |  TIME  |   TIME   | TIME  |\n\
+-------+--------+----------+-------+\n");
    for(int i = 0; i < origin_numprocess; i++){
        printf("|  %3d  |", p[i].p_id);
        printf("  %5d |", p[i].responsetime);
        printf("    %5d |", p[i].turnaroundtime);
        printf(" %5d |\n", p[i].waittime);

        aver_response += p[i].responsetime;
        aver_turnaround += p[i].turnaroundtime;
        aver_waiting += p[i].waittime;
    }
    printf("+-------+--------+----------+-------+\n\n");

    aver_response /= origin_numprocess;
    aver_turnaround /= origin_numprocess;
    aver_waiting /= origin_numprocess;

    printf("Total %d Processes\n", origin_numprocess);
    printf("Total Runtime           : %d\n", currenttime-1);
    printf("Utilization             : %.3lf%%\n", utilization);
    printf("Idle Time               : %d\n", idle_time);
    printf("Average Waiting Time    : %.3f\n", aver_waiting);
    printf("Average Turnaround Time : %.3f\n", aver_turnaround);
    printf("Average Response Time   : %.3f\n", aver_response);
    printf("\n");
}

int main(){
    int input;
    int ok=0;
    struct Process p[100]; //프로세스 리스트
    int num_process = input_data(p); // 파일에서 입력받기 입력받은 프로세스 의 수
    printf("\n");
    float aver_response = 0, aver_turnaround = 0, aver_waiting = 0;
    printf("\
+-------+--------+----------+\n\
|    1.RR     |    2.SJF    |\n\
|  Scheduler  |  Scheduler  |\n\
+-------+--------+----------+\n\
|PROCESS| ARRIVE |   BURST  |\n\
|   ID  |  TIME  |   TIME   |\n\
+-------+--------+----------+\n");
    for(int i = 0; i < num_process; i++){
        printf("|  %3d  |", p[i].p_id);
        printf("  %5d |", p[i].arrivetime);
        printf("    %5d |\n", p[i].bursttime);
    }
    printf("+-------+--------+----------+\n\n");

    while(!ok){
        printf("Select Scheduler (Input 1 or 2) : ");
        scanf("%d",&input);
        if(input == 1) {
            RRscheduler(p, num_process);
            ok = 1;
        } else if(input == 2) {
            SJFscheduler(p, num_process);
            ok = 1;
        } else {
            printf("Please Input 1 or 2\n");
	    __fpurge(stdin); 
        }
    }
    return 0;
}
