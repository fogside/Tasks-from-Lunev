//
//  main.c
//  reader
//
//  Created by Евгения Ческидова on 02.11.14.
//  Copyright (c) 2014 fogside. All rights reserved.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#define myEOF 32767

int Reader()
{
    int buf = myEOF;
    char char_buf[30];
    int count; // to count how much we read
    int dMainFifo = open("./main_fifo", O_RDONLY);
    while(buf == myEOF)
    {
        read(dMainFifo, (char *) &buf, sizeof(int)); // reading the name of id_fifo
    }
    //printf("MY ID: %d\n",buf);

    char id_fifo_path[100];
    sprintf(id_fifo_path, "./%d", buf);

    //*** open fifo with the name as id ***
    int dIdFifo = open(id_fifo_path, O_RDONLY);

    //===read file from this fifo and write it to stdout ===
    while ((count = read(dIdFifo, char_buf, 10)) > 0)
    {
        write(STDOUT_FILENO, char_buf, (size_t) count);
        fflush(stdout);
    }
    close(dIdFifo);
    close(dMainFifo);
    return 0;

}

int CreateIdFifo()
{
    int k = -1;
    int myID = 1;
    while(k==-1)//if file exists
    {
        myID++; //generating id of fifo
        char id_fifo_path[100];
        sprintf(id_fifo_path, "./%d", myID);
        k = mkfifo(id_fifo_path, 0666);
    }
    return myID;

}
void CreateMainFifo(int* myID)
{
    //*** creating main fifo ***
    mkfifo("./main_fifo", 0666);
    int dMainFifo = open("./main_fifo", O_WRONLY);
    write(dMainFifo, (char *) myID, sizeof(int)); //add self id to the main fifo
    //close(dMainFifo);
}
void WritingFileToFifoID(char *path, int fifoID)
{
    //Open the file to transfer to put it into the idFifo:
    int dFile = open(path, O_RDONLY); // file for transfer
    int count; // to count how much we read
    char buf[30]; //tmp

    //Creating the path to fifoID
    char id_fifo_path[30];
    sprintf(id_fifo_path, "./%d",fifoID);


    int dIdFifo = open(id_fifo_path, O_WRONLY);
    //===write dFile to this fifo===
    while ((count = read(dFile, buf, 10)) > 0)
    {
        write(dIdFifo, buf, (size_t) count);
    }

    close(dFile);
    //close(dIdFifo);
}


int Writer(char *path)
{
    //Firstly we create the id fifo -- pipe between one reader and one writer:
    int fifoID = CreateIdFifo();

    //Creating MainFifo and writing fifoID into it
    CreateMainFifo(&fifoID);

    //open the file to transfer and put it into id_fifo
    WritingFileToFifoID(path, fifoID);

    //====THAT'S ALL!====
    return 0;

}

int main(int argc, char *argv[])
{
    if (argc > 1)
        Writer(argv[1]);
    else
        Reader();
    return 0;
}