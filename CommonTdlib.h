#pragma once


#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>


namespace td_api = td::td_api;
using Object = td_api::object_ptr<td_api::Object>;
using ClientId = td::ClientManager::ClientId;

inline std::string fmm(const Object &object) {
    auto error_str = std::string("Unexpected obj id: ") + std::to_string(object->get_id());
    if (object->get_id() == td_api::error::ID) {
        error_str += " " + to_string(object);
    }
    return error_str;
}

namespace detail {
    template <class... Fs>
    struct overload;

    template <class F>
    struct overload<F> : public F {
        explicit overload(F f) : F(f) {
        }
    };
    template <class F, class... Fs>
    struct overload<F, Fs...>
            : public overload<F>
                    , public overload<Fs...> {
        overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {
        }
        using overload<F>::operator();
        using overload<Fs...>::operator();
    };
}  // namespace detail

template <class... F>
auto overloaded(F... f) {
    return detail::overload<F...>(f...);
}