# SimpleGit
Simple git.  A wrapper around git to make it easier to work with

# Why
Git is very powerful but the UI to use it is very complicated.  Simple things
require you to remember long strings of commands and options and sometime
many steps to figure out the command you want.

For example the simple request "show me the clone URL" end up being complex
lines like "git ls-remote --get-url" or "git config --get remote.origin.url" or
"git remote show origin".  I can never remember any of these and have to
search the web for them.  SVN in contrast is just do "svn info" and it tells
you.  Mercurial uses "hg paths".

SimpleGit tries to make git more friendly by changing the UI to git and then
shelling out it git to actually do the command.  So to get the clone URL you
use "sgit info".

# Now
At this point it is more of a simple wrapper that makes my life easier.  It will
likely be inconsistent, missing things, and have things change all the time
(as I change my mind).  Still it's helping me so I though I would share.

# Examples
## Get Clone URL
Get the URL that can be used to clone this repo.
```
sgit info
```

## Checked in changes to branch (compared to main)
Show all the files that have been checked in to a branch since the last time
the branch was merged.
```
sgit branch status
```

## Visual Diff
Uses the visual diff tool to show the diff.  This is the same as difftool but
is easier to remember.
```
sgit vdiff
```

## Base Branch Point
Get the hash of where the branch will merge from (where the branch was made)
```
sgit branch base
```

# Building
Not much of a build setup at this point. Just
```
gcc main.c -o sgit
```
This works with gcc on Windows:
```
C:\sandboxes\SimpleGit>gcc --version
gcc (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 7.3.0
Copyright (C) 2017 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
