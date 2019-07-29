#include"CDFS.cpp"
using namespace CDFS;

int main()
{
    char *Readptr=NULL;
    char Username[10]="Ravi";
    char Password[10]="12345";
    int i=0,DataLen=0;
    char command[4][80],str[80],Arr[1024],UsrArr[10],PassArr[5];
    int count=0,ret=0;

     //UserName and Password Verification
        cout<<"Enter Username\t";
        cin>>UsrArr;
        if(strcasecmp(UsrArr,Username)==0)
        {
            cout<<"Enter Password\t";
            cin>>PassArr;
            if(strcasecmp(PassArr,Password)==0)
            {
                cout<<"Welcome:\t"<<UsrArr<<"\n";
                i=1;
            }
        } 
        Helper uobj=Helper();

    while(i)
    {
        cout<<"Marvellous-CDFS :~$";
        fgets(str,80,stdin);
        count=sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);
     
        if(count==1)
        {
            if(strcasecmp(command[0],"help")==0)
            {
                uobj.DisplayHelp();
                continue;
            }         
            else if(strcasecmp(command[0],"exit")==0)
            {
                break;
            }   
            else if(strcasecmp(command[0],"ls")==0)
            {
                uobj.ls();
                continue;
            }
            else if(strcasecmp(command[0],"clear")==0)
            {
                system("cls");
                continue;
            }
            else if(strcasecmp(command[0],"closeall")==0)
            {
               uobj.CloseAll(command[1]);
               continue;
            }
              else
            {
            cout<<"ERROR :Command not found\n";
            continue;
            }
        
        }
        else if(count==2)
        {
            if(strcasecmp(command[0],"write")==0)
            {
                cout<<"Enter The Data:";
                fgets(Arr,1024,stdin);
                DataLen=strlen(Arr);
        
                if(DataLen==0)
                {
                    cout<<"Incorrect Parameters";
                }
                ret=uobj.Write(command[1],Arr,DataLen);
                if(ret>=0)
                {
                    cout<<"File is succesfully Written the Data of Length:"<<"\t"<<ret<<"\n";
                }
                if(ret==-1)
                {
                    cout<<"ERROR :No File is Open with same Name\n";
                }
                if(ret==-2)
                {
                    cout<<"ERROR :Permission Denied\n";
                }
                if(ret==-3)
                {
                    cout<<"ERROR :File is not RegularFile\n";
                }
                if(ret==-4)
                {
                    cout<<"ERROR :Memory allocation failure\n";
                }
                    continue;
            }
            else if(strcasecmp(command[0],"man")==0)
            {
                uobj.man(command[1]);
                continue;
            }
            else if(strcasecmp(command[0],"rm")==0)
            {
             
                ret=uobj.rm(command[1]);
                if(ret==-1)
                {
                    cout<<"File Not Exist\n";
                }
                else
                {
                    cout<<"File is deleted Successfully\n";
                }
                
                continue;
            }
            else if(strcasecmp(command[0],"close")==0)
            {
                ret=uobj.Close(command[1]);
                if(ret==-1)
                {
                    cout<<"File Not Found\n";
                }
               continue;
            }
            else if((strcasecmp(command[0],"ls")==0)&&(strcasecmp(command[1],"-l")==0))
            {
                uobj.ls(10);
                continue;
            }
            else if(strcasecmp(command[0],"stat")==0)
            {
                ret=uobj.stat(command[1]);
                if(ret==-1)
                {
                    cout<<"File Not Found\n";
                }
                if(ret==-2)
                {
                    cout<<"File is not Opened\n";
                }
                continue;
            }
            else if(strcasecmp(command[0],"fstat")==0)
            {
                i=uobj.GetFDFromName(command[1]);
                ret=uobj.fstat(i);
                if(ret==-1)
                {
                    cout<<"File Not Found\n";
                }
                if(ret==-2)
                {
                    cout<<"File is not Opened\n";
                }
               
                continue;
            }
            else if(strcasecmp(command[0],"truncate")==0)
            {
                ret=uobj.truncate(command[1]);
                if(ret==-1)
                {
                    cout<<"File Not Found\n";
                }
                continue;
            }
            else
            {
                printf("ERROR : Incorrect command\n");
                continue;
            }
        }
        else if(count==3)
        {
            if(strcasecmp(command[0],"create")==0)
            {
                ret=uobj.CreateFile(command[1],atoi(command[2]));
                if(ret>=0)
                {
                    cout<<"File is succesfully created with file descriptor:"<<ret<<"\n";
                }
                if(ret==-1)
                {
                    cout<<"ERROR :Incorrect parameters\n";
                }
                if(ret==-2)
                {
                    cout<<"ERROR :There is no ptrinode\n";
                }
                if(ret==-3)
                {
                    cout<<"ERROR :File already exists\n";
                }
                if(ret==-4)
                {
                    cout<<"ERROR :Memory allocation failure\n";
                }
                    continue;
            }
            else if(strcasecmp(command[0],"open")==0)
            {
                ret=uobj.Open(command[1],atoi(command[2]));
                if(ret>=0)
                {
                    cout<<"File is succesfully Opened with file descriptor:"<<ret<<"\n";
                }
                if(ret==-1)
                { 
                    cout<<"ERROR :Incorrect parameters\n";
                }
                if(ret==-2)
                {
                    cout<<"ERROR :File Not Exist\n";
                }
                if(ret==-3)
                {
                    cout<<"ERROR :Permission Denied\n";
                }

                continue;
            }
            else if(strcasecmp(command[0],"read")==0)
            {
                Readptr=new char[1024];
                ret=uobj.Read(command[1],Readptr,atoi(command[2]));
                
                if(ret==-1)
                {
                    cout<<"ERROR :File not existing\n";
                }
                if(ret==-2)
                {
                    cout<<"ERROR :Permission denied\n";
                }
                if(ret==-3)
                {
                    cout<<"ERROR :Reach at end of file\n";
                }
                if(ret==-4)
                {
                    cout<<"ERROR :It is not a regular file\n";
                }
                if(ret==0)
                {
                    cout<<"ERROR :File empty\n";
                }
                if(ret>0)
                {
                    cout<<Readptr<<"\n";
                    delete []Readptr;
                }
                 continue;
            }
            else
            {
                cout<<"ERROR :Command not found\n";
                continue;
            }
        }
        else if(count==4)
        {
            if(strcasecmp(command[0],"lseek")==0)
            {
                ret=uobj.lseek(command[1],atoi(command[2]),atoi(command[3]));
                if(ret==-1)
                {
                    cout<<"File not Found\n";
                }
                if(ret==-2)
                {
                   cout<<"FT Not Available\n";
                } 
                if(ret==-3)
                {
                    cout<<"Incorrect Parameter\n";
                }
                if(ret==-4)
                {
                    cout<<"At End of file\n";
                }
                if(ret>=0)
                {
                   cout<<command[3]<<"\tMoved Size:\t"<<ret<<"\n"; 
                }
                 continue;
             
            }
        }
        else
        {
            cout<<"Command Not Found\n";
            continue;
        }
    }  
    return 0;
}
