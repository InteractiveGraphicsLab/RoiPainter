#pragma once

namespace RoiPainter4D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegStrokeFfd �̊T�v
	/// </summary>
	public ref class FormSegStrokeFfd : public System::Windows::Forms::Form
	{
	private:
		static FormSegStrokeFfd^ m_singleton;
		FormSegStrokeFfd(void)
		{
			InitializeComponent();
		}

	public: 
		static FormSegStrokeFfd^ GetInst()
		{
			if (m_singleton == nullptr) {
				m_singleton = gcnew FormSegStrokeFfd();
			}
			return m_singleton;
		}

		void MyFunction() {
			std::cout << "aaaa\n";
		}


	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~FormSegStrokeFfd()
		{
			if (components)
			{
				delete components;
			}
		}

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
			this->SuspendLayout();
			// 
			// FormSegStrokeFfd
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(465, 435);
			this->Name = L"FormSegStrokeFfd";
			this->Text = L"FormSegStrokeFfd";
			this->ResumeLayout(false);

		}
#pragma endregion
	};

	inline void FormSegStrokeFfd_Show() { FormSegStrokeFfd::GetInst()->Show(); }
	inline void FormSegStrokeFfd_Hide() { FormSegStrokeFfd::GetInst()->Hide(); }

	inline void FormSegStrokeFfd_MyFunction() 
	{
		FormSegStrokeFfd::GetInst()->MyFunction();
	}
}
