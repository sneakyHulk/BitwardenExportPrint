#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include <sstream>
#include <nlohmann/json.hpp>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::BitwardenExportPrint::implementation {
	MainPage::MainPage() {
		InitializeComponent();
	}

	winrt::Windows::Foundation::Collections::IObservableVector<BitwardenExportPrint::PasswordElement> MainPage::password_elements() {
		return _password_elements;
	}
}

void winrt::BitwardenExportPrint::implementation::MainPage::RegisterForPrinting() {
	_print_document = Windows::UI::Xaml::Printing::PrintDocument();
	_print_document_source = _print_document.DocumentSource();
	_print_document.Paginate(Windows::UI::Xaml::Printing::PaginateEventHandler(this, &MainPage::Paginate));
	_print_document.GetPreviewPage(Windows::UI::Xaml::Printing::GetPreviewPageEventHandler(this, &MainPage::GetPreviewPage));
	_print_document.AddPages(Windows::UI::Xaml::Printing::AddPagesEventHandler(this, &MainPage::AddPages));

	_print_manager = Windows::Graphics::Printing::PrintManager::GetForCurrentView();
	_print_task_requested_event_token = _print_manager.PrintTaskRequested(Windows::Foundation::TypedEventHandler<Windows::Graphics::Printing::PrintManager, Windows::Graphics::Printing::PrintTaskRequestedEventArgs>(this, &MainPage::PrintTaskRequested));
}

void winrt::BitwardenExportPrint::implementation::MainPage::UnregisterForPrinting() {
	_print_document_source = nullptr;
	_print_document = nullptr;

	_print_manager.PrintTaskRequested(_print_task_requested_event_token);
	_print_manager = nullptr;
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

	UnregisterForPrinting();
}

