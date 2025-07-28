/*
Adding an local alias.

Local alias's will let you add an alias for branches (and maybe hashs? tags? other things?).

sgit local alias set [name] [value]
sgit local alias list
sgit local alias delete [name]

Still to do:
 * Delete
 * Change load so it adds them in ABC order
 * Make them actually do something...
 * Change to local instead of just alias



----

Add a git show for that last commit of a file (will need to lookup the commit
id's that effect this file

git difftool HEAD~3 Code/app/app0/Task_Cyclic10/Cals/PS_Cals/PS_CALS_Data.c

Doesn't work 100% correctly (HEAD~3 is three commit on the branch not the file

-----------------
Add:
    branch delete
    branch delete -f (force)
    branch delete -r (remote)
https://stackoverflow.com/questions/2003505/how-do-i-delete-a-git-branch-locally-and-remotely

----------------
Showing the last commit info for a file:
    First try:
#    git log --pretty=format:"%H" -2 Rakefile.rb
    git log --pretty=format:"%H" -1 --skip 1 Rakefile.rb    # Get second last hash for this file
    sgit vdiff HEAD ac9e8a299cf5d934c7813c80207b8d8e4a0e5fb5 Rakefile.rb

    Second try:
    git log -2 --pretty=format:"%H" master file.a   # Get the lastest hashs for file.a on the master branch
    git difftool ####1 ####2 file.a

==== Clean up branches ====
 * git remote update origin --prune
 * git fetch --prune

*/

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
 #include <shlobj.h>
#else
 #include <unistd.h>
 #include <sys/types.h>
 #include <pwd.h>
#endif

/*** DEFINES                  ***/
#define BUFFER_GROW_SIZE    1000

/* Version */
#define SGIT_VERSION_MAJOR       0
#define SGIT_VERSION_MINOR       4
#define SGIT_VERSION_REV         0
#define SGIT_VERSION_PATCH       0

#define SGIT_VERSION ((SGIT_VERSION_MAJOR<<24) | \
                    (SGIT_VERSION_MINOR<<16) | \
                    (SGIT_VERSION_REV<<8)  | \
                    (SGIT_VERSION_PATCH))

#define VER_STR_HELPER(x) #x
#define VER_STR(x) VER_STR_HELPER(x)

#define SGIT_VERSION_STR  VER_STR(SGIT_VERSION_MAJOR) "." VER_STR(SGIT_VERSION_MINOR) "."  VER_STR(SGIT_VERSION_REV) "."  VER_STR(SGIT_VERSION_PATCH)

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_Cmd_Help,
    e_Cmd_Status,
    e_Cmd_BranchStatus,
    e_Cmd_BranchBase,
    e_Cmd_BranchCreate,
    e_Cmd_BranchList,
    e_Cmd_BranchParent,
    e_Cmd_BranchDiff,
    e_Cmd_BranchVDiff,
    e_Cmd_Info,
    e_Cmd_Diff,
    e_Cmd_VDiff,
    e_Cmd_Switch,
    e_Cmd_Revert,
    e_Cmd_Pull,
    e_Cmd_Push,
    e_Cmd_Uncommit,
    e_Cmd_Commit,
    e_Cmd_Clone,
    e_Cmd_Rename,
//    e_Cmd_SetAlias,
//    e_Cmd_ListAliases,
//    e_Cmd_DeleteAlias,
    e_CmdMAX,
} e_CmdType;

typedef enum
{
    e_LocalRepo_NoChange,
    e_LocalRepo_Ahead,
    e_LocalRepo_Behind,
    e_LocalRepo_Deverged,
    e_LocalRepoMAX,
} e_LocalRepoType;

typedef enum
{
    e_BranchStatusOutputs_Mod,
    e_BranchStatusOutputs_Rename,
    e_BranchStatusOutputs_Add,
    e_BranchStatusOutputs_Delete,
    e_BranchStatusOutputs_Unknown,
    e_BranchStatusOutputsMAX
} e_BranchStatusOutputsType;

//struct Alias
//{
//    char *Name;
//    char *Value;
//    struct Alias *Next;
//};

/*** FUNCTION PROTOTYPES      ***/
//void SaveAliases(void);
//const char *GetAlias(const char *Name);
//const char *GetAliasedAs(const char *Value);
//void FreeAliases(void);
const char *GetLocalStoragePath(const char *Filename);
void setupConsole(void);
void restoreConsole(void);
void ProcessBranchStatusResults(const char *Output,e_BranchStatusOutputsType Look4,const char *Title);
bool GetRepoCommitsCounts(int *Behind,int *Ahead);
char *FindLine(char *Buffer,char *Find);
char *GetMainBranchName(void);
char *GetCurrentBranchName(void);
const char *GetParentBranchName(const char *BranchToLookup);
char *ShellAndGrab(const char *Cmd);
int ShellOut(const char *cmd);
char *Skip2StartOfNextLine(char *p);
void rtrim(char *Str);
bool GetBranchCommitsCounts(int *Behind,int *Ahead,const char *MainBranchName,
        const char *BranchName);
void Do_Status(void);
int Do_BranchStatus(void);
int Do_BranchCreate(void);
int Do_BranchParent(void);
int Do_BranchList(int OptionIndex);
int Do_Info(void);
int Do_ShowHelp(void);
int Do_PassThough(const char *GitCmd);
int Do_BranchBase(void);
int Do_BranchVDiff(void);
int Do_BranchDiff(void);
int Do_Revert(void);
int Do_UnCommit(void);
//int Do_ListAliases(void);
//int Do_SetAlias(void);

/*** VARIABLE DEFINITIONS     ***/
//bool g_AliasLoadedTried=false;
//struct Alias *g_AliasList=NULL;

bool g_ShowGit;
char *m_ShellAndGrabBuffer;
unsigned int m_ShellAndGrabBufferSize;

const char *m_Cmds[20];
int m_CmdsCount;
const char *m_CmdOptions[20][20];
int m_CmdOptionsCount[20];

int Do_ShowHelp(void)
{
    printf("USAGE:\n");
    printf("    sgit [GlobalOptions] command sub-command\n");
    printf("\n");
    printf("WHERE\n");
    printf("    GlobalOptions -- These are options that apply to all sgit\n");
    printf("                     commands.  Supported options:\n");
    printf("                        --show -- Show the git commands being run\n");
    printf("                        --version -- Show the version of sgit\n");
    printf("    command -- The main command to do.  It is one of the following:\n");
    printf("        info -- Get info about the repo\n");
    printf("        status (s) -- Get the current status of files\n");
    printf("        branch -- Commands that work on branches.  Supported sub commands:\n");
    printf("            status (s) -- Get a list of files that have been changed on the\n");
    printf("                      current branch\n");
    printf("            base -- Get the hash of where the branch will merge from\n");
    printf("            create -- Make a new branch from the currently checked out branch.\n");
    printf("            list -- List currently available branches (-a to include server branches). This is the same as branches\n");
//    printf("            diff -- Do a diff of this branch and the branchs parent branch.  And be followed by the filename for just that file\n");
/* DEBUG PAUL: ^^^ git diff master... -- filename <- but you need the parent branch name */
    printf("            parent -- Show the parent branch of the current branch\n");
    printf("            diff -- Show the changes that where made to this branch\n");
    printf("                    You can follow this with a file name to just\n");
    printf("                    show that files differences\n");
    printf("            vdiff -- The same as 'diff' but visual diff\n");
    printf("        branches -- List currently available branches (-a to include server branches).\n");
    printf("        diff -- Do a git diff\n");
    printf("        vdiff -- Do a git visual diff (using extern diff tool)\n");
    printf("        switch (sw) -- Change to a different branch\n");
    printf("        revert -- Throw away any local changes (-a to throw away local commits as well)\n");
    printf("        commit -- Commit staged changes to the repo\n");
    printf("        uncommit -- Pull the last commit back out to the working copy and delete the commit\n");
//    printf("        regret -- I regret doing something, let me fix it.  Sub commands:\n");
//    printf("            commit -- Pull the last commit back out to the working copy\n");
    printf("        pull -- Do a git pull\n");
    printf("        push -- Do a git push\n");
    printf("        clone -- Do a clone of a repo\n");
    printf("        rename (mv) -- Rename a file\n");
    printf("        local alias set (add) -- Add / change an alias\n");
    printf("        local alias list -- List current aliases\n");
    printf("        local alias delete -- Remove an alias\n");
//    printf("        undo -- Check in a set of commits undoing commits\n");
    printf("    sub-commands -- Depends on 'command' (see above)\n");

    return 0;
}

int main(int argc,const char *argv[])
{
    int r;
    e_CmdType Cmd;
    int RetValue;
    int OptionIndex;

    g_ShowGit=false;

    m_CmdsCount=0;
    Cmd=e_CmdMAX;
    for(r=1;r<argc;r++)
    {
        if(argv[r][0]=='-')
        {
            if(m_CmdsCount==0)
            {
                /* Global options */
                if(strcmp(argv[r],"--show")==0)
                    g_ShowGit=true;
                else if(strcmp(argv[r],"--version")==0)
                {
                    printf("sgit version %s\n",SGIT_VERSION_STR);
                    return 0;
                }
                else if(strcmp(argv[r],"--help")==0)
                {
                    Do_ShowHelp();
                    return 0;
                }
                else
                {
                    printf("Unknown global option\n");
                    return 1;
                }
            }
            else
            {
                /* Command Option */
                m_CmdOptions[m_CmdsCount-1][m_CmdOptionsCount[m_CmdsCount-1]]=argv[r];
                m_CmdOptionsCount[m_CmdsCount-1]++;
            }
        }
        else
        {
            m_Cmds[m_CmdsCount++]=argv[r];
        }
    }
    if(m_CmdsCount<1)
    {
        m_Cmds[0]="help";
    }

    OptionIndex=0;

    if(strcmp(m_Cmds[0],"help")==0)
    {
        Cmd=e_Cmd_Help;
    }
    else if(strcmp(m_Cmds[0],"status")==0 || strcmp(m_Cmds[0],"s")==0)
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
    else if(strcmp(m_Cmds[0],"switch")==0 || strcmp(m_Cmds[0],"sw")==0)
    {
        Cmd=e_Cmd_Switch;
    }
    else if(strcmp(m_Cmds[0],"revert")==0)
    {
        Cmd=e_Cmd_Revert;
    }
    else if(strcmp(m_Cmds[0],"pull")==0)
    {
        Cmd=e_Cmd_Pull;
    }
    else if(strcmp(m_Cmds[0],"push")==0)
    {
        Cmd=e_Cmd_Push;
    }
    else if(strcmp(m_Cmds[0],"commit")==0)
    {
        Cmd=e_Cmd_Commit;
    }
    else if(strcmp(m_Cmds[0],"uncommit")==0)
    {
        Cmd=e_Cmd_Uncommit;
    }
    else if(strcmp(m_Cmds[0],"branch")==0)
    {
        if(m_CmdsCount>=2 && (strcmp(m_Cmds[1],"status")==0 || strcmp(m_Cmds[1],"s")==0))
            Cmd=e_Cmd_BranchStatus;
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"base")==0)
            Cmd=e_Cmd_BranchBase;
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"create")==0)
            Cmd=e_Cmd_BranchCreate;
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"parent")==0)
            Cmd=e_Cmd_BranchParent;
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"list")==0)
        {
            OptionIndex=1;
            Cmd=e_Cmd_BranchList;
        }
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"diff")==0)
            Cmd=e_Cmd_BranchDiff;
        if(m_CmdsCount>=2 && strcmp(m_Cmds[1],"vdiff")==0)
            Cmd=e_Cmd_BranchVDiff;
    }
    else if(strcmp(m_Cmds[0],"branches")==0)
    {
        OptionIndex=0;
        Cmd=e_Cmd_BranchList;
    }
    else if(strcmp(m_Cmds[0],"clone")==0)
    {
        Cmd=e_Cmd_Clone;
    }
    else if(strcmp(m_Cmds[0],"rename")==0 || strcmp(m_Cmds[0],"mv")==0)
    {
        Cmd=e_Cmd_Rename;
    }
//    else if(strcmp(m_Cmds[0],"local")==0)
//    {
//        if(strcmp(m_Cmds[1],"alias")==0)
//        {
//            if(m_CmdsCount>=3 && (strcmp(m_Cmds[2],"set")==0 || strcmp(m_Cmds[2],"add")==0))
//                Cmd=e_Cmd_SetAlias;
//            if(m_CmdsCount>=3 && strcmp(m_Cmds[2],"list")==0)
//                Cmd=e_Cmd_ListAliases;
//            if(m_CmdsCount>=3 && strcmp(m_Cmds[2],"delete")==0)
//                Cmd=e_Cmd_DeleteAlias;
//        }
//    }

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
            RetValue=Do_PassThough("diff");
        break;
        case e_Cmd_VDiff:
            RetValue=Do_PassThough("difftool");
        break;
        case e_Cmd_BranchStatus:
            RetValue=Do_BranchStatus();
        break;
        case e_Cmd_BranchBase:
            RetValue=Do_BranchBase();
        break;
        case e_Cmd_BranchCreate:
            RetValue=Do_BranchCreate();
        break;
        case e_Cmd_BranchParent:
            RetValue=Do_BranchParent();
        break;
        case e_Cmd_BranchDiff:
            RetValue=Do_BranchDiff();
        break;
        case e_Cmd_BranchVDiff:
            RetValue=Do_BranchVDiff();
        break;
        case e_Cmd_BranchList:
            RetValue=Do_BranchList(OptionIndex);
        break;
        case e_Cmd_Info:
            RetValue=Do_Info();
        break;
        case e_Cmd_Switch:
            RetValue=Do_PassThough("switch");
        break;
        case e_Cmd_Revert:
            RetValue=Do_Revert();
        break;
        case e_Cmd_Pull:
            RetValue=Do_PassThough("pull");
        break;
        case e_Cmd_Push:
            RetValue=Do_PassThough("push");
        break;
        case e_Cmd_Uncommit:
            RetValue=Do_UnCommit();
        break;
        case e_Cmd_Commit:
            RetValue=Do_PassThough("commit");
        break;
        case e_Cmd_Clone:
            RetValue=Do_PassThough("clone");
        break;
        case e_Cmd_Rename:
            RetValue=Do_PassThough("mv");
        break;
//        case e_Cmd_SetAlias:
//            RetValue=Do_SetAlias();
//        break;
//        case e_Cmd_ListAliases:
//            RetValue=Do_ListAliases();
//        break;
//        case e_Cmd_DeleteAlias:
//        break;
        case e_CmdMAX:
        default:
            printf("Unknown command\n");
        RetValue=1;
    }

    restoreConsole();

    if(m_ShellAndGrabBuffer!=NULL)
        free(m_ShellAndGrabBuffer);

//    FreeAliases();

    return RetValue;
}

#ifdef WIN32
const char *GetLocalStoragePath(const char *Filename)
{
    static char szPath[MAX_PATH];

    // Get path for each computer, non-user specific and non-roaming data.
    if(SHGetFolderPathA(NULL,CSIDL_APPDATA,NULL,0,szPath)==S_OK)
    {
        // Append product-specific path
        strncat(szPath,"\\sgit\\",sizeof(szPath)-1);
        if(strlen(szPath)>=sizeof(szPath)-1)
            return NULL;

        CreateDirectoryA(szPath,NULL);

        strncat(szPath,Filename,sizeof(szPath)-1);
        if(strlen(szPath)>=sizeof(szPath)-1)
            return NULL;
    }
    else
    {
        return NULL;
    }

    return szPath;
}
#else

const char *GetLocalStoragePath(const char *Filename)
{
    static char Path[1000];
    const char *homedir;

    homedir=getenv("HOME");
    if(homedir==NULL)
        homedir=getpwuid(getuid())->pw_dir;

    strncpy(Path,homedir,sizeof(Path)-1);
    Path[sizeof(Path)-1]=0;
    strncat(Path,"/.config/",sizeof(Path)-1);
    if(strlen(Path)>=sizeof(Path)-1)
        return NULL;
    mkdir(Path,S_IRWXU|S_IRWXG|S_IRWXO);

    strncat(Path,"sgit/",sizeof(Path)-1);
    if(strlen(Path)>=sizeof(Path)-1)
        return NULL;
    mkdir(Path,S_IRWXU|S_IRWXG|S_IRWXO);

    strncat(Path,Filename,sizeof(Path)-1);
    if(strlen(Path)>=sizeof(Path)-1)
        return NULL;

    return Path;
}

#endif

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
#else
void setupConsole(void)
{
}

void restoreConsole(void)
{
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

const char *GetParentBranchName(const char *BranchToLookup)
{
    char buff[1000];
    int Bytes;
    const char *RetValue;
    char RetBuff[1000];
    char *Buffer;
    const char *CurrentBranchName;
    char *Pos;
    char *CurrentLine;
    char *BranchName;
    char *EndOfLine;
    bool Found;
    bool ThisBranchFound;
    bool ParentBranchFound;

    /* DEBUG PAUL: Doesn't always seem to work..... */
    /* https://stackoverflow.com/questions/3161204/how-to-find-the-nearest-parent-of-a-git-branch/68673744 */
    /* Do a: git show-branch | grep '*' | grep -v "$(git rev-parse --abbrev-ref HEAD)" | head -n1 ' */
    RetValue=NULL;
    ctry(const char *)
    {
        if(BranchToLookup==NULL)
        {
            CurrentBranchName=GetCurrentBranchName();
            if(CurrentBranchName==NULL)
                cthrow("Failed to get current branch name");
        }
        else
        {
            CurrentBranchName=BranchToLookup;
        }

        Buffer=ShellAndGrab("git show-branch -a");
        if(Buffer==NULL)
            cthrow("Failed to execute git show-branch");

        /* Ok, this seems to be the way this works:
            The command lists all the branches that are related to the current
              branch.  It lists then above a ---- line with a * before the
              current head, and ! before other heads.  After the ---- line
              it list all the branches in order of creation with a + sign
              beside it when it's effected by the head above.  It also uses
              a * to mark that this branch is the effected.

           So we:
            - Find the ---- line
            - Only process lines that have a * before the [
            - Find our branch in the list
            - Return the next branch name after that (that also starts with a
              star).

            ! [master] First
             ! [A] Added on branch A
              * [B] Changed on B
               ! [origin/A] Added on branch A
                ! [origin/B] Changed on B
                 ! [origin/master] First
            ------
              * +  [B] Changed on B
             +*++  [A] Added on branch A
            ++*+++ [master] First

            [] -- The name of the branch
            * -- An important branch

        */
        /* First find the ------ line */
        Pos=Buffer;
        while(*Pos!=0)
        {
            if(strncmp(Pos,"------",6)==0)
                break;
            Pos++;
        }
        if(*Pos==0)
        {
            /* Didn't find the --- line */
            cthrow("Failed to parse the show-branch output");
        }

        /* Move to the next line */
        while(*Pos!=0 && *Pos!='\n')
            Pos++;

        if(*Pos==0)
            cthrow("Failed to parse the show-branch output");

        Pos++;
        CurrentLine=Pos;
        BranchName=NULL;
        ThisBranchFound=false;
        ParentBranchFound=false;
        while(*CurrentLine!=0)
        {
            /* See if it starts with a '*' */
            Pos=CurrentLine;
            Found=false;
            while(*Pos!=0 && *Pos!='\n' && *Pos!='[')
            {
                if(*Pos=='*')
                    Found=true;
                Pos++;
            }

            if(*Pos==0 || *Pos=='\n' || !Found)
            {
                /* We didn't find a '*', skip the end of the line */
                goto NextLine;
            }

            /* Ok, this is an important line, extract the branch name */
            BranchName=Pos+1;
            while(*Pos!=0 && *Pos!=']' && *Pos!='~' && *Pos!='^')
                Pos++;
            if(*Pos==0)
                goto NextLine;
            *Pos=0; // Branch is a string

            CurrentLine=Pos+1;  // Move the start of the line after the branch name

            /* Ok, see if this is the current branch */
            if(strcmp(BranchName,CurrentBranchName)==0)
            {
                /* Yep, grab the next one */
                ThisBranchFound=true;
            }
            else
            {
                if(ThisBranchFound)
                {
                    /* This is the entry after the current branch return this
                       one */
                    ParentBranchFound=true;
                    break;
                }
            }

NextLine:
            /* Move to the next line */
            while(*CurrentLine!=0 && *CurrentLine!='\n')
                CurrentLine++;
            if(*CurrentLine=='\n')
                CurrentLine++;
        }

        if(!ParentBranchFound)
        {
            /* Not found */
            cthrow("Failed to find parent branch\n");
        }
        strncpy(RetBuff,BranchName,sizeof(RetBuff)-1);
        RetBuff[sizeof(RetBuff)-1]=0;
        RetValue=RetBuff;
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=NULL;
    }

    return RetValue;
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

//void LoadAliases(void)
//{
//    const char *Filename;
//    FILE *in;
//    char LineBuff[200];
//    char *Name;
//    char *Value;
//    char *p;
//    struct Alias *NewAlias;
//
//    /* Even if we fail we don't want to try again */
//    g_AliasLoadedTried=true;
//
//    Filename=GetLocalStoragePath("Aliases");
//    if(Filename==NULL)
//        return;
//
//    in=fopen(Filename,"r");
//    if(in==NULL)
//        return;
//
//    ctry(const char *)
//    {
//        while(fgets(LineBuff,sizeof(LineBuff),in)!=NULL)
//        {
//            /* Break the line up.  Format is:
//                Name=Value\n
//            */
//            Name=LineBuff;
//            Value=strchr(Name,'=');
//            if(Value==NULL)
//                cthrow("Alias file is incorrectly formated");
//            *Value=0;
//            Value++;
//            p=Value;
//            while(*p!=0)
//            {
//                if(*p=='\n')
//                    *p=0;
//                p++;
//            }
//
//            /* Allocate an entry for this alias */
//            NewAlias=malloc(sizeof(struct Alias));
//            if(NewAlias==NULL)
//                cthrow("Out of memory");
//
//            NewAlias->Name=malloc(strlen(Name)+1);
//            if(NewAlias->Name==NULL)
//                cthrow("Out of memory");
//
//            NewAlias->Value=malloc(strlen(Value)+1);
//            if(NewAlias->Value==NULL)
//                cthrow("Out of memory");
//
//            strcpy(NewAlias->Name,Name);
//            strcpy(NewAlias->Value,Value);
//
//            NewAlias->Next=g_AliasList;
//            g_AliasList=NewAlias;
//        }
//    }
//    ccatch(const char *Msg)
//    {
//        fprintf(stderr,"Failed to load alias's:%s\n",Msg);
//    }
//
//    fclose(in);
//}

//void SaveAliases(void)
//{
//    const char *Filename;
//    FILE *out;
//    struct Alias *a;
//
//    Filename=GetLocalStoragePath("Aliases");
//    if(Filename==NULL)
//        return;
//
//    out=fopen(Filename,"w");
//    if(out==NULL)
//        return;
//
//    for(a=g_AliasList;a!=NULL;a=a->Next)
//        fprintf(out,"%s=%s\n",a->Name,a->Value);
//
//    fclose(out);
//}

//void FreeAliases(void)
//{
//    struct Alias *TmpAliasPtr;
//
//    while(g_AliasList!=NULL)
//    {
//        TmpAliasPtr=g_AliasList->Next;
//        free(g_AliasList);
//        g_AliasList=TmpAliasPtr;
//    }
//}

//const char *GetAlias(const char *Name)
//{
//    struct Alias *s;
//
//    if(!g_AliasLoadedTried)
//        LoadAliases();
//
//    for(s=g_AliasList;s!=NULL;s=s->Next)
//    {
//        if(strcasecmp(s->Name,Name)==0)
//            break;
//    }
//    if(s==NULL)
//        return NULL;
//    return s->Value;
//}

//const char *GetAliasedAs(const char *Value)
//{
//    struct Alias *s;
//
//    if(!g_AliasLoadedTried)
//        LoadAliases();
//
//    for(s=g_AliasList;s!=NULL;s=s->Next)
//    {
//        if(strcasecmp(s->Value,Value)==0)
//            break;
//    }
//    if(s==NULL)
//        return NULL;
//    return s->Name;
//}

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

bool GetRepoCommitsCounts(int *Behind,int *Ahead)
{
    char *Buffer;
    char *End;
    char *MainBranch;
    char *RetStr;

//    if(ShellOut("git fetch")<0)
//        return false;

    Buffer=ShellAndGrab("git rev-list --count \"HEAD..@{u}\"");
    if(Buffer==NULL)
        return false;

    *Behind=atoi(Buffer);

    Buffer=ShellAndGrab("git rev-list --count \"@{u}..HEAD\"");
    if(Buffer==NULL)
        return false;

    *Ahead=atoi(Buffer);

    return true;
}

bool GetBranchCommitsCounts(int *Behind,int *Ahead,const char *MainBranchName,
        const char *BranchName)
{
    char *Buffer;
    char *End;
    char *MainBranch;
    char *AfterStr;
    char buff[1000];
    int Bytes;

//    if(ShellOut("git fetch")<0)
//        return false;

    Bytes=snprintf(buff,sizeof(buff),"git rev-list --left-right --count \"%s\"...\"%s\"",
            MainBranchName,BranchName);
    if(Bytes>sizeof(buff))
        return false;

    Buffer=ShellAndGrab(buff);
    if(Buffer==NULL)
        return false;

    *Behind=atoi(Buffer);
    AfterStr=strchr(Buffer,'\t');
    if(AfterStr==NULL)
    {
        AfterStr=strchr(Buffer,' ');
        if(AfterStr==NULL)
            return false;
    }

    *Ahead=atoi(AfterStr);

    return true;
}

e_LocalRepoType GetLocalRepoCommitStatus(void)
{
    int Behind;
    int Ahead;

    if(!GetRepoCommitsCounts(&Behind,&Ahead))
        return e_LocalRepoMAX;

    if(Behind==0 && Ahead==0)
        return e_LocalRepo_NoChange;

    if(Behind>0 && Ahead==0)
        return e_LocalRepo_Behind;

    if(Behind==0 && Ahead>0)
        return e_LocalRepo_Ahead;

    return e_LocalRepo_Deverged;
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
    if(g_ShowGit)
        printf("\33[35m%s\33[m\n",Cmd);
#ifdef WIN32
    fp=popen(Cmd,"rb"); // Don't know if we need the 'b' in Windows or not, but it messes with Linux so...
#else
    fp=popen(Cmd,"r");
#endif
    if(fp==NULL)
    {
        printf("Fail here\n");
        return NULL;
    }

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
    ShellOut("git status");
}

int ShellOut(const char *Cmd)
{
    if(g_ShowGit)
        printf("\33[35m%s\33[m\n",Cmd);
    return system(Cmd);
}

int Do_BranchStatus(void)
{
    char *MainBranchName;
    char *CurrentBranchName;
    char *Str;
    char buff[1000];
    int Bytes;
    char *MergeHashResult;
    char MergeHash[100];
    int RetValue;
    char *Output;
    char *StartOfLine;
    int Behind;
    int Ahead;
    bool ShowMasterChanges;
    bool ShowBranchChanges;

    MainBranchName=NULL;
    CurrentBranchName=NULL;

    /* Do a: git diff --name-only $(git merge-base master HEAD) */
    RetValue=0;
    ctry(const char *)
    {
        ShowMasterChanges=false;
        ShowBranchChanges=false;

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

        MergeHashResult=ShellAndGrab(buff);
        if(MergeHashResult==NULL)
            cthrow("Out of memory");

        if(strlen(MergeHashResult)>sizeof(MergeHash)-1)
            cthrow("Internal hash buffer too small");
        strcpy(MergeHash,MergeHashResult);
        if(MergeHash[strlen(MergeHash)-1]=='\n')
            MergeHash[strlen(MergeHash)-1]=0;
        if(MergeHash[strlen(MergeHash)-1]=='\r')
            MergeHash[strlen(MergeHash)-1]=0;

        Bytes=snprintf(buff,sizeof(buff),"git diff --name-status --find-renames %s",MergeHash);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        printf("On branch %s\n",CurrentBranchName);

        if(!GetBranchCommitsCounts(&Behind,&Ahead,MainBranchName,
                CurrentBranchName))
        {
            cthrow("Failed to get ahead/behind info about the current branch");
        }

        if(Behind==0 && Ahead==0)
        {
            printf("Your branch is up to date with '%s'.\n",CurrentBranchName);
        }
        else if(Behind>0 && Ahead==0)
        {
            printf("Your branch is behind '%s' by %d commits\n",MainBranchName,Behind);
            ShowMasterChanges=true;
        }
        else if(Behind==0 && Ahead>0)
        {
            printf("Your branch is ahead of '%s' by %d commits.\n",MainBranchName,Ahead);
            ShowBranchChanges=true;
        }
        else
        {
            printf("Your branch and '%s' have diverged.\n",MainBranchName);
            printf("    Your branch is ahead of '%s' by %d commits.\n",MainBranchName,Ahead);
            printf("    Your branch is behind '%s' by %d commits\n",MainBranchName,Behind);
            ShowBranchChanges=true;
            ShowMasterChanges=true;
        }

        Output=ShellAndGrab(buff);
        if(Output==NULL)
            cthrow("Failed to execute git command");

        if(ShowBranchChanges)
        {
            printf("Changes on this branch:\n");

            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Add,"\33[32mnew file");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Delete,"\33[31mdeleted");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Rename,"\33[33mrenamed");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Mod,"\33[34mmodified");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Unknown,"[33[35munknown change");
            printf("\33[m");
        }

        if(ShowMasterChanges)
        {
            /* Show what files changed on master */
            Bytes=snprintf(buff,sizeof(buff),"git diff --name-status --find-renames %s..%s",
                    MergeHash,MainBranchName);
            if(Bytes>sizeof(buff))
                cthrow("Internal buffer to small");

            Output=ShellAndGrab(buff);
            if(Output==NULL)
                cthrow("Failed to execute git command");

            printf("Changes on %s since last merge base:\n",MainBranchName);

            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Add,"\33[32mnew file");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Delete,"\33[31mdeleted");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Rename,"\33[33mrenamed");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Mod,"\33[34mmodified");
            ProcessBranchStatusResults(Output,e_BranchStatusOutputs_Unknown,"[33[35munknown change");
            printf("\33[m");
        }
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    free(MainBranchName);
    free(CurrentBranchName);

    return RetValue;
}

