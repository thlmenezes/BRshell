# BRshell

An easy to use shell interface for brazilian speaking users

## Command Syntax

A command can consist of 2 formats:

- Simple Command: A name from executable, found in PATH, followed by one or more arguments. Ex: ps -elf

`cmdo_name arg[1] ... arg[n]`

- One or more "pipe sections": each section being a simple command separated by the caracter `|`. Ex: ps -elf | grep user | more

`pipe_section[1] | ... | pipe_section[n]`

## .BRbshrc_profile

```txt
PATH=path1;path2;path3; . . . ;pathN
```

## .BRshrc

```txt
alias "cd" "mudar"
```

## Roadmap

- [] BRShell scripts
- [] Infinite loop receiving commands of variable length
- [] Command `ver`: BRshell version, update date, author
- [] Command `histórico`: 10 commands history queue
- [] Command `histórico [n]`: run command in history queue position `n`
    - in execution, adds the command to queue
    - IndexOutOfBounds: shows `Comando fora do intervalo do histórico`
- [] Read $HOME/.BRbshrc_profile
- [] search commands in PATH
- [] enable pipe usage
- [] Read $HOME/.BRshrc
- [] Redirect std{in,out,err}
- [] Background jobs