#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void input_loop(istream &file)
{
  string command;

  while (file.good())
  {
    getline(file, command);
    cout << command;
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
    input_loop(is);
    fb.close();
  }
  else
  {
    input_loop(cin);
  }

  return 0;
}