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
void run_external(vector<string>);
string translate_alias(string);
void exec_command(vector<string>);

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
      string command_clone = command.substr(5);
      auto index = command_clone.find(";");

      while (index != string::npos)
      {
        path.emplace_back(command_clone.substr(0, index));
        command_clone = command_clone.substr(index + 1);
        index = command_clone.find(";");
      }

      if (!command_clone.empty())
        path.emplace_back(command_clone);
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
  if (command.empty())
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
  // string.split() pythonico
  string command_clone(command);
  smatch match;
  regex e("[^\\s]+");
  vector<string> args;

  while (regex_search(command_clone, match, e))
  {
    args.emplace_back(match[0]);
    // DEBUG: cout << match[0] << endl;
    command_clone = match.suffix().str();
  }

  if (args.empty())
    return;

  // use alias for command
  args[0] = translate_alias(args[0]);

  if (args[0] == "exit")
  {
    int exitCode = 0;
    try
    {
      if (args.size() > 1)
        exitCode = stoi(args[1]);
    }
    catch (const std::invalid_argument &ia)
    {
      exitCode = 0;
    }
    exit(exitCode);
  }
  else if (args[0] == "ver")
  {
    version();
  }
  else if (args[0] == "historico")
  {
    if (args.size() == 1)
    {
      print_history();
      return;
    }

    try
    {
      int number = stoi(args[1]);
      run_history(number);
    }
    catch (const std::invalid_argument &ia)
    {
      cerr << ia.what() << endl;
    }
  }
  else
  {
    if (command.find('|') != string::npos)
      return;

    run_external(args);
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

void run_external(vector<string> args)
{
  pid_t pid = fork();

  if (pid == -1)
  {
    cout << "Falha ao criar fork" << endl;
    return;
  }
  else if (pid == 0)
  {
    exec_command(args);
    exit(0);
  }
  else
  {
    // waiting for child to terminate
    wait(NULL);
    return;
  }
}

string translate_alias(string alias)
{
  if (aliases.count(alias) > 0)
    return aliases[alias];
  return alias;
}

void exec_command(vector<string> args)
{
  // Converte os argumentos string->const char *
  vector<char *> args_formated;
  args_formated.reserve(args.size());
  for (auto &arg : args)
  {
    args_formated.push_back(const_cast<char *>(arg.c_str()));
  }

  // Para cada path especificado no profile, tenta executar o comando
  for (unsigned i = 0; i < path.size(); i++)
  {
    string cmd = path[i] + args[0];
    args_formated[0] = const_cast<char *>(cmd.c_str());
    execv(cmd.c_str(), &args_formated[0]);
  }

  // Caso não tenha conseguido executar o comando, tenta executar um arquivo
  args_formated[0] = const_cast<char *>(args[0].c_str());
  execv(args[0].c_str(), &args_formated[0]);

  // Caso não tenha conseguido executar um comando ou arquivo, avisa o usuário
  cout << "Nao achei o comando: " << args[0] << endl;
}