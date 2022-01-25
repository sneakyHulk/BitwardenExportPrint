#include "pch.h"
#include "PasswordPrintPage.h"
#if __has_include("PasswordPrintPage.g.cpp")
#include "PasswordPrintPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::BitwardenExportPrint::implementation {
	PasswordPrintPage::PasswordPrintPage(winrt::BitwardenExportPrint::MainPage const& main_page, uint32_t const& page_number) : _main_page(main_page), _page_number(page_number), _final_page_number(page_number + 1), _now(winrt::clock::now()) {
		InitializeComponent();
	}

	void PasswordPrintPage::UpdateData(uint32_t const& value) {
		if (_final_page_number != value) {
			_final_page_number = value;

			_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"final_page_number" });
		}

		auto now = winrt::clock::now();

		if (_now != now) {
			_now = now;

			_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"now" });
		}
	}
	uint32_t PasswordPrintPage::page_number() {
		return _page_number + 1;
	}
	uint32_t PasswordPrintPage::final_page_number() {
		return _final_page_number;
	}
	winrt::Windows::Foundation::DateTime PasswordPrintPage::now() {
		return _now;
	}

	winrt::Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> PasswordPrintPage::password_elements() {
		return _password_elements;
	}
}
