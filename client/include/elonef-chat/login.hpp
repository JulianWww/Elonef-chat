#pragma once

#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-chat/client.hpp>

namespace Chat {
    Elonef::PrivateClientKey login(Client& client);
}