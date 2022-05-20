#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormStackOrder �̊T�v
	/// </summary>
	public ref class FormStackOrder : public System::Windows::Forms::Form
	{
	public:
		FormStackOrder(void)
		{
			InitializeComponent();
		}

    int m_flg;
	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~FormStackOrder()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::PictureBox^  pictureBox1;
  protected:
  private: System::Windows::Forms::Button^  btn_ascending;
  private: System::Windows::Forms::Button^  btn_descending;

	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�[�ϐ��ł��B
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i�[ �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�[�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void)
		{
      System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(FormStackOrder::typeid));
      this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
      this->btn_ascending = (gcnew System::Windows::Forms::Button());
      this->btn_descending = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
      this->SuspendLayout();
      // 
      // pictureBox1
      // 
      this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
      this->pictureBox1->Location = System::Drawing::Point(2, 0);
      this->pictureBox1->Name = L"pictureBox1";
      this->pictureBox1->Size = System::Drawing::Size(548, 374);
      this->pictureBox1->TabIndex = 0;
      this->pictureBox1->TabStop = false;
      // 
      // btn_ascending
      // 
      this->btn_ascending->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btn_ascending->Font = (gcnew System::Drawing::Font(L"���C���I", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_ascending->Location = System::Drawing::Point(47, 380);
      this->btn_ascending->Name = L"btn_ascending";
      this->btn_ascending->Size = System::Drawing::Size(146, 43);
      this->btn_ascending->TabIndex = 1;
      this->btn_ascending->Text = L"ascending(����)";
      this->btn_ascending->UseVisualStyleBackColor = true;
      this->btn_ascending->Click += gcnew System::EventHandler(this, &FormStackOrder::btn_ascending_Click);
      // 
      // btn_descending
      // 
      this->btn_descending->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btn_descending->Font = (gcnew System::Drawing::Font(L"���C���I", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_descending->Location = System::Drawing::Point(314, 380);
      this->btn_descending->Name = L"btn_descending";
      this->btn_descending->Size = System::Drawing::Size(146, 43);
      this->btn_descending->TabIndex = 1;
      this->btn_descending->Text = L"decending(�~��)";
      this->btn_descending->UseVisualStyleBackColor = true;
      this->btn_descending->Click += gcnew System::EventHandler(this, &FormStackOrder::btn_descending_Click);
      // 
      // FormStackOrder
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(551, 435);
      this->Controls->Add(this->btn_descending);
      this->Controls->Add(this->btn_ascending);
      this->Controls->Add(this->pictureBox1);
      this->Name = L"FormStackOrder";
      this->Text = L"FormStackOrder";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
      this->ResumeLayout(false);

    }
#pragma endregion
  private: System::Void btn_ascending_Click(System::Object^  sender, System::EventArgs^  e) 
  {
    m_flg = 0;

  }

  private: System::Void btn_descending_Click(System::Object^  sender, System::EventArgs^  e) 
  {
    m_flg = 1;
  
  }

  };

  // this functin returns flg : 0 descending,  1.ascending order
  inline int formStackOrder_showModalDialog()
  {
    FormStackOrder ^modal = gcnew FormStackOrder();
    if (modal->ShowDialog() == DialogResult::OK) return modal->m_flg;

    //�L�����Z���{�^����������Ă��܂����������x�����\���iescape�L�[�Ȃǁj
    if (modal->ShowDialog() == DialogResult::OK) return modal->m_flg;
    return 0;
  }

}
