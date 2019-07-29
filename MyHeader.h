#include<iostream>
#include<string.h>


#define MAXINODE 50
#define MAXFILESIZE 1024
#define R 2
#define W 4

#define REGFILE 1

#define START 0
#define CURRENT 1
#define END 2

using namespace std;

//1.Data structures Of H D D
    //1 Boot Block
    //2 Super Block
        struct SuperBlock
        {
            int TotalNoOfInodes;
            int TotalFreeInodes;
        };
    //4 Data Block
        struct DB
        {
            char Block[1024];
        };
    //3 Disk Indexed List Block / IIT
        typedef struct Inode
        {
            int InodeNo;
            char FileName[50];
            int FileType;
            int FileSize;
            int FileActualSize;
            struct DB *DBPtr=NULL;
            int LinkCount;
            int ReferenceCount;
            int Permission;
            struct Inode *next=NULL;
        }INODE,* PINODE;
        
    //Run Time DataStructures of Kernel
        //1 FileTable
        typedef struct FileTable
        {
            int Readoffset;
            int Writeoffset;
            int mode;
            int LCount;
            PINODE Inodeptr=NULL;
        }FT,* PFT;
        //2 U-Area
        struct ufdt 
        {
            PFT PtrFT=NULL;
        };