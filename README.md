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

- [x] BRShell scripts
- [x] Infinite loop receiving commands of variable length
- [x] Command `ver`: BRshell version, update date, author
- [x] Command `historico`: 10 commands history queue
- [] Command `historico [n]`: run command in history queue position `n`
    - in execution, adds the command to queue
    - IndexOutOfBounds: shows `Comando fora do intervalo do histórico`
- [] Ignore lines starting with `#`
- [] Redirect std{in,out} to files
- [] Read PATH from $HOME/.BRbshrc_profile
- [] search commands in PATH
- [] enable pipe usage
- [] Read aliases from $HOME/.BRshrc
- [] Add Translation Preprocessing using aliases
- [] Background jobs