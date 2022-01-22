#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include <sstream>
#include <nlohmann/json.hpp>
#include <winrt/Windows.Storage.Pickers.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::BitwardenExportPrint::implementation {
	MainPage::MainPage() {
		InitializeComponent();
	}
	void MainPage::OnNavigatedTo([[maybe_unused]] Windows::UI::Xaml::Navigation::NavigationEventArgs const& args) {
		_print_manager = Windows::Graphics::Printing::PrintManager::GetForCurrentView();
		_print_manager.PrintTaskRequested(Windows::Foundation::TypedEventHandler<Windows::Graphics::Printing::PrintManager, Windows::Graphics::Printing::PrintTaskRequestedEventArgs>(this, &MainPage::PrintTaskRequested));

		_print_document = Windows::UI::Xaml::Printing::PrintDocument();
		_print_document_source = _print_document.DocumentSource();
		_print_document.Paginate(Windows::UI::Xaml::Printing::PaginateEventHandler(this, &MainPage::Paginate));
		_print_document.GetPreviewPage(Windows::UI::Xaml::Printing::GetPreviewPageEventHandler(this, &MainPage::GetPreviewPage));
		_print_document.AddPages(Windows::UI::Xaml::Printing::AddPagesEventHandler(this, &MainPage::AddPages));
	}
	winrt::Windows::Foundation::Collections::IObservableVector<winrt::BitwardenExportPrint::PasswordElement> MainPage::password_elements() {
		return _password_elements;
	}
}

void winrt::BitwardenExportPrint::implementation::MainPage::Print_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Windows::UI::Xaml::RoutedEventArgs const& args) {
	if (Windows::Graphics::Printing::PrintManager::IsSupported()) {
		try {
			_print_manager.ShowPrintUIAsync();
		}
		catch (...) {
			Windows::UI::Xaml::Controls::ContentDialog no_printing;
			no_printing.Title(winrt::box_value(L"Printing error"));
			no_printing.Content(winrt::box_value(L"\nSorry, printing can' t proceed at this time."));
			no_printing.PrimaryButtonText(L"OK");

			no_printing.ShowAsync();
		}
	}
	else {
		Windows::UI::Xaml::Controls::ContentDialog no_printing;
		no_printing.Title(winrt::box_value(L"Printing not supported"));
		no_printing.Content(winrt::box_value(L"\nSorry, printing is not supported on this device."));
		no_printing.PrimaryButtonText(L"OK");

		no_printing.ShowAsync();
	}
}

void winrt::BitwardenExportPrint::implementation::MainPage::PrintTaskRequested([[maybe_unused]] Windows::Graphics::Printing::PrintManager const& sender, Windows::Graphics::Printing::PrintTaskRequestedEventArgs args) {
	Windows::Graphics::Printing::PrintTask print_task = args.Request().CreatePrintTask(L"Print", Windows::Graphics::Printing::PrintTaskSourceRequestedHandler([=](Windows::Graphics::Printing::PrintTaskSourceRequestedArgs args) {
		args.SetSource(_print_document_source);
	}));

	print_task.Completed(Windows::Foundation::TypedEventHandler<Windows::Graphics::Printing::PrintTask, Windows::Graphics::Printing::PrintTaskCompletedEventArgs>(this, &MainPage::PrintTaskCompleted));
}

void winrt::BitwardenExportPrint::implementation::MainPage::PrintTaskCompleted([[maybe_unused]] Windows::Graphics::Printing::PrintTask const& sender, Windows::Graphics::Printing::PrintTaskCompletedEventArgs args) {
	if (args.Completion() == Windows::Graphics::Printing::PrintTaskCompletion::Failed) {
		Windows::UI::Xaml::Controls::ContentDialog no_printing;
		no_printing.Title(winrt::box_value(L"Printing error"));
		no_printing.Content(winrt::box_value(L"\nSorry, printing can' t proceed at this time."));
		no_printing.PrimaryButtonText(L"OK");

		no_printing.ShowAsync();
	}
}

void winrt::BitwardenExportPrint::implementation::MainPage::Paginate([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Printing::PaginateEventArgs args) {
	Windows::Graphics::Printing::PrintTaskOptions printing_options(args.PrintTaskOptions());
	Windows::Graphics::Printing::PrintPageDescription page_description = printing_options.GetPageDescription(0);

	_print_page.Width(page_description.PageSize.Width);
	_print_page.Height(page_description.PageSize.Height);

	Windows::UI::Xaml::Controls::Grid printable_area = _print_page.FindName(L"PrintableArea").as<Windows::UI::Xaml::Controls::Grid>();

	double margin_width = (std::max)(page_description.PageSize.Width - page_description.ImageableRect.Width, page_description.PageSize.Width * _application_content_margin_left * 2);
	double margin_height = (std::max)(page_description.PageSize.Height - page_description.ImageableRect.Height, page_description.PageSize.Height * _application_content_margin_top * 2);

	printable_area.Width(_print_page.Width() - margin_width);
	printable_area.Height(_print_page.Height() - margin_height);

	_print_document.SetPreviewPageCount(1, Windows::UI::Xaml::Printing::PreviewPageCountType::Final);
}

void winrt::BitwardenExportPrint::implementation::MainPage::GetPreviewPage([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Printing::GetPreviewPageEventArgs args) {
	_print_document.SetPreviewPage(args.PageNumber(), _print_page);
}

void winrt::BitwardenExportPrint::implementation::MainPage::AddPages([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Printing::AddPagesEventArgs args) {
	_print_document.AddPage(_print_page);

	_print_document.AddPagesComplete();
}

winrt::fire_and_forget winrt::BitwardenExportPrint::implementation::MainPage::pick_file() {
	try {
		Windows::Storage::Pickers::FileOpenPicker open_picker;

		open_picker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Downloads);
		open_picker.FileTypeFilter().Append(L".json");

		auto file = co_await open_picker.PickSingleFileAsync();

		auto filecontent = co_await Windows::Storage::FileIO::ReadTextAsync(file);

		nlohmann::json j = nlohmann::json::parse(winrt::to_string(filecontent));


		for (auto e : j["items"]) {
			_password_elements.Append(winrt::make<winrt::BitwardenExportPrint::implementation::PasswordElement>(winrt::to_hstring(e["name"].get<std::string>()), winrt::to_hstring(e["login"]["username"].get<std::string>()), winrt::to_hstring(e["login"]["password"].get<std::string>())));
		}
	}
	catch (...) {
		Windows::UI::Xaml::Controls::ContentDialog no_picking;

		no_picking.Title(winrt::box_value(L"File picking error"));
		no_picking.Content(winrt::box_value(L"\nSorry, picking and parsing the file can' t proceed at this time."));
		no_picking.PrimaryButtonText(L"OK");
		no_picking.ShowAsync();
	}
}

void winrt::BitwardenExportPrint::implementation::MainPage::Do_Stuff_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Windows::UI::Xaml::RoutedEventArgs const& e) {
	pick_file();
}
