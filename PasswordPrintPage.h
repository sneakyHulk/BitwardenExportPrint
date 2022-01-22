#pragma once

#include "PasswordPrintPage.g.h"

#include "PasswordElement.h"

namespace winrt::BitwardenExportPrint::implementation {
	struct PasswordPrintPage : PasswordPrintPageT<PasswordPrintPage> {
		PasswordPrintPage() = default;

		PasswordPrintPage(winrt::BitwardenExportPrint::MainPage const&);

		winrt::Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> PasswordPrintPage::password_elements();

	private:
		BitwardenExportPrint::MainPage _main_page;
	};
}

namespace winrt::BitwardenExportPrint::factory_implementation {
	struct PasswordPrintPage : PasswordPrintPageT<PasswordPrintPage, implementation::PasswordPrintPage> {};
}
