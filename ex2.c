#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <fcntl.h>
#include<sys/types.h>


int checkDir(char *arr)// check in which state we are, > or >>
{
    int counter = 0;
    for(int i = 0;i<strlen(arr);i++)
        if(arr[i]=='>')
            counter++;
    return counter;
}

//////////////////////////////////////////////////////////////

int splitArr(char *arr, char** imp,char *check) // find the pipe and tell us if we can use it
{
    for(int i =0; i<2;i++)
    {
        imp[i]=strsep(&arr,check);
        if(imp[i]==NULL)
            break;
    }
    if(imp[1]==NULL)
        return 0;
    return 1;
}

//////////////////////////////////////////////////////////////

int wordCount(char* arr) // count how many words
{
    int state = 0;
    int counter = 0;
    while(*arr)
    {
        if(*arr==' '|| *arr=='\n' || *arr =='\t')
            state = 0;
        else if(state == 0)
        {
            state=1;
            ++counter;
        }
        ++arr;
    }
    return counter;
}

//////////////////////////////////////////////////////////////

char** dimLine(char* arr) // make 2d array from 1d
{
    char* tmp;
    char** retArr;
    char* cpyArr;
    int cnt=0;
    int i = 0;
    int numWords = wordCount(arr)+1;
    
    cpyArr=(char*)malloc(strlen(arr)*sizeof(char*)); // copy arr to not work on the original one 
    retArr=(char**)malloc(numWords*sizeof(char*)); // dimensional array 
    if(retArr==NULL) // check malloc
    {
        perror("ERROR");
        exit(1);
    }

    strcpy(cpyArr,arr);
    tmp = strtok(cpyArr," ");
    while(tmp!=NULL) // using strtok
    {
        cnt=strlen(tmp);
        retArr[i]=(char*)malloc((cnt+1)*sizeof(char)); // give size of 1 words to each cell
        if(retArr[i]==NULL)
        {
            perror("ERROR");
            exit(1);
        }
        strcpy(retArr[i],tmp);
        tmp=strtok(NULL," ");
        i++;
    }
    //free(cpyArr);
    retArr[numWords] = NULL;// last cell = null
    return retArr;
}

