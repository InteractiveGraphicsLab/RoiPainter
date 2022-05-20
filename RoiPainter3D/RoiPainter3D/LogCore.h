#ifndef LOGCORE_H_
#define LOGCORE_H_
#pragma unmanaged

#include <string>
#include <vector>
#include <map>


class LogCore
{
  bool m_b_recording;
  std::vector<std::pair<std::string, std::string> > m_lines;

  LogCore();
public:
  ~LogCore(){}

  static LogCore* GetInst(){
    static LogCore p;
    return &p;
  }

  //interface
  void StartLogger();
  void Add( std::string s);
  void CloseLogger(std::string filename);
};

#endif