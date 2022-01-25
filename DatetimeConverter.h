#pragma once
#include "DatetimeConverter.g.h"

namespace winrt::BitwardenExportPrint::implementation {
    struct DatetimeConverter : DatetimeConverterT<DatetimeConverter> {
        DatetimeConverter() = default;

        Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}
namespace winrt::BitwardenExportPrint::factory_implementation {
    struct DatetimeConverter : DatetimeConverterT<DatetimeConverter, implementation::DatetimeConverter> {};
}
