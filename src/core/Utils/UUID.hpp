//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_UUID_HPP
#define SPARROW_UUID_HPP

#include <cstddef>
#include <uuid.h>
#include "MacroUtils.h"
#include <assert.h>

namespace SPW {
    // UUID
    class UUID {
    public:
        UUID() {
            id = randomID();
            assert(!id.is_nil());
        }

        UUID(bool nil)
        {
	        id = uuids::uuid();
            assert(id.is_nil());
        }

    	UUID(const char *str) {
            id = IDFromString(str);
            assert(!id.is_nil());
        }

    	UUID(const std::string& str)
        {
            id = uuids::uuid::from_string(str).value();
            assert(!id.is_nil());
        }

        UUID(const UUID &other) {
            id = other.id;
        }

        [[nodiscard]] std::string toString() const {
            return to_string(id);
        }

        bool empty() const
        {
            if(id.is_nil())
                return true;
			return false;
        }

        bool operator==(const UUID &other) const {
            return id == other.id;
        }

        // NULL id
        static UUID noneID() {
            UUID id;
            id.id = uuids::uuid();
            return id;
        }
        // generate a random id
        static UUID randomUUID() {
            return {};
        }
        // initialize an ID with uuid-string
        static UUID fromString(const char *str) {
            return {str};
        }

        // hash
        struct hash {
            std::size_t operator()(const UUID &uuid) const {
                return hash_str(to_string(uuid.id).c_str());
            }
        };
    private:
        static uuids::uuid IDFromString(const char *str) {
            return uuids::uuid::from_string(str).value();
        }
        static uuids::uuid randomID() {
            std::random_device rd;
            auto seed_data = std::array<int, std::mt19937::state_size> {};
            std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
            std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
            std::mt19937 generator(seq);
            uuids::uuid_random_generator gen{generator};
            return gen();
        }
        uuids::uuid id;
    };
}

#endif //SPARROW_UUID_HPP