void winrt::BitwardenExportPrint::implementation::MainPage::Paginate([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Printing::PaginateEventArgs args) {
	PreprintFrame().Children().Clear();
	
	Windows::Graphics::Printing::PrintTaskOptions printing_options(args.PrintTaskOptions());
	Windows::Graphics::Printing::PrintPageDescription page_description = printing_options.GetPageDescription(0);

	double margin_width = (std::max)(page_description.PageSize.Width - page_description.ImageableRect.Width, page_description.PageSize.Width * _application_content_margin_left * 2);
	double margin_height = (std::max)(page_description.PageSize.Height - page_description.ImageableRect.Height, page_description.PageSize.Height * _application_content_margin_top * 2);

	double printable_area_width = page_description.PageSize.Width - margin_width;
	double printable_area_height = page_description.PageSize.Height - margin_height;

	uint32_t i = 0;

	auto page = winrt::make<winrt::BitwardenExportPrint::implementation::PasswordPrintPage>(*this, i);
	page.Width(page_description.PageSize.Width);
	Windows::UI::Xaml::Controls::Grid printable_area = page.FindName(L"PrintableArea").as<Windows::UI::Xaml::Controls::Grid>();
	printable_area.Width(printable_area_width);

	PreprintFrame().Children().Append(page);
	
	for (auto e : _password_elements) {
		page.password_elements().Append(e);

		PreprintFrame().InvalidateMeasure();
		PreprintFrame().UpdateLayout();

		double total_height = printable_area.ActualHeight();

		if (total_height > printable_area_height) {
			page.password_elements().RemoveAtEnd();

			page.Height(page_description.PageSize.Height);

			PreprintFrame().InvalidateMeasure();
			PreprintFrame().UpdateLayout();

			printable_area.Height(printable_area_height);

			PreprintFrame().InvalidateMeasure();
			PreprintFrame().UpdateLayout();

			if (!page.password_elements().Size()) {
				Windows::UI::Xaml::Controls::ContentDialog no_printing;

				no_printing.Title(winrt::box_value(L"Element too big to print"));
				no_printing.Content(winrt::box_value(L"\nSorry, printing could only proceed to that point."));
				no_printing.PrimaryButtonText(L"OK");

				no_printing.ShowAsync();

				_print_document.SetPreviewPageCount(i + 1, Windows::UI::Xaml::Printing::PreviewPageCountType::Final);

				return;
			}
			
			page = winrt::make<winrt::BitwardenExportPrint::implementation::PasswordPrintPage>(*this, ++i);
			page.Width(page_description.PageSize.Width);
			printable_area = page.FindName(L"PrintableArea").as<Windows::UI::Xaml::Controls::Grid>();
			printable_area.Width(printable_area_width);

			PreprintFrame().Children().Append(page);

			page.password_elements().Append(e);
		}
	}
	
	page.Height(page_description.PageSize.Height);

	PreprintFrame().InvalidateMeasure();
	PreprintFrame().UpdateLayout();

	printable_area.Height(printable_area_height);

	PreprintFrame().InvalidateMeasure();
	PreprintFrame().UpdateLayout();

	// Looks stupid, but is necessary. Printing under Windows is a bit buggy.
	for (auto e : PreprintFrame().Children()) {
		e.as<BitwardenExportPrint::PasswordPrintPage>().UpdateData(PreprintFrame().Children().Size());
	}

	PreprintFrame().InvalidateMeasure();
	PreprintFrame().UpdateLayout();

	_print_document.SetPreviewPageCount(i + 1, Windows::UI::Xaml::Printing::PreviewPageCountType::Final);
}

void winrt::BitwardenExportPrint::implementation::MainPage::GetPreviewPage([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Printing::GetPreviewPageEventArgs args) {
	_print_document.SetPreviewPage(args.PageNumber(), PreprintFrame().Children().GetAt(args.PageNumber() - 1));
}

void winrt::BitwardenExportPrint::implementation::MainPage::AddPages([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Printing::AddPagesEventArgs args) {
	for (auto e : PreprintFrame().Children()) {
		_print_document.AddPage(e);
	}

	_print_document.AddPagesComplete();
}

winrt::fire_and_forget winrt::BitwardenExportPrint::implementation::MainPage::pick_file() {
	Windows::Storage::StorageFile file = nullptr;

	try {
		Windows::Storage::Pickers::FileOpenPicker open_picker;

		open_picker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Downloads);
		open_picker.FileTypeFilter().Append(L".json");

		auto lifetime = get_strong();

		file = co_await open_picker.PickSingleFileAsync();
	}
	catch (...) {
		Windows::UI::Xaml::Controls::ContentDialog no_picking;

		no_picking.Title(winrt::box_value(L"File picking error"));
		no_picking.Content(winrt::box_value(L"\nSorry, picking a file can' t proceed at this time."));
		no_picking.PrimaryButtonText(L"OK");
		no_picking.ShowAsync();

		co_return;
	}

	if (file != nullptr);
	else {
		Windows::UI::Xaml::Controls::ContentDialog no_picking;

		no_picking.Title(winrt::box_value(L"Operation cancelled"));
		no_picking.Content(winrt::box_value(L"\nNo file was selected."));
		no_picking.PrimaryButtonText(L"OK");
		no_picking.ShowAsync();

		co_return;
	}

	winrt::hstring filecontent;

	try {
		filecontent = co_await Windows::Storage::FileIO::ReadTextAsync(file);
	}
	catch (...) {
		Windows::UI::Xaml::Controls::ContentDialog no_reading;

		no_reading.Title(winrt::box_value(L"File reading error"));
		no_reading.Content(winrt::box_value(L"\nSorry, reading the file can' t proceed at this time."));
		no_reading.PrimaryButtonText(L"OK");
		no_reading.ShowAsync();

		co_return;
	}

	nlohmann::json j;

	try {
		j = nlohmann::json::parse(winrt::to_string(filecontent));
	}
	catch (...) {
		Windows::UI::Xaml::Controls::ContentDialog no_parsing;

		no_parsing.Title(winrt::box_value(L"File parsing error"));
		no_parsing.Content(winrt::box_value(L"\nSorry, parsing the file can' t proceed at this time. (Wrong file format)"));
		no_parsing.PrimaryButtonText(L"OK");
		no_parsing.ShowAsync();

		co_return;
	}

	if (!j.contains("items")) {
		Windows::UI::Xaml::Controls::ContentDialog no_contents;

		no_contents.Title(winrt::box_value(L"File has no Bitwarden contents"));
		no_contents.Content(winrt::box_value(L"\nSorry, no contents were detected."));
		no_contents.PrimaryButtonText(L"OK");
		no_contents.ShowAsync();

		co_return;
	}

	_password_elements.Clear();

	for (auto e : j["items"]) {
		std::string description;
		std::string username;
		std::string password;

		try {
			description = e["name"].get<std::string>();
		}
		catch (...) {}

		try {
			username = e["login"]["username"].get<std::string>();
		}
		catch (...) {}

		try {
			password = e["login"]["password"].get<std::string>();
		}
		catch (...) {}

		if (description.empty() && username.empty() && password.empty()) continue;

		_password_elements.Append(winrt::make<winrt::BitwardenExportPrint::implementation::PasswordElement>(winrt::to_hstring(description), winrt::to_hstring(username), winrt::to_hstring(password)));
	}
}

void winrt::BitwardenExportPrint::implementation::MainPage::Print_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Windows::UI::Xaml::RoutedEventArgs const& args) {
	if (Windows::Graphics::Printing::PrintManager::IsSupported()) {
		RegisterForPrinting();

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

void winrt::BitwardenExportPrint::implementation::MainPage::Delete_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Windows::UI::Xaml::RoutedEventArgs const& e) {
	_password_elements.Clear();
}

void winrt::BitwardenExportPrint::implementation::MainPage::Load_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Windows::UI::Xaml::RoutedEventArgs const& e) {
	pick_file();
}