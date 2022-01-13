#pragma once

#include "LinearAlgebra.hpp"
#include "Print.hpp"
#include "Tensor.hpp"
#include "range/v3/view/cartesian_product.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace EinsumsInCpp::TensorAlgebra {

namespace Index {

#define MAKE_INDEX(x)                                                                                                                      \
    struct x {                                                                                                                             \
        static constexpr char letter = static_cast<const char (&)[2]>(#x)[0];                                                              \
        constexpr x() = default;                                                                                                           \
    };                                                                                                                                     \
    static struct x x;                                                                                                                     \
    inline auto operator<<(std::ostream &os, const struct x &)->std::ostream & {                                                           \
        os << x::letter;                                                                                                                   \
        return os;                                                                                                                         \
    }

MAKE_INDEX(A); // NOLINT
MAKE_INDEX(a); // NOLINT
MAKE_INDEX(B); // NOLINT
MAKE_INDEX(b); // NOLINT
MAKE_INDEX(C); // NOLINT
MAKE_INDEX(c); // NOLINT
MAKE_INDEX(D); // NOLINT
MAKE_INDEX(d); // NOLINT
MAKE_INDEX(E); // NOLINT
MAKE_INDEX(e); // NOLINT
MAKE_INDEX(F); // NOLINT
MAKE_INDEX(f); // NOLINT

MAKE_INDEX(I); // NOLINT
MAKE_INDEX(i); // NOLINT
MAKE_INDEX(J); // NOLINT
MAKE_INDEX(j); // NOLINT
MAKE_INDEX(K); // NOLINT
MAKE_INDEX(k); // NOLINT
MAKE_INDEX(L); // NOLINT
MAKE_INDEX(l); // NOLINT
MAKE_INDEX(M); // NOLINT
MAKE_INDEX(m); // NOLINT
MAKE_INDEX(N); // NOLINT
MAKE_INDEX(n); // NOLINT

MAKE_INDEX(P); // NOLINT
MAKE_INDEX(p); // NOLINT
MAKE_INDEX(Q); // NOLINT
MAKE_INDEX(q); // NOLINT
MAKE_INDEX(R); // NOLINT
MAKE_INDEX(r); // NOLINT
MAKE_INDEX(S); // NOLINT
MAKE_INDEX(s); // NOLINT

#undef MAKE_INDEX
} // namespace Index

template <typename... Args>
struct Indices : public std::tuple<Args...> {
    Indices(Args... args) : std::tuple<Args...>(args...){};
};

namespace Detail {

template <template <size_t, typename> typename TensorType, size_t Rank, typename... Indices, typename... FilterIndices,
          std::size_t... FilterIndex, typename T = double>
auto determine_dim_ranges(const TensorType<Rank, T> &source, const std::tuple<Indices...> &sourceIndices,
                          const std::tuple<FilterIndices...> &filterIndices) {}
} // namespace Detail

template <template <size_t, typename> typename TensorType, size_t Rank, typename... Indices, typename... FilterIndices, typename T = double>
auto determine_dim_ranges(const TensorType<Rank, T> &source, const std::tuple<Indices...> &sourceIndices,
                          const std::tuple<FilterIndices...> &filterIndices) {
    return Detail::determine_dim_ranges(source, sourceIndices, filterIndices, std::make_index_sequence<sizeof...(FilterIndices)>{});
}

namespace Detail {

template <size_t Rank, typename... Args, std::size_t... I>
auto order_indices(const std::tuple<Args...> &combination, const std::array<size_t, Rank> &order, std::index_sequence<I...>) {
    return std::tuple{get_from_tuple<size_t>(combination, order[I])...};
}

} // namespace Detail

template <size_t Rank, typename... Args>
auto order_indices(const std::tuple<Args...> &combination, const std::array<size_t, Rank> &order) {
    return Detail::order_indices(combination, order, std::make_index_sequence<Rank>{});
}

#if 0
template <template <size_t, typename> typename AType, template <size_t, typename> typename BType, size_t Rank, typename T = double>
void sort(const std::string &operation, AType<Rank, T> *target, BType<Rank, T> &source) {
    // sort("prqs=pqrs", target, source);

    // Implement a slow version first:

    // 1. Determine mapping from source to target
    std::string::size_type equals = operation.find('=');
    std::string target_indices = operation.substr(0, equals);
    std::string source_indices = operation.substr(equals + 1);

    // 2. ensure length of the source and target match
    if (Rank != target_indices.size() || Rank != source_indices.size()) {
        throw std::runtime_error("target and/or source indices do not match expected rank");
    }

    // 3. find the position of source indices in target
    std::array<size_t, Rank> source_index_in_target;
    std::array<size_t, Rank> target_index_in_source;
    for (size_t i = 0; i < Rank; i++) {
        std::string::size_type location = target_indices.find(source_indices[i]);
        if (location != std::string::npos) {
            source_index_in_target[i] = location;
        } else {
            throw std::runtime_error("not all source indices were found in target indices");
        }

        // perform the reverse source to ensure all target indices are found in source
        location = source_indices.find(target_indices[i]);
        if (location != std::string::npos) {
            target_index_in_source[i] = location;
        } else {
            throw std::runtime_error("not all target indices were found in source indices");
        }
    }

    // Mapping obtained.
    // 4. Ensure that the dimensions match between the source and target
    for (size_t i = 0; i < Rank; i++) {
        if (target->dim(source_index_in_target[i]) != source.dim(i)) {
            throw std::runtime_error("target dimensions do not match source dimensions");
        }
    }

    // 5. Slow version: Go through all possible source index combinations and assign to targets location one-by-one.
    auto source_dims = get_dim_ranges<Rank>(source);
    for (auto combination : std::apply(ranges::views::cartesian_product, source_dims)) {
        T value = std::apply(source, combination);
        auto target_order = order_indices(combination, target_index_in_source);
        T &target_value = std::apply(*target, target_order);
        target_value = value;
    }
}
#endif

namespace Detail {

template <typename T, int Position>
constexpr auto _find_type_with_position() {
    return std::make_tuple();
}

template <typename T, int Position, typename Head, typename... Args>
constexpr auto _find_type_with_position() {
    if constexpr (std::is_same_v<std::decay_t<Head>, std::decay_t<T>>) {
        return std::tuple_cat(std::make_pair(std::decay_t<T>(), Position), _find_type_with_position<T, Position + 1, Args...>());
    } else {
        return _find_type_with_position<T, Position + 1, Args...>();
    }
}

// template <typename... Ts, typename... Args, std::size_t... Is>
// constexpr auto find_type_with_position(const std::tuple<Ts...> &, const std::tuple<Args...> &, std::index_sequence<Is...>) {
//     return std::tuple_cat(Detail::find_type_with_position<std::tuple_element_t<Is, std::tuple<Ts...>>, 0, Args...>()...);
// }

// template <typename... Ts, typename... Args, std::size_t... Is>
// constexpr auto find_type_with_position(std::index_sequence<Is...>) {
//     return std::tuple_cat(Detail::find_type_with_position<std::tuple_element_t<Is, std::tuple<Ts...>>, 0, Args...>()...);
// }

template <template <size_t, typename> typename TensorType, size_t Rank, typename... Args, std::size_t... I, typename T = double>
auto get_dim_ranges_for(const TensorType<Rank, T> &tensor, const std::tuple<Args...> &args, std::index_sequence<I...>) {
    return std::tuple{ranges::views::ints(0, (int)tensor.dim(std::get<2 * I + 1>(args)))...};
}

template <typename T, int Position>
constexpr auto find_position() {
    return -1;
}

template <typename T, int Position, typename Head, typename... Args>
constexpr auto find_position() {
    if constexpr (std::is_same_v<std::decay_t<Head>, std::decay_t<T>>) {
        // Found it
        return Position;
    } else {
        return find_position<T, Position + 1, Args...>();
    }
}

template <typename AIndex, typename... Args>
constexpr auto find_position() {
    return find_position<AIndex, 0, Args...>();
}

template <typename AIndex, typename... TargetCombination>
constexpr auto find_position(const std::tuple<TargetCombination...> &) {
    return Detail::find_position<AIndex, TargetCombination...>();
}

// template <typename... Ts, typename... Args>
// constexpr auto find_type_with_position(const std::tuple<Ts...> &T, const std::tuple<Args...> &A) {
//     return Detail::find_type_with_position(T, A, std::make_index_sequence<sizeof...(Ts)>());
// }

template <typename S1, typename... S2, std::size_t... Is>
constexpr auto _find_type_with_position(std::index_sequence<Is...>) {
    return std::tuple_cat(Detail::_find_type_with_position<std::tuple_element_t<Is, S1>, 0, S2...>()...);
}

template <typename... Ts, typename... Us>
constexpr auto find_type_with_position(const std::tuple<Ts...> &, const std::tuple<Us...> &) {
    return _find_type_with_position<std::tuple<Ts...>, Us...>(std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts, typename... Args>
[[deprecated]] constexpr auto find_type_with_position() {
    return Detail::find_type_with_position<std::tuple<Ts...>, std::tuple<Args...>>(std::make_index_sequence<sizeof...(Ts)>());
}

template <template <size_t, typename> typename TensorType, size_t Rank, typename... Args, typename T = double>
auto get_dim_ranges_for(const TensorType<Rank, T> &tensor, const std::tuple<Args...> &args) {
    return Detail::get_dim_ranges_for(tensor, args, std::make_index_sequence<sizeof...(Args) / 2>{});
}

template <typename AIndex, typename... TargetCombination, typename... TargetPositionInC, typename... LinkCombination,
          typename... LinkPositionInLink>
auto construct_index(const std::tuple<TargetCombination...> &target_combination, const std::tuple<TargetPositionInC...> &,
                     const std::tuple<LinkCombination...> &link_combination, const std::tuple<LinkPositionInLink...> &) {

    constexpr auto IsAIndexInC = Detail::find_position<AIndex, TargetPositionInC...>();
    constexpr auto IsAIndexInLink = Detail::find_position<AIndex, LinkPositionInLink...>();

    static_assert(IsAIndexInC != -1 || IsAIndexInLink != -1, "Looks like the indices in your einsum are not quite right! :(");

    if constexpr (IsAIndexInC != -1) {
        return std::get<IsAIndexInC / 2>(target_combination);
    } else if constexpr (IsAIndexInLink != -1) {
        return std::get<IsAIndexInLink / 2>(link_combination);
    } else {
        return -1;
    }
}

template <typename... AIndices, typename... TargetCombination, typename... TargetPositionInC, typename... LinkCombination,
          typename... LinkPositionInLink>
constexpr auto
construct_indices(const std::tuple<TargetCombination...> &target_combination, const std::tuple<TargetPositionInC...> &target_position_in_C,
                  const std::tuple<LinkCombination...> &link_combination, const std::tuple<LinkPositionInLink...> &link_position_in_link) {
    return std::make_tuple(construct_index<AIndices>(target_combination, target_position_in_C, link_combination, link_position_in_link)...);
}

// template <typename... PositionsInX, std::size_t... I>
// constexpr auto _contiguous_positions(const std::tuple<PositionsInX...> &positions_in_x, std::index_sequence<I...>) -> bool {
//     return ((std::get<(2 * I) + 1>(positions_in_x) + 1 == std::get<(2 * (I + 1)) + 1>(positions_in_x)) && ...);
// }

template <typename... PositionsInX, std::size_t... I>
constexpr auto _contiguous_positions(const std::tuple<PositionsInX...> &x, std::index_sequence<I...>) -> bool {
    return ((std::get<2 * I + 1>(x) == std::get<2 * I + 3>(x) - 1) && ... && true);
}

template <typename... PositionsInX>
constexpr auto contiguous_positions(const std::tuple<PositionsInX...> &x) -> bool {
    if constexpr (sizeof...(PositionsInX) <= 2) {
        return true;
    } else {
        return _contiguous_positions(x, std::make_index_sequence<sizeof...(PositionsInX) / 2 - 1>{});
    }
}

template <typename... PositionsInX, typename... PositionsInY, std::size_t... I>
constexpr auto _is_same_ordering(const std::tuple<PositionsInX...> &positions_in_x, const std::tuple<PositionsInY...> &positions_in_y,
                                 std::index_sequence<I...>) {
    return (std::is_same_v<decltype(std::get<2 * I>(positions_in_x)), decltype(std::get<2 * I>(positions_in_y))> && ...);
}

template <typename... PositionsInX, typename... PositionsInY>
constexpr auto is_same_ordering(const std::tuple<PositionsInX...> &positions_in_x, const std::tuple<PositionsInY...> &positions_in_y) {
    // static_assert(sizeof...(PositionsInX) == sizeof...(PositionsInY) && sizeof...(PositionsInX) > 0);
    if constexpr (sizeof...(PositionsInX) == 0 || sizeof...(PositionsInY) == 0)
        return false; // NOLINT
    else if constexpr (sizeof...(PositionsInX) != sizeof...(PositionsInY))
        return false;
    else
        return _is_same_ordering(positions_in_x, positions_in_y, std::make_index_sequence<sizeof...(PositionsInX) / 2>{});
}

template <template <size_t, typename> typename XType, size_t XRank, typename... PositionsInX, std::size_t... I, typename T = double>
constexpr auto product_dims(const std::tuple<PositionsInX...> &indices, const XType<XRank, T> &X, std::index_sequence<I...>) -> size_t {
    return (X.dim(std::get<2 * I + 1>(indices)) * ... * 1);
}

template <template <size_t, typename> typename XType, size_t XRank, typename... PositionsInX, std::size_t... I, typename T = double>
constexpr auto product_strides(const std::tuple<PositionsInX...> &indices, const XType<XRank, T> &X, std::index_sequence<I...>) -> size_t {
    return (X.stride(std::get<2 * I + 1>(indices)) * ... * 1);
}

template <typename LHS, typename RHS, std::size_t... I>
constexpr auto same_indices(std::index_sequence<I...>) {
    return (std::is_same_v<std::tuple_element_t<I, LHS>, std::tuple_element_t<I, RHS>> && ...);
}

template <template <size_t, typename> typename XType, size_t XRank, typename... PositionsInX, typename T = double>
constexpr auto product_dims(const std::tuple<PositionsInX...> &indices, const XType<XRank, T> &X) -> size_t {
    return Detail::product_dims(indices, X, std::make_index_sequence<sizeof...(PositionsInX) / 2>());
}

template <template <size_t, typename> typename XType, size_t XRank, typename... PositionsInX, typename T = double>
constexpr auto product_strides(const std::tuple<PositionsInX...> &indices, const XType<XRank, T> &X) -> size_t {
    return Detail::product_strides(indices, X, std::make_index_sequence<sizeof...(PositionsInX) / 2>());
}

template <typename LHS, typename RHS>
constexpr auto same_indices() {
    if constexpr (std::tuple_size_v<LHS> != std::tuple_size_v<RHS>)
        return false;
    else
        return Detail::same_indices<LHS, RHS>(std::make_index_sequence<std::tuple_size_v<LHS>>());
}

template <typename T, typename... CIndices, typename... AIndices, typename... BIndices, typename... TargetDims, typename... LinkDims,
          typename... TargetPositionInC, typename... LinkPositionInLink, template <size_t, typename> typename CType, size_t CRank,
          template <size_t, typename> typename AType, size_t ARank, template <size_t, typename> typename BType, size_t BRank>
void einsum_generic_algorithm(const std::tuple<CIndices...> & /*C_indices*/, const std::tuple<AIndices...> & /*A_indices*/,
                              const std::tuple<BIndices...> & /*B_indices*/, const std::tuple<TargetDims...> &target_dims,
                              const std::tuple<LinkDims...> &link_dims, const std::tuple<TargetPositionInC...> &target_position_in_C,
                              const std::tuple<LinkPositionInLink...> &link_position_in_link, const T C_prefactor, CType<CRank, T> *C,
                              const T AB_prefactor, const AType<ARank, T> &A, const BType<BRank, T> &B) {
    Timer::push("generic algorithm");
    // println("A index: {}", print_tuple_no_type(A_indices));
    // println("B index: {}", print_tuple_no_type(B_indices));
    // println("target_position_in_C: {}", print_tuple_no_type(target_position_in_C));
    // println("link_position_in_link: {}", print_tuple_no_type(link_position_in_link));
    // Print::Indent _outer;

    if constexpr (sizeof...(LinkDims) != 0) {
        for (auto target_combination : std::apply(ranges::views::cartesian_product, target_dims)) {
            // println("target_combination: {}", print_tuple_no_type(target_combination));
            auto C_order =
                Detail::construct_indices<CIndices...>(target_combination, target_position_in_C, std::tuple<>(), target_position_in_C);
            // println("C_order: {}", print_tuple_no_type(C_order));

            // This is the generic case.
            T sum{0};
            for (auto link_combination : std::apply(ranges::views::cartesian_product, link_dims)) {
                // Print::Indent _indent;

                // Construct the tuples that will be used to access the tensor elements of A and B
                auto A_order = Detail::construct_indices<AIndices...>(target_combination, target_position_in_C, link_combination,
                                                                      link_position_in_link);
                auto B_order = Detail::construct_indices<BIndices...>(target_combination, target_position_in_C, link_combination,
                                                                      link_position_in_link);

                // println("link_combination: {}", print_tuple_no_type(link_combination));
                // println("A_order: {}", print_tuple_no_type(A_order));
                // println("B_order: {}", print_tuple_no_type(B_order));

                // Get the tensor element using the operator()(MultiIndex...) function of Tensor.
                T A_value = std::apply(A, A_order);
                T B_value = std::apply(B, B_order);

                sum += AB_prefactor * A_value * B_value;

                // println("A: %lf B: %lf RunningSum: %lf", A_value, B_value, sum);
            }

            T &target_value = std::apply(*C, C_order);
            target_value *= C_prefactor;
            target_value += sum;

            // println("C: %lf", sum);
        }
    } else {
        // println("target_dims: {}", print_tuple_no_type(target_dims));
        for (auto target_combination : std::apply(ranges::views::cartesian_product, target_dims)) {
            // println("target_combination: {}", print_tuple_no_type(target_combination));

            // This is the generic case.
            T sum{0};
            // Print::Indent _indent;

            // Construct the tuples that will be used to access the tensor elements of A and B
            auto A_order =
                Detail::construct_indices<AIndices...>(target_combination, target_position_in_C, std::tuple<>(), target_position_in_C);
            auto B_order =
                Detail::construct_indices<BIndices...>(target_combination, target_position_in_C, std::tuple<>(), target_position_in_C);
            auto C_order =
                Detail::construct_indices<CIndices...>(target_combination, target_position_in_C, std::tuple<>(), target_position_in_C);

            // println("link_combination: {}", print_tuple_no_type(link_combination));
            // println("A_order: {}", print_tuple_no_type(A_order));
            // println("B_order: {}", print_tuple_no_type(B_order));

            // Get the tensor element using the operator()(MultiIndex...) function of Tensor.
            T A_value = std::apply(A, A_order);
            T B_value = std::apply(B, B_order);

            sum += AB_prefactor * A_value * B_value;

            // println("A: %lf B: %lf RunningSum: %lf", A_value, B_value, sum);

            T &target_value = std::apply(*C, C_order);
            target_value *= C_prefactor;
            target_value += sum;

            // println("C: %lf", sum);
        }
    }
    Timer::pop();
}

template <bool OnlyUseGenericAlgorithm, template <size_t, typename> typename AType, size_t ARank,
          template <size_t, typename> typename BType, size_t BRank, template <size_t, typename> typename CType, size_t CRank,
          typename... CIndices, typename... AIndices, typename... BIndices, typename T = double>
auto einsum(const T C_prefactor, const std::tuple<CIndices...> & /*Cs*/, CType<CRank, T> *C, const T AB_prefactor,
            const std::tuple<AIndices...> & /*As*/, const AType<ARank, T> &A, const std::tuple<BIndices...> & /*Bs*/,
            const BType<BRank, T> &B)
    -> std::enable_if_t<std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<ARank, T>, AType<ARank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<BRank, T>, BType<BRank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<CRank, T>, CType<CRank, T>>> {
    Print::Indent _indent;

    constexpr auto A_indices = std::tuple<AIndices...>();
    constexpr auto B_indices = std::tuple<BIndices...>();
    constexpr auto C_indices = std::tuple<CIndices...>();

    // 1. Ensure the ranks are correct. (Compile-time check.)
    static_assert(sizeof...(CIndices) == CRank, "Rank of C does not match Indices given for C.");
    static_assert(sizeof...(AIndices) == ARank, "Rank of A does not match Indices given for A.");
    static_assert(sizeof...(BIndices) == BRank, "Rank of B does not match Indices given for B.");

    // 2. Determine the links from AIndices and BIndices
    constexpr auto links0 = intersect_t<std::tuple<AIndices...>, std::tuple<BIndices...>>();
    // 2a. Remove any links that appear in the target
    constexpr auto links = difference_t<decltype(links0), std::tuple<CIndices...>>();

    // 3. Determine the links between CIndices and AIndices
    constexpr auto CAlinks = intersect_t<std::tuple<CIndices...>, std::tuple<AIndices...>>();

    // 4. Determine the links between CIndices and BIndices
    constexpr auto CBlinks = intersect_t<std::tuple<CIndices...>, std::tuple<BIndices...>>();

    // 3. Check the rank of C against A, B, and links.
    // static_assert(sizeof...(AIndices) + sizeof...(BIndices) - 2 * std::tuple_size_v<decltype(links)> == sizeof...(CIndices),
    //   "Rank of C does not matched deduced constraction size from A and B.");

    // Determine unique indices in A
    constexpr auto A_only = difference_t<std::tuple<AIndices...>, decltype(links)>();
    constexpr auto B_only = difference_t<std::tuple<BIndices...>, decltype(links)>();

    constexpr auto A_unique = unique_t<std::tuple<AIndices...>>();
    constexpr auto B_unique = unique_t<std::tuple<BIndices...>>();
    constexpr auto C_unique = unique_t<std::tuple<CIndices...>>();
    constexpr auto link_unique = c_unique_t<decltype(links)>();

    constexpr bool A_hadamard_found = std::tuple_size_v<std::tuple<AIndices...>> != std::tuple_size_v<decltype(A_unique)>;
    constexpr bool B_hadamard_found = std::tuple_size_v<std::tuple<BIndices...>> != std::tuple_size_v<decltype(B_unique)>;
    constexpr bool C_hadamard_found = std::tuple_size_v<std::tuple<CIndices...>> != std::tuple_size_v<decltype(C_unique)>;

    // println("Hadamard found: C {}, A {}, B {}", C_hadamard_found, A_hadamard_found, B_hadamard_found);
    // println("C_unique {}", print_tuple_no_type(C_unique));
    // println("A_unique {}", print_tuple_no_type(A_unique));
    // println("B_unique {}", print_tuple_no_type(B_unique));
    // println("link_unique {}", print_tuple_no_type(link_unique));

    constexpr auto link_position_in_A = Detail::find_type_with_position(links, A_indices);
    constexpr auto link_position_in_B = Detail::find_type_with_position(links, B_indices);
    constexpr auto link_position_in_link = Detail::find_type_with_position(links, links);

    constexpr auto target_position_in_A = Detail::find_type_with_position(C_indices, A_indices);
    constexpr auto target_position_in_B = Detail::find_type_with_position(C_indices, B_indices);
    constexpr auto target_position_in_C = Detail::find_type_with_position(C_indices, C_indices);

    constexpr auto A_target_position_in_C = Detail::find_type_with_position(A_indices, C_indices);
    constexpr auto B_target_position_in_C = Detail::find_type_with_position(B_indices, C_indices);

    auto target_dims = Detail::get_dim_ranges_for(*C, Detail::find_type_with_position(C_unique, C_indices));
    auto link_dims = Detail::get_dim_ranges_for(A, link_position_in_A);

    constexpr auto contiguous_link_position_in_A = Detail::contiguous_positions(link_position_in_A);
    constexpr auto contiguous_link_position_in_B = Detail::contiguous_positions(link_position_in_B);

    constexpr auto contiguous_target_position_in_A = Detail::contiguous_positions(target_position_in_A);
    constexpr auto contiguous_target_position_in_B = Detail::contiguous_positions(target_position_in_B);

    constexpr auto contiguous_A_targets_in_C = Detail::contiguous_positions(A_target_position_in_C);
    constexpr auto contiguous_B_targets_in_C = Detail::contiguous_positions(B_target_position_in_C);

    constexpr auto same_ordering_link_position_in_AB = Detail::is_same_ordering(link_position_in_A, link_position_in_B);
    constexpr auto same_ordering_target_position_in_CA = Detail::is_same_ordering(target_position_in_A, A_target_position_in_C);
    constexpr auto same_ordering_target_position_in_CB = Detail::is_same_ordering(target_position_in_B, B_target_position_in_C);

    constexpr auto C_exactly_matches_A =
        sizeof...(CIndices) == sizeof...(AIndices) && same_indices<std::tuple<CIndices...>, std::tuple<AIndices...>>();
    constexpr auto C_exactly_matches_B =
        sizeof...(CIndices) == sizeof...(BIndices) && same_indices<std::tuple<CIndices...>, std::tuple<BIndices...>>();
    constexpr auto A_exactly_matches_B = same_indices<std::tuple<AIndices...>, std::tuple<BIndices...>>();

    // println("---------------------------------------------------------------------------------------------------------------------------");
    // println("C: {}", print_tuple_no_type(C_indices));
    // println("A: {}", print_tuple_no_type(A_indices));
    // println("B: {}", print_tuple_no_type(B_indices));
    // println("Link: {}", print_tuple_no_type(links));
    // println("links contiguous in A          : {}", contiguous_link_position_in_A);
    // println("links contiguous in B          : {}", contiguous_link_position_in_B);

    // println("target contiguous in A         : {}", contiguous_target_position_in_A);
    // println("target position in A           : {}", print_tuple_no_type(target_position_in_A));
    // println("target contiguous in B         : {}", contiguous_target_position_in_B);

    // println("A contiguous in C              : {}", contiguous_A_targets_in_C);
    // println("A target position in C         : {}", print_tuple_no_type(A_target_position_in_C));
    // println("B contiguous in C              : {}", contiguous_B_targets_in_C);
    // println("B target position in C         : {}", print_tuple_no_type(B_target_position_in_C));

    // println("links in same order in A & B   : {}", same_ordering_link_position_in_AB);
    // println("targets in same order in C & A : {}", same_ordering_target_position_in_CA);
    // println("targets in same order in C & B : {}", same_ordering_target_position_in_CB);

    constexpr auto is_gemm_possible = contiguous_link_position_in_A && contiguous_link_position_in_B && contiguous_target_position_in_A &&
                                      contiguous_target_position_in_B && contiguous_A_targets_in_C && contiguous_B_targets_in_C &&
                                      same_ordering_link_position_in_AB && same_ordering_target_position_in_CA &&
                                      same_ordering_target_position_in_CB && !A_hadamard_found && !B_hadamard_found && !C_hadamard_found;
    constexpr auto is_gemv_possible = contiguous_link_position_in_A && contiguous_link_position_in_B && contiguous_target_position_in_A &&
                                      same_ordering_link_position_in_AB && same_ordering_target_position_in_CA &&
                                      !same_ordering_target_position_in_CB && std::tuple_size_v<decltype(B_target_position_in_C)> == 0 &&
                                      !A_hadamard_found && !B_hadamard_found && !C_hadamard_found;

    constexpr auto element_wise_multiplication =
        C_exactly_matches_A && C_exactly_matches_B && !A_hadamard_found && !B_hadamard_found && !C_hadamard_found;
    constexpr auto dot_product =
        sizeof...(CIndices) == 0 && A_exactly_matches_B && !A_hadamard_found && !B_hadamard_found && !C_hadamard_found;

    if constexpr (dot_product) {
        T temp = LinearAlgebra::dot(A, B);
        (*C) *= C_prefactor;
        (*C) += AB_prefactor * temp;

        return;
    } else if constexpr (element_wise_multiplication) {
        Dim<1> common;
        common[0] = 1;
        for (auto el : C->dims()) {
            common[0] *= el;
        }

        TensorView<1, T> tC{*C, common};
        const TensorView<1, T> tA{const_cast<AType<ARank, T> &>(A), common}, tB{const_cast<BType<BRank, T> &>(B), common};

        for (size_t i = 0; i < common[0]; i++) {
            T temp = AB_prefactor * tA(i) * tB(i);
            T &target_value = tC(i);
            target_value *= C_prefactor;
            target_value += temp;
        }

        // return;
    } else if constexpr (!OnlyUseGenericAlgorithm) {
        // To use a gemm the input tensors need to be at least rank 2
        if constexpr (CRank >= 2 && ARank >= 2 && BRank >= 2) {
            if constexpr (!A_hadamard_found && !B_hadamard_found && !C_hadamard_found) {
                if constexpr (is_gemv_possible) {
                    // println("GEMV appears possible for this case but is not coded.");

                    constexpr bool transpose_A = std::get<1>(link_position_in_A) == 0;

                    // println("transpose A? {}", transpose_A);
                    // println("link_position_in_A: {}", print_tuple_no_type(link_position_in_A));

                    Dim<2> dA;
                    Dim<1> dB, dC;

                    dA[0] = product_dims(A_target_position_in_C, *C);
                    dA[1] = product_dims(link_position_in_A, A);

                    dB[0] = product_dims(link_position_in_A, A);
                    dC[0] = product_dims(A_target_position_in_C, *C);

                    const TensorView<2, T> tA{const_cast<AType<ARank, T> &>(A), dA};
                    const TensorView<1, T> tB{const_cast<BType<BRank, T> &>(B), dB};
                    TensorView<1, T> tC{*C, dC};

                    if constexpr (transpose_A) {
                        LinearAlgebra::gemv<true>(AB_prefactor, tA, tB, C_prefactor, &tC);
                    } else {
                        LinearAlgebra::gemv<false>(AB_prefactor, tA, tB, C_prefactor, &tC);
                    }

                    return;
                } else if constexpr (is_gemm_possible) {
                    // if (false) {
                    // println("GEMM appears possible for this case.");

                    constexpr bool transpose_A = std::get<1>(link_position_in_A) == 0;
                    constexpr bool transpose_B = std::get<1>(link_position_in_B) != 0;
                    constexpr bool transpose_C = std::get<1>(A_target_position_in_C) != 0;

                    // println("transpose A? {}", transpose_A);
                    // println("link_position_in_A: {}", print_tuple_no_type(link_position_in_A));
                    // println("transpose B? {}", transpose_B);
                    // println("link_position_in_B: {}", print_tuple_no_type(link_position_in_B));
                    // println("transpose C? {}", transpose_C);
                    // println("A_target_position_in_C: {}", print_tuple_no_type(A_target_position_in_C));
                    // println("B_target_position_in_C: {}", print_tuple_no_type(B_target_position_in_C));

                    Dim<2> dA, dB, dC;

                    dA[0] = product_dims(A_target_position_in_C, *C);
                    dA[1] = product_dims(link_position_in_A, A);
                    if (transpose_A)
                        std::swap(dA[0], dA[1]);

                    dB[0] = product_dims(link_position_in_B, B);
                    dB[1] = product_dims(B_target_position_in_C, *C);
                    if (transpose_B)
                        std::swap(dB[0], dB[1]);

                    dC[0] = product_dims(A_target_position_in_C, *C);
                    dC[1] = product_dims(B_target_position_in_C, *C);
                    if (transpose_C)
                        std::swap(dC[0], dC[1]);

                    // println("Dims of C: {} {}", dC[0], dC[1]);
                    // println("Dims of A: {} {}", dA[0], dA[1]);
                    // println("Dims of B: {} {}", dB[0], dB[1]);

                    TensorView<2, T> tC{*C, dC};
                    const TensorView<2, T> tA{const_cast<AType<ARank, T> &>(A), dA}, tB{const_cast<BType<BRank, T> &>(B), dB};

                    if constexpr (!transpose_C && !transpose_A && !transpose_B) {
                        // println("Calling gemm<false, false>");
                        LinearAlgebra::gemm<false, false>(AB_prefactor, tA, tB, C_prefactor, &tC);
                        return;
                    } else if constexpr (!transpose_C && !transpose_A && transpose_B) {
                        // println("Calling gemm<false, true>");
                        LinearAlgebra::gemm<false, true>(AB_prefactor, tA, tB, C_prefactor, &tC);
                        return;
                    } else if constexpr (!transpose_C && transpose_A && !transpose_B) {
                        // println("Calling gemm<true, false>");
                        LinearAlgebra::gemm<true, false>(AB_prefactor, tA, tB, C_prefactor, &tC);
                        return;
                    } else if constexpr (!transpose_C && transpose_A && transpose_B) {
                        // println("Calling gemm<true, true>");
                        LinearAlgebra::gemm<true, true>(AB_prefactor, tA, tB, C_prefactor, &tC);
                        return;
                    } else if constexpr (transpose_C && !transpose_A && !transpose_B) {
                        // println("Calling gemm<true, true> === true, false, false");
                        LinearAlgebra::gemm<true, true>(AB_prefactor, tB, tA, C_prefactor, &tC);
                        return;
                    } else if constexpr (transpose_C && !transpose_A && transpose_B) {
                        // println("Calling C^T gemm<false, true>");
                        // println(tA);
                        // println(tB);
                        // println(tC);
                        LinearAlgebra::gemm<false, true>(AB_prefactor, tB, tA, C_prefactor, &tC);
                        return;
                    } else if constexpr (transpose_C && transpose_A && !transpose_B) {
                        // println("Calling C^T gemm<true, false>");
                        LinearAlgebra::gemm<true, false>(AB_prefactor, tB, tA, C_prefactor, &tC);
                        return;
                    } else if constexpr (transpose_C && transpose_A && transpose_B) {
                        // println("Calling C^T gemm<false, false> === true, true, true");
                        LinearAlgebra::gemm<false, false>(AB_prefactor, tB, tA, C_prefactor, &tC);
                        return;
                    } else {
                        println("This GEMM case is not programmed: transpose_C {}, transpose_A {}, transpose_B {}", transpose_C,
                                transpose_A, transpose_B);
                        std::abort();
                    }
                }
            }
        }
    }

    if constexpr (!dot_product && !element_wise_multiplication) {
        einsum_generic_algorithm<T>(C_indices, A_indices, B_indices, target_dims, link_dims, target_position_in_C, link_position_in_link,
                                    C_prefactor, C, AB_prefactor, A, B);
    }
}
} // namespace Detail

template <template <size_t, typename> typename AType, size_t ARank, template <size_t, typename> typename BType, size_t BRank,
          template <size_t, typename> typename CType, size_t CRank, typename... CIndices, typename... AIndices, typename... BIndices,
          typename T = double>
auto einsum(const T C_prefactor, const std::tuple<CIndices...> &C_indices, CType<CRank, T> *C, const T AB_prefactor,
            const std::tuple<AIndices...> &A_indices, const AType<ARank, T> &A, const std::tuple<BIndices...> &B_indices,
            const BType<BRank, T> &B)
    -> std::enable_if_t<std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<ARank, T>, AType<ARank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<BRank, T>, BType<BRank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<CRank, T>, CType<CRank, T>>> {
    Timer::push(fmt::format("einsum: C({}) = {} A({}) * B({}) + {} C({})", print_tuple_no_type(C_indices), AB_prefactor,
                            print_tuple_no_type(A_indices), print_tuple_no_type(B_indices), C_prefactor, print_tuple_no_type(C_indices)));

#ifdef CONTINUOUSLY_TEST_EINSUM
    // Clone C into a new tensor
    Tensor<CRank, T> testC{C->dims()};
    testC = *C;

    // Perform the einsum using only the generic algorithm
    Timer::push("testing");
    Detail::einsum<true>(C_prefactor, C_indices, &testC, AB_prefactor, A_indices, A, B_indices, B);
    Timer::pop();
#endif

    // Perform the actual einsum
    Detail::einsum<false>(C_prefactor, C_indices, C, AB_prefactor, A_indices, A, B_indices, B);

#ifdef CONTINUOUSLY_TEST_EINSUM
    if constexpr (CRank != 0) {
        // Need to walk through the entire C and testC comparing values and reporting differences.
        auto target_dims = get_dim_ranges<CRank>(*C);

        for (auto target_combination : std::apply(ranges::views::cartesian_product, target_dims)) {
            const double Cvalue = std::apply(*C, target_combination);
            const double Ctest = std::apply(testC, target_combination);

            if (std::fabs(Cvalue - Ctest) > 1.0E-6) {
                println(emphasis::bold | bg(fmt::color::red) | fg(fmt::color::white), "    !!! EINSUM ERROR !!!");
                println(bg(fmt::color::red) | fg(fmt::color::white), "    Expected {:20.14f}", Ctest);
                println(bg(fmt::color::red) | fg(fmt::color::white), "    Obtained {:20.14f}", Cvalue);

                println(bg(fmt::color::red) | fg(fmt::color::white), "    tensor element ({:})", print_tuple_no_type(target_combination));
                println(bg(fmt::color::red) | fg(fmt::color::white), "    {:f} C({:}) += {:f} A({:}) * B({:})", C_prefactor,
                        print_tuple_no_type(C_indices), AB_prefactor, print_tuple_no_type(A_indices), print_tuple_no_type(B_indices));

#ifdef CONTINUOUSLY_TEST_EINSUM_ABORT
                std::abort();
#endif
            }
        }
    } else {
        const double Cvalue = *C;
        const double Ctest = testC;

        if (std::fabs(Cvalue - testC) > 1.0E-6) {
            println(emphasis::bold | bg(fmt::color::red) | fg(fmt::color::white), "!!! EINSUM ERROR !!!");
            println(bg(fmt::color::red) | fg(fmt::color::white), "    Expected {:20.14f}", Ctest);
            println(bg(fmt::color::red) | fg(fmt::color::white), "    Obtained {:20.14f}", Cvalue);

            println(bg(fmt::color::red) | fg(fmt::color::white), "    tensor element ()");
            println(bg(fmt::color::red) | fg(fmt::color::white), "    {:f} C() += {:f} A({:}) * B({:})", C_prefactor, AB_prefactor,
                    print_tuple_no_type(A_indices), print_tuple_no_type(B_indices));

#ifdef CONTINUOUSLY_TEST_EINSUM_ABORT
            std::abort();
#endif
        }
    }
#endif
    Timer::pop();
}

template <template <size_t, typename> typename AType, size_t ARank, template <size_t, typename> typename BType, size_t BRank,
          template <size_t, typename> typename CType, size_t CRank, typename... CIndices, typename... AIndices, typename... BIndices,
          typename T = double>
auto einsum(const std::tuple<CIndices...> &C_indices, CType<CRank, T> *C, const std::tuple<AIndices...> &A_indices,
            const AType<ARank, T> &A, const std::tuple<BIndices...> &B_indices, const BType<BRank, T> &B)
    -> std::enable_if_t<std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<ARank, T>, AType<ARank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<BRank, T>, BType<BRank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<CRank, T>, CType<CRank, T>>> {
    einsum(T{0}, C_indices, C, T{1}, A_indices, A, B_indices, B);
}

template <template <size_t, typename> typename AType, size_t ARank, template <size_t, typename> typename CType, size_t CRank,
          typename... CIndices, typename... AIndices, typename T = double>
auto sort(const T C_prefactor, const std::tuple<CIndices...> &C_indices, CType<CRank, T> *C, const T A_prefactor,
          const std::tuple<AIndices...> &A_indices, const AType<ARank, T> &A)
    -> std::enable_if_t<std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<CRank, T>, CType<CRank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<ARank, T>, AType<ARank, T>> &&
                        sizeof...(CIndices) == sizeof...(AIndices) && sizeof...(CIndices) == CRank && sizeof...(AIndices) == ARank> {

    Timer::push(fmt::format("sort: C({}) = {} A({}) + {} C({})", print_tuple_no_type(C_indices), A_prefactor,
                            print_tuple_no_type(A_indices), C_prefactor, print_tuple_no_type(C_indices)));

    // Error check:  If there are any remaining indices then we cannot perform a sort
    constexpr auto check = difference_t<std::tuple<AIndices...>, std::tuple<CIndices...>>();
    static_assert(std::tuple_size_v<decltype(check)> == 0);

    auto target_position_in_A = Detail::find_type_with_position(C_indices, A_indices);

    auto target_dims = get_dim_ranges<CRank>(*C);
    auto a_dims = Detail::get_dim_ranges_for(A, target_position_in_A);

    if constexpr (std::is_same_v<decltype(A_indices), decltype(C_indices)>) {
        // println("Using axpy variant algorithm.");
        if (C_prefactor != T{1.0})
            LinearAlgebra::scale(C_prefactor, C);
        LinearAlgebra::axpy(A_prefactor, A, C);
    } else {
        // println("Using generic sorting algorithm.");
        for (auto target_combination : std::apply(ranges::views::cartesian_product, target_dims)) {
            auto A_order =
                Detail::construct_indices<AIndices...>(target_combination, target_position_in_A, target_combination, target_position_in_A);

            T &target_value = std::apply(*C, target_combination);
            T A_value = std::apply(A, A_order);

            target_value = C_prefactor * target_value + A_prefactor * A_value;
        }
    }
    Timer::pop();
}

template <template <size_t, typename> typename AType, size_t ARank, template <size_t, typename> typename CType, size_t CRank,
          typename... CIndices, typename... AIndices, typename T = double>
auto sort(const std::tuple<CIndices...> &C_indices, CType<CRank, T> *C, const std::tuple<AIndices...> &A_indices, const AType<ARank, T> &A)
    -> std::enable_if_t<std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<CRank, T>, CType<CRank, T>> &&
                        std::is_base_of_v<::EinsumsInCpp::Detail::TensorBase<ARank, T>, AType<ARank, T>> &&
                        sizeof...(CIndices) == sizeof...(AIndices) && sizeof...(CIndices) == CRank && sizeof...(AIndices) == ARank> {
    sort(T{0}, C_indices, C, T{1}, A_indices, A);
}

} // namespace EinsumsInCpp::TensorAlgebra