#pragma once
#include "PasswordElement.g.h"

namespace winrt::BitwardenExportPrint::implementation {
	struct PasswordElement : PasswordElementT<PasswordElement> {
		PasswordElement() = default;

		PasswordElement(hstring const& description, hstring const& username, hstring const& password);
		hstring description();
		hstring username();
		hstring password();

	private:
		hstring _description;
		hstring _username;
		hstring _password;
	};
}
namespace winrt::BitwardenExportPrint::factory_implementation {
	struct PasswordElement : PasswordElementT<PasswordElement, implementation::PasswordElement> {};
}
