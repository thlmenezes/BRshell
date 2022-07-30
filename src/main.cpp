#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <queue>

using namespace std;

queue<string> history;

void input_loop(istream &file, bool interactivePrompt = true)
{
  string command;
  char *username;
  username = getenv("USER");

  while (file.good())
  {
    if (interactivePrompt)
    {
      char buffer[PATH_MAX];
      getcwd(buffer, sizeof(buffer));
      cout << "BRsh-"
           << username
           << "-"
           << buffer
           << ">";
    }
    getline(file, command);
    if (command.compare("ver") == 0)
    {
      cout << "BRshell v0.0.0 - 2022/July/31 - Thales Menezes <@thlmenezes>" << endl;
    }
    if (command.compare(0, 9, "historico") == 0)
    {
      int index = 1;
      for (queue<string> dump(history); !dump.empty(); dump.pop())
      {
        cout << index++ << " " << dump.front() << '\n';
      }
    }
    history.push(command);
    if (history.size() > 10)
    {
      history.pop();
    }
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