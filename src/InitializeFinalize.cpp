#include "einsums/Blas.hpp"
#include "einsums/OpenMP.h"
#include "einsums/Timer.hpp"
#include "einsums/_Common.hpp"
#include "einsums/parallel/MPI.hpp"

#include <h5cpp/all>

namespace einsums {

auto initialize() -> int {
#if defined(EINSUMS_IN_PARALLEL)
    ErrorOr<void, mpi::Error> result = mpi::initialize(0, nullptr);
    if (result.is_error())
        return 1;
#endif

    timer::initialize();
    blas::initialize();

    // Disable nested omp regions
    omp_set_max_active_levels(1);

    // Disable HDF5 diagnostic reporting.
    H5Eset_auto(0, nullptr, nullptr);

    return 0;
}

void finalize(bool timerReport) {
    blas::finalize();

#if defined(EINSUMS_IN_PARALLEL)
    ErrorOr<void, mpi::Error> result = mpi::finalize();
#endif

    if (timerReport)
        timer::report();

    timer::finalize();
}

} // namespace einsums