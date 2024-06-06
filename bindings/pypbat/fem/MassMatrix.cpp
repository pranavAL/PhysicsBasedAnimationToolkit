#include "MassMatrix.h"

#include "For.h"
#include "Mesh.h"

#include <pbat/common/ConstexprFor.h>
#include <pbat/fem/MassMatrix.h>
#include <pybind11/eigen.h>
#include <utility>

namespace pbat {
namespace py {
namespace fem {

namespace pyb = pybind11;

void BindMassMatrix(pyb::module& m)
{
    ForMeshTypes([&]<class MeshType>() {
        auto constexpr kDimsMax            = 3;
        auto constexpr kQuadratureOrderMax = 6;
        pbat::common::ForRange<1, kDimsMax + 1>([&]<auto Dims>() {
            pbat::common::ForRange<1, kQuadratureOrderMax + 1>([&]<auto QuadratureOrder>() {
                using MassMatrixType = pbat::fem::MassMatrix<MeshType, Dims, QuadratureOrder>;
                std::string const className =
                    "MassMatrix_Dims_" + std::to_string(Dims) + "_QuadratureOrder_" +
                    std::to_string(QuadratureOrder) + "_" + MeshTypeName<MeshType>();
                pyb::class_<MassMatrixType>(m, className.data())
                    .def(
                        pyb::init([](MeshType const& mesh, Eigen::Ref<MatrixX const> const& detJe) {
                            return MassMatrixType(mesh, detJe);
                        }),
                        pyb::arg("mesh"),
                        pyb::arg("detJe"))
                    .def(
                        pyb::init([](MeshType const& mesh,
                                     Eigen::Ref<MatrixX const> const& detJe,
                                     Scalar rho) { return MassMatrixType(mesh, detJe, rho); }),
                        pyb::arg("mesh"),
                        pyb::arg("detJe"),
                        pyb::arg("rho"))
                    .def(
                        pyb::init(
                            [](MeshType const& mesh,
                               Eigen::Ref<MatrixX const> const& detJe,
                               VectorX const& rhoe) { return MassMatrixType(mesh, detJe, rhoe); }),
                        pyb::arg("mesh"),
                        pyb::arg("detJe"),
                        pyb::arg("rhoe"))
                    .def_readonly("Me", &MassMatrixType::Me)
                    .def_property_readonly(
                        "shape",
                        [](MassMatrixType const& M) {
                            return std::make_pair(M.OutputDimensions(), M.InputDimensions());
                        })
                    .def("to_matrix", &MassMatrixType::ToMatrix)
                    .def(
                        "compute_element_mass_matrices",
                        [](MassMatrixType& M, VectorX const& rhoe) {
                            M.ComputeElementMassMatrices(rhoe);
                        },
                        pyb::arg("rhoe"));
            });
        });
    });
}

} // namespace fem
} // namespace py
} // namespace pbat
