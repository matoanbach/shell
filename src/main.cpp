#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include <string>

// Data Structures
#include <vector>
#include <map>
#include <set>

#include <filesystem>

#include <sys/wait.h>

using namespace std;

bool is_running = true;
const int BUF_SIZE = 1024;
vector<string> command_paths;
set<string> builtins = {"echo", "cd", "exit", "type", "pwd"};
vector<pid_t> child_processes;

int handle_command(string buf);
vector<string> extract(string buf, string delimeter);
int on_echo(string buf, vector<string> extracted_buf);
int on_type(string buf, vector<string> extracted_buf);
int on_exec(string buf, vector<string> extracted_buf);
int execute(string buf, vector<string> parameters);
int on_pwd(string buf, vector<string> parameters);
int on_cd(string buf, vector<string> extracted_buf);
int on_exec(string buf, vector<string> extracted_buf);
string join(vector<string> extracted_buf, string delimeter);

int main()
{

  // Flush after every cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  string PATH_env;
  PATH_env = getenv("PATH");

  // builtin commands exists at /usr/bin/[cd, cat, ...]
  // non-builtin commands exists at /bin/

  // cout << "$PATH:" << PATH_env << endl;
  if (!PATH_env.empty())
    command_paths = extract(PATH_env, ":");

  // for (auto e : command_paths) cout << e << endl;

  string buf;
  int len;
  while (cin && is_running)
  {
    cout << "$ ";
    if (!getline(cin, buf))
      cout << "error: read buffer" << endl;

    handle_command(buf);
  }
  // cout << "exit(0)" << endl;
  // int status = -1;
  // pid_t pid;
  // while (pid >= 0)
  //   pid = wait(&status);

  exit(0);
}

int on_echo(string buf, vector<string> extracted_buf)
{
  for (size_t i = 1; i < extracted_buf.size(); i++)
    cout << extracted_buf[i] << (i == extracted_buf.size() - 1 ? "\n" : " ");
  return 0;
}

int on_type(string buf, vector<string> extracted_buf)
{
  if (command_paths.empty())
  {
    cout << "on_type: empty PATH" << endl;
    return -1;
  }

  string type = extracted_buf[1];
  for (auto path : command_paths)
  {
    string file_path = path + "/" + type;
    // cout << file_path << endl;
    if (filesystem::exists(file_path) || builtins.find(type) != builtins.end())
    {
      cout << type << " is " << (builtins.find(type) != builtins.end() ? "a shell builtin" : file_path) << endl;
      return 0;
    }
  }
  cout << type << ": not found" << endl;
  return -1;
}

int on_pwd(string buf, vector<string> extracted_buf)
{
  cout << filesystem::current_path().string() << endl;
  return 0;
}

int on_cd(string buf, vector<string> extracted_buf)
{

  string file = extracted_buf[1];
  vector<string> current_path;
  string new_path;
  if (!file.empty())
    current_path = extract(filesystem::current_path().string(), "/");

  // for (auto e : extracted_buf) cout << e << endl;
  // cout << current_path.size() << endl;


  if (file == "~")
  {
    new_path = getenv("HOME");
  }
  else if (file == "..")
  {
    current_path.pop_back();
    new_path = join(current_path, "/");
  }
  else if (file[0] == '/')
  {
    new_path = file;
  }
  else if (!file.empty())
  {
    // if (file[0] == '.')
    //   current_path.push_back(file.substr(1, file.size() - 1));
    // else
    
    current_path.push_back(file);
    
    new_path = join(current_path, "/");
  }

  try
  {
    filesystem::current_path(new_path);
  }
  catch (const filesystem::filesystem_error &e)
  {
    cout << "cd: " << file << ": No such file or directory" << endl;
  }

  return 0;
}

int on_exec(string buf, vector<string> extracted_buf)
{
  string command = extracted_buf[0];
  // cout << buf << endl;

  for (auto path : command_paths)
  {
    string file_path = path + "/" + command;
    if (filesystem::exists(file_path))
    {
      extracted_buf[0] = file_path;
      execute(buf, extracted_buf);
      return 0;
    }
  }
  cout << command << ": command not found" << endl;
  return 0;
}

char **copy(vector<string> parameters) // (dest, src)
{
  char **argv = new char *[parameters.size()];
  for (size_t i = 0; i < parameters.size(); i++)
    argv[i] = (char *)parameters[i].c_str();
  return argv;
}

int execute(string buf, vector<string> parameters)
{

  // // Convert vector<string> to vector<char>
  char **argv = new char *[parameters.size()];
  for (size_t i = 0; i < parameters.size(); i++)
    argv[i] = (char *)parameters[i].c_str();

  pid_t new_child = fork();
  if (new_child == 0)
  {
    // execl(parameters[0].c_str(), parameters[0].c_str(), parameters[1].c_str(), (char *)NULL);
    execv(argv[0], argv);
    cout << "execute: " << buf << " failed" << endl;
    exit(EXIT_FAILURE);
  }
  else if (new_child > 0)
  {
    int status;
    waitpid(new_child, &status, 0);
  }
  else
  {
    cout << "execute: fork failed" << endl;
  }
  return 0;
}

vector<string> extract(string buf, string delimeter)
{
  vector<string> tokens;
  size_t start = 0;
  size_t end = buf.find(delimeter);
  while (end != string::npos)
  {
    string token = buf.substr(start, end - start);
    if (token.size() > 1 || token != ".")
      tokens.push_back(token);
    start = end + 1;
    end = buf.find(delimeter, start);
  }
  tokens.push_back(buf.substr(start));

  // for (auto e : tokens) cout << e << endl;

  return tokens;
}

string join(vector<string> extracted_buf, string delimeter)
{
  if (extracted_buf.empty())
    return "";
  string new_string;
  for (auto str : extracted_buf)
    new_string += str + delimeter;
  return new_string;
}

int handle_command(string buf)
{
  vector<string> extracted_buf = extract(buf, " ");
  string command = extracted_buf[0];

  if (command == "exit")
    is_running = false;
  else if (command == "echo")
    on_echo(buf, extracted_buf);
  else if (command == "type")
    on_type(buf, extracted_buf);
  else if (command == "pwd")
    on_pwd(buf, extracted_buf);
  else if (command == "cd")
    on_cd(buf, extracted_buf);
  else
    on_exec(buf, extracted_buf);
  return 0;
}
