#include "pch.h"
#include "PasswordPrintPage.h"
#if __has_include("PasswordPrintPage.g.cpp")
#include "PasswordPrintPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::BitwardenExportPrint::implementation {
	/*PasswordPrintPage::PasswordPrintPage() {
		InitializeComponent();
	}*/
	PasswordPrintPage::PasswordPrintPage(winrt::BitwardenExportPrint::MainPage const& main_page) : _main_page(main_page) {
		InitializeComponent();
	}
	winrt::Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> PasswordPrintPage::password_elements() {
		return _main_page.password_elements();
	}
}
