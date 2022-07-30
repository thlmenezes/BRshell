#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <deque>

using namespace std;

deque<string> history;

void prompt();
void version();
void add_history(string);
void print_history();
void run_command(string);

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
  history.push_back(command);
  if (history.size() > 10)
  {
    history.pop_front();
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
    print_history();
  }
}
