#pragma once

#include "MainPage.g.h"

#include <winrt/Windows.UI.Xaml.Printing.h>
#include <winrt/Windows.Graphics.Printing.h>
#include "PasswordPrintPage.h"
#include "PasswordElement.h"

namespace winrt::BitwardenExportPrint::implementation {
	struct MainPage : MainPageT<MainPage> {
		MainPage();

		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

		winrt::Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> password_elements();

	private:
		Windows::Graphics::Printing::PrintManager _print_manager = nullptr;
		Windows::Graphics::Printing::IPrintDocumentSource _print_document_source = nullptr;
		Windows::UI::Xaml::Printing::PrintDocument _print_document = nullptr;

		float _application_content_margin_left = 0.075f;
		float _application_content_margin_top = 0.03f;

		Windows::UI::Xaml::Controls::Page _print_page = winrt::make<winrt::BitwardenExportPrint::implementation::PasswordPrintPage>(*this);

		Windows::Foundation::Collections::IObservableVector<BitwardenExportPrint::PasswordElement> _password_elements = winrt::single_threaded_observable_vector<BitwardenExportPrint::PasswordElement>();

		void PrintTaskRequested(Windows::Graphics::Printing::PrintManager const&, Windows::Graphics::Printing::PrintTaskRequestedEventArgs);
		void PrintTaskCompleted(Windows::Graphics::Printing::PrintTask const&, Windows::Graphics::Printing::PrintTaskCompletedEventArgs);
		void Paginate(winrt::Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Printing::PaginateEventArgs);
		void GetPreviewPage(winrt::Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Printing::GetPreviewPageEventArgs);
		void AddPages(winrt::Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Printing::AddPagesEventArgs);

		winrt::fire_and_forget pick_file();

	public:
		void Print_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::RoutedEventArgs const&);
		void Do_Stuff_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::RoutedEventArgs const&);
	};
}

namespace winrt::BitwardenExportPrint::factory_implementation {
	struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
}