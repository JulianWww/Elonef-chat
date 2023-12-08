#pragma once

#include <string>
#include <cryptopp/queue.h>

namespace Chat {
    class ChatData {
        public: std::string name;
        public: std::string id;

        public: ChatData(const std::string& name, const std::string& id);
        public: ChatData();

        public: static ChatData load(CryptoPP::ByteQueue& queue);
        public: CryptoPP::ByteQueue toQueue() const;

        struct Compare {
            inline constexpr bool operator()( const ChatData& lhs, const ChatData& rhs ) const {
                if (lhs.id == rhs.id) {
                    return lhs.name < rhs.name;
                }
                return lhs.id < rhs.id;
            }
        };
    };
}
