#pragma once

#include "PasswordPrintPage.g.h"

#include "PasswordElement.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::BitwardenExportPrint::implementation {
	struct PasswordPrintPage : PasswordPrintPageT<PasswordPrintPage> {
		PasswordPrintPage() = default;

		PasswordPrintPage(winrt::BitwardenExportPrint::MainPage const&, uint32_t const&);

		winrt::Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> PasswordPrintPage::password_elements();

	private:
		BitwardenExportPrint::MainPage _main_page;
		uint32_t _page_number;
	};
}

namespace winrt::BitwardenExportPrint::factory_implementation {
	struct PasswordPrintPage : PasswordPrintPageT<PasswordPrintPage, implementation::PasswordPrintPage> {};
}
