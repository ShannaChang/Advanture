/*Tien-Lung, Chang
#changti@onid.oregonstate.edu
Program 2
 */
#include <stdio.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>

# define ROOM_NAME ".rooms."
# define CREATE_ROOM_NAME(_BUF_,_PID_) sprintf(_BUF_,"%s%s%d",getenv("LOGNAME"),ROOM_NAME,_PID_);
# define FILE_FLAGS (O_CREAT | O_RDWR | O_EXCL)
# define FILE_PERMISSIONS (S_IRUSR | S_IWUSR | S_IRGRP)
# define buffer_size 1024

int main(int argc, char **argv)
{
    int i,j,k,s,g,u,count_step,check,check_room_conn,check_pre,start_room_num,end_room_num,check_enter_opt;
    int Rand_room_name,Rand_num_of_conn,Rand_room_conn,first_open,conn_match,conn_match_num;
    int open_dir,file_room,find_room,temp_room,path_result;
    char RNAME_BUFFER[buffer_size];
    char wr_room_name[buffer_size],wr_room_type[buffer_size],wr_room_conn[buffer_size];
    const char *fName[11];
    int name_list[8],conn_rep[8],temp_rep[8];
    char message[buffer_size],read_buffer[buffer_size],path_buffer[buffer_size];
    char pre_step[buffer_size],pre_pre_step[buffer_size];
    FILE *path;

    //create directory name
    CREATE_ROOM_NAME(RNAME_BUFFER,getpid());
    open_dir = mkdir(RNAME_BUFFER, 0755);
    if (open_dir < 0)
    {
    	printf("create dir error");
    	exit(0);
    }

    //change directory and generate 7 room in this directory
    chdir(RNAME_BUFFER);

    //generate room name list
    fName[1] = "Covell";
    fName[2] = "Langton";
    fName[3] = "MU";
    fName[4] = "Poling";
    fName[5] = "Dixon";
    fName[6] = "Weniger";
    fName[7] = "Kelley";
    fName[8] = "LPSC";
    fName[9] = "Gilfillan";
    fName[10] = "Reser_Stadium";

    srand(time(NULL));

    //generate 7 room 
    for(i=1;i<=7;i++)
    {
        Rand_room_name = (rand()%10) + 1;
        check = 0;

        //check room name repetition
        for(j=1;j<i;j++)
        {
            if(Rand_room_name == name_list[j])
            {
                i--;
                check = 1;
            }
        }
        if(check == 0)
        {
            name_list[i] = Rand_room_name;
            file_room = open(fName[Rand_room_name], FILE_FLAGS, FILE_PERMISSIONS);
            if(file_room <0)
            {
                printf("unamle to create a room.\n");
                exit(0);
            }
            //write room name into file
            sprintf(wr_room_name, "ROOM NAME: %s\n", fName[Rand_room_name]);
            write(file_room,wr_room_name,strlen(wr_room_name));
            memset(wr_room_name, 0, sizeof(wr_room_name));

            //write room type into file
            if(i==1)
            {
                start_room_num = Rand_room_name;
            }
            else if(i==7)
            {
                end_room_num = Rand_room_name;
                sprintf(wr_room_type, "ROOM TYPE: END_ROOM\n");
                write(file_room,wr_room_type,strlen(wr_room_type));
                memset(wr_room_type, 0, sizeof(wr_room_type));
            }
            else
            {
                sprintf(wr_room_type, "ROOM TYPE: MID_ROOM\n");
                write(file_room,wr_room_type,strlen(wr_room_type));
                memset(wr_room_type, 0, sizeof(wr_room_type));
            }
            close(file_room);
        }
    }
    count_step = 0;
    //generate first room description.
    first_open = open(fName[start_room_num], O_RDWR);
    if(first_open>0)
    {
        sprintf(wr_room_name, "ROOM NAME: %s\n", fName[start_room_num]);
        write(first_open,wr_room_name,strlen(wr_room_name));
        memset(wr_room_name, 0, sizeof(wr_room_name));

        printf("CURRENT LOCATION: %s\n",fName[start_room_num]);
        printf("POSSIBLE CONNECTIONS: ");
        Rand_num_of_conn = (rand()%4) + 3;
        for(k=1;k<=Rand_num_of_conn;k++)
        {
            Rand_room_conn = (rand()%7) + 1;
            check_room_conn = 0;
            for(s=1;s<=k;s++)
            {
                if(Rand_room_conn == conn_rep[s])
                {
                    k--;
                    check_room_conn = 1;
                }
            }
            if(name_list[Rand_room_conn] == start_room_num)
            {
                k--;
                check_room_conn = 1;
            }
            if(check_room_conn == 0)
            {
                conn_rep[k] = Rand_room_conn;
                printf("%s ",fName[name_list[Rand_room_conn]]);
                sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                write(first_open,wr_room_conn,strlen(wr_room_conn));
                memset(wr_room_conn, 0, sizeof(wr_room_conn));
            }
        }
        sprintf(wr_room_type, "ROOM TYPE: START_ROOM\n");
        write(first_open,wr_room_type,strlen(wr_room_type));
        memset(wr_room_type, 0, sizeof(wr_room_type));
    }
    close(first_open);
    strcpy(pre_step,fName[start_room_num]);
    printf("\nWHERE TO? >");
    count_step = 0;
    conn_match = 0;
    conn_match_num == Rand_num_of_conn;
    
    //game interface start from here
    while(fgets(message, sizeof(message), stdin)!=NULL)
    {
        message[strlen(message)-1] = '\0';

        //compare if the input in in the connection or not
        if(count_step == 0){
            for(u=1;u<=Rand_num_of_conn;u++)
            {
                if(strncmp(message, fName[name_list[conn_rep[u]]], strlen(fName[name_list[conn_rep[u]]])) == 0)
                {
                    conn_match = 0;
                    break;
                }
                else
                {
                    conn_match = 1;
                }
            }

        }
        else
        {
            for(i=1;i<=Rand_num_of_conn;i++)
            {
                if(strncmp(message, fName[name_list[conn_rep[i]]], strlen(fName[name_list[conn_rep[i]]])) == 0)
                {
                    conn_match = 0;
                    break;
                }
                else if(check_enter_opt == 1)
                {
                    if(strncmp(message, pre_pre_step, strlen(pre_pre_step)) == 0)
                    {
                        conn_match = 0;
                        break;
                    }
                }
                else
                {
                    conn_match = 1;
                }
            }
        }

        printf("\n");
        //if the connection is match open the room.
        if(conn_match==0)
        {
            find_room = open(message, O_RDWR);
            if(find_room > 0)
            {
                count_step++;
                path = fopen("path","a");
                fprintf(path,"%s\n",message);
                fclose(path);
                check_enter_opt = 0;
                for (i = 1; i < 8; i++)
                {
                    conn_rep[i] = 0;
                }

                check_pre = 0;
                //if the input is end room, print the meesage and exit the program.
                if(strncmp(message, fName[end_room_num], strlen(fName[end_room_num])) == 0)
                {
                    temp_room = open("temp", FILE_FLAGS, FILE_PERMISSIONS);
                    sprintf(wr_room_name, "ROOM NAME: %s\n", message);
                    write(temp_room,wr_room_name,strlen(wr_room_name));
                    memset(wr_room_name, 0, sizeof(wr_room_name));
                    printf("CURRENT LOCATION: %s\n",message);

                    printf("POSSIBLE CONNECTIONS: ");
                    Rand_num_of_conn = (rand()%4) + 3;
                    for(k=1;k<=Rand_num_of_conn;k++)
                    {
                        Rand_room_conn = (rand()%7) + 1;
                        check_room_conn = 0;
                        for(s=1;s<=k;s++)
                        {
                            if(Rand_room_conn == temp_rep[s])
                            {
                                k--;
                                check_room_conn = 1;
                            }
                        }
                        if(check_room_conn == 0)
                        {
                            temp_rep[k] = Rand_room_conn;
                            if(strncmp(pre_step, fName[name_list[Rand_room_conn]], strlen(fName[name_list[Rand_room_conn]])) == 0)
                            {
                                check_pre = 1;
                            }
                            if(check_pre == 0)
                            {
                                if(k==Rand_num_of_conn)
                                {
                                    printf("%s ",pre_step);
                                    sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,pre_step);
                                    write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                    memset(wr_room_conn, 0, sizeof(wr_room_conn));
                                }
                            }
                            if(k!=6)
                            {
                                printf("%s ",fName[name_list[Rand_room_conn]]);
                                sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                                write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                memset(wr_room_conn, 0, sizeof(wr_room_conn));
                            }  
                            else if(k == 6)
                            {
                                if(check_pre == 1)
                                {
                                    printf("%s ",fName[name_list[Rand_room_conn]]);
                                    sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                                    write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                    memset(wr_room_conn, 0, sizeof(wr_room_conn));
                                }
                            }                     
                        }
                    }
                    sprintf(wr_room_type, "ROOM TYPE: END_ROOM\n");
                    write(temp_room,wr_room_type,strlen(wr_room_type));
                    close(temp_room);
                    rename("temp", message);
                    memset(wr_room_type, 0, sizeof(wr_room_type));

                    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
                    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",count_step);
                    path_result = open("path",O_RDONLY);
                    read(path_result,path_buffer,sizeof(path_buffer));
                    printf("%s\n",path_buffer);
                    remove("path");
                    exit(0);
                }
                else if(strncmp(message, fName[start_room_num], strlen(fName[start_room_num])) == 0)
                {
                    //if the room name is first room generate the message.
                    temp_room = open("temp", FILE_FLAGS, FILE_PERMISSIONS);
                    sprintf(wr_room_name, "ROOM NAME: %s\n", message);
                    write(temp_room,wr_room_name,strlen(wr_room_name));
                    memset(wr_room_name, 0, sizeof(wr_room_name));
                    printf("CURRENT LOCATION: %s\n",message);
                    check_enter_opt = 0;
                    printf("POSSIBLE CONNECTIONS: ");
                    Rand_num_of_conn = (rand()%4) + 3;
                    for(k=1;k<=Rand_num_of_conn;k++)
                    {
                        Rand_room_conn = (rand()%7) + 1;
                        check_room_conn = 0;
                        for(s=1;s<=k;s++)
                        {
                            if(Rand_room_conn == conn_rep[s])
                            {
                                k--;
                                check_room_conn = 1;
                            }
                        }
                        if(check_room_conn == 0)
                        {
                            if(strncmp(pre_step, fName[name_list[Rand_room_conn]], strlen(fName[name_list[Rand_room_conn]])) == 0)
                            {
                                check_pre = 1;
                            }
                            if(check_pre == 0)
                            {
                                if(k==Rand_num_of_conn)
                                {
                                    check_enter_opt = 1;
                                    strcpy(pre_pre_step,pre_step);
                                    printf("%s ",pre_step);
                                    sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,pre_step);
                                    write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                    memset(wr_room_conn, 0, sizeof(wr_room_conn));
                                }
                            }
                            if(k!=6)
                            {
                                conn_rep[k] = Rand_room_conn;
                                printf("%s ",fName[name_list[Rand_room_conn]]);
                                sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                                write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                memset(wr_room_conn, 0, sizeof(wr_room_conn));
                            }  
                            else if(k == 6)
                            {
                                if(check_pre == 1)
                                {
                                    conn_rep[k] = Rand_room_conn;
                                    printf("%s ",fName[name_list[Rand_room_conn]]);
                                    sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                                    write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                    memset(wr_room_conn, 0, sizeof(wr_room_conn));
                                }
                            }                     
                        }
                    }
                    sprintf(wr_room_type, "ROOM TYPE: START_ROOM\n");
                    write(temp_room,wr_room_type,strlen(wr_room_type));
                    close(temp_room);
                    rename("temp", message);
                    memset(wr_room_type, 0, sizeof(wr_room_type));
                }
                else//write Mid room description into file
                {
                    temp_room = open("temp", FILE_FLAGS, FILE_PERMISSIONS);
                    sprintf(wr_room_name, "ROOM NAME: %s\n", message);
                    write(temp_room,wr_room_name,strlen(wr_room_name));
                    memset(wr_room_name, 0, sizeof(wr_room_name));
                    printf("CURRENT LOCATION: %s\n",message);
                    check_enter_opt = 0;
                    printf("POSSIBLE CONNECTIONS: ");
                    Rand_num_of_conn = (rand()%4) + 3;
                    for(k=1;k<=Rand_num_of_conn;k++)
                    {
                        Rand_room_conn = (rand()%7) + 1;
                        check_room_conn = 0;
                        for(s=1;s<=k;s++)
                        {
                            if(Rand_room_conn == conn_rep[s])
                            {
                                k--;
                                check_room_conn = 1;
                            }
                        }
                        if(check_room_conn == 0)
                        {
                            if(strncmp(pre_step, fName[name_list[Rand_room_conn]], strlen(fName[name_list[Rand_room_conn]])) == 0)
                            {
                                check_pre = 1;
                            }
                            if(check_pre == 0)
                            {
                                if(k==Rand_num_of_conn)
                                {
                                    check_enter_opt = 1;
                                    strcpy(pre_pre_step,pre_step);
                                    printf("%s ",pre_step);
                                    sprintf(wr_room_conn, "CONNECTION %d: %s\n",k+1,pre_step);
                                    write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                    memset(wr_room_conn, 0, sizeof(wr_room_conn));
                                }
                            }
                            if(k != 6)
                            {
                                conn_rep[k] = Rand_room_conn;
                                printf("%s ",fName[name_list[Rand_room_conn]]);
                                sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                                write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                memset(wr_room_conn, 0, sizeof(wr_room_conn));
                            }
                            else if(k == 6)
                            {
                                if(check_pre == 1)
                                {
                                    conn_rep[k] = Rand_room_conn;
                                    printf("%s ",fName[name_list[Rand_room_conn]]);
                                    sprintf(wr_room_conn, "CONNECTION %d: %s\n",k,fName[name_list[Rand_room_conn]]);
                                    write(temp_room,wr_room_conn,strlen(wr_room_conn));
                                    memset(wr_room_conn, 0, sizeof(wr_room_conn));
                                }
                            } 
                        }
                    }
                    sprintf(wr_room_type, "ROOM TYPE: MID_ROOM\n");
                    write(temp_room,wr_room_type,strlen(wr_room_type));
                    close(temp_room);
                    rename("temp", message);
                    memset(wr_room_type, 0, sizeof(wr_room_type));
                }
                memset(pre_step, 0, sizeof(pre_step));
                strcpy(pre_step,message);
                memset(message, 0, sizeof(message));
                memset(read_buffer, 0, sizeof(read_buffer));

            }  
            else if(find_room == -1)
            {
                //if the room can not open.
                printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");

            }
            close(find_room);
        }
        else{
                //if the input room name doesn't exist.
                printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
            }
        printf("\nWHERE TO? >"); 
    }
}

// Reference:
// generate name:
// http://cboard.cprogramming.com/cplusplus-programming/15080-random-name-generator.html
// rand without repetition:
// http://www.cplusplus.com/forum/general/7784/

