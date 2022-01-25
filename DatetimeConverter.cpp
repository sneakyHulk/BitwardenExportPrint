#include "pch.h"
#include "DatetimeConverter.h"
#include "DatetimeConverter.g.cpp"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.System.UserProfile.h>

namespace winrt::BitwardenExportPrint::implementation {
	Windows::Foundation::IInspectable DatetimeConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language) {
        Windows::Globalization::DateTimeFormatting::DateTimeFormatter datetime_formatter(Windows::Globalization::DateTimeFormatting::YearFormat::Full,
            Windows::Globalization::DateTimeFormatting::MonthFormat::Numeric,
            Windows::Globalization::DateTimeFormatting::DayFormat::Default,
            Windows::Globalization::DateTimeFormatting::DayOfWeekFormat::None,
            Windows::Globalization::DateTimeFormatting::HourFormat::Default,
            Windows::Globalization::DateTimeFormatting::MinuteFormat::Default,
            Windows::Globalization::DateTimeFormatting::SecondFormat::Default,
            Windows::System::UserProfile::GlobalizationPreferences::Languages());
        return winrt::box_value(datetime_formatter.Format(winrt::unbox_value<Windows::Foundation::DateTime>(value)));
	}
	Windows::Foundation::IInspectable DatetimeConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language) {
		throw hresult_not_implemented();
	}

}
