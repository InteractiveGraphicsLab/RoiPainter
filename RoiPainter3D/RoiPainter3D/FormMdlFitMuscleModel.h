#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMdlFitMuscleModel の概要
	/// </summary>
	public ref class FormMdlFitMuscleModel : public System::Windows::Forms::Form
	{


	private:
		static FormMdlFitMuscleModel^ m_singleton;
		FormMdlFitMuscleModel(void)
		{
			InitializeComponent();
		}


	public:
		static FormMdlFitMuscleModel^ getInst()
		{
			if (m_singleton == nullptr) m_singleton = gcnew FormMdlFitMuscleModel();
			return m_singleton;
		}


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormMdlFitMuscleModel()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"FormMdlFitMuscleModel";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};

	inline void formMdlFitMuscle_Show() { FormMdlFitMuscleModel::getInst()->Show(); }
	inline void formMdlFitMuscle_Hide() { FormMdlFitMuscleModel::getInst()->Hide(); }


}
