#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <deque>
#include <vector>
#include <sys/wait.h>
#include <map>
#include <regex>

#define MAX_SIZE_HISTORY 10

using namespace std;

deque<string> history;
map<string, string> aliases;
vector<string> path;

vector<char *> split_command(string);
void load_config();
void load_aliases();
void load_path();
void prompt();
void version();
void add_history(string);
void print_history();
void run_command(string);
void run_history(int);
void run_external(char **);

void interpreter(istream &file, bool interactivePrompt = true)
{
  string command;

  while (file.good())
  {
    if (interactivePrompt)
    {
      prompt();
    }
    do
    {
      getline(file, command);
    } while (command.compare(0, 1, "#") == 0);
    add_history(command);
    run_command(command);
  }
}

int main(int argc, char *argv[])
{
  load_config();
  if (argc > 1)
  {
    filebuf fb;
    if (!fb.open(argv[1], ios::in))
      exit(-1);

    istream is(&fb);
    interpreter(is, false);
    fb.close();
  }
  else
  {
    interpreter(cin);
  }

  return 0;
}

// FUNÇÕES

vector<char *> split_command(string command)
{
  vector<char *> args;

  char *token;

  token = strtok(&command[0], " ");

  while (token != NULL)
  {
    cout << token << endl;
    args.push_back(token);
    token = strtok(NULL, " ");
  }

  return args;
}

void load_config()
{
  load_aliases();
  load_path();
}

void load_path()
{
  string brshrc(getenv("HOME")), command;

  filebuf fb;
  if (!fb.open(brshrc + "/.BRbshrc_profile", ios::in))
    exit(-1);

  istream file(&fb);
  while (file.good())
  {
    getline(file, command);
    if (command.compare(0, 5, "PATH=") == 0)
    {
      string command_clone(command);
      smatch m;
      regex e("[=;]\\w+");

      while (regex_search(command_clone, m, e))
      {
        path.emplace_back(m[0].str().substr(1));
        command_clone = m.suffix().str();
      }
    }
  }
  fb.close();
}

void load_aliases()
{
  string brshrc(getenv("HOME")), command;

  filebuf fb;
  if (!fb.open(brshrc + "/.BRshrc", ios::in))
    exit(-1);

  istream file(&fb);
  while (file.good())
  {
    getline(file, command);
    if (command.compare(0, 5, "alias") == 0)
    {
      string command_clone(command);
      smatch m;
      regex e("\"(.+?)\"");
      vector<string> args;

      while (regex_search(command_clone, m, e))
      {
        args.emplace_back(m[1]);
        command_clone = m.suffix().str();
      }

      aliases.emplace(args[1], args[0]);
    }
  }
  fb.close();
}

void prompt()
{
  char *username;
  username = getenv("USER");
  char buffer[PATH_MAX];
  getcwd(buffer, sizeof(buffer));
  cout << "BRsh-"
       << username
       << "-"
       << buffer
       << ">";
}

void version()
{
  cout << "BRshell v0.0.0 - 2022/July/31 - Thales Menezes <@thlmenezes>" << endl;
}

void add_history(string command)
{
  if (command.compare(0, 9, "historico") == 0)
    return;
  history.push_front(command);
  if (history.size() > MAX_SIZE_HISTORY)
  {
    history.pop_back();
  }
}

void print_history()
{
  int size = history.size();
  for (int index = 0; index < size; index++)
  {
    cout << index + 1 << " " << history[index] << '\n';
  }
}

void run_command(string command)
{
  if (command.compare(0, 4, "exit") == 0)
  {
    string arg = command.substr(4);
    int exitCode = 0;
    try
    {
      exitCode = stoi(arg);
    }
    catch (const std::invalid_argument &ia)
    {
      exitCode = 0;
    }
    exit(exitCode);
  }
  if (command.compare("ver") == 0)
  {
    version();
  }
  else if (command.compare(0, 9, "historico") == 0)
  {
    string arg = command.substr(9);
    if (arg.empty())
    {
      print_history();
    }
    else
    {
      try
      {
        int number = stoi(arg);
        run_history(number);
      }
      catch (const std::invalid_argument &ia)
      {
        cerr << ia.what() << endl;
      }
    }
  }
  else
  {
    if (command.find('|') != string::npos)
      return;
    string command_clone(command);
    smatch match;
    regex e("\\w+");
    vector<string> args;

    while (regex_search(command_clone, match, e))
    {
      args.emplace_back(match[0]);
      command_clone = match.suffix().str();
    }

    vector<char *> parsed;
    parsed.reserve(args.size());
    for (auto &arg : args)
    {
      string translated(arg);
      if (aliases.find(arg) != aliases.end())
      {
        translated = aliases[arg];
      }
      parsed.push_back(const_cast<char *>(translated.c_str()));
    }

    run_external(&parsed[0]);
  }
}

void run_history(int position)
{
  int size = history.size();
  if (position < 1 || position > size)
  {
    cout << "Comando fora do intervalo do historico" << endl;
  }

  string command = history[position - 1];
  add_history(command);
  run_command(command);
}

void run_external(char **args)
{
  pid_t pid = fork();

  if (pid == -1)
  {
    cout << "Falha ao criar fork" << endl;
    return;
  }
  else if (pid == 0)
  {
    if (execvp(args[0], args) < 0)
    {
      cout << "Comando não pode ser executado" << endl;
    }
    exit(0);
  }
  else
  {
    // waiting for child to terminate
    wait(NULL);
    return;
  }
}