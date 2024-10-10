#pragma unmanaged

#include "GlslShader.h"
#include <iostream>

#pragma warning(disable : 4996)
//床井先生のページ参照 http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051006


// シェーダーのソースプログラムをメモリに読み込む by Tokoi-senssei
int t_readShaderSource(GLuint shader, const char *file)
{
  //ファイルを開く
  FILE *fp = fopen(file, "rb");
  if (fp == NULL) {
    perror(file);
    return -1;
  }

  //ファイルの末尾に移動し現在位置（つまりファイルサイズ）を得る
  fseek(fp, 0L, SEEK_END);
  GLsizei length = ftell(fp);

  //ファイルサイズのメモリを確保 
  const GLchar *source = (GLchar *)malloc(length);
  if (source == NULL) 
  {
    std::cout << "Could not allocate read buffer.\n";
    return -1;
  }

  // ファイルを先頭から読み込む 
  fseek(fp, 0L, SEEK_SET);
  int ret = fread((void *)source, 1, length, fp) != (size_t)length;
  fclose(fp);

  // シェーダのソースプログラムのシェーダオブジェクトへの読み込み
  if (ret) std::cout << "Could not read file:" <<  file << "\n";
  else glShaderSource(shader, 1, &source, &length);

  free((void *)source);

  return ret;
}

//by Tokoi Sensei
void printShaderInfoLog(GLuint shader)
{
  GLsizei bufSize;

  /* シェーダのコンパイル時のログの長さを取得する */
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

  if (bufSize > 1) {
    GLchar *infoLog = (GLchar *)malloc(bufSize);

    if (infoLog != NULL) {
      GLsizei length;

      /* シェーダのコンパイル時のログの内容を取得する */
      glGetShaderInfoLog(shader, bufSize, &length, infoLog);
      std::cout << "InfoLog:\n";
      std::cout <<  infoLog << "\n\n";
      free(infoLog);
    }
    else
    {
      std::cout << "Could not allocate InfoLog buffer.\n";
    }
  }
}


bool t_initializeShader
(
  const char* vtxFname,
  const char* frgFname,
  GLuint &_gl2Program
)
{
  std::cout << "initializeShader\n";
  GLuint  vertShaderId;
  GLuint  fragShaderId;

  // gen shader object and read shader programs
  vertShaderId = glCreateShader(GL_VERTEX_SHADER);
  fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  if (t_readShaderSource(vertShaderId, vtxFname)) return false;
  if (t_readShaderSource(fragShaderId, frgFname)) return false;


  // compile vertex shader and fragment shader 
  GLint isCompiled;
  glCompileShader(vertShaderId);
  glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    std::cout << "Compile error in vertex shader.\n";
    printShaderInfoLog(vertShaderId);
    return false;
  }
  glCompileShader(fragShaderId);
  glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    std::cout << "Compile error in fragment shader.\n";
    printShaderInfoLog(fragShaderId);
    return false;
  }


  // generate program object 
  _gl2Program = glCreateProgram();
  glAttachShader(_gl2Program, vertShaderId);
  glAttachShader(_gl2Program, fragShaderId);
  glDeleteShader(vertShaderId);
  glDeleteShader(fragShaderId);

  GLint isLined;
  glLinkProgram(_gl2Program);
  glGetProgramiv(_gl2Program, GL_LINK_STATUS, &isLined);
  if (isLined == GL_FALSE) 
  {
    std::cout << "Link error.\n";
    exit(1);
  }

  std::cout << "success initialize shader!!\n";

  return true;
}







void GlslShaderVolume::Bind
(
  int UnitID_vol ,//3D 
  int UnitID_gMag,//3D 
  int UnitID_flg ,//3D 
  int UnitID_mask,//3D
  int UnitID_tf  ,//1D
  int UnitID_psu ,//1D
  int UnitID_mskC,//1D
  float   alpha  ,
  EVec3i   reso  ,
  EVec3f camPos  ,
  bool doPsuedo  ,
  bool visOthorRois
)
{
  if (!m_b_initialized)
  {
    t_initializeShader(m_vtxshader_fname.c_str(), m_frgshader_fname.c_str(), m_gl_program);
    m_b_initialized = true;
  }
  glUseProgram(m_gl_program);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_vol" ), UnitID_vol);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_gMag"), UnitID_gMag);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_flg" ), UnitID_flg);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_mask"), UnitID_mask);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_tf"  ), UnitID_tf);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_psu" ), UnitID_psu);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_mskC"), UnitID_mskC);
  glUniform1f(glGetUniformLocation(m_gl_program, "u_alpha"    ), alpha);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_doPsuedo" ), doPsuedo ? 1 : 0);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_doOtherROIs"), visOthorRois ? 1 : 0);
  glUniform4f(glGetUniformLocation(m_gl_program, "u_texCdOfst"), (GLfloat)1.0 / reso[0], (GLfloat)1.0 / reso[1], (GLfloat)1.0 / reso[2], 0);
  glUniform4f(glGetUniformLocation(m_gl_program, "u_eyePos"   ), (GLfloat)camPos[0], (GLfloat)camPos[1], (GLfloat)camPos[2], 0);
}



void GlslShaderCrsSec::Bind
(
  int UnitID_vol,//3D 
  int UnitID_gMag,//3D 
  int UnitID_flg,//3D 
  int UnitID_mask,//3D
  int UnitID_mskC,//1D
  EVec3i reso,
  bool   gMag,
  bool   doHL     //do high light 
)
{
  if (!m_b_initialized)
  {
    t_initializeShader(m_vtxshader_fname.c_str(), m_frgshader_fname.c_str(), m_gl_program);
    m_b_initialized = true;
  }
  glUseProgram(m_gl_program);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_vol"), UnitID_vol);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_gMag"), UnitID_gMag);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_flg"), UnitID_flg);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_mask"), UnitID_mask);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_mskC"), UnitID_mskC);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_gMag"), gMag ? 1 : 0);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_doHL"), doHL ? 1 : 0);
  glUniform4f(glGetUniformLocation(m_gl_program, "u_texCdOfst"), (GLfloat)1.0 / reso[0], (GLfloat)1.0 / reso[1], (GLfloat)1.0 / reso[2], 0);
}




void GlslShaderVolumePolar::Bind(
  int UnitID_vol,//3D 
  int UnitID_gMag,//3D 
  int UnitID_flg,//3D 
  int UnitID_mask,//3D
  int UnitID_tf,//1D
  int UnitID_psu,//1D
  int UnitID_mskC,//1D
  float alpha, EVec3i reso, EVec3f camPos,
  EVec3f center, float radi, float minPhi, float maxPhi,
  bool tf)
{
  if (!m_b_initialized)
  {
    t_initializeShader(m_vtxshader_fname.c_str(), m_frgshader_fname.c_str(), m_gl_program);
    m_b_initialized = true;
  }

  glUseProgram(m_gl_program);
  EVec3f pixOfst(1.0f / reso[0], 1.0f / reso[1], 1.0f / reso[2]);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_vol"), UnitID_vol);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_flg"), UnitID_flg);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img3_mask"), UnitID_mask);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_tf"), UnitID_tf);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_psu"), UnitID_psu);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_img1_mskC"), UnitID_mskC);
  glUniform1f(glGetUniformLocation(m_gl_program, "u_alpha"), alpha);
  glUniform1i(glGetUniformLocation(m_gl_program, "u_doPsuedo"), tf ? 1 : 0);

  glUniform3f(glGetUniformLocation(m_gl_program, "u_center"), center[0], center[1], center[2]);
  glUniform1f(glGetUniformLocation(m_gl_program, "u_radi"), radi);
  glUniform1f(glGetUniformLocation(m_gl_program, "u_minPhi"), minPhi);
  glUniform1f(glGetUniformLocation(m_gl_program, "u_maxPhi"), maxPhi);

  glUniform4f(glGetUniformLocation(m_gl_program, "u_texCdOfst"), pixOfst[0], pixOfst[1], pixOfst[2], 0);
  glUniform4f(glGetUniformLocation(m_gl_program, "u_eyePos"), camPos[0], camPos[1], camPos[2], 0);
}

