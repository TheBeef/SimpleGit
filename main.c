/*******************************************************************************
 * FILENAME: main.c
 *
 * PROJECT:
 *    
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2024 Paul Hutchinson
 *
 * CREATED BY:
 *    pahuxa (17 Jun 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "trycatch.h"
#ifdef WIN32
 #include <windows.h>
void setupConsole(void);
void restoreConsole(void);
#endif

/*** DEFINES                  ***/
#define BUFFER_GROW_SIZE    1000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_Cmd_Help,
    e_Cmd_Status,
    e_Cmd_BranchStatus,
    e_Cmd_BranchBase,
    e_Cmd_Info,
    e_Cmd_Diff,
    e_Cmd_VDiff,
    e_Cmd_Switch,
    e_CmdMAX,
} e_CmdType;

/*** FUNCTION PROTOTYPES      ***/
char *FindLine(char *Buffer,char *Find);
char *GetMainBranchName(void);
char *GetCurrentBranchName(void);
char *ShellAndGrab(const char *Cmd);
char *Skip2StartOfNextLine(char *p);
void rtrim(char *Str);
void Do_Status(void);
int Do_BranchStatus(void);
int Do_Info(void);
int Do_ShowHelp(void);
int Do_Diff(void);
int Do_VDiff(void);
int Do_BranchBase(void);
int Do_Switch(void);

/*** VARIABLE DEFINITIONS     ***/
char *m_ShellAndGrabBuffer;
unsigned int m_ShellAndGrabBufferSize;

const char *m_Cmds[20];
int m_CmdsCount;
const char *m_CmdOptions[20][20];
int m_CmdOptionsCount[20];

int Do_ShowHelp(void)
{
    printf("USAGE:\n");
    printf("    sgit command sub-command\n");
    printf("\n");
    printf("WHERE\n");
    printf("    command -- The main command to do.  It is one of the following:\n");
    printf("        info -- Get info about the repo\n");
    printf("        status -- Get the current status of files\n");
    printf("        branch -- Commands that work on branches.  Supported sub commands:\n");
    printf("            status -- Get a list of files that have been changed on the\n");
    printf("                      current branch\n");
    printf("            base -- Get the hash of where the branch will merge from\n");
    printf("        diff -- Do a git diff\n");
    printf("        vdiff -- Do a git visual diff (using extern diff tool)\n");
    printf("        switch -- Change to a different branch\n");
    printf("    sub-commands -- Depends on 'command' (see above)\n");

    return 0;
}

int main(int argc,const char *argv[])
{
    int r;
    e_CmdType Cmd;
    int RetValue;

    m_CmdsCount=0;
    Cmd=e_CmdMAX;
    for(r=1;r<argc;r++)
    {
        if(argv[r][0]=='-')
        {
            /* Option */
            m_CmdOptions[m_CmdsCount][m_CmdOptionsCount[m_CmdsCount]]=argv[r];
            m_CmdOptionsCount[m_CmdsCount]++;
        }
        else
        {
            m_Cmds[m_CmdsCount++]=argv[r];
        }
    }

    if(m_CmdsCount<1)
    {
        fprintf(stderr,"No commands");
        return 1;
    }

    if(strcmp(m_Cmds[0],"help")==0)
    {
        Cmd=e_Cmd_Help;
    }
    else if(strcmp(m_Cmds[0],"status")==0 || strcmp(m_Cmds[0],"stat")==0)
    {
        Cmd=e_Cmd_Status;
    }
    else if(strcmp(m_Cmds[0],"diff")==0)
    {
        Cmd=e_Cmd_Diff;
    }
    else if(strcmp(m_Cmds[0],"vdiff")==0 || strcmp(m_Cmds[0],"difftool")==0)
    {
        Cmd=e_Cmd_VDiff;
    }
    else if(strcmp(m_Cmds[0],"info")==0)
    {
        Cmd=e_Cmd_Info;
    }
    else if(strcmp(m_Cmds[0],"switch")==0)
    {
        Cmd=e_Cmd_Switch;
    }
    else if(strcmp(m_Cmds[0],"branch")==0)
    {
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"status")==0)
            Cmd=e_Cmd_BranchStatus;
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"base")==0)
            Cmd=e_Cmd_BranchBase;
    }

    setupConsole();

    RetValue=0;
    switch(Cmd)
    {
        case e_Cmd_Help:
            RetValue=Do_ShowHelp();
        break;
        case e_Cmd_Status:
            Do_Status();
        break;
        case e_Cmd_Diff:
            RetValue=Do_Diff();
        break;
        case e_Cmd_VDiff:
            RetValue=Do_VDiff();
        break;
        case e_Cmd_BranchStatus:
            RetValue=Do_BranchStatus();
        break;
        case e_Cmd_BranchBase:
            RetValue=Do_BranchBase();
        break;
        case e_Cmd_Info:
            RetValue=Do_Info();
        break;
        case e_Cmd_Switch:
            RetValue=Do_Switch();
        break;
        case e_CmdMAX:
        default:
            printf("Unknown command\n");
        RetValue=1;
    }

    restoreConsole();

    if(m_ShellAndGrabBuffer!=NULL)
        free(m_ShellAndGrabBuffer);

    return RetValue;
}

#ifdef WIN32
 // Some old MinGW/CYGWIN distributions don't define this:
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

static HANDLE stdoutHandle;
static DWORD outModeInit;

void setupConsole(void)
{
    DWORD outMode = 0;
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(stdoutHandle == INVALID_HANDLE_VALUE) {
        exit(GetLastError());
    }
    
    if(!GetConsoleMode(stdoutHandle, &outMode)) {
        exit(GetLastError());
    }

    outModeInit = outMode;
    
    // Enable ANSI escape codes
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if(!SetConsoleMode(stdoutHandle, outMode)) {
        exit(GetLastError());
    }   
}

void restoreConsole(void)
{
    // Reset colors
    printf("\x1b[0m");  
    
    // Reset console mode
    if(!SetConsoleMode(stdoutHandle, outModeInit)) {
        exit(GetLastError());
    }
}
#endif

char *GetMainBranchName(void)
{
    char *Buffer;
    char *End;
    char *MainBranch;
    char *RetStr;

    Buffer=ShellAndGrab("git remote show origin");
    if(Buffer==NULL)
        return NULL;

    /* Find the "HEAD branch: " line */
    MainBranch=FindLine(Buffer,"  HEAD branch: ");
    End=strchr(MainBranch,'\n');
    if(End==NULL)
        return NULL;
    *End=0;

    RetStr=malloc(strlen(MainBranch)+1);
    if(RetStr==NULL)
        return NULL;

    strcpy(RetStr,MainBranch);

    return RetStr;
}

void rtrim(char *Str)
{
    char *p;

    /* Goto the end of the string */
    p=Str+strlen(Str);
    while(p>Str && (*p=='\n' || *p=='\r' || *p==' ' || *p=='\t' || *p==0))
    {
        *p=0;
        p--;
    }
}

char *GetCurrentBranchName(void)
{
    char *Buffer;
    char *End;
    char *MainBranch;
    char *RetStr;

    Buffer=ShellAndGrab("git rev-parse --abbrev-ref HEAD");
    if(Buffer==NULL)
        return NULL;

    rtrim(Buffer);

    RetStr=malloc(strlen(Buffer)+1);
    if(RetStr==NULL)
        return NULL;

    strcpy(RetStr,Buffer);

    return RetStr;
}

char *ShellAndGrab(const char *Cmd)
{
    FILE *fp;
    char buff[100];
    int Space;
    char *NewBuffer;
    unsigned int Len;
    unsigned int Bytes;
    int NewSize;

    /* Open the command for reading. */
    fp=popen(Cmd, "rb");
    if(fp==NULL)
        return NULL;

    if(m_ShellAndGrabBuffer==NULL)
    {
        m_ShellAndGrabBuffer=malloc(BUFFER_GROW_SIZE);
        if(m_ShellAndGrabBuffer==NULL)
            return NULL;
        m_ShellAndGrabBufferSize=BUFFER_GROW_SIZE;
    }

    /* Read the output a line at a time - output it. */
    Space=0;
    do
    {
        Bytes=fread(buff,1,sizeof(buff),fp);
        Len=Bytes;
        if(Space+Len>=m_ShellAndGrabBufferSize)
        {
            /* We need to grow the buffer */
            if(Len<BUFFER_GROW_SIZE)
                NewSize=m_ShellAndGrabBufferSize+BUFFER_GROW_SIZE;
            else
                NewSize=m_ShellAndGrabBufferSize+Len+1;
            NewBuffer=realloc(m_ShellAndGrabBuffer,NewSize);
            if(NewBuffer==NULL)
                return NULL;
            m_ShellAndGrabBuffer=NewBuffer;
            m_ShellAndGrabBufferSize=NewSize;
            m_ShellAndGrabBuffer[m_ShellAndGrabBufferSize-1]=0;
        }
        memcpy(&m_ShellAndGrabBuffer[Space],buff,Len);
        Space+=Len;
    } while(Bytes==sizeof(buff));
    m_ShellAndGrabBuffer[Space]=0;

    pclose(fp);

    return m_ShellAndGrabBuffer;
}

char *Skip2StartOfNextLine(char *p)
{
    while(*p=='\n' || *p=='\r')
        p++;
    return p;
}

char *FindLine(char *Buffer,char *Find)
{
    char *Pos;
    Pos=strstr(Buffer,Find);
    if(Pos==NULL)
    {
        /* We didn't find the string we wanted */
        return NULL;
    }
    if(*(Pos-1)!='\n' && *(Pos-1)!='\r')
    {
        /* Not this line */
        return FindLine(Pos+1,Find);
    }
    return Pos+strlen(Find);
}

void Do_Status(void)
{
    system("git status");
}