void ProcessBranchStatusResults(const char *Output,e_BranchStatusOutputsType Look4,const char *Title)
{
    const char *StartOfLine;
    const char *p;
    const char *end;
    bool Process;
    bool First;

    First=true;
    StartOfLine=Output;
    for(p=Output;*p!=0;)
    {
        /* First comes the type of change */
        Process=false;
        switch(Look4)
        {
            case e_BranchStatusOutputs_Mod:
                if(*p=='M')
                    Process=true;
            break;
            case e_BranchStatusOutputs_Rename:
                if(*p=='R')
                    Process=true;
            break;
            case e_BranchStatusOutputs_Add:
                if(*p=='A')
                    Process=true;
            break;
            case e_BranchStatusOutputs_Delete:
                if(*p=='D')
                    Process=true;
            break;
            case e_BranchStatusOutputs_Unknown:
                if(*p!='M' && *p!='R' && *p!='A' && *p!='D')
                    Process=true;
            break;
            case e_BranchStatusOutputsMAX:
            break;
        }
        if(!Process)
        {
            /* Skip this line */
            while(*p!=0 && *p!='\n')
                p++;
            while(*p=='\n' || *p=='\r')
                p++;
            continue;
        }

        /* Skip any chars that are not a space */
        while(*p!=' ' && *p!='\t' && *p!=0)
            p++;
        if(*p==0)
            break;
        /* Skip the spaces */
        while(*p==' ' || *p=='\t')
            p++;
        if(*p==0)
            break;

        /* We are at the start of the filename */
        if(First)
        {
            printf("        %s:\n",Title);
            First=false;
        }
        printf("                   ");
        while(*p!=0 && *p!=' ' && *p!='\t' && *p!='\r' && *p!='\n')
        {
            printf("%c",*p);
            p++;
        }
        if(Look4==e_BranchStatusOutputs_Rename)
        {
            printf(" -> ");
            while(*p==' ' || *p=='\t')
                p++;
            while(*p!=0 && *p!=' ' && *p!='\t' && *p!='\r' && *p!='\n')
            {
                printf("%c",*p);
                p++;
            }
        }
        printf("\n");
    }
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

int Do_PassThough(const char *GitCmd)
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
        Bytes=snprintf(buff,sizeof(buff),"git %s ",GitCmd);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        for(o=0;o<m_CmdOptionsCount[0];o++)
        {
            strncat(buff,m_CmdOptions[0][o],sizeof(buff)-1);
            strncat(buff," ",sizeof(buff)-1);
        }

        for(r=1;r<m_CmdsCount;r++)
        {
            strncat(buff,"\"",sizeof(buff)-1);
            strncat(buff,m_Cmds[r],sizeof(buff)-1);
            strncat(buff,"\" ",sizeof(buff)-1);

            for(o=0;o<m_CmdOptionsCount[r];o++)
            {
                strncat(buff,m_CmdOptions[r][o],sizeof(buff)-1);
                strncat(buff," ",sizeof(buff)-1);
            }
        }
        ShellOut(buff);
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

int Do_BranchCreate(void)
{
    char buff[1000];
    int Bytes;
    int RetValue;

    /* Do a: git checkout -b [name] */
    RetValue=0;
    ctry(const char *)
    {
        if(m_CmdsCount<3)
            cthrow("Missing new branch name");
        Bytes=snprintf(buff,sizeof(buff),"git checkout -b \"%s\"",m_Cmds[2]);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        ShellOut(buff);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    return RetValue;
}

int Do_BranchParent(void)
{
    char buff[1000];
    int Bytes;
    int RetValue;
    char *Buffer;
    char *CurrentBranchName;
    char *CurrentLine;
    char *BranchName;
    char *EndOfLine;

    /* DEBUG PAUL: Doesn't always seem to work..... */
    /* https://stackoverflow.com/questions/3161204/how-to-find-the-nearest-parent-of-a-git-branch/68673744 */
    /* Do a: git show-branch | grep '*' | grep -v "$(git rev-parse --abbrev-ref HEAD)" | head -n1 ' */
    RetValue=0;
    ctry(const char *)
    {
        CurrentBranchName=GetCurrentBranchName();
        if(CurrentBranchName==NULL)
            cthrow("Failed to get current branch name");

        Buffer=ShellAndGrab("git show-branch -a");
        if(Buffer==NULL)
            cthrow("Failed to execute git show-branch");

        /* Ok we:
            - Only look at lines with a "*" on it before the "["
            - Only look at lines that don't have the current branch name in between the []'s (stop at ~ or ^ in name)
            - Take the first line that matchs above
            - Take the name between the []'s (removing the ~ and ^ stuff)
        */
        CurrentLine=Buffer;
        BranchName=NULL;
        while(*CurrentLine!=0)
        {
            /* Find the branch name [] */
            BranchName=strchr(CurrentLine,'[');
            if(BranchName==NULL)
            {
                /* Not found, move to the next line */
                while(*CurrentLine!='\n' && *CurrentLine!=0)
                    CurrentLine++;
                if(*CurrentLine==0)
                {
                    BranchName=NULL;
                    break;
                }
                CurrentLine++;  // Move on the next char
                if(*CurrentLine=='\r')
                    CurrentLine++;
                continue;
            }
            *BranchName=0;  // Make the stuff before the branch name into a string
            BranchName++;
            /* Find the end of the branch name */
            EndOfLine=BranchName;
            while(*EndOfLine!='~' && *EndOfLine!=']' && *EndOfLine!='^')
                EndOfLine++;
            *EndOfLine=0;   // Make the branch name a string
            EndOfLine++;
            /* Find the end of the line */
            while(*EndOfLine!='\n' && EndOfLine!=0)
                EndOfLine++;
            if(*EndOfLine!=0)
                EndOfLine++;
            if(*EndOfLine=='\r')
                EndOfLine++;

            /* Process this line */
            /* See if we have a '*' in the status */
            if(strchr(CurrentLine,'*')!=NULL)
            {
                /* Make sure this isn't the current branch */
                if(strcmp(BranchName,CurrentBranchName)!=0)
                {
                    /* Ok, this is our match (because this is the first time
                       we got here) */
                    break;
                }
            }

            CurrentLine=EndOfLine;
        }
        if(BranchName==NULL)
        {
            /* Not found */
            cthrow("Failed to find parent\n");
        }
        printf("%s\n",BranchName);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    return RetValue;
}

int Do_BranchList(int OptionIndex)
{
    char buff[1000];
    int Bytes;
    int RetValue;
    const char *Options;
    char *CurrentBranchName;
    int o;
    char *Output;
    char *p;
    bool StartOfLine;
    char *BranchName;
    char *ArrowStart;
    bool StarFound;
//    const char *AliasedAs;

    /* Do a: git branch */
    RetValue=0;
    CurrentBranchName=NULL;
    ctry(const char *)
    {
        Options="";

        /* See if there is a -a option */
        for(o=0;o<m_CmdOptionsCount[OptionIndex];o++)
        {
            if(strcmp(m_CmdOptions[OptionIndex][o],"-a")==0)
                Options="-a";
        }

        CurrentBranchName=GetCurrentBranchName();

        Bytes=snprintf(buff,sizeof(buff),"git branch %s",Options);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

//        ShellOut(buff);

//        Bytes=snprintf(buff,sizeof(buff),"git branch --format=%%(refname:short) %s",Options);
//        if(Bytes>sizeof(buff))
//            cthrow("Internal buffer to small");

        Output=ShellAndGrab(buff);

//        printf("%s\n",Output);
        /* Walk each line looking for any aliases */
        p=Output;
        while(*p!=0)
        {
            ArrowStart=NULL;
            StarFound=false;

            /* We are at the start of the line, search for the first non space */
            while(*p==' ' || *p=='\t')
                p++;

            if(*p==0)
                continue;

            if(*p=='*')
            {
                StarFound=true;
                p++;
                /* Continue */
                while(*p==' ' || *p=='\t')
                    p++;
            }

            /* We should be at the branch name */
            BranchName=p;

            /* Find the next space or end of line */
            while(*p!=' ' && *p!='\t' && *p!='\n' && *p!='\r' && *p!=0)
                p++;

            /* We are now at the end name */
            if(*p==0)
            {
                /* End of string */
            }
            else if(*p!='\n' && *p!='\r')
            {
                *p=0;
                p++;
                /* Now see if there is any thing else after the name */
                while(*p==' ' || *p=='\t')
                {
                    printf("'%c'\n",*p);
                    p++;
                }
                if(*p=='-' && *(p+1)=='>')
                {
                    /* We found an arrow */
                    ArrowStart=p;
                }
                /* Skip to the end of the line */
                while(*p!='\n' && *p!='\r' && *p!=0)
                    p++;
                *p=0;
                p++;
            }
            else
            {
                *p=0;
                p++;
            }

            /* Move to the next line */
            while(*p=='\n' || *p=='\r')
                p++;

            /* Output this line */
            if(StarFound)
                printf("* ");
            else
                printf("  ");
            if(strcmp(CurrentBranchName,BranchName)==0)
                printf("\33[32m");
            if(strncmp(BranchName,"remotes/",8)==0)
                printf("\33[31m");
            printf("%s\33[m",BranchName);

//            /* See if this branch is in the alias list */
//            AliasedAs=GetAliasedAs(BranchName);
//            if(AliasedAs!=NULL)
//                printf("  (%s)",AliasedAs);

            if(ArrowStart!=NULL)
                printf(" %s",ArrowStart);
            printf("\n");
        }
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    if(CurrentBranchName!=NULL)
        free(CurrentBranchName);

    return RetValue;
}

int Do_BranchDiff(void)
{
    char buff[1000];
    int Bytes;
    int RetValue;
    const char *ParentBranch;
    const char *CurrentBranch;
    char Filename[1000];

    /* Do a: git diff branch1 branch2 [filename] */
    RetValue=0;
    ctry(const char *)
    {
        *Filename=0;
        if(m_CmdsCount>=3)
        {
            /* We have an optional filename */
            Bytes=snprintf(Filename,sizeof(Filename),"\"%s\"",m_Cmds[2]);
            if(Bytes>sizeof(Filename))
                cthrow("Internal buffer to small");
        }
        CurrentBranch=GetCurrentBranchName();
        ParentBranch=GetParentBranchName(NULL);
        if(ParentBranch==NULL)
            cthrow(NULL);

        Bytes=snprintf(buff,sizeof(buff),"git diff \"%s\" \"%s\" %s",
                CurrentBranch,ParentBranch,Filename);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        ShellOut(buff);
    }
    ccatch(const char *Msg)
    {
        if(Msg!=NULL)
            fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    return RetValue;
}

int Do_BranchVDiff(void)
{
    char buff[1000];
    int Bytes;
    int RetValue;
    const char *ParentBranch;
    const char *CurrentBranch;
    char Filename[1000];

    /* Do a: git diff branch1 branch2 [filename] */
    RetValue=0;
    ctry(const char *)
    {
        *Filename=0;
        if(m_CmdsCount>=3)
        {
            /* We have an optional filename */
            Bytes=snprintf(Filename,sizeof(Filename),"\"%s\"",m_Cmds[2]);
            if(Bytes>sizeof(Filename))
                cthrow("Internal buffer to small");
        }
        CurrentBranch=GetCurrentBranchName();
        ParentBranch=GetParentBranchName(NULL);
        if(ParentBranch==NULL)
            cthrow(NULL);

        Bytes=snprintf(buff,sizeof(buff),"git difftool \"%s\" \"%s\" %s",
                CurrentBranch,ParentBranch,Filename);
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");

        ShellOut(buff);
    }
    ccatch(const char *Msg)
    {
        if(Msg!=NULL)
            fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    return RetValue;
}

int Do_Revert(void)
{
    char *CurrentBranchName;
    char buff[1000];
    int RetValue;
    int Bytes;
    int r;
    int o;
    bool RepoAsWell;
    const char *File2Restore;

    CurrentBranchName=NULL;
    RetValue=0;
    ctry(const char *)
    {
        /* Check all our options */
        RepoAsWell=false;
        for(r=0;r<m_CmdsCount;r++)
        {
            for(o=0;o<m_CmdOptionsCount[r];o++)
            {
                if(strcmp(m_CmdOptions[r][o],"-a")==0)
                    RepoAsWell=true;
            }
        }

        if(RepoAsWell)
        {
            CurrentBranchName=GetCurrentBranchName();
            if(CurrentBranchName==NULL)
                cthrow("Failed to get current branch name");

            /* hard = toss commits, delete working
               mix = uncommit file and put back in working
               soft = uncommit files, but leave them stashed
            */
            Bytes=snprintf(buff,sizeof(buff),"git reset --hard \"origin/%s\" ",
                    CurrentBranchName);
            if(Bytes>sizeof(buff))
                cthrow("Internal buffer to small");
        }
        else
        {
            File2Restore=".";
            if(m_CmdsCount>=2)
            {
                /* User provided a filename to revert */
                File2Restore=m_Cmds[1];
            }

            /* We are currently just doing a pass though */
            Bytes=snprintf(buff,sizeof(buff),"git restore \"%s\"",File2Restore);
            if(Bytes>sizeof(buff))
                cthrow("Internal buffer to small");
        }
        ShellOut(buff);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }

    free(CurrentBranchName);

    return RetValue;
}

/* DEBUG PAUL: Does this work if the branch you are on has not been pushed to
   the server??? */
int Do_UnCommit(void)
{
    char *CurrentBranchName;
    char buff[1000];
    int RetValue;
    int Bytes;
    int r;
    int o;
    const char *File2Restore;
    int Behind;
    int Ahead;

    RetValue=0;
    ctry(const char *)
    {
        if(!GetRepoCommitsCounts(&Behind,&Ahead))
            cthrow("Failed to get commit counts");

        if(Ahead==0)
        {
            /* We can't uncommit things that wheren't pushed */
            cthrow("Repo has no local commits.  Can't uncommit remote commits.");
        }

        /* hard = toss commits, delete working
           mix = uncommit file and put back in working
           soft = uncommit files, but leave them stashed
        */
        Bytes=snprintf(buff,sizeof(buff),"git reset --mixed \"HEAD^\"");
        if(Bytes>sizeof(buff))
            cthrow("Internal buffer to small");
        ShellOut(buff);
    }
    ccatch(const char *Msg)
    {
        fprintf(stderr,"%s\n",Msg);
        RetValue=1;
    }
    return RetValue;
}

//int Do_ListAliases(void)
//{
//    struct Alias *a;
//
//    if(!g_AliasLoadedTried)
//        LoadAliases();
//
//    for(a=g_AliasList;a!=NULL;a=a->Next)
//        printf("%-20s %s\n",a->Name,a->Value);
//}
//
//int Do_SetAlias(void)
//{
//    char buff[1000];
//    int Bytes;
//    int RetValue;
//    struct Alias *a;
//
//    if(m_CmdsCount<5)
//    {
//        /* Ok, we are actually deleting this one */
//        // Do_DeleteAlias
//        return 0;
//    }
//
//    RetValue=0;
//    ctry(const char *)
//    {
//        if(m_CmdsCount<4)
//            cthrow("Missing alias name");
//
//        if(!g_AliasLoadedTried)
//            LoadAliases();
//
//        /* See if this alias already exists */
//        for(a=g_AliasList;a!=NULL;a=a->Next)
//            if(strcasecmp(a->Name,m_Cmds[3])==0)
//                break;
//        if(a==NULL)
//        {
//            /* Adding */
//            a=malloc(sizeof(struct Alias));
//            if(a==NULL)
//                cthrow("Out of memory");
//
//            a->Name=malloc(strlen(m_Cmds[3])+1);
//            if(a->Name==NULL)
//                cthrow("Out of memory");
//            strcpy(a->Name,m_Cmds[3]);
//
//            /* Link it in */
//            a->Next=g_AliasList;
//            g_AliasList=a;
//        }
//        else
//        {
//            /* Updating */
//            free(a->Value);
//        }
//        a->Value=malloc(strlen(m_Cmds[4])+1);
//        if(a->Value==NULL)
//            cthrow("Out of memory");
//        strcpy(a->Value,m_Cmds[4]);
//        SaveAliases();
//    }
//    ccatch(const char *Msg)
//    {
//        fprintf(stderr,"%s\n",Msg);
//        RetValue=1;
//    }
//
//    return RetValue;
//}

