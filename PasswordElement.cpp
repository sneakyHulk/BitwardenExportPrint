#include "pch.h"
#include "PasswordElement.h"
#include "PasswordElement.g.cpp"

namespace winrt::BitwardenExportPrint::implementation {
	PasswordElement::PasswordElement(hstring const& description, hstring const& username, hstring const& password) :
		_description(description), _username(username), _password(password) {}
	hstring PasswordElement::description() {
		return _description;
	}
	hstring PasswordElement::username() {
		return _username;
	}
	hstring PasswordElement::password() {
		return _password;
	}
}
