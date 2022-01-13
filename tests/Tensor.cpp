#include <H5Fpublic.h>
#include <type_traits>
#define ORB_TESTING
#include "EinsumsInCpp/LinearAlgebra.hpp"
#include "EinsumsInCpp/Print.hpp"
#include "EinsumsInCpp/Tensor.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Tensor creation", "[tensor]") {
    EinsumsInCpp::Tensor<2, double> A("A", 3, 3);
    EinsumsInCpp::Tensor<2, double> B("B", 3, 3);
    EinsumsInCpp::Tensor<2, double> C("C", 3, 3);

    REQUIRE((A.dim(0) == 3 && A.dim(1) == 3));
    REQUIRE((B.dim(0) == 3 && B.dim(1) == 3));
    REQUIRE((C.dim(0) == 3 && C.dim(1) == 3));

    CHECK_THAT(A.vector_data(), Catch::Matchers::Equals<double>({0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}));
    CHECK_THAT(B.vector_data(), Catch::Matchers::Equals<double>({0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}));
    CHECK_THAT(C.vector_data(), Catch::Matchers::Equals<double>({0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}));

    // Set A and B to identity
    A(0, 0) = 1.0;
    A(1, 1) = 1.0;
    A(2, 2) = 1.0;

    CHECK_THAT(A.vector_data(), Catch::Matchers::Equals<double>({1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));

    B(0, 0) = 1.0;
    B(1, 1) = 1.0;
    B(2, 2) = 1.0;

    CHECK_THAT(B.vector_data(), Catch::Matchers::Equals<double>({1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));

    // Perform basic matrix multiplication
    EinsumsInCpp::LinearAlgebra::gemm<false, false>(1.0, A, B, 0.0, &C);

    CHECK_THAT(C.vector_data(), Catch::Matchers::Equals<double>({1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
}

TEST_CASE("Tensor GEMMs", "[tensor]") {
    EinsumsInCpp::Tensor<2, double> A("A", 3, 3);
    EinsumsInCpp::Tensor<2, double> B("B", 3, 3);
    EinsumsInCpp::Tensor<2, double> C("C", 3, 3);

    REQUIRE((A.dim(0) == 3 && A.dim(1) == 3));
    REQUIRE((B.dim(0) == 3 && B.dim(1) == 3));
    REQUIRE((C.dim(0) == 3 && C.dim(1) == 3));

    A.vector_data() = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    B.vector_data() = std::vector<double>{11.0, 22.0, 33.0, 44.0, 55.0, 66.0, 77.0, 88.0, 99.0};

    EinsumsInCpp::LinearAlgebra::gemm<false, false>(1.0, A, B, 0.0, &C);
    CHECK_THAT(C.vector_data(), Catch::Matchers::Equals<double>({330.0, 396.0, 462.0, 726.0, 891.0, 1056.0, 1122.0, 1386.0, 1650.0}));

    EinsumsInCpp::LinearAlgebra::gemm<true, false>(1.0, A, B, 0.0, &C);
    CHECK_THAT(C.vector_data(), Catch::Matchers::Equals<double>({726.0, 858.0, 990.0, 858.0, 1023.0, 1188.0, 990.0, 1188.0, 1386.0}));

    EinsumsInCpp::LinearAlgebra::gemm<false, true>(1.0, A, B, 0.0, &C);
    CHECK_THAT(C.vector_data(), Catch::Matchers::Equals<double>({154.0, 352.0, 550.0, 352.0, 847.0, 1342.0, 550.0, 1342.0, 2134.0}));

    EinsumsInCpp::LinearAlgebra::gemm<true, true>(1.0, A, B, 0.0, &C);
    CHECK_THAT(C.vector_data(), Catch::Matchers::Equals<double>({330.0, 726.0, 1122.0, 396.0, 891.0, 1386.0, 462.0, 1056.0, 1650.0}));
}

TEST_CASE("Tensor GEMVs", "[tensor]") {
    EinsumsInCpp::Tensor<2, double> A("A", 3, 3);
    EinsumsInCpp::Tensor<1, double> x("x", 3);
    EinsumsInCpp::Tensor<1, double> y("y", 3);

    REQUIRE((A.dim(0) == 3 && A.dim(1) == 3));
    REQUIRE((x.dim(0) == 3));
    REQUIRE((y.dim(0) == 3));

    A.vector_data() = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    x.vector_data() = std::vector<double>{11.0, 22.0, 33.0};

    EinsumsInCpp::LinearAlgebra::gemv<false>(1.0, A, x, 0.0, &y);
    CHECK_THAT(y.vector_data(), Catch::Matchers::Equals<double>({154.0, 352.0, 550.0}));

    EinsumsInCpp::LinearAlgebra::gemv<true>(1.0, A, x, 0.0, &y);
    CHECK_THAT(y.vector_data(), Catch::Matchers::Equals<double>({330.0, 396.0, 462.0}));
}

TEST_CASE("Tensor SYEVs", "[tensor]") {
    EinsumsInCpp::Tensor<2, double> A("A", 3, 3);
    EinsumsInCpp::Tensor<1, double> x("x", 3);

    REQUIRE((A.dim(0) == 3 && A.dim(1) == 3));
    REQUIRE((x.dim(0) == 3));

    A.vector_data() = std::vector<double>{1.0, 2.0, 3.0, 2.0, 4.0, 5.0, 3.0, 5.0, 6.0};

    EinsumsInCpp::LinearAlgebra::syev(&A, &x);

    CHECK_THAT(x(0), Catch::Matchers::WithinRel(-0.515729, 0.00001));
    CHECK_THAT(x(1), Catch::Matchers::WithinRel(+0.170915, 0.00001));
    CHECK_THAT(x(2), Catch::Matchers::WithinRel(+11.344814, 0.00001));
}

TEST_CASE("TensorView creation", "[tensor]") {
    EinsumsInCpp::Tensor<3, double> A("A", 3, 3, 3);
    EinsumsInCpp::TensorView<2, double> viewA(A, EinsumsInCpp::Dim<2>{3, 9});

    for (int i = 0, ijk = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++, ijk++)
                A(i, j, k) = ijk;

    REQUIRE((A.dim(0) == 3 && A.dim(1) == 3 && A.dim(2) == 3));
    REQUIRE((viewA.dim(0) == 3 && viewA.dim(1) == 9));

    for (int i = 0, ij = 0; i < 3; i++)
        for (int j = 0; j < 9; j++, ij++)
            REQUIRE(viewA(i, j) == ij);
}

TEST_CASE("Tensor-2D HDF5") {
    EinsumsInCpp::Tensor<2, double> A("A", 3, 3);

    for (int i = 0, ij = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++, ij++) {
            A(i, j) = ij;
        }
    }

    {
        h5::fd_t fd = h5::create("tensor.h5", H5F_ACC_TRUNC);
        h5::ds_t ds = h5::create<double>(fd, "Matrix A", h5::current_dims{10, 20}, h5::max_dims{10, 20});
        h5::write(ds, A, h5::count{2, 3}, h5::offset{2, 2}, h5::stride{1, 3});
    }

    {
        auto B = h5::read<EinsumsInCpp::Tensor<2, double>>("tensor.h5", "Matrix A");

        REQUIRE((B.dim(0) == 10 && B.dim(1) == 20));
        REQUIRE(B(2, 2) == 0.0);
        REQUIRE(B(2, 5) == 1.0);
        REQUIRE(B(2, 8) == 2.0);
        REQUIRE(B(3, 2) == 3.0);
        REQUIRE(B(3, 5) == 4.0);
        REQUIRE(B(3, 8) == 5.0);
    }
}

TEST_CASE("Tensor-1D HDF5") {
    auto A = EinsumsInCpp::create_random_tensor("A", 3);

    {
        h5::fd_t fd = h5::create("tensor-1d.h5", H5F_ACC_TRUNC);
        h5::ds_t ds = h5::create<double>(fd, "A", h5::current_dims{3}, h5::max_dims{3});
        h5::write(ds, A);
    }

    {
        auto B = h5::read<EinsumsInCpp::Tensor<1, double>>("tensor-1d.h5", "A");

        REQUIRE(A(0) == B(0));
        REQUIRE(A(1) == B(1));
        REQUIRE(A(2) == B(2));
    }
}

TEST_CASE("Tensor-3D HDF5") {
    auto A = EinsumsInCpp::create_random_tensor("A", 3, 2, 1);

    {
        h5::fd_t fd = h5::create("tensor-3d.h5", H5F_ACC_TRUNC);
        h5::ds_t ds = h5::create<double>(fd, "A", h5::current_dims{3, 2, 1});
        h5::write(ds, A);
    }

    {
        auto B = h5::read<EinsumsInCpp::Tensor<3, double>>("tensor-3d.h5", "A");

        REQUIRE(B.dim(0) == 3);
        REQUIRE(B.dim(1) == 2);
        REQUIRE(B.dim(2) == 1);
    }
}

TEST_CASE("TensorView-2D HDF5") {
    SECTION("Subview Offset{0,0,0}") {
        auto A = EinsumsInCpp::create_random_tensor("A", 3, 3, 3);
        EinsumsInCpp::TensorView<2, double> viewA(A, EinsumsInCpp::Dim<2>{3, 9});

        REQUIRE((A.dim(0) == 3 && A.dim(1) == 3 && A.dim(2) == 3));
        REQUIRE((viewA.dim(0) == 3 && viewA.dim(1) == 9));

        {
            h5::fd_t fd = h5::create("tensorview-2d.h5", H5F_ACC_TRUNC);
            h5::ds_t ds = h5::create<double>(fd, "A", h5::current_dims{3, 9});
            h5::write(ds, viewA);
        }

        {
            auto B = h5::read<EinsumsInCpp::Tensor<2, double>>("tensorview-2d.h5", "A");
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 9; j++)
                    REQUIRE(viewA(i, j) == B(i, j));
        }
    }

    SECTION("Subviews") {
        auto C = EinsumsInCpp::create_random_tensor("C", 3, 3);
        EinsumsInCpp::TensorView<2> viewC(C, EinsumsInCpp::Dim<2>{2, 2}, EinsumsInCpp::Offset<2>{1, 1}, EinsumsInCpp::Stride<2>{3, 1});

        // EinsumsInCpp::println("C strides: %zu %zu\n", C.strides()[0], C.strides()[1]);

        REQUIRE(C(1, 1) == viewC(0, 0));
        REQUIRE(C(1, 2) == viewC(0, 1));
        REQUIRE(C(2, 1) == viewC(1, 0));
        REQUIRE(C(2, 2) == viewC(1, 1));
    }

    SECTION("Subviews 2") {
        auto C = EinsumsInCpp::create_random_tensor("C", 3, 3);
        // std::array<EinsumsInCpp::Range, 2> test;
        EinsumsInCpp::TensorView<2> viewC = C(EinsumsInCpp::Range{1, 3}, EinsumsInCpp::Range{1, 3});

        // EinsumsInCpp::println(C);
        // EinsumsInCpp::println(viewC);

        REQUIRE(C(1, 1) == viewC(0, 0));
        REQUIRE(C(1, 2) == viewC(0, 1));
        REQUIRE(C(2, 1) == viewC(1, 0));
        REQUIRE(C(2, 2) == viewC(1, 1));
    }
}

TEST_CASE("Tensor 2D - HDF5 wrapper") {
    auto A = EinsumsInCpp::create_random_tensor("A", 3, 3);

    h5::fd_t fd = h5::create("tensor-wrapper.h5", H5F_ACC_TRUNC);

    EinsumsInCpp::write(fd, A);

    auto B = EinsumsInCpp::read<2>(fd, "A");

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            REQUIRE(B(i, j) == B(i, j));
}