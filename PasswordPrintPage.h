#pragma once

#include "PasswordPrintPage.g.h"

#include "PasswordElement.h"
#include "DatetimeConverter.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::BitwardenExportPrint::implementation {
	struct PasswordPrintPage : PasswordPrintPageT<PasswordPrintPage> {
		PasswordPrintPage() = default;
		PasswordPrintPage(winrt::BitwardenExportPrint::MainPage const&, uint32_t const&);

		void UpdateData(uint32_t const&);
		uint32_t page_number();
		uint32_t final_page_number();
		winrt::Windows::Foundation::DateTime now();

		winrt::Windows::Foundation::Collections::IObservableVector<BitwardenExportPrint::PasswordElement> PasswordPrintPage::password_elements();

	private:
		BitwardenExportPrint::MainPage _main_page;
		uint32_t _page_number;
		uint32_t _final_page_number;
		Windows::Foundation::DateTime _now;

		Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> _password_elements = winrt::single_threaded_observable_vector<BitwardenExportPrint::PasswordElement>();

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
	public:
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler) {
			return _propertyChanged.add(handler);
		}
		void PropertyChanged(winrt::event_token const& token) noexcept {
			_propertyChanged.remove(token);
		}
	};
}

namespace winrt::BitwardenExportPrint::factory_implementation {
	struct PasswordPrintPage : PasswordPrintPageT<PasswordPrintPage, implementation::PasswordPrintPage> {};
}