//////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

    // needed to print the user and dir

    int size = 50; // give the size to dir
    char buf[size]; // get the dir
    struct passwd *p; // to get the user

    // 3 calculated functions need to be returned when Done

    int lengthLine=0;
    int cntLine=0;
    double avgLine=0;
    int pipeCounter=0;
    int dirCounter =0;
    
    //rest of the needed functs

    char line[510]; // get the input
    pid_t pid; // make a fork
    char** dimArr; // make array for excevp
    int wordsCnt=0;  // count how many words for 

    p=getpwuid(geteuid()); // get user
    getcwd(buf,size); // get dir
   // printf("%s@%s>",p->pw_name , buf); 

    while(strcmp(line, "done")!=0) // as soon as we did not finish the program
    {
        // our input details
        printf("%s@%s>",p->pw_name , buf);
        fgets(line,510,stdin);
        line[strlen(line)-1]='\0'; // put instead \n \0

        // save int log.txt every comand we enter
        int fdr=open(argv[1],O_APPEND|O_WRONLY,NULL); // open the log.txt through argv[1] - could been edited
        write(fdr,line,strlen(line)+1); // write to fdr channel
        close(fdr);

        
        
        //starting to check in what state we are

        if(strcmp(line, "done")==0) // out from loop
            break;

        else if(strcmp(line,"cd")==0)
            printf("Command not supported \n");

        else if(strchr(line,'|') != NULL &&strchr(line,'>')==NULL &&strchr(line,'<')==NULL&&strchr(line,'@')==NULL&&strchr(line,'#')==NULL) // check if pipe needed
        {
            char** leftArr;
            char** rightArr;
            char *helper[2];

            cntLine++;
            pipeCounter++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);

            int x = splitArr(line,helper,"|"); // split the pipe into two functions
            leftArr=dimLine(helper[0]);  // get the first method 
            rightArr=dimLine(helper[1]); // get the second method

            int pipefd[2];
            pid_t p1,p2;

            if(pipe(pipefd)<0)
            {
                perror("FAILURE (pipe for pipe+dir)");
                return;
            }

            p1=fork(); // making fork
            {
                if(p1<0)//iff fail
                {
                    perror("FAILURE(fork p1 for pipe+dir)");
                    exit(0);
                }

                if(p1==0)// child on
                {
                    close(pipefd[0]);
                    dup2(4,1); // redirect
                  //  close(pipefd[1]);

                    if(execvp(leftArr[0],leftArr)<0) // if running function left fail
                    {
                        perror("FAILURE(execvp pipe)");
                        exit(0);
                    }
                }

                else
                {
                    p2=fork(); // second son
                    if(p2<0)
                    {
                        perror("FAILURE fork p2 for pipe");
                        exit(0);
                    }
                    if(p2==0)
                    {
                        close(pipefd[1]);
                        dup2(3,0); //redirect
                        close(pipefd[0]);

                        if(execvp(rightArr[0],rightArr)<0)
                        {
                            perror("FAILURE");
                            exit(0);
                        }
                    }
                    else
                    { // wait for both done  
                        wait(NULL);
                        //wait(NULL);
                        close(pipefd[0]);
                        close(pipefd[1]);
                        }
                }
                
            }
           // printf("%s",helper[0]);
           // printf("%s",helper[1]);
          // execvp(leftArr[0],leftArr);
        for(int i =0; i<wordCount(helper[0])-2;i++)
            free(leftArr[i]);
        free(leftArr);
        for(int i =0; i<wordCount(helper[1])-2;i++)
            free(rightArr[i]);
        free(rightArr);
            
        }

        else if(strchr(line,'|') != NULL&&strchr(line,'>')!=NULL ||strchr(line,'|') != NULL&&strchr(line,'<')!=NULL || strchr(line,'|') != NULL&&strchr(line,'#')!=NULL || strchr(line,'|') != NULL&&strchr(line,'@')!=NULL) // check if pipe needed
        {
            
            char t;
            char** leftArr;
            char** rightArr;
            char** rightArrF;
            char *helper[2];
            char *helper1[2];

            // check in wich case we are
            if(strchr(line,'>')!=NULL)
                t='>';
            else if(strchr(line,'<')!=NULL)
                t='<';
            else if(strchr(line,'@')!=NULL)
                t='@';
            else if(strchr(line,'#')!=NULL)
                t='#';

            // leftarr takes first command / right arr takes dir rightarrf take second command
            cntLine++;
            pipeCounter++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);
            printf("%c",t);
            int y =splitArr(line,helper1,&t);
            int x = splitArr(line,helper,"|"); // split the pipe into two functions
            leftArr=dimLine(helper[0]);  // get the first method 
            rightArr=dimLine(helper1[1]); // get the path
            rightArrF=dimLine(helper1[0]); //get the function

            int pipefd[2];
            pid_t p1,p2;

            if(pipe(pipefd)<0)
            {
                printf("FAILURE");
                exit(1);
            }

            p1=fork(); // making fork
            {
                if(p1<0)//iff fail
                {
                    printf("FAILURE");
                    exit(0);
                }

                if(p1==0)// child on
                {
                    close(pipefd[0]);
                    dup2(4,1); // redirect
                  //  close(pipefd[1]);

                    if(execvp(leftArr[0],leftArr)<0) // if running function left fail
                    {
                        perror("FAILURE");
                        exit(0);
                    }
                }

                else if(t=='>')
                {
                    p2=fork(); // second son
                    if(p2<0)
                    {
                        printf("FAILURE");
                        exit(0);
                    }
                    if(p2==0)
                    {
                       int fd=open(rightArr[0],O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU|S_IRWXO|S_IRWXG); // open / create file 
                          if(fd==-1)
                            perror("FAILURE at OPEN(>)");
                      int re = dup2(fd,STDOUT_FILENO); // change FD from output to input

                      if(re<0)
                        {
                            perror("FAILURE at DUP2(>)");
                            exit(1);
                        }

                      if(execvp(rightArrF[0],leftArr)<0) // check the func
                    {
                        perror("FAILURE at EXECVP(>)");
                        exit(0);
                    }
                       
                    }
                }
                else if(t=='@') /////////////////////////////////////////////////////// @==2>
                {
                    p2=fork();
                    if(p2<0)
                    {
                        perror("FAILURE at(@)");
                    }
                    if(p2==0)
                    {
                        int fd=open(rightArr[0],O_CREAT|O_WRONLY|O_APPEND,S_IRWXU); // open / create file 
                if(fd==-1)
                    printf("FAILURE");
                int re = dup2(fd,STDERR_FILENO); // change FD from output to input
                if(re<0)
                    {
                        perror("FAILURE");
                        exit(1);
                    }
                if(execvp(leftArr[0],leftArr)<0) // check the func
                {
                    perror("error");
                    exit(0);
                    }
                }
                }
                else if(t=='#') /////////////////////////////////////////////////////////////////////// # == >>
                {
                    {
                    p2=fork(); // second son
                     if(p2<0)
            {
                perror("FAILURE at (#)");
            }

            if(p2==0)
            {

                int fd=open(rightArr[0],O_WRONLY|O_CREAT|O_APPEND,S_IRWXU); // open / create file 
                if(fd==-1)
                    perror("FAILURE");
                int re = dup2(fd,STDOUT_FILENO); // change FD from output to input
                if(re<0)
                    {
                        perror("FAILURE at DUP2(#)");
                        exit(1);
                    }
                if(execvp(rightArrF[0],rightArrF)<0) // check the func
                {
                    perror("error");
                    exit(1);
                }
                wait(NULL);
                close(fd);
                
            }
                }                        
                
        }
                wait(NULL);
                 //wait(NULL);
                 close(pipefd[0]);
                 close(pipefd[1]);
        }
        }
        //Redircetion functions
        else if(checkDir(line)==1) // check if we are in >
        {
            
            // helping methods
            char** leftArr;
            char** rightArr;
            char *helper[2];

            cntLine++;
            dirCounter++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);
            dimArr = dimLine(line);

            int x = splitArr(line,helper,">"); // split the pipe into two functions
            leftArr=dimLine(helper[0]);  // get the first method 
            rightArr=dimLine(helper[1]); // get the second method
           // printf("we are here>");
         
            pid_t pid;
            int fd;
           
            pid=fork();
            if(pid<0)
            {
                perror("FAILURE at FORK(>)");
            }
            if(pid==0)
            {
                fd=open(rightArr[0],O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU|S_IRWXO|S_IRWXG); // open / create file 
                if(fd==-1)
                    perror("FAILURE at OPEN(>)");
                int re = dup2(fd,STDOUT_FILENO); // change FD from output to input

                if(re<0)
                    {
                        perror("FAILURE at DUP2(>)");
                        exit(1);
                    }

                if(execvp(leftArr[0],leftArr)<0) // check the func
                {
                    perror("FAILURE at EXECVP(>)");
                    exit(0);
                }
                wait(NULL);
                close(fd);
            } 
        for(int i =0; i<wordCount(helper[0])-2;i++)
            free(leftArr[i]);
        free(leftArr);
        for(int i =0; i<wordCount(helper[1])-2;i++)
            free(rightArr[i]);
        free(rightArr);     
        }

        // if we are in <state
        else if(strchr(line,'<')!=NULL)
        {
             // helping methods
            char** leftArr;
            char** rightArr;
            char *helper[2];

            cntLine++;
            dirCounter++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);
            dimArr = dimLine(line);

            int x = splitArr(line,helper,"<"); // split the pipe into two functions
            leftArr=dimLine(helper[0]);  // get the first method 
            rightArr=dimLine(helper[1]); // get the second method
           // printf("we are here<");
         
            pid_t pid;
            int fd;
           
            pid=fork();
            if(pid<0)
            {
                perror("FAILURE at FORK(<)");
            }

            if(pid==0)
            {
                fd=open(rightArr[0],O_RDONLY); // open / create file 
                if(fd==-1)
                    perror("FAILURE at OPEN(<)");
                dup2(fd,0); // change FD from output to input
            
                if(execvp(leftArr[0],leftArr)<0) // check the func
                {
                    perror("FAILURE at OPEN(<)");
                    exit(0);
                }
                wait(NULL);
                close(fd);
            } 
        for(int i =0; i<wordCount(helper[0])-2;i++)
            free(leftArr[i]);
        free(leftArr);
        for(int i =0; i<wordCount(helper[1])-2;i++)
            free(rightArr[i]);
        free(rightArr);
        }

        //riderciton # = >>
         else if(strchr(line,'#')) // # = >> ive tried to do with >> but with no success unfortunatly.. im sorry but atleast it work
        {
            // helping methods
            char** leftArr;
            char** rightArr;
            char *helper[2];

            cntLine++;
            dirCounter++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);
            dimArr = dimLine(line);

            int x = splitArr(line,helper,"#"); // split the pipe into two functions
            leftArr=dimLine(helper[0]);  // get the first method 
            rightArr=dimLine(helper[1]); // get the second method
           // printf("we are here#");
         
            pid_t pid;
            int fd;
           
            pid=fork();
            if(pid<0)
            {
                perror("FAILURE at (#)");
            }

            if(pid==0)
            {

                fd=open(rightArr[0],O_WRONLY|O_CREAT|O_APPEND,S_IRWXU); // open / create file 
                if(fd==-1)
                    perror("FAILURE");
                int re = dup2(fd,STDOUT_FILENO); // change FD from output to input
                if(re<0)
                    {
                        perror("FAILURE at DUP2(#)");
                        exit(1);
                    }
                if(execvp(leftArr[0],leftArr)<0) // check the func
                {
                    perror("error");
                    exit(1);
                }
                wait(NULL);
                close(fd);
            }
        for(int i =0; i<wordCount(helper[0])-2;i++)
            free(leftArr[i]);
        free(leftArr);
        for(int i =0; i<wordCount(helper[1])-2;i++)
            free(rightArr[i]);
        free(rightArr);
        }

        //we are at 2> = @
        else if(strchr(line,'@')!=NULL) // @ = 2> ... again im sorry but i tried to reubuild things at it make the work so hard.
        {
            // helping methods
            char** leftArr;
            char** rightArr;
            char *helper[2];

            cntLine++;
            dirCounter++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);
            dimArr = dimLine(line);

            int x = splitArr(line,helper,"@"); // split the pipe into two functions
            leftArr=dimLine(helper[0]);  // get the first method 
            rightArr=dimLine(helper[1]); // get the second method
           // printf("we are here>");
         
            pid_t pid;
            int fd;
           
            pid=fork();
            if(pid<0)
            {
                perror("FAILURE");
            }
            if(pid==0)
            {
                fd=open(rightArr[0],O_CREAT|O_WRONLY|O_APPEND,S_IRWXU); // open / create file 
                if(fd==-1)
                    printf("FAILURE");
                int re = dup2(fd,STDERR_FILENO); // change FD from output to input
                if(re<0)
                    {
                        perror("FAILURE");
                        exit(1);
                    }
                if(execvp(leftArr[0],leftArr)<0) // check the func
                {
                    perror("error");
                    exit(0);
                }
                wait(NULL);
                close(fd);
            }
        for(int i =0; i<wordCount(helper[0])-2;i++)
            free(leftArr[i]);
        free(leftArr);
        for(int i =0; i<wordCount(helper[1])-2;i++)
            free(rightArr[i]);
        free(rightArr);
        }
        
      
        else
        {
            cntLine++;
            lengthLine = lengthLine+wordCount(line);
            avgLine = lengthLine/cntLine;
            int l = wordCount(line);
            dimArr = dimLine(line);
            
            pid=fork();
            if(pid==0)
                execvp(dimArr[0],dimArr);
            if(pid==-1)
                printf("Error");
            wait(NULL);

           // printf("%d",wordCount(line));
           for(int i = 0; i<wordCount(line)-2; i++)
                free(dimArr[i]);
            free(dimArr);
        }
       
         
    
    }
    printf("Num of commands:%d \n",cntLine);
    printf("Num of Pipes:%d \n",pipeCounter);
    printf("Num of Redirections:%d \n",dirCounter);
    printf("Total length of all commands:%d \n", lengthLine);
    printf("Average length of all commands:%f \n",avgLine);
    printf("See You Next time ! \n");
    return 0;
}