int Do_BranchStatus(void)
{
    char *MainBranchName;
    char *CurrentBranchName;
    char *Str;
    char buff[1000];
    int Bytes;
    char *MergeHash;
    int RetValue;
    char *Output;
    char *p;
    char *StartOfLine;

    /* Do a: git diff --name-only $(git merge-base master HEAD) */
    RetValue=0;
    ctry(const char *)
    {
        MainBranchName=GetMainBranchName();
        if(MainBranchName==NULL)
            cthrow("Failed to get main branch name");

        CurrentBranchName=GetCurrentBranchName();
        if(CurrentBranchName==NULL)
            cthrow("Failed to get current branch name");

        Bytes=snprintf(buff,sizeof(buff),"git merge-base \"%s\" HEAD",
                MainBranchName);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        MergeHash=ShellAndGrab(buff);
        if(MergeHash==NULL)
            cthrow("Out of memory");

        Bytes=snprintf(buff,sizeof(buff),"git diff --name-only %s",MergeHash);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        Output=ShellAndGrab(buff);
        if(Output==NULL)
            cthrow("Failed to execute git command");

        printf("On branch %s\n",CurrentBranchName);

/* DEBUG PAUL: Should including this info: */
//        printf("Your branch is ahead of 'xxx' by x commit");

        printf("Changes on this branch:\n");
//        printf("        modified:  ");
        printf("                   ");

/* DEBUG PAUL: Would be nice to figure out what's changed /deleted / added and color them */
        StartOfLine=Output;
        for(p=Output;*p!=0;)
        {
            if(*p=='\n')
            {
                /* Output this line */
                *p=0;
                printf("\33[34m%s\33[m\n",StartOfLine);

                p=Skip2StartOfNextLine(p+1);
                StartOfLine=p;
                if(*p!=0)
                {
//                    printf("        modified:  ");
                    printf("                   ");
                }
                continue;
            }
            p++;
        }
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    free(MainBranchName);

    return RetValue;
}




int Do_Info(void)
{
    char *Output;
    int RetValue;

    RetValue=0;
    ctry(const char *)
    {
        Output=ShellAndGrab("git ls-remote --get-url");
        if(Output==NULL)
            cthrow("Failed to execute git command");

        printf("URL: %s\n",Output);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    return RetValue;
}

int Do_Diff(void)
{
    char buff[1000];
    int RetValue;
    int Bytes;
    int r;
    int o;

    RetValue=0;
    ctry(const char *)
    {
        /* We are currently just doing a pass though */
        Bytes=snprintf(buff,sizeof(buff),"git diff ");
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        for(r=1;r<m_CmdsCount;r++)
        {
            for(o=0;o<m_CmdOptionsCount[r];o++)
            {
                strncat(buff,m_CmdOptions[r][o],sizeof(buff));
                strncat(buff," ",sizeof(buff));
            }

            strncat(buff,m_Cmds[r],sizeof(buff));
            strncat(buff," ",sizeof(buff));
        }
        system(buff);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }
    return RetValue;
}

int Do_VDiff(void)
{
    char buff[1000];
    int RetValue;
    int Bytes;
    int r;
    int o;

    RetValue=0;
    ctry(const char *)
    {
        /* We are currently just doing a pass though */
        Bytes=snprintf(buff,sizeof(buff),"git difftool ");
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        for(r=1;r<m_CmdsCount;r++)
        {
            for(o=0;o<m_CmdOptionsCount[r];o++)
            {
                strncat(buff,m_CmdOptions[r][o],sizeof(buff));
                strncat(buff," ",sizeof(buff));
            }

            strncat(buff,m_Cmds[r],sizeof(buff));
            strncat(buff," ",sizeof(buff));
        }
        system(buff);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }
    return RetValue;
}

int Do_BranchBase(void)
{
    char *MainBranchName;
    const char *BranchName;
    char buff[1000];
    int Bytes;
    char *MergeHash;
    int RetValue;

    /* Do a: git diff --name-only $(git merge-base master HEAD) */
    RetValue=0;
    ctry(const char *)
    {
        BranchName="HEAD";
        if(m_CmdsCount>=3)
            BranchName=m_Cmds[2];

        MainBranchName=GetMainBranchName();
        if(MainBranchName==NULL)
            cthrow("Failed to get main branch name");

        Bytes=snprintf(buff,sizeof(buff),"git merge-base \"%s\" \"%s\"",
                MainBranchName,BranchName);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        MergeHash=ShellAndGrab(buff);
        if(MergeHash==NULL)
            cthrow("Out of memory");

        printf("%s\n",MergeHash);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    free(MainBranchName);

    return RetValue;
}

int Do_Switch(void)
{
    char buff[1000];
    int RetValue;
    int Bytes;
    int r;
    int o;

    RetValue=0;
    ctry(const char *)
    {
        /* We are currently just doing a pass though */
        Bytes=snprintf(buff,sizeof(buff),"git switch ");
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        for(r=1;r<m_CmdsCount;r++)
        {
            for(o=0;o<m_CmdOptionsCount[r];o++)
            {
                strncat(buff,m_CmdOptions[r][o],sizeof(buff));
                strncat(buff," ",sizeof(buff));
            }

            strncat(buff,m_Cmds[r],sizeof(buff));
            strncat(buff," ",sizeof(buff));
        }
        system(buff);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }
    return RetValue;
}
