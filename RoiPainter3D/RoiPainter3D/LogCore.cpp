#pragma unmanaged

#include <iostream>
#include <fstream>
#include "LogCore.h"
#include "time.h"


LogCore::LogCore()
{
  m_b_recording = false;
  m_lines.clear();
}


void LogCore::StartLogger()
{
  m_b_recording = true;
  m_lines.clear();
  Add("StartLogging");
}


void LogCore::Add( std::string s)
{
  if(!m_b_recording ) return;

  time_t currenttime = time(NULL);
  struct tm t;
  errno_t err;
  err = localtime_s(&t, &currenttime);
  std::string log = 
      std::to_string( t.tm_year+1900 )
    + std::to_string( t.tm_mon + 1   )
    + std::string((t.tm_mday < 10 ) ? "0" : "") + std::to_string( t.tm_mday )
    + std::string((t.tm_hour < 10 ) ? "0" : "") + std::to_string( t.tm_hour )
    + std::string((t.tm_min  < 10 ) ? "0" : "") + std::to_string( t.tm_min  )
    + std::string((t.tm_sec  < 10 ) ? "0" : "") + std::to_string( t.tm_sec  );
  m_lines.push_back(std::make_pair(log,s));
}



void LogCore::CloseLogger(std::string filename)
{
  if(!m_b_recording ) return;

  //export 
  std::ofstream ofs( filename.c_str() );

  if (!ofs){
    std::cout << "failed to open the file " << filename.c_str() << "\n";
    return;
  }

  ofs << "TIME, MESSAGE\n";
  for ( auto line : m_lines) 
    ofs << line.first.c_str() << ", " << line.second.c_str() << "\n";
  ofs.close();

  m_b_recording = false;
}

