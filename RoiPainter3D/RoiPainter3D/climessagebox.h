#pragma once

namespace RoiPainter3D {

  using namespace System;
  using namespace System::Text;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  // UTF-8の(const char *)文字列をUTF-16の(String ^)に変換
  // フォーム上では問題ないが、ダイアログ上ではこの変換が必要
  inline String^ Utf8ToUtf16(const char* text)
  {
    // UTF-8バイト配列を作成
    cli::array<Byte>^ utf8Bytes = gcnew cli::array<Byte>((int)strlen(text));
    for (int i = 0; i < utf8Bytes->Length; ++i)
    {
      utf8Bytes[i] = text[i];
    }

    // UTF-8バイト配列をUTF-16文字配列に変換
    cli::array<Char>^ utf16Chars = Encoding::UTF8->GetChars(utf8Bytes);

    // UTF-16文字配列をStringに変換
    String^ textStr = gcnew String(utf16Chars);

    return textStr;
  }

  inline bool CLI_MessageBox_OK_Show(const char* text, const char* dlg_title)
  {
    String^ textStr = Utf8ToUtf16(text);
    String^ capStr = Utf8ToUtf16(dlg_title);
    MessageBox::Show(textStr, capStr, MessageBoxButtons::OK, MessageBoxIcon::Asterisk);
    delete(textStr);
    delete(capStr);
    return true;
  }

  inline bool CLI_MessageBox_YESNO_Show(const char* text, const char* dlg_title)
  {
    String^ textStr = Utf8ToUtf16(text);
    String^ capStr = Utf8ToUtf16(dlg_title);
    return DialogResult::Yes == MessageBox::Show(textStr, capStr, MessageBoxButtons::YesNo, MessageBoxIcon::Question);
    delete(textStr);
    delete(capStr);

  }

}
