#include"MyHeader.h"
#undef unicode
//HelperClass

namespace CDFS
{
    class Helper
    {
        private:
        PINODE Head=NULL;
        struct SuperBlock SB;
        struct ufdt UFDT[MAXINODE]={0};
        
        public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Name:           Helper
        /// Input:          -
        /// Output:         -
        /// Description:    This CallBack function is for Creating and Iniilizing Blocks(SB,DILB).
        ///////////////////////////////////////////////////////////////////////////////////////////
        Helper()
        {
            PINODE Temp =NULL;
            int i=1;
            SB.TotalFreeInodes=MAXINODE;
            SB.TotalNoOfInodes=MAXINODE;
            //2.Initializing DILB with Data Block
            while(i<=MAXINODE)
            {
                if(Temp==NULL)
                {
                    Temp=new INODE;
                    Head=Temp;
                    Temp->InodeNo=i;
                    Temp->DBPtr=NULL;
                    Temp->FileType=0;
                    Temp->LinkCount=Temp->ReferenceCount=Temp->Permission=0;
                    i++;
                }
                else
                {
                    Temp->next=new INODE;
                    Temp=Temp->next;
                    Temp->InodeNo=i;
                    Temp->DBPtr=NULL;
                    Temp->FileType=0;
                    Temp->LinkCount=Temp->ReferenceCount=Temp->Permission=0;
                    i++;       
                }
            }
        }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Name:           ~Helper
    /// Input:          -
    /// Output:         -
    /// Description:    This CallBack function is for Deleting all Created and Iniilizing Blocks(SB,DILB,UFDT).
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~Helper()
    {
        int i=0;
        PINODE Temp=Head,Temp1=NULL;

        while(i<SB.TotalNoOfInodes)
        {
            if(Temp->DBPtr!=NULL)
            {
                delete(Temp->DBPtr);
                Temp->DBPtr=NULL;

                Temp1=Temp;
                Temp=Temp->next;
                delete (Temp1);

                delete (UFDT[i].PtrFT);
            }
            else
            {
                Temp1=Temp;
                Temp=Temp->next;
                delete (Temp1);

                delete (UFDT[i].PtrFT);
            }
            i++;
            
        }
        cout<<"Inside Destructor\n";
       
    }
    /////////////////////////////////////////////////////////////////////////////
    ////// Name of Function : GetFDFromName                      
    /////  Input            : character *                             
    /////  Output           : integer                                 
    /////  Description      : This function return FileDescriptor from File_name    
    /////////////////////////////////////////////////////////////////////////////
    int GetFDFromName(char *Str)
    {
        int i=0;
        for( ;i<SB.TotalNoOfInodes;i++)
        {
            if(strcmp(UFDT[i].PtrFT->Inodeptr->FileName,Str)==0)
            {
                break;
            }
        }
        if(i==SB.TotalNoOfInodes)
        {
            return -1;//(File Not Exist)
        }
        else
        {
            return i;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    ////// Name of Function : GetInodeFromName                      
    /////  Input            : character *                             
    /////  Output           : integer                                 
    /////  Description      : This function returns Inode Address from DILB of Same FileName  
    /////////////////////////////////////////////////////////////////////////////////////////
    PINODE GetInodeFromName(char * Str)
    {
        int i=0;
        PINODE Temp=Head;
        for(i=1;i<=SB.TotalFreeInodes;i++)
        {
            if(strcmp(Temp->FileName,Str)==0)
            {
                break;
            }
            else
            {
                Temp=Temp->next;
            }
            
        }
        if(i==SB.TotalFreeInodes+1)
        {
            return NULL;
        }
        else
        {
            return Temp;
        }
        
    }

//System Calls
    ////////////////////////////////////////////////////////////////////////////////////
    /// Name:           CreateFile
    /// Input:          character *,integer
    /// Output:         integer
    /// Description:    This function is Creating the New File[1024]Bytes and return fd.
    ////////////////////////////////////////////////////////////////////////////////////
    int CreateFile(char * Str,int Permission)
    {
        int i=0,fd=0;
        PINODE Temp=Head,Newn=NULL,Temp1=NULL;
        
        if((NULL==Str)||(Permission<=0)||(Permission>6))
        {
            return -1;//(Incorrect Parameters)
        }
        if(SB.TotalFreeInodes==0)
        {
            return -2;//(No Inodes available in DILB)
        }
        
        for(i=1;i<=SB.TotalNoOfInodes;i++)
        {
            if(strcasecmp(Temp->FileName,Str)==0)
            {
                return -3;//(Same Name File Occured)
                break;
            }
            else
            {
                Temp=Temp->next;
            }
        }
        Temp=Head;
        while(Temp!=NULL)
        {
            if(Temp->FileType==0)
            {
                break;
            }
            else 
            {
                Temp=Temp->next;
            }
        }
        while(fd<MAXINODE)
        {
            if(UFDT[fd].PtrFT==NULL)
            {
                break;
            }
            else
            {
                fd++;
            }
        }
        //Allocating Memory for FT node and Initializing Members of FT node
        UFDT[fd].PtrFT=new FT;
        if(UFDT[fd].PtrFT==NULL)
        {
            return -4;//(Memory Allocation of FT Failure)
        }
        UFDT[fd].PtrFT->LCount=1;
        UFDT[fd].PtrFT->mode=Permission;
        UFDT[fd].PtrFT->Readoffset=0;
        UFDT[fd].PtrFT->Writeoffset=0;
        UFDT[fd].PtrFT->Inodeptr=Temp;
        //Allocating Memory of DB Block and Initializing Members of Inode
        UFDT[fd].PtrFT->Inodeptr->DBPtr=new DB;
        strcpy(UFDT[fd].PtrFT->Inodeptr->FileName,Str);
        UFDT[fd].PtrFT->Inodeptr->FileSize=MAXFILESIZE;
        UFDT[fd].PtrFT->Inodeptr->FileActualSize=0;
        UFDT[fd].PtrFT->Inodeptr->FileType=REGFILE;
        UFDT[fd].PtrFT->Inodeptr->LinkCount=1;
        UFDT[fd].PtrFT->Inodeptr->ReferenceCount=1;
        UFDT[fd].PtrFT->Inodeptr->Permission=Permission;
        //Decrementing count of TotalFreeInodes
        SB.TotalFreeInodes--;
        return fd;  
    }
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           rm
    /// Input:          character *,integer
    /// Output:         integer
    /// Description:    This function Open the file and return the FileDescriptor.
    //////////////////////////////////////////////////////////////////////////////////
    int rm(char *Str)
    {
        int fd=this->GetFDFromName(Str);
        PINODE Temp=this->GetInodeFromName(Str);
        
        if((fd==-1)||(Temp==NULL))
        {
            return -1;//(File Not Exist)
        }

        (UFDT[fd].PtrFT->Inodeptr->LinkCount)--;

        if(UFDT[fd].PtrFT->Inodeptr->LinkCount==0)
        {
            UFDT[fd].PtrFT->Inodeptr->FileType=0;
            UFDT[fd].PtrFT->Inodeptr->FileActualSize=0;
            strncpy(UFDT[fd].PtrFT->Inodeptr->FileName,"",80);
            UFDT[fd].PtrFT->Inodeptr->Permission=0;
            delete (UFDT[fd].PtrFT->Inodeptr->DBPtr);
            UFDT[fd].PtrFT->Inodeptr->DBPtr=NULL;
            delete(UFDT[fd].PtrFT);
        }

        UFDT[fd].PtrFT=NULL;
        (SB.TotalFreeInodes)++;

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           Open
    /// Input:          character *,integer
    /// Output:         integer
    /// Description:    This function Open the file and return the FileDescriptor.
    //////////////////////////////////////////////////////////////////////////////////
    int Open(char *Str,int Permission)
    {
        int i=this->GetFDFromName(Str);
        PINODE Temp=this->GetInodeFromName(Str);

        if((Str==NULL)||(Permission<=0)||(Permission>6))
        {
            return -1;//(Incorrect Parameters)
        }
        
        if((i==-1)||(Temp==NULL))
        {
            return -2;//(File Not Exist)
        }
        else
        {
            if(Permission > Temp->Permission)
            {
                return -3;//(Permission Denied)
            }
            else
            {
                i=this->GetFDFromName(Str);
                    
            }
        }
        
        UFDT[i].PtrFT=new FT;
        UFDT[i].PtrFT->LCount=1;
        UFDT[i].PtrFT->mode=Permission;
        
        if(Permission==R)
        {
            UFDT[i].PtrFT->Readoffset=0;
        }
        else if (Permission==R+W)
        {
            UFDT[i].PtrFT->Readoffset=0;
            UFDT[i].PtrFT->Writeoffset=0;
        }
        else if(Permission==W)
        {
            UFDT[i].PtrFT->Writeoffset=0;
        }
        
        UFDT[i].PtrFT->Inodeptr=Temp;
        UFDT[i].PtrFT->Inodeptr->ReferenceCount++;

        return i;
    }
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           Close
    /// Input:          character *,
    /// Output:         integer
    /// Description:    This function Close the file which is opened.
    //////////////////////////////////////////////////////////////////////////////////
    int Close(char *Str)
    {
        int i=this->GetFDFromName(Str);
        PINODE Temp=this->GetInodeFromName(Str);
      
        if((i==-1)||(Temp==NULL))
        {
            return -1;//(File Not Exist)
        }

        UFDT[i].PtrFT->Readoffset=0;
        UFDT[i].PtrFT->Writeoffset=0;
        (UFDT[i].PtrFT->Inodeptr->ReferenceCount)--;
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           CloseAll
    /// Input:          character *,
    /// Output:         integer
    /// Description:    This function Close All the files which is opened.
    //////////////////////////////////////////////////////////////////////////////////
    void CloseAll(char *Str)
    {
        int i=0;
        while(i<MAXINODE)
        {
            if(UFDT[i].PtrFT!=NULL)
            {
                UFDT[i].PtrFT->Readoffset=0;
                UFDT[i].PtrFT->Writeoffset=0;
                (UFDT[i].PtrFT->Inodeptr->ReferenceCount)--;
                i++;
            }
            else
            {
                break;
            }
        }

    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Name:           Write
    /// Input:          character *,character *,integer
    /// Output:         integer
    /// Description:    This function Write the Data into the file and return length of written Data.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    int Write(char *Str,char * FData,int DataLen)
    {
        int i=this->GetFDFromName(Str);
        int j=0;
        
        if(((UFDT[i].PtrFT->Inodeptr->Permission==W)||(UFDT[i].PtrFT->Inodeptr->Permission==R+W))&&
           ((UFDT[i].PtrFT->Inodeptr->Permission==W)||(UFDT[i].PtrFT->Inodeptr->Permission==R+W)))
        {
            if(UFDT[i].PtrFT->Inodeptr->FileType!=REGFILE)
            {
                return -3;//(File is not RegularFile)
            }
        }
        else
        {
            return -2;//(Permission Denied)
        }

        if(UFDT[i].PtrFT->Writeoffset==MAXFILESIZE)
        {
            return -4;//(No Memory to write in file)
        }
        
        if(UFDT[i].PtrFT->Inodeptr->FileActualSize==MAXFILESIZE)
        {
            return -4;
        }

        strncpy((UFDT[i].PtrFT->Inodeptr->DBPtr->Block)+(UFDT[i].PtrFT->Writeoffset),FData,DataLen);

        UFDT[i].PtrFT->Writeoffset=UFDT[i].PtrFT->Writeoffset+DataLen;
        UFDT[i].PtrFT->Inodeptr->FileActualSize=UFDT[i].PtrFT->Writeoffset;

        return DataLen;
    }
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           Read
    /// Input:          character *,character *,integer
    /// Output:         integer
    /// Description:    This function is used to read the contetns of file.
    //////////////////////////////////////////////////////////////////////////////////
    int Read(char *Str,char *RArray,int iSize)
    {
        int i=this->GetFDFromName(Str);
        int readsize=0;

        if(i==-1)
        {
            return -1;//(File not Found)
        }

        if((UFDT[i].PtrFT->Readoffset)==(UFDT[i].PtrFT->Inodeptr->FileActualSize))
        {
            return -3;//(Reached at end of actual size of file)
        }

        if(((UFDT[i].PtrFT->mode==R+W)||(UFDT[i].PtrFT->mode==R))&&
            ((UFDT[i].PtrFT->Inodeptr->Permission==R+W)||(UFDT[i].PtrFT->Inodeptr->Permission==R)))
        {
            if(UFDT[i].PtrFT->Inodeptr->FileType!=REGFILE)
            {
                return -4;//(File Type is not regular)
            }
        }
        else
        {
            return -2;//(Permission Denied)
        }
        
        //Logic
        readsize=UFDT[i].PtrFT->Inodeptr->FileActualSize-UFDT[i].PtrFT->Readoffset;

        if(readsize<iSize)
        {
            strncpy(RArray,(UFDT[i].PtrFT->Inodeptr->DBPtr->Block)+(UFDT[i].PtrFT->Readoffset),readsize);
        }
        else
        {
           strncpy(RArray,(UFDT[i].PtrFT->Inodeptr->DBPtr->Block)+(UFDT[i].PtrFT->Readoffset),iSize); 
        }
        
        UFDT[i].PtrFT->Readoffset=UFDT[i].PtrFT->Readoffset+iSize;

        return iSize;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Name:           lseek
    /// Input:          character *,integer integer
    /// Output:         integer
    /// Description:    This function change the offset as per the size & mode and return its updated length.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    int lseek(char * Str,int iSize,int from)
    {
        int fd=this->GetFDFromName(Str);
        if(fd==MAXINODE)
        {
            return -1;//(File not Found)
        }
        if(UFDT[fd].PtrFT==NULL)
        {
            return -2;//(FT Not Available)
        }
        //Read Mode
        if((UFDT[fd].PtrFT->mode==R)||(UFDT[fd].PtrFT->mode==R+W))
        {
            if(from==START)
            {
                if((iSize<0)||(iSize>UFDT[fd].PtrFT->Inodeptr->FileActualSize))
                {
                     return -3;//(Incorrect Parameter)
                }
                 
                UFDT[fd].PtrFT->Readoffset=iSize;

            }
            else if(from==CURRENT)
            {
                if(((UFDT[fd].PtrFT->Readoffset)+iSize)<0)
                {
                    return -3;//(Incorrect Parameter)
                }
                if(((UFDT[fd].PtrFT->Readoffset)+iSize)>(UFDT[fd].PtrFT->Inodeptr->FileActualSize))
                {
                    return -4;//(At End of file)
                }
                
                UFDT[fd].PtrFT->Readoffset=( UFDT[fd].PtrFT->Readoffset)+iSize;
            }
            else if(from==END)
            {
                if(((UFDT[fd].PtrFT->Inodeptr->FileActualSize)+iSize)>=MAXFILESIZE)
                {
                    return -3;//(Incorrect Parameter)
                }
                if(((UFDT[fd].PtrFT->Readoffset)+iSize)<0)
                {
                    return -3;//(Incorrect Parameter)
                }

                UFDT[fd].PtrFT->Readoffset=(UFDT[fd].PtrFT->Inodeptr->FileActualSize)+iSize;

            }
            
        }
        if((UFDT[fd].PtrFT->mode==W)||(UFDT[fd].PtrFT->mode==R+W))
        {
            if(from==START)
            {
                if((iSize<0)||(iSize>UFDT[fd].PtrFT->Inodeptr->FileActualSize))
                {
                     return -3;//(Incorrect Parameter)
                }
                 
                UFDT[fd].PtrFT->Writeoffset=iSize;

            }
            else if(from==CURRENT)
            {
                if(((UFDT[fd].PtrFT->Writeoffset)+iSize)<0)
                {
                    return -3;//(Incorrect Parameter)
                }
                if(((UFDT[fd].PtrFT->Writeoffset)+iSize)>(UFDT[fd].PtrFT->Inodeptr->FileActualSize))
                {
                    return -4;//(At End of file)
                }
                
                UFDT[fd].PtrFT->Writeoffset=( UFDT[fd].PtrFT->Writeoffset)+iSize;
            }
            else if (from==END)
            {
                if(((UFDT[fd].PtrFT->Inodeptr->FileActualSize)+iSize)>=MAXFILESIZE)
                {
                    return -3;//(Incorrect Parameter)
                }
                if(((UFDT[fd].PtrFT->Writeoffset)+iSize)<0)
                {
                    return -3;//(Incorrect Parameter)
                }

                UFDT[fd].PtrFT->Writeoffset=(UFDT[fd].PtrFT->Inodeptr->FileActualSize)+iSize;

            }
            
        }
    return iSize;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Name:           truncate
    /// Input:          character *,
    /// Output:         integer
    /// Description:    This function clears the file All Data.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    int truncate(char *Str)
    {
        int fd=this->GetFDFromName(Str);
        if(fd==-1)
        {
            return -1;
        }

        strncpy(UFDT[fd].PtrFT->Inodeptr->DBPtr->Block," ",MAXFILESIZE);
        UFDT[fd].PtrFT->Inodeptr->FileActualSize=0;
        UFDT[fd].PtrFT->Readoffset=0;
        UFDT[fd].PtrFT->Writeoffset=0;
    }
//Terminal Commands
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           ls
    /// Input:          -
    /// Output:         void
    /// Description:    This Comand is used to show list Directory Contents.
    //////////////////////////////////////////////////////////////////////////////////
    inline void ls()
    {
        PINODE Temp=Head;
        while((Temp!=NULL)&&(Temp->FileType==REGFILE))
        {
            cout<<"File Name:-"<<Temp->FileName<<"\n";
            Temp=Temp->next;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////
    /// Name:           ls
    /// Input:          -
    /// Output:         void
    /// Description:    This Comand is used to show list Directory Contents.
    //////////////////////////////////////////////////////////////////////////////////
    inline void ls(int) 
    {
        PINODE Temp=Head;
        cout<<"Total Files:"<<MAXINODE-SB.TotalFreeInodes<<"\n";
            while((Temp!=NULL)&&(Temp->FileType==REGFILE))
            {
        
                cout<<"\nPermission:\t"<<Temp->Permission<<"\tInode No:\t"<<Temp->InodeNo<<"\tFile Name:-\t"<<Temp->FileName<<"\n";
                Temp=Temp->next;
            }
    }
    ///////////////////////////////////////////////////////////////////////
    ////// Name of Function : DisplayHelp                 
    /////  Input parameters : -                          
    /////  Return value     : void                       
    /////  Description      : Display Help commands      
    /////////////////////////////////////////////////////////////////////
    inline void DisplayHelp() const
    {
        cout<<"ls :To list out all files\n";
        cout<<"clear :To clear console\n";
        cout<<"open :To open the file\n";
        cout<<"close :To close the file\n";
        cout<<"read :To Read the contents from file\n";
        cout<<"write :To write contents into file\n";
        cout<<"exit :To Terminate file systeam\n";
        cout<<"stat :To Display information of file using name\n";
        cout<<"truncate :To Remove all data from file\n";
        cout<<"rm :To delete the file\n";
    }
    ///////////////////////////////////////////////////////////////////////
    ////// Name of Function : man                         
    /////  Input parameters : character *               
    /////  Return value     : void                       
    /////  Description      : Manual Entry of commands
    ///////////////////////////////////////////////////////////////////////
    inline void man(char *name) const 
    {
        if(name==NULL)
        {
        return ;
        }
        if(strcasecmp(name,"create")==0)
        {
            cout<<"Description :Used to create new regular file\n";
            cout<<"Usage :create File_name Permission\n";
        }
        else if(strcasecmp(name,"ls")==0)
        {
            cout<<"Description :List  information  about  the FILEs (the current directory by default\n";
            cout<<"Usage : ls Or ls -l\n";
        }
        else if(strcasecmp(name,"open")==0)
        {
            cout<<"Description :Used to open existing file\n";
            cout<<"Usage :open File_Name Permission\n";
 
        }
        else if(strcmp(name,"read")==0)
        {
            cout<<"Description :Used to read the data from regular file\n";
            cout<<"Usage :read File_name No_Of_Bytes to read";
        }
        else if(strcmp(name,"write")==0)
        {
            cout<<"Description :Used to write the data from regular file\n";
            cout<<"Usage :write File_name\n After this enter the data from file\n";
        }
        else if(strcmp(name,"stat")==0)
        {
            cout<<"Description :Used to display information of file\n";
            cout<<"Usage :stat File_Descriptor\n";
        }
        else if(strcmp(name,"fstat")==0)
        {
            cout<<"Description :Used to display information of file\n";
            cout<<"Usage :stat File_Descriptor\n";
        }
        else if(strcmp(name,"truncate")==0)
        {
            cout<<"Description :Used to remove data from the file\n";
            cout<<"Usage :truncate File_name\n";
        }
        else if(strcmp(name,"close")==0)
        {
            cout<<"Description :Used to close opened file\n";
            cout<<"Usage :close File_name\n";
        }
        else if(strcmp(name,"lseek")==0)
        {
            cout<<"Description :Used to change file offset\n";
            cout<<"Usage :lseek File_name ChangeInOffset StartPoint\n";
        }
        else if(strcmp(name,"rm")==0)
        {
            cout<<"Description :Used to delete the file\n";
            cout<<"Usage :rm File_name\n";
        }
        else
        {
        cout<<"ERROR:No manual entry found\n";
        }
    }
    //////////////////////////////////////////////////////////////////////////////
    ////// Name of Function : fstat                        
    /////  Input parameters : integer               
    /////  Return value     : void                       
    /////  Description      : It accept fd and Display its statistical information
    //////////////////////////////////////////////////////////////////////////////
    inline int fstat(int fd) const
    {
        PINODE Temp=NULL;
        if(fd==-1)
        {
            return -1;//(File Not Exist)
        }
        if(UFDT[fd].PtrFT==NULL)
        {
            return -2;//(File is not opened)
        }
        Temp=UFDT[fd].PtrFT->Inodeptr;

        cout<<"\n---------Statistical Information about file-------------\n";
        cout<<"File name:\t"<<Temp->FileName<<"\n";
        cout<<"Inode Number:\t"<<Temp->InodeNo<<"\n";
        cout<<"Filesize:\t"<<Temp->FileActualSize<<"\n";
        cout<<"Linkcount:\t"<<Temp->LinkCount<<"\n";
        cout<<"Referencecount:\t"<<Temp->ReferenceCount<<"\n";

        if(Temp->Permission==R)
        {
            cout<<"File Permission Read only\n";
        }
        else if(Temp->Permission==W)
        {
            cout<<"File Permission Write \n";
        }
        else if(Temp->Permission=R+W)
        {
            cout<<"File Permission is read and write\n";
        }
        cout<<"------------------------------------------------------------------\n";

    }
    ///////////////////////////////////////////////////////////////////////////////
    ////// Name of Function : stat                        
    /////  Input parameters : character *               
    /////  Return value     : integer                       
    /////  Description      : This Function Display Statistical Information of File
    ////////////////////////////////////////////////////////////////////////////////
    inline int stat(char* Str)
    {
        PINODE Temp=NULL;
        int fd=0;
        fd=this->GetFDFromName(Str);
        if(fd==-1)
        {
            return -1;//(File Not Exist)
        }
        if(UFDT[fd].PtrFT==NULL)
        {
            return -2;//(File is not opened)
        }
        Temp=UFDT[fd].PtrFT->Inodeptr;

        cout<<"\n---------Statistical Information about file-------------\n";
        cout<<"File name:\t"<<Temp->FileName<<"\n";
        cout<<"Inode Number:\t"<<Temp->InodeNo<<"\n";
        cout<<"Filesize:\t"<<Temp->FileActualSize<<"\n";
        cout<<"Linkcount:\t"<<Temp->LinkCount<<"\n";
        cout<<"Referencecount:\t"<<Temp->ReferenceCount<<"\n";

        if(Temp->Permission==R)
        {
            cout<<"File Permission Read only\n";
        }
        else if(Temp->Permission==W)
        {
            cout<<"File Permission Write \n";
        }
        else if(Temp->Permission=R+W)
        {
            cout<<"File Permission is read and write\n";
        }
        cout<<"------------------------------------------------------------------\n";

    }

};

};