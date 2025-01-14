#pragma once

#include "einsums/_Common.hpp"

#include <iterator>
#include <variant>

namespace einsums {

struct Empty {};

// struct Error {
//     EINSUMS_ALWAYS_INLINE      Error(Error &&)                = default;
//     EINSUMS_ALWAYS_INLINE auto operator=(Error &&) -> Error & = default;
// };

template <typename T, typename E>
struct [[nodiscard]] ErrorOr {
  private:
    template <typename U, typename F>
    friend class ErrorOr;

  public:
    using ResultType = T;
    using ErrorType  = E;

    ErrorOr() = default;

    ErrorOr(ErrorOr &&) noexcept                     = default;
    auto operator=(ErrorOr &&) noexcept -> ErrorOr & = default;

    ErrorOr(ErrorOr const &)                     = delete;
    auto operator=(ErrorOr const &) -> ErrorOr & = delete;

    template <typename U>
    ErrorOr(ErrorOr<U, ErrorType> &&value) : _value_or_error(std::move(value._value_or_error)) {}

    template <typename U>
    ErrorOr(U &&value) : _value_or_error(std::forward<U>(value)) {}

    auto value() -> ResultType & { return std::get<ResultType>(_value_or_error); }
    auto value() const -> ResultType const & { return std::get<ResultType>(_value_or_error); }

    auto error() -> ErrorType & { return std::get<ErrorType>(_value_or_error); }
    auto error() const -> ErrorType const & { return std::get<ErrorType>(_value_or_error); }

    [[nodiscard]] auto is_error() const -> bool {
        try {
            std::get<ErrorType>(_value_or_error);
            return true;
        } catch (const std::bad_variant_access &ex) {
            return false;
        }
    }

  private:
    std::variant<ResultType, ErrorType> _value_or_error;
};

template <typename ErrorType>
struct [[nodiscard]] ErrorOr<void, ErrorType> : public ErrorOr<Empty, ErrorType> {
    using ResultType = void;
    using ErrorOr<Empty, ErrorType>::ErrorOr;
};

} // namespace einsums

// using einsums::ErrorOr;