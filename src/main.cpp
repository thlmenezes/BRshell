#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <deque>

#define MAX_SIZE_HISTORY 10

using namespace std;

deque<string> history;

void prompt();
void version();
void add_history(string);
void print_history();
void run_command(string);
void run_history(int);

void input_loop(istream &file, bool interactivePrompt = true)
{
  string command;

  while (file.good())
  {
    if (interactivePrompt)
    {
      prompt();
    }
    getline(file, command);
    add_history(command);
    run_command(command);
  }
}

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    filebuf fb;
    if (!fb.open(argv[1], ios::in))
      exit(-1);

    istream is(&fb);
    input_loop(is, false);
    fb.close();
  }
  else
  {
    input_loop(cin);
  }

  return 0;
}

// FUNÇÕES

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
  if (command.compare("ver") == 0)
  {
    version();
  }
  if (command.compare(0, 9, "historico") == 0)
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