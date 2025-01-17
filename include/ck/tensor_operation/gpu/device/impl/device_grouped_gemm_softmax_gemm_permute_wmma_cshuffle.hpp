// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include <iostream>
#include <sstream>
#include <numeric>
#include <initializer_list>
#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/utility/common_header.hpp"
#include "ck/tensor_description/tensor_descriptor.hpp"
#include "ck/tensor_description/tensor_descriptor_helper.hpp"
#include "ck/tensor_operation/gpu/device/device_grouped_gemm_softmax_gemm_permute.hpp"
#include "ck/tensor_operation/gpu/device/gemm_specialization.hpp"
#include "ck/tensor_operation/gpu/device/matrix_padder.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/grid/gridwise_batched_gemm_softmax_gemm_wmma_cshuffle.hpp"
#include "ck/tensor_operation/operator_transform/transform_contraction_to_gemm_arraybase.hpp"
#include "ck/host_utility/device_prop.hpp"
#include "ck/host_utility/kernel_launch.hpp"

namespace ck {
namespace tensor_operation {
namespace device {

template <typename DeviceOp,
          typename GridwiseGemm,
        //   typename ADataType,
        //   typename B0DataType,
        //   typename B1DataType,
        //   typename CDataType,
          typename GroupKernelArg,
          typename AElementwiseOperation,
          typename B0ElementwiseOperation,
          typename AccElementwiseOperation,
          typename B1ElementwiseOperation,
          typename CElementwiseOperation,
          bool HasMainKBlockLoop>
__global__ void
#if CK_USE_LAUNCH_BOUNDS
    __launch_bounds__(CK_MAX_THREAD_PER_BLOCK, CK_MIN_BLOCK_PER_CU)
#endif
        kernel_grouped_gemm_softmax_gemm_wmma_cshuffle(
                                                    //    const ADataType* __restrict__ p_a_grid,
                                                    //    const B0DataType* __restrict__ p_b0_grid,
                                                    //    const B1DataType* __restrict__ p_b1_grid,
                                                    //    CDataType* __restrict__ p_c_grid,
                                                    //    index_t M,
                                                    //    index_t N,
                                                    //    index_t K,
                                                    //    index_t O,
                                                    //    index_t G0,
                                                    //    index_t G1,
                                                    //    float alpha,
                                                    //    bool input_permute,
                                                    //    bool output_permute
                                                       const void CK_CONSTANT_ADDRESS_SPACE* group_kernel_args,
                                                       const index_t group_count,
                                                       const AElementwiseOperation a_element_op,
                                                       const B0ElementwiseOperation b_element_op,
                                                       const AccElementwiseOperation acc_element_op,
                                                       const B1ElementwiseOperation b1_element_op,
                                                       const CElementwiseOperation c_element_op
                                                       )
{
#if(!defined(__HIP_DEVICE_COMPILE__) || defined(__gfx1100__) || defined(__gfx1101__) || \
    defined(__gfx1102__))

    // clang-format off
// ***************************************************
// Make Tensor Descriptors
    // constexpr index_t array_size = 4;
    // std::array<ck::index_t, array_size> a_gs_ms_ks_lengths{G0, G1, M, K};
    // std::array<ck::index_t, array_size> a_gs_ms_ks_strides =
    //     input_permute
    //         ? std::array<ck::index_t, array_size>{M * G1 * K, K, G1 * K, 1} // A layout [G0, M, G1, K]
    //         : std::array<ck::index_t, array_size>{G1 * M * K, M * K, K, 1}; // A layout [G0, G1, M, K]

    // std::array<ck::index_t, array_size> b0_gs_ns_ks_lengths{G0, G1, N, K};
    // std::array<ck::index_t, array_size> b0_gs_ns_ks_strides =
    //     input_permute
    //         ? std::array<ck::index_t, array_size>{N * G1 * K, K, G1 * K, 1} // B0 layout [G0, N, G1, K]
    //         : std::array<ck::index_t, array_size>{G1 * N * K, N * K, K, 1}; // B0 layout [G0, G1, N, K]

    // std::array<ck::index_t, array_size> b1_gs_os_ns_lengths{G0, G1, O, N};
    // std::array<ck::index_t, array_size> b1_gs_os_ns_strides =
    //     input_permute
    //         ? std::array<ck::index_t, array_size>{N * G1 * O, O, 1, G1 * O} // B1 layout [G0, N, G1, O]
    //         : std::array<ck::index_t, array_size>{G1 * N * O, N * O, 1, O}; // B1 layout [G0, G1, N, O]

    // std::array<ck::index_t, array_size> c_gs_ms_os_lengths{G0, G1, M, O};
    // std::array<ck::index_t, array_size> c_gs_ms_os_strides =
    //     output_permute
    //         ? std::array<ck::index_t, array_size>{M * G1 * O, O, G1 * O, 1} // C layout [G0, M, G1, O]
    //         : std::array<ck::index_t, array_size>{G1 * M * O, M * O, O, 1}; // C layout [G0, G1, M, O]

    // const auto a_element_op    = AElementwiseOperation{};
    // const auto b0_element_op   = B0ElementwiseOperation{};
    // const auto acc0_element_op = AccElementwiseOperation{alpha};
    // const auto b1_element_op   = B1ElementwiseOperation{};
    // const auto c_element_op    = CElementwiseOperation{};
    // // fail to reuse DeviceOp::MakeArgument() because of the __device__ function required.

    // const auto a_grid_desc = DeviceOp::MakeAGridDescriptor(a_gs_ms_ks_lengths, a_gs_ms_ks_strides);
    // const auto b0_grid_desc =
    //     DeviceOp::MakeB0GridDescriptor(b0_gs_ns_ks_lengths, b0_gs_ns_ks_strides);
    // const auto b1_grid_desc =
    //     DeviceOp::MakeB1GridDescriptor(b1_gs_os_ns_lengths, b1_gs_os_ns_strides);
    // const auto c_grid_desc_m_n =
    //               DeviceOp::Transform::MakeCGridDescriptor_M_N(c_gs_ms_os_lengths, c_gs_ms_os_strides);
    // const auto c_grid_desc_mblock_mperblock_nblock_nperblock = 
    //               GridwiseGemm::MakeCGridDescriptor_MBlock_MPerBlock_NBlock_NPerBlock(c_grid_desc_m_n);
    // const auto block_2_ctile_map = GridwiseGemm::MakeDefaultBlock2CTileMap(c_grid_desc_m_n, 1, 1);

    // const auto a_grid_desc_g_m_k =
    //               DeviceOp::Transform::MakeAGridDescriptor_G_M_K(a_gs_ms_ks_lengths, a_gs_ms_ks_strides);
    // const auto b0_grid_desc_g_l_k = 
    //               DeviceOp::Transform::MakeB0GridDescriptor_G_N_K(b0_gs_ns_ks_lengths, b0_gs_ns_ks_strides);
    // const auto b1_grid_desc_g_n_l = 
    //               DeviceOp::Transform::MakeB1GridDescriptor_G_N_K(b1_gs_os_ns_lengths, b1_gs_os_ns_strides);
    // const auto c_grid_desc_g_m_n =
    //               DeviceOp::Transform::MakeCGridDescriptor_G_M_N(c_gs_ms_os_lengths, c_gs_ms_os_strides);
    // const auto compute_base_ptr_of_batch = 
    //               typename DeviceOp::ComputeBasePtrOfStridedBatch{a_grid_desc_g_m_k, b0_grid_desc_g_l_k, b1_grid_desc_g_n_l, c_grid_desc_g_m_n};
    // index_t batch_count = c_grid_desc_g_m_n.GetLength(Number<0>{});
    // const auto c0_matrix_mask = typename DeviceOp::C0MatrixMask{b0_grid_desc_g_l_k.GetLength(Number<1>{})};

    // // clang-format on
    // __shared__ char p_shared[GridwiseGemm::GetSharedMemoryNumberOfByte()];
    // const index_t num_blocks_per_batch =
    //     __builtin_amdgcn_readfirstlane(get_grid_size() / batch_count);
    // const index_t g_idx = __builtin_amdgcn_readfirstlane(get_block_1d_id() / num_blocks_per_batch);

    // const long_index_t a_batch_offset = __builtin_amdgcn_readfirstlane(
    //     static_cast<long_index_t>(compute_base_ptr_of_batch.GetABasePtr(g_idx)));
    // const long_index_t b0_batch_offset = __builtin_amdgcn_readfirstlane(
    //     static_cast<long_index_t>(compute_base_ptr_of_batch.GetB0BasePtr(g_idx)));
    // const long_index_t b1_batch_offset = __builtin_amdgcn_readfirstlane(
    //     static_cast<long_index_t>(compute_base_ptr_of_batch.GetB1BasePtr(g_idx)));
    // const long_index_t c_batch_offset = __builtin_amdgcn_readfirstlane(
    //     static_cast<long_index_t>(compute_base_ptr_of_batch.GetCBasePtr(g_idx)));

    // // per-group batch offset
    // const index_t num_blocks_per_batch = arg_ptr[group_id].num_blocks_per_batch_;
    // const index_t g_idx                = __builtin_amdgcn_readfirstlane(
    //     (block_id - arg_ptr[group_id].block_start_) / num_blocks_per_batch);




    // GridwiseGemm::template Run<HasMainKBlockLoop>(p_a_grid + a_batch_offset,
    //                                             p_b0_grid + b0_batch_offset,
    //                                             p_b1_grid + b1_batch_offset,
    //                                             p_c_grid + c_batch_offset,
    //                                             p_shared,
    //                                             a_grid_desc,
    //                                             b0_grid_desc,
    //                                             b1_grid_desc,
    //                                             c_grid_desc_mblock_mperblock_nblock_nperblock,
    //                                             a_element_op,
    //                                             b0_element_op,
    //                                             acc0_element_op,
    //                                             b1_element_op,
    //                                             c_element_op,
    //                                             c0_matrix_mask,
    //                                             block_2_ctile_map);


    // grouped

    __shared__ char p_shared[GridwiseGemm::GetSharedMemoryNumberOfByte()];

    const index_t block_id = get_block_1d_id();

    const auto arg_ptr = reinterpret_cast<const GroupKernelArg*>(
        cast_pointer_to_generic_address_space(group_kernel_args));

    index_t left     = 0;
    index_t right    = group_count;
    index_t group_id = index_t((left + right) / 2);

    while(
        (!(block_id >= arg_ptr[group_id].block_start_ && block_id < arg_ptr[group_id].block_end_)))
    {
        if(block_id < arg_ptr[group_id].block_start_)
        {
            right = group_id;
        }
        else
        {
            left = group_id;
        }
        group_id = index_t((left + right) / 2);
    }

    // per-group batch offset
    const index_t num_blocks_per_batch = arg_ptr[group_id].num_blocks_per_batch_;
    const index_t g_idx                = __builtin_amdgcn_readfirstlane(
        (block_id - arg_ptr[group_id].block_start_) / num_blocks_per_batch);

    const long_index_t a_batch_offset = __builtin_amdgcn_readfirstlane(
        static_cast<long_index_t>(arg_ptr[group_id].compute_base_ptr_of_batch_.GetABasePtr(g_idx)));
    const long_index_t b_batch_offset = __builtin_amdgcn_readfirstlane(
        static_cast<long_index_t>(arg_ptr[group_id].compute_base_ptr_of_batch_.GetB0BasePtr(g_idx)));
    const long_index_t b1_batch_offset = __builtin_amdgcn_readfirstlane(static_cast<long_index_t>(
        arg_ptr[group_id].compute_base_ptr_of_batch_.GetB1BasePtr(g_idx)));
    const long_index_t c_batch_offset  = __builtin_amdgcn_readfirstlane(
        static_cast<long_index_t>(arg_ptr[group_id].compute_base_ptr_of_batch_.GetCBasePtr(g_idx)));

    GridwiseGemm::template Run<HasMainKBlockLoop>(
        arg_ptr[group_id].p_a_grid_ + a_batch_offset,
        arg_ptr[group_id].p_b_grid_ + b_batch_offset,
        arg_ptr[group_id].p_b1_grid_ + b1_batch_offset,
        arg_ptr[group_id].p_c_grid_ + c_batch_offset,
        p_shared,
        arg_ptr[group_id].a_grid_desc_ak0_m_ak1_,
        arg_ptr[group_id].b_grid_desc_bk0_n_bk1_,
        arg_ptr[group_id].b1_grid_desc_bk0_n_bk1_,
        arg_ptr[group_id].c_grid_desc_mblock_mperblock_nblock_nperblock_,
        a_element_op,
        b_element_op,
        acc_element_op,
        b1_element_op,
        c_element_op,
        arg_ptr[group_id].c0_matrix_mask_,
        arg_ptr[group_id].block_2_ctile_map_);

#else
    // ignore = p_a_grid;
    // ignore = p_b0_grid;
    // ignore = p_b1_grid;
    // ignore = p_c_grid;
    // ignore = M;
    // ignore = N;
    // ignore = K;
    // ignore = O;
    // ignore = G0;
    // ignore = G1;
    // ignore = input_permute;
    // ignore = output_permute;
    ignore = group_kernel_args;
    ignore = group_count;
    ignore = a_element_op;
    ignore = b_element_op;
    ignore = acc_element_op;
    ignore = b1_element_op;
    ignore = c_element_op;
#endif // end of if (defined(__gfx1100__))
}

// Computes C = A  * B0 * B1
//         MN = MK * KL * LN
//              ^^^^^^ (Acc0)
//              ^^^^^^^^^^^ (Acc1)
template <index_t NumDimG,
          index_t NumDimM,
          index_t NumDimL,
          index_t NumDimK,
          index_t NumDimN,
          typename ADataType,
          typename B0DataType,
          typename B1DataType,
          typename CDataType,
          typename Acc0BiasDataType,
          typename Acc0DataType,
          typename Acc1BiasDataType,
          typename Acc1DataType,
          typename CShuffleDataType,
          typename AElementwiseOperation,
          typename B0ElementwiseOperation,
          typename AccElementwiseOperation,
          typename B1ElementwiseOperation,
          typename CElementwiseOperation,
          GemmSpecialization GemmSpec,
          TensorSpecialization ASpec,
          TensorSpecialization B0Spec,
          TensorSpecialization B1Spec,
          TensorSpecialization CSpec,
          ck::index_t NumPrefetch,
          ck::index_t BlockSize,
          ck::index_t MPerBlock,
          ck::index_t LPerBlock,
          ck::index_t KPerBlock,
          ck::index_t AK1,
          ck::index_t BK1,
          ck::index_t NPerBlock,
          ck::index_t LTilePerBlock,
          ck::index_t L1,
          ck::index_t MPerWmma,
          ck::index_t LPerWmma,
          ck::index_t NPerWmma,
          ck::index_t MRepeat,
          ck::index_t LRepeat,
          ck::index_t NRepeat,
          typename ABlockTransferThreadClusterLengths_K0_M_K1,
          typename ABlockTransferThreadClusterArrangeOrder,
          typename ABlockTransferSrcAccessOrder,
          ck::index_t ABlockTransferSrcVectorDim,
          ck::index_t ABlockTransferSrcScalarPerVector,
          ck::index_t ABlockTransferDstScalarPerVector_K1,
          bool ABlockLdsAddExtraM,
          typename B0BlockTransferThreadClusterLengths_K0_L_K1,
          typename B0BlockTransferThreadClusterArrangeOrder,
          typename B0BlockTransferSrcAccessOrder,
          ck::index_t B0BlockTransferSrcVectorDim,
          ck::index_t B0BlockTransferSrcScalarPerVector,
          ck::index_t B0BlockTransferDstScalarPerVector_K1,
          bool B0BlockLdsAddExtraL,
          typename B1BlockTransferThreadClusterLengths_L0_N_L1,
          typename B1BlockTransferThreadClusterArrangeOrder,
          typename B1BlockTransferSrcAccessOrder,
          ck::index_t B1BlockTransferSrcVectorDim,
          ck::index_t B1BlockTransferSrcScalarPerVector,
          ck::index_t B1BlockTransferDstScalarPerVector_L1,
          bool B1BlockLdsAddExtraN,
          index_t CShuffleMRepeatPerShuffle,
          index_t CShuffleNRepeatPerShuffle,
          typename CShuffleBlockTransferClusterLengths_MBlock_MPerBlock_NBlock_NPerBlock,
          index_t CShuffleBlockTransferScalarPerVector_NPerBlock,
          MaskingSpecialization MaskingSpec,
          ck::LoopScheduler LoopSched     = make_default_loop_scheduler(),
          ck::PipelineVersion PipelineVer = ck::PipelineVersion::v1>
struct DeviceGroupedGemmSoftmaxGemmPermute_Wmma_CShuffle
    : public DeviceGroupedGemmSoftmaxGemmPermute<NumDimG,
                                                 NumDimM,
                                                 NumDimL,
                                                 NumDimK,
                                                 NumDimN,
                                                 ADataType,
                                                 B0DataType,
                                                 B1DataType,
                                                 CDataType,
                                                 Acc0BiasDataType,
                                                 Acc1BiasDataType,
                                                 AElementwiseOperation,
                                                 B0ElementwiseOperation,
                                                 AccElementwiseOperation,
                                                 B1ElementwiseOperation,
                                                 CElementwiseOperation,
                                                 MaskingSpec>
{
    static_assert(NumDimG > 0 && NumDimM > 0 && NumDimL > 0 && NumDimK > 0 && NumDimN > 0,
                  "Number of dimension must be greater than 0");

    static constexpr index_t NumAcc0Bias = Acc0BiasDataType::Size();
    static constexpr index_t NumAcc1Bias = Acc1BiasDataType::Size();

    // TODO ANT: implement bias combination
    static_assert(NumAcc0Bias == 0 && NumAcc0Bias == 0, "Bias addition is unimplemented");

    static constexpr index_t NumDimGemm0M = NumDimM;
    static constexpr index_t NumDimGemm0N = NumDimL;
    static constexpr index_t NumDimGemm0K = NumDimK;
    static constexpr index_t NumDimGemm1M = NumDimM;
    static constexpr index_t NumDimGemm1N = NumDimN;
    static constexpr index_t NumDimGemm1K = NumDimL;

    using DeviceOp = DeviceGroupedGemmSoftmaxGemmPermute_Wmma_CShuffle;
    using ProblemDesc = typename DeviceGroupedGemmSoftmaxGemmPermute<NumDimG,
                                                                     NumDimM,
                                                                     NumDimL,
                                                                     NumDimK,
                                                                     NumDimN,
                                                                     ADataType,
                                                                     B0DataType,
                                                                     B1DataType,
                                                                     CDataType,
                                                                     Acc0BiasDataType,
                                                                     Acc1BiasDataType,
                                                                     AElementwiseOperation,
                                                                     B0ElementwiseOperation,
                                                                     AccElementwiseOperation,
                                                                     B1ElementwiseOperation,
                                                                     CElementwiseOperation,
                                                                     MaskingSpec>::ProblemDesc;

    static constexpr auto I0 = Number<0>{};
    static constexpr auto I1 = Number<1>{};
    static constexpr auto I2 = Number<2>{};
    static constexpr auto I3 = Number<3>{};
    static constexpr auto I4 = Number<4>{};
    static constexpr auto I5 = Number<5>{};
    static constexpr auto I6 = Number<6>{};

    static constexpr auto WmmaK = 16;

    static constexpr auto MWaves = MPerBlock / (MRepeat * MPerWmma);
    static constexpr auto LWaves = LPerBlock / (LRepeat * LPerWmma);
    static constexpr auto NWaves = NPerBlock / (NRepeat * NPerWmma);

    static constexpr auto AEnableLds_auto  = LWaves == 1 ? false : true;
    static constexpr auto B0EnableLds_auto = MWaves == 1 ? false : true;
    static constexpr auto B1EnableLds_auto = MWaves == 1 ? false : true;

    static constexpr auto AEnableLds_manu  = false;
    static constexpr auto B0EnableLds_manu = true;
    static constexpr auto B1EnableLds_manu = true;

    static constexpr auto AEnableLds  = AEnableLds_auto || AEnableLds_manu || (NumPrefetch > 1);
    static constexpr auto B0EnableLds = B0EnableLds_auto || B0EnableLds_manu || (NumPrefetch > 1);
    static constexpr auto B1EnableLds = B1EnableLds_auto || B1EnableLds_manu || (NumPrefetch > 1);

    using Transform = TransformBatchedContractionContractionToBatchedGemmGemm_Wmma<
        Sequence<NumDimG, NumDimM, NumDimL, NumDimK, NumDimN>,
        Sequence<MPerBlock, LPerBlock, KPerBlock, NPerBlock>,
        GemmSpec,
        ASpec,
        B0Spec,
        B1Spec,
        CSpec>;

    __host__ __device__ static auto MakeAGridDescriptor(
        const std::array<index_t, NumDimG + NumDimM + NumDimN>& a_gs_ms_ks_lengths_vec,
        const std::array<index_t, NumDimG + NumDimM + NumDimN>& a_gs_ms_ks_strides_vec)
    {
        if constexpr(AEnableLds)
        {
            return Transform::MakeAGridDescriptor_AK0_M_AK1(
                Transform::MakeAGridDescriptor_M_K(a_gs_ms_ks_lengths_vec, a_gs_ms_ks_strides_vec),
                Number<AK1>{});
        }
        else
        {
            return Transform::
                MakeAGridDescriptor_AKWmma_MBlockRepeat_MWaves_AK0PerWmma_AKRow_MPerWmma_AK1(
                    Transform::MakeAGridDescriptor_M_K(a_gs_ms_ks_lengths_vec,
                                                       a_gs_ms_ks_strides_vec),
                    Number<WmmaK>{},
                    Number<MRepeat>{},
                    Number<MWaves>{},
                    Number<MPerWmma>{},
                    Number<AK1>{});
        }
    }

    __host__ __device__ static auto MakeB0GridDescriptor(
        const std::array<index_t, NumDimG + NumDimM + NumDimN>& b0_gs_ls_ks_lengths_vec,
        const std::array<index_t, NumDimG + NumDimM + NumDimN>& b0_gs_ls_ks_strides_vec)
    {
        if constexpr(B0EnableLds)
        {
            return Transform::MakeB0GridDescriptor_BK0_N_BK1(
                Transform::MakeB0GridDescriptor_N_K(b0_gs_ls_ks_lengths_vec,
                                                    b0_gs_ls_ks_strides_vec),
                Number<BK1>{});
        }
        else
        {
            return Transform::
                MakeB0GridDescriptor_BKWmma_LBlockRepeat_LWaves_BK0PerWmma_BKRow_LPerWmma_BK1(
                    Transform::MakeB0GridDescriptor_N_K(b0_gs_ls_ks_lengths_vec,
                                                        b0_gs_ls_ks_strides_vec),
                    Number<WmmaK>{},
                    Number<LRepeat>{},
                    Number<LWaves>{},
                    Number<LPerWmma>{},
                    Number<BK1>{});
        }
    }

    __host__ __device__ static auto MakeB1GridDescriptor(
        const std::array<index_t, NumDimG + NumDimM + NumDimN>& b1_gs_ns_ls_lengths_vec,
        const std::array<index_t, NumDimG + NumDimM + NumDimN>& b1_gs_ns_ls_strides_vec)
    {
        if constexpr(B1EnableLds)
        {
            return Transform::MakeB1GridDescriptor_BK0_N_BK1(
                Transform::MakeB1GridDescriptor_N_K(b1_gs_ns_ls_lengths_vec,
                                                    b1_gs_ns_ls_strides_vec),
                Number<L1>{});
        }
        else
        {
            return Transform::
                MakeB1GridDescriptor_BLWmma_NBlockRepeat_NWaves__BL0PerWmma_BLRow_NPerWmma_BL1(
                    Transform::MakeB1GridDescriptor_N_K(b1_gs_ns_ls_lengths_vec,
                                                        b1_gs_ns_ls_strides_vec),
                    Number<WmmaK>{},
                    Number<NRepeat>{},
                    Number<NWaves>{},
                    Number<NPerWmma>{},
                    Number<L1>{});
        }
    }

    using AGridDesc        = decltype(MakeAGridDescriptor({}, {}));
    using B0GridDesc       = decltype(MakeB0GridDescriptor({}, {}));
    using B1GridDesc       = decltype(MakeB1GridDescriptor({}, {}));
    using CGridDesc_M_N    = decltype(Transform::MakeCGridDescriptor_M_N({}, {}));
    using AGridDesc_G_M_K  = decltype(Transform::MakeAGridDescriptor_G_M_K({}, {}));
    using B0GridDesc_G_L_K = decltype(Transform::MakeB0GridDescriptor_G_N_K({}, {}));
    using B1GridDesc_G_N_L = decltype(Transform::MakeB1GridDescriptor_G_N_K({}, {}));
    using CGridDesc_G_M_N  = decltype(Transform::MakeCGridDescriptor_G_M_N({}, {}));

    __host__ __device__ constexpr static auto make_MaskOutPredicate()
    {
        if constexpr(MaskingSpec == MaskingSpecialization::MaskDisabled)
        {
            return MaskDisabledPredicate{};
        }
        else if constexpr(MaskingSpec == MaskingSpecialization::MaskOutUpperTriangle)
        {
            return MaskOutUpperTrianglePredicate{};
        }
    }
    using C0MatrixMask = C0MatrixMask_impl<decltype(make_MaskOutPredicate())>;

    struct ComputeBasePtrOfStridedBatch
    {
        __host__ __device__ ComputeBasePtrOfStridedBatch(const AGridDesc_G_M_K& a_grid_desc_g_m_k,
                                                         const B0GridDesc_G_L_K& b0_grid_desc_g_l_k,
                                                         const B1GridDesc_G_N_L& b1_grid_desc_g_n_l,
                                                         const CGridDesc_G_M_N& c_grid_desc_g_m_n)
            : a_grid_desc_g_m_k_(a_grid_desc_g_m_k),
              b0_grid_desc_g_l_k_(b0_grid_desc_g_l_k),
              b1_grid_desc_g_n_l_(b1_grid_desc_g_n_l),
              c_grid_desc_g_m_n_(c_grid_desc_g_m_n)
        {
        }

        __host__ __device__ constexpr long_index_t GetABasePtr(index_t g_idx) const
        {
            return a_grid_desc_g_m_k_.CalculateOffset(make_multi_index(g_idx, 0, 0));
        }

        __host__ __device__ constexpr long_index_t GetB0BasePtr(index_t g_idx) const
        {
            return b0_grid_desc_g_l_k_.CalculateOffset(make_multi_index(g_idx, 0, 0));
        }

        __host__ __device__ constexpr long_index_t GetB1BasePtr(index_t g_idx) const
        {
            return b1_grid_desc_g_n_l_.CalculateOffset(make_multi_index(g_idx, 0, 0));
        }

        __host__ __device__ constexpr long_index_t GetCBasePtr(index_t g_idx) const
        {
            return c_grid_desc_g_m_n_.CalculateOffset(make_multi_index(g_idx, 0, 0));
        }

        private:
        AGridDesc_G_M_K a_grid_desc_g_m_k_;
        B0GridDesc_G_L_K b0_grid_desc_g_l_k_;
        B1GridDesc_G_N_L b1_grid_desc_g_n_l_;
        CGridDesc_G_M_N c_grid_desc_g_m_n_;
    };

    // GridwiseGemm
    using GridwiseGemm = GridwiseBatchedGemmSoftmaxGemm_Wmma<
        // DataType Family
        ADataType,
        B0DataType,
        Acc0DataType,
        B1DataType,
        Acc1DataType,
        CShuffleDataType,
        CDataType,
        // ElementwiseOp Family
        AElementwiseOperation,
        B0ElementwiseOperation,
        AccElementwiseOperation,
        B1ElementwiseOperation,
        CElementwiseOperation,
        InMemoryDataOperationEnum::Set,
        // InMemory Data Descriptor
        AGridDesc,
        B0GridDesc,
        B1GridDesc,
        CGridDesc_M_N,
        // Tiling Family
        MPerBlock,
        LPerBlock,
        KPerBlock,
        AK1,
        BK1,
        NPerBlock,
        LTilePerBlock,
        L1,
        MPerWmma,
        LPerWmma,
        NPerWmma,
        MRepeat,
        LRepeat,
        NRepeat,
        // ThreadCluster Family
        BlockSize,
        ABlockTransferThreadClusterLengths_K0_M_K1,
        ABlockTransferThreadClusterArrangeOrder,
        ABlockTransferSrcAccessOrder,
        ABlockTransferSrcVectorDim,
        ABlockTransferSrcScalarPerVector,
        ABlockTransferDstScalarPerVector_K1,
        true,
        AEnableLds,
        ABlockLdsAddExtraM,
        B0BlockTransferThreadClusterLengths_K0_L_K1,
        B0BlockTransferThreadClusterArrangeOrder,
        B0BlockTransferSrcAccessOrder,
        B0BlockTransferSrcVectorDim,
        B0BlockTransferSrcScalarPerVector,
        B0BlockTransferDstScalarPerVector_K1,
        true,
        B0EnableLds,
        B0BlockLdsAddExtraL,
        B1BlockTransferThreadClusterLengths_L0_N_L1,
        B1BlockTransferThreadClusterArrangeOrder,
        B1BlockTransferSrcAccessOrder,
        B1BlockTransferSrcVectorDim,
        B1BlockTransferSrcScalarPerVector,
        B1BlockTransferDstScalarPerVector_L1,
        false,
        B1EnableLds,
        B1BlockLdsAddExtraN,
        CShuffleMRepeatPerShuffle,
        CShuffleNRepeatPerShuffle,
        CShuffleBlockTransferClusterLengths_MBlock_MPerBlock_NBlock_NPerBlock,
        CShuffleBlockTransferScalarPerVector_NPerBlock,
        Transform::matrix_padder.PadN,
        MaskingSpec == MaskingSpecialization::MaskOutUpperTriangle,
        NumPrefetch,
        LoopSched,
        PipelineVer>;

//     struct RawArg : public BaseArgument
//     {
//         RawArg(const ADataType* p_a_grid,
//                const B0DataType* p_b0_grid,
//                const B1DataType* p_b1_grid,
//                CDataType* p_c_grid,
//                index_t M,
//                index_t N,
//                index_t K,
//                index_t O,
//                index_t G0,
//                index_t G1,
//                float alpha,
//                bool input_permute,
//                bool output_permute)
//             : p_a_grid_{p_a_grid},
//               p_b0_grid_{p_b0_grid},
//               p_b1_grid_{p_b1_grid},
//               p_c_grid_{p_c_grid},
//               M_{M},
//               N_{N},
//               K_{K},
//               O_{O},
//               G0_{G0},
//               G1_{G1},
//               alpha_{alpha},
//               input_permute_{input_permute},
//               output_permute_{output_permute}
//         {
//         }
//         // Pointers
//         const ADataType* p_a_grid_;
//         const B0DataType* p_b0_grid_;
//         const B1DataType* p_b1_grid_;
//         CDataType* p_c_grid_;

//         // Raw Problem Size
//         index_t M_;
//         index_t N_;
//         index_t K_;
//         index_t O_;
//         index_t G0_;
//         index_t G1_;
//         float alpha_;
//         bool input_permute_;
//         bool output_permute_;
//     };

//     static auto MakeArgument(const ADataType* p_a,
//                              const B0DataType* p_b0,
//                              const B1DataType* p_b1,
//                              CDataType* p_c,
//                              index_t M,
//                              index_t N,
//                              index_t K,
//                              index_t O,
//                              index_t G0,
//                              index_t G1,
//                              float alpha,
//                              bool input_permute,
//                              bool output_permute)
//     {
//         return RawArg{
//             p_a, p_b0, p_b1, p_c, M, N, K, O, G0, G1, alpha, input_permute, output_permute};
//     }

//     static bool IsSupportedArgument(const RawArg& arg)
//     {
//         if(ck::get_device_name() == "gfx1100" || ck::get_device_name() == "gfx1101" ||
//            ck::get_device_name() == "gfx1102")
//         {
//             if constexpr(!(is_same_v<Acc0DataType, float> || is_same_v<Acc0DataType, int32_t>))
//             {
//                 printf("DeviceOp: Acc0 Type err");
//                 return false;
//             }

//             if constexpr(!(is_same_v<Acc1DataType, float> || is_same_v<Acc1DataType, int32_t>))
//             {
//                 printf("DeviceOp: Acc1 Type err");
//                 return false;
//             }
//         }
//         else
//         {
//             printf("DeviceOp: Arch err");
//             return false;
//         }

//         constexpr index_t array_size = 4;
//         ck::index_t G0               = arg.G0_;
//         ck::index_t G1               = arg.G1_;
//         ck::index_t M                = arg.M_;
//         ck::index_t N                = arg.N_;
//         ck::index_t K                = arg.K_;
//         ck::index_t O                = arg.O_;
//         bool input_permute           = arg.input_permute_;
//         bool output_permute          = arg.output_permute_;

//         std::array<ck::index_t, array_size> a_gs_ms_ks_lengths{G0, G1, M, K};
//         std::array<ck::index_t, array_size> a_gs_ms_ks_strides =
//             input_permute ? std::array<ck::index_t, array_size>{M * G1 * K, K, G1 * K, 1}
//                           // A layout [G0, M, G1, K]
//                           : std::array<ck::index_t, array_size>{
//                                 G1 * M * K, M * K, K, 1}; // A layout [G0, G1, M, K]

//         std::array<ck::index_t, array_size> b0_gs_ns_ks_lengths{G0, G1, N, K};
//         std::array<ck::index_t, array_size> b0_gs_ns_ks_strides =
//             input_permute ? std::array<ck::index_t, array_size>{N * G1 * K, K, G1 * K, 1}
//                           // B0 layout [G0, N, G1, K]
//                           : std::array<ck::index_t, array_size>{
//                                 G1 * N * K, N * K, K, 1}; // B0 layout [G0, G1, N, K]

//         std::array<ck::index_t, array_size> b1_gs_os_ns_lengths{G0, G1, O, N};
//         std::array<ck::index_t, array_size> b1_gs_os_ns_strides =
//             input_permute ? std::array<ck::index_t, array_size>{N * G1 * O, O, 1, G1 * O}
//                           // B1 layout [G0, N, G1, O]
//                           : std::array<ck::index_t, array_size>{
//                                 G1 * N * O, N * O, 1, O}; // B1 layout [G0, G1, N, O]

//         std::array<ck::index_t, array_size> c_gs_ms_os_lengths{G0, G1, M, O};
//         std::array<ck::index_t, array_size> c_gs_ms_os_strides =
//             output_permute ? std::array<ck::index_t, array_size>{M * G1 * O, O, G1 * O, 1}
//                            // C layout [G0, M, G1, O]
//                            : std::array<ck::index_t, array_size>{
//                                  G1 * M * O, M * O, O, 1}; // C layout [G0, G1, M, O]

//         const auto a_grid_desc =
//             DeviceOp::MakeAGridDescriptor(a_gs_ms_ks_lengths, a_gs_ms_ks_strides);
//         const auto b0_grid_desc =
//             DeviceOp::MakeB0GridDescriptor(b0_gs_ns_ks_lengths, b0_gs_ns_ks_strides);
//         const auto b1_grid_desc =
//             DeviceOp::MakeB1GridDescriptor(b1_gs_os_ns_lengths, b1_gs_os_ns_strides);
//         const auto c_grid_desc_m_n =
//             DeviceOp::Transform::MakeCGridDescriptor_M_N(c_gs_ms_os_lengths, c_gs_ms_os_strides);

//         const auto block_2_ctile_map = GridwiseGemm::MakeDefaultBlock2CTileMap(c_grid_desc_m_n, 1, 1);

//         const auto c_grid_desc_g_m_n =
//             DeviceOp::Transform::MakeCGridDescriptor_G_M_N(c_gs_ms_os_lengths, c_gs_ms_os_strides);
//         index_t batch_count = c_grid_desc_g_m_n.GetLength(Number<0>{});

//         if(!GridwiseGemm::CheckValidity(
//                a_grid_desc, b0_grid_desc, b1_grid_desc, c_grid_desc_m_n, block_2_ctile_map))
//         {
//             return false;
//         }

//         // Check if C permute dimension matches GEMM + GEMM shape
//         const index_t c_g = c_grid_desc_g_m_n.GetLength(I0); // unpadded

//         if(!(c_g == batch_count))
//         {
//             printf("DeviceOp: BatchCount err");
//             return false;
//         }

//         // Note: we need raw lengths since threadwise copy can not handle vector load when part of
//         // vector is out of bounds
//         // Note: need lowest dim in Ms/Ns/Ks/Os, not merged M/N/K/O
//         const auto MzRaw = M;
//         const auto LzRaw = N;
//         const auto KzRaw = K;
//         const auto NzRaw = O;

//         // Check scalar per vector requirement
//         const auto a_extent_lowest  = ABlockTransferSrcVectorDim == 2 ? KzRaw : MzRaw;
//         const auto b0_extent_lowest = B0BlockTransferSrcVectorDim == 2 ? KzRaw : LzRaw;
//         const auto b1_extent_lowest = B1BlockTransferSrcVectorDim == 2 ? LzRaw : NzRaw;
//         const auto c_extent_lowest  = NzRaw;

//         if(!(a_extent_lowest % ABlockTransferSrcScalarPerVector == 0 &&
//              b0_extent_lowest % B0BlockTransferSrcScalarPerVector == 0 &&
//              b1_extent_lowest % B1BlockTransferSrcScalarPerVector == 0 &&
//              c_extent_lowest % CShuffleBlockTransferScalarPerVector_NPerBlock == 0))
//         {
//             printf("DeviceOp: Data Transfer Vector scalar err");
//             return false;
//         }

//         std::array<index_t, NumDimG + NumDimM + NumDimN> a_mz_kz_strides_{
//             a_gs_ms_ks_strides[NumDimG + NumDimM - 1],
//             a_gs_ms_ks_strides[NumDimG + NumDimM + NumDimK - 1]};
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b0_lz_kz_strides_{
//             b0_gs_ns_ks_strides[NumDimG + NumDimL - 1],
//             b0_gs_ns_ks_strides[NumDimG + NumDimL + NumDimK - 1]};
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b1_nz_lz_strides_{
//             b1_gs_os_ns_strides[NumDimG + NumDimN - 1],
//             b1_gs_os_ns_strides[NumDimG + NumDimN + NumDimL - 1]};
//         std::array<index_t, NumDimG + NumDimM + NumDimN> c_mz_nz_strides_{
//             c_gs_ms_os_strides[NumDimG + NumDimM - 1],
//             c_gs_ms_os_strides[NumDimG + NumDimM + NumDimN - 1]};

//         // Check vector load/store requirement
//         const auto a_stride_lowest =
//             ABlockTransferSrcVectorDim == 2 ? a_mz_kz_strides_[1] : a_mz_kz_strides_[0];
//         const auto b0_stride_lowest =
//             B0BlockTransferSrcVectorDim == 2 ? b0_lz_kz_strides_[1] : b0_lz_kz_strides_[0];
//         const auto b1_stride_lowest =
//             B1BlockTransferSrcVectorDim == 2 ? b1_nz_lz_strides_[1] : b1_nz_lz_strides_[0];
//         const auto c_stride_lowest = c_mz_nz_strides_[1];

//         if(!(a_stride_lowest == 1 || b0_stride_lowest == 1 || b1_stride_lowest == 1 ||
//              c_stride_lowest == 1))
//         {
//             printf("DeviceOp: Data Vectorize transfer err");
//             return false;
//         }

//         return true;
//     }

//     // polymorphic
//     bool IsSupportedArgument(const BaseArgument* p_arg) override
//     {
//         return IsSupportedArgument(*dynamic_cast<const RawArg*>(p_arg));
//     }

//     // Argument
//     struct Argument : public BaseArgument
//     {
//         Argument(
//             const ADataType* p_a_grid,
//             const B0DataType* p_b0_grid,
//             const B1DataType* p_b1_grid,
//             CDataType* p_c_grid,
//             const std::array<void*, NumAcc0Bias> p_acc0_biases,
//             const std::array<void*, NumAcc1Bias> p_acc1_biases,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& a_gs_ms_ks_lengths,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& a_gs_ms_ks_strides,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& b0_gs_ls_ks_lengths,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& b0_gs_ls_ks_strides,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& b1_gs_ns_ls_lengths,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& b1_gs_ns_ls_strides,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& c_gs_ms_ns_lengths,
//             const std::array<index_t, NumDimG + NumDimM + NumDimN>& c_gs_ms_ns_strides,
//             const std::array<std::vector<ck::index_t>, NumAcc0Bias> acc0_biases_gs_ms_ls_lengths,
//             const std::array<std::vector<ck::index_t>, NumAcc0Bias> acc0_biases_gs_ms_ls_strides,
//             const std::array<std::vector<ck::index_t>, NumAcc1Bias> acc1_biases_gs_ms_ns_lengths,
//             const std::array<std::vector<ck::index_t>, NumAcc1Bias> acc1_biases_gs_ms_ns_strides,
//             const index_t M01,
//             const index_t N01,
//             AElementwiseOperation a_element_op,
//             B0ElementwiseOperation b0_element_op,
//             AccElementwiseOperation acc_element_op,
//             B1ElementwiseOperation b1_element_op,
//             CElementwiseOperation c_element_op)
//             : p_a_grid_{p_a_grid},
//               p_b0_grid_{p_b0_grid},
//               p_b1_grid_{p_b1_grid},
//               p_c_grid_{p_c_grid},
//               a_grid_desc{DeviceOp::MakeAGridDescriptor(a_gs_ms_ks_lengths, a_gs_ms_ks_strides)},
//               b0_grid_desc{
//                   DeviceOp::MakeB0GridDescriptor(b0_gs_ls_ks_lengths, b0_gs_ls_ks_strides)},
//               b1_grid_desc{
//                   DeviceOp::MakeB1GridDescriptor(b1_gs_ns_ls_lengths, b1_gs_ns_ls_strides)},
//               c_grid_desc_m_n_{
//                   Transform::MakeCGridDescriptor_M_N(c_gs_ms_ns_lengths, c_gs_ms_ns_strides)},
//               a_grid_desc_g_m_k_{
//                   Transform::MakeAGridDescriptor_G_M_K(a_gs_ms_ks_lengths, a_gs_ms_ks_strides)},
//               b0_grid_desc_g_l_k_{
//                   Transform::MakeB0GridDescriptor_G_N_K(b0_gs_ls_ks_lengths, b0_gs_ls_ks_strides)},
//               b1_grid_desc_g_n_l_{
//                   Transform::MakeB1GridDescriptor_G_N_K(b1_gs_ns_ls_lengths, b1_gs_ns_ls_strides)},
//               c_grid_desc_g_m_n_{
//                   Transform::MakeCGridDescriptor_G_M_N(c_gs_ms_ns_lengths, c_gs_ms_ns_strides)},
//               c_grid_desc_mblock_mperblock_nblock_nperblock_{},
//               block_2_ctile_map_{GridwiseGemm::MakeDefaultBlock2CTileMap(c_grid_desc_m_n_, M01, N01)},
//               a_element_op_{a_element_op},
//               b0_element_op_{b0_element_op},
//               acc_element_op_{acc_element_op},
//               b1_element_op_{b1_element_op},
//               c_element_op_{c_element_op},
//               c0_matrix_mask_{b0_grid_desc_g_l_k_.GetLength(I1)},
//               raw_lengths_mz_lz_kz_nz_{a_gs_ms_ks_lengths[NumDimG + NumDimM - 1],
//                                        b0_gs_ls_ks_lengths[NumDimG + NumDimL - 1],
//                                        b0_gs_ls_ks_lengths[NumDimG + NumDimL + NumDimK - 1],
//                                        b1_gs_ns_ls_lengths[NumDimG + NumDimN - 1]},
//               a_mz_kz_strides_{a_gs_ms_ks_strides[NumDimG + NumDimM - 1],
//                                a_gs_ms_ks_strides[NumDimG + NumDimM + NumDimK - 1]},
//               b0_lz_kz_strides_{b0_gs_ls_ks_strides[NumDimG + NumDimL - 1],
//                                 b0_gs_ls_ks_strides[NumDimG + NumDimL + NumDimK - 1]},
//               b1_nz_lz_strides_{b1_gs_ns_ls_strides[NumDimG + NumDimN - 1],
//                                 b1_gs_ns_ls_strides[NumDimG + NumDimN + NumDimL - 1]},
//               c_mz_nz_strides_{c_gs_ms_ns_strides[NumDimG + NumDimM - 1],
//                                c_gs_ms_ns_strides[NumDimG + NumDimM + NumDimN - 1]},
//               batch_count_{c_grid_desc_g_m_n_.GetLength(I0)},
//               compute_ptr_offset_of_batch_{
//                   a_grid_desc_g_m_k_, b0_grid_desc_g_l_k_, b1_grid_desc_g_n_l_, c_grid_desc_g_m_n_}
//         {
//             // TODO ANT: implement bias addition
//             ignore = p_acc0_biases;
//             ignore = p_acc1_biases;
//             ignore = acc0_biases_gs_ms_ls_lengths;
//             ignore = acc0_biases_gs_ms_ls_strides;
//             ignore = acc1_biases_gs_ms_ns_lengths;
//             ignore = acc1_biases_gs_ms_ns_strides;

//             if(GridwiseGemm::CheckValidity(
//                    a_grid_desc, b0_grid_desc, b1_grid_desc, c_grid_desc_m_n_, block_2_ctile_map_))
//             {
//                 c_grid_desc_mblock_mperblock_nblock_nperblock_ =
//                     GridwiseGemm::MakeCGridDescriptor_MBlock_MPerBlock_NBlock_NPerBlock(
//                         c_grid_desc_m_n_);
//             }
//         }

//         // Pointers
//         const ADataType* p_a_grid_;
//         const B0DataType* p_b0_grid_;
//         const B1DataType* p_b1_grid_;
//         CDataType* p_c_grid_;

//         // Tensor Descriptors
//         AGridDesc a_grid_desc;
//         B0GridDesc b0_grid_desc;
//         B1GridDesc b1_grid_desc;
//         CGridDesc_M_N c_grid_desc_m_n_;

//         AGridDesc_G_M_K a_grid_desc_g_m_k_;
//         B0GridDesc_G_L_K b0_grid_desc_g_l_k_;
//         B1GridDesc_G_N_L b1_grid_desc_g_n_l_;
//         CGridDesc_G_M_N c_grid_desc_g_m_n_;

//         typename GridwiseGemm::CGridDescriptor_MBlock_MPerBlock_NBlock_NPerBlock
//             c_grid_desc_mblock_mperblock_nblock_nperblock_;

//         // Block to Tile mapping
//         typename GridwiseGemm::DefaultBlock2CTileMap block_2_ctile_map_;

//         // ElementwiseOp
//         AElementwiseOperation a_element_op_;
//         B0ElementwiseOperation b0_element_op_;
//         AccElementwiseOperation acc_element_op_;
//         B1ElementwiseOperation b1_element_op_;
//         CElementwiseOperation c_element_op_;

//         // check C0 masking and padding
//         C0MatrixMask c0_matrix_mask_;

//         // Strides for the last M/N/K dimensions of A/B0/B1/C
//         //   for sanity check of vector load/store
//         std::array<index_t, NumDimG + NumDimM + NumDimN> raw_lengths_mz_lz_kz_nz_;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> a_mz_kz_strides_;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b0_lz_kz_strides_;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b1_nz_lz_strides_;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> c_mz_nz_strides_;

//         index_t batch_count_;
//         // Batch Offset
//         ComputeBasePtrOfStridedBatch compute_ptr_offset_of_batch_;
//     };

//     struct Invoker : public BaseInvoker
//     {
//         using Argument = DeviceOp::RawArg;

//         float Run(const Argument& arg, const StreamConfig& stream_config = StreamConfig{})
//         {
//             const auto M0 = math::integer_divide_ceil(arg.M_, MPerBlock);
//             const auto N0 = math::integer_divide_ceil(arg.O_, NPerBlock);

//             const index_t grid_size = arg.G0_ * arg.G1_ * M0 * N0;
//             const auto K            = arg.K_;
//             // printf("HasKBlockLoop: %d\n", GridwiseGemm::CalculateHasMainKBlockLoop(K));
//             auto launch_kernel = [&](auto has_main_k_block_loop) {
//                 const auto kernel =
//                     kernel_grouped_gemm_softmax_gemm_wmma_cshuffle<DeviceOp,
//                                                                    GridwiseGemm,
//                                                                    ADataType,
//                                                                    B0DataType,
//                                                                    B1DataType,
//                                                                    CDataType,
//                                                                    AElementwiseOperation,
//                                                                    B0ElementwiseOperation,
//                                                                    AccElementwiseOperation,
//                                                                    B1ElementwiseOperation,
//                                                                    CElementwiseOperation,
//                                                                    has_main_k_block_loop>;

//                 return launch_and_time_kernel(stream_config,
//                                               kernel,
//                                               dim3(grid_size),
//                                               dim3(BlockSize),
//                                               0,
//                                               arg.p_a_grid_,
//                                               arg.p_b0_grid_,
//                                               arg.p_b1_grid_,
//                                               arg.p_c_grid_,
//                                               arg.M_,
//                                               arg.N_,
//                                               arg.K_,
//                                               arg.O_,
//                                               arg.G0_,
//                                               arg.G1_,
//                                               arg.alpha_,
//                                               arg.input_permute_,
//                                               arg.output_permute_);
//             };

//             if(GridwiseGemm::CalculateHasMainKBlockLoop(K))
//             {
//                 return launch_kernel(integral_constant<bool, true>{});
//             }
//             else
//             {
//                 return launch_kernel(integral_constant<bool, false>{});
//             }
//         }

//         // polymorphic
//         float Run(const BaseArgument* p_arg,
//                   const StreamConfig& stream_config = StreamConfig{}) override
//         {
//             return Run(*dynamic_cast<const Argument*>(p_arg), stream_config);
//         }
//     };

//     static constexpr bool IsValidCompilationParameter()
//     {
//         // TODO: properly implement this check
//         return true;
//     }
// #if 0
//     static bool IsSupportedArgument(const Argument& arg)
//     {
//         if(ck::get_device_name() == "gfx1100" || ck::get_device_name() == "gfx1101" ||
//            ck::get_device_name() == "gfx1102")
//         {
//             if constexpr(!(is_same_v<Acc0DataType, float> || is_same_v<Acc0DataType, int32_t>))
//             {
//                 printf("DeviceOp: Acc0 Type err");
//                 return false;
//             }

//             if constexpr(!(is_same_v<Acc1DataType, float> || is_same_v<Acc1DataType, int32_t>))
//             {
//                 printf("DeviceOp: Acc1 Type err");
//                 return false;
//             }
//         }
//         else
//         {
//             printf("DeviceOp: Arch err");
//             return false;
//         }

//         if(!GridwiseGemm::CheckValidity(arg.a_grid_desc,
//                                       arg.b0_grid_desc,
//                                       arg.b1_grid_desc,
//                                       arg.c_grid_desc_m_n_,
//                                       arg.block_2_ctile_map_))
//         {
//             return false;
//         }

//         // Check if C permute dimension matches GEMM + GEMM shape
//         const index_t c_g = arg.c_grid_desc_g_m_n_.GetLength(I0); // unpadded

//         if(!(c_g == arg.batch_count_))
//         {
//             printf("DeviceOp: BatchCount err");
//             return false;
//         }

//         // Note: we need raw lengths since threadwise copy can not handle vector load when part of
//         // vector is out of bounds
//         // Note: need lowest dim in Ms/Ns/Ks/Os, not merged M/N/K/O
//         const auto MzRaw = arg.raw_lengths_mz_lz_kz_nz_[0];
//         const auto LzRaw = arg.raw_lengths_mz_lz_kz_nz_[1];
//         const auto KzRaw = arg.raw_lengths_mz_lz_kz_nz_[2];
//         const auto NzRaw = arg.raw_lengths_mz_lz_kz_nz_[3];

//         // Check scalar per vector requirement
//         const auto a_extent_lowest  = ABlockTransferSrcVectorDim == 2 ? KzRaw : MzRaw;
//         const auto b0_extent_lowest = B0BlockTransferSrcVectorDim == 2 ? KzRaw : LzRaw;
//         const auto b1_extent_lowest = B1BlockTransferSrcVectorDim == 2 ? LzRaw : NzRaw;
//         const auto c_extent_lowest  = NzRaw;

//         if(!(a_extent_lowest % ABlockTransferSrcScalarPerVector == 0 &&
//              b0_extent_lowest % B0BlockTransferSrcScalarPerVector == 0 &&
//              b1_extent_lowest % B1BlockTransferSrcScalarPerVector == 0 &&
//              c_extent_lowest % CShuffleBlockTransferScalarPerVector_NPerBlock == 0))
//         {
//             printf("DeviceOp: Data Transfer Vector scalar err");
//             return false;
//         }

//         // Check vector load/store requirement
//         const auto a_stride_lowest =
//             ABlockTransferSrcVectorDim == 2 ? arg.a_mz_kz_strides_[1] : arg.a_mz_kz_strides_[0];
//         const auto b0_stride_lowest =
//             B0BlockTransferSrcVectorDim == 2 ? arg.b0_lz_kz_strides_[1] : arg.b0_lz_kz_strides_[0];
//         const auto b1_stride_lowest =
//             B1BlockTransferSrcVectorDim == 2 ? arg.b1_nz_lz_strides_[1] : arg.b1_nz_lz_strides_[0];
//         const auto c_stride_lowest = arg.c_mz_nz_strides_[1];

//         if(!(a_stride_lowest == 1 || b0_stride_lowest == 1 || b1_stride_lowest == 1 ||
//              c_stride_lowest == 1))
//         {
//             printf("DeviceOp: Data Vectorize transfer err");
//             return false;
//         }

//         return true;
//     }

//     // polymorphic
//     bool IsSupportedArgument(const BaseArgument* p_arg) override
//     {
//         return IsSupportedArgument(*dynamic_cast<const Argument*>(p_arg));
//     }

//     static auto MakeArgument(
//         const ADataType* p_a,
//         const B0DataType* p_b0,
//         const B1DataType* p_b1,
//         CDataType* p_c,
//         const std::array<void*, NumAcc0Bias> p_acc0_biases,
//         const std::array<void*, NumAcc1Bias> p_acc1_biases,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& a_gs_ms_ks_lengths,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& a_gs_ms_ks_strides,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& b0_gs_ls_ks_lengths,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& b0_gs_ls_ks_strides,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& b1_gs_ns_ls_lengths,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& b1_gs_ns_ls_strides,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& c_gs_ms_ns_lengths,
//         const std::array<index_t, NumDimG + NumDimM + NumDimN>& c_gs_ms_ns_strides,
//         const std::array<std::vector<ck::index_t>, NumAcc0Bias> acc0_biases_gs_ms_ls_lengths,
//         const std::array<std::vector<ck::index_t>, NumAcc0Bias> acc0_biases_gs_ms_ls_strides,
//         const std::array<std::vector<ck::index_t>, NumAcc1Bias> acc1_biases_gs_ms_ns_lengths,
//         const std::array<std::vector<ck::index_t>, NumAcc1Bias> acc1_biases_gs_ms_ns_strides,
//         AElementwiseOperation a_element_op,
//         B0ElementwiseOperation b0_element_op,
//         AccElementwiseOperation acc_element_op,
//         B1ElementwiseOperation b1_element_op,
//         CElementwiseOperation c_element_op)
//     {
//         return Argument{p_a,
//                         p_b0,
//                         p_b1,
//                         p_c,
//                         p_acc0_biases,
//                         p_acc1_biases,
//                         a_gs_ms_ks_lengths,
//                         a_gs_ms_ks_strides,
//                         b0_gs_ls_ks_lengths,
//                         b0_gs_ls_ks_strides,
//                         b1_gs_ns_ls_lengths,
//                         b1_gs_ns_ls_strides,
//                         c_gs_ms_ns_lengths,
//                         c_gs_ms_ns_strides,
//                         acc0_biases_gs_ms_ls_lengths,
//                         acc0_biases_gs_ms_ls_strides,
//                         acc1_biases_gs_ms_ns_lengths,
//                         acc1_biases_gs_ms_ns_strides,
//                         1,
//                         1,
//                         a_element_op,
//                         b0_element_op,
//                         acc_element_op,
//                         b1_element_op,
//                         c_element_op};
//     }
// #endif

//     // polymorphic
//     std::unique_ptr<BaseArgument> MakeArgumentPointer(
//         const void* p_a,
//         const void* p_b0,
//         const void* p_b1,
//         void* p_c,
//         const std::array<void*, NumAcc0Bias> p_acc0_biases,
//         const std::array<void*, NumAcc1Bias> p_acc1_biases,
//         const std::vector<index_t>& a_gs_ms_ks_lengths,
//         const std::vector<index_t>& a_gs_ms_ks_strides,
//         const std::vector<index_t>& b0_gs_ls_ks_lengths,
//         const std::vector<index_t>& b0_gs_ls_ks_strides,
//         const std::vector<index_t>& b1_gs_ns_ls_lengths,
//         const std::vector<index_t>& b1_gs_ns_ls_strides,
//         const std::vector<index_t>& c_gs_ms_ns_lengths,
//         const std::vector<index_t>& c_gs_ms_ns_strides,
//         const std::array<std::vector<ck::index_t>, NumAcc0Bias> acc0_biases_gs_ms_ls_lengths,
//         const std::array<std::vector<ck::index_t>, NumAcc0Bias> acc0_biases_gs_ms_ls_strides,
//         const std::array<std::vector<ck::index_t>, NumAcc1Bias> acc1_biases_gs_ms_ns_lengths,
//         const std::array<std::vector<ck::index_t>, NumAcc1Bias> acc1_biases_gs_ms_ns_strides,
//         AElementwiseOperation a_element_op,
//         B0ElementwiseOperation b0_element_op,
//         AccElementwiseOperation acc_element_op,
//         B1ElementwiseOperation b1_element_op,
//         CElementwiseOperation c_element_op) override
//     {
//         std::array<index_t, NumDimG + NumDimM + NumDimN> a_lengths;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> a_strides;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b0_lengths;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b0_strides;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b1_lengths;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> b1_strides;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> c_lengths;
//         std::array<index_t, NumDimG + NumDimM + NumDimN> c_strides;
//         std::transform(a_gs_ms_ks_lengths.begin(),
//                        a_gs_ms_ks_lengths.end(),
//                        a_lengths.begin(),
//                        [](index_t i) { return i; });
//         std::transform(a_gs_ms_ks_strides.begin(),
//                        a_gs_ms_ks_strides.end(),
//                        a_strides.begin(),
//                        [](index_t i) { return i; });
//         std::transform(b0_gs_ls_ks_lengths.begin(),
//                        b0_gs_ls_ks_lengths.end(),
//                        b0_lengths.begin(),
//                        [](index_t i) { return i; });
//         std::transform(b0_gs_ls_ks_strides.begin(),
//                        b0_gs_ls_ks_strides.end(),
//                        b0_strides.begin(),
//                        [](index_t i) { return i; });
//         std::transform(b1_gs_ns_ls_lengths.begin(),
//                        b1_gs_ns_ls_lengths.end(),
//                        b1_lengths.begin(),
//                        [](index_t i) { return i; });
//         std::transform(b1_gs_ns_ls_strides.begin(),
//                        b1_gs_ns_ls_strides.end(),
//                        b1_strides.begin(),
//                        [](index_t i) { return i; });
//         std::transform(c_gs_ms_ns_lengths.begin(),
//                        c_gs_ms_ns_lengths.end(),
//                        c_lengths.begin(),
//                        [](index_t i) { return i; });
//         std::transform(c_gs_ms_ns_strides.begin(),
//                        c_gs_ms_ns_strides.end(),
//                        c_strides.begin(),
//                        [](index_t i) { return i; });
//         return std::make_unique<Argument>(static_cast<const ADataType*>(p_a),
//                                           static_cast<const B0DataType*>(p_b0),
//                                           static_cast<const B1DataType*>(p_b1),
//                                           static_cast<CDataType*>(p_c),
//                                           p_acc0_biases,
//                                           p_acc1_biases,
//                                           a_lengths,
//                                           a_strides,
//                                           b0_lengths,
//                                           b0_strides,
//                                           b1_lengths,
//                                           b1_strides,
//                                           c_lengths,
//                                           c_strides,
//                                           acc0_biases_gs_ms_ls_lengths,
//                                           acc0_biases_gs_ms_ls_strides,
//                                           acc1_biases_gs_ms_ns_lengths,
//                                           acc1_biases_gs_ms_ns_strides,
//                                           1,
//                                           1,
//                                           a_element_op,
//                                           b0_element_op,
//                                           acc_element_op,
//                                           b1_element_op,
//                                           c_element_op);
//     }

//     static auto MakeInvoker() { return Invoker{}; }

//     // polymorphic
//     std::unique_ptr<BaseInvoker> MakeInvokerPointer() override
//     {
//         return std::make_unique<Invoker>(Invoker{});
//     }

//     // polymorphic
//     std::string GetTypeString() const override
//     {
//         auto str = std::stringstream();

//         std::map<LoopScheduler, std::string> LoopSchedToString{
//             {LoopScheduler::Default, "Default"}, {LoopScheduler::Interwave, "Interwave"}};

//         std::map<PipelineVersion, std::string> PipelineVersionToString{{PipelineVersion::v1, "v1"},
//                                                                        {PipelineVersion::v2, "v2"}};

//         // clang-format off
//         str << "DeviceGroupedGemmSoftmaxGemmPermute_Wmma_CShuffle"
//             << "<"
//             << BlockSize << ", "
//             << MPerBlock << ", "
//             << LPerBlock << ", "
//             << KPerBlock << ", "
//             << AK1 << ", "
//             << BK1 << ", "
//             << MPerBlock << ", "
//             << NPerBlock << ", "
//             << LTilePerBlock << ", "
//             << L1 << ", "
//             << getGemmSpecializationString(GemmSpec) << ", "
//             << "ASpec" << getTensorSpecializationString(ASpec) << ", "
//             << "B0Spec" << getTensorSpecializationString(B0Spec) << ", "
//             << "B1Spec" << getTensorSpecializationString(B1Spec) << ", "
//             << "CSpec" << getTensorSpecializationString(CSpec) << ", "
//             << getMaskingSpecializationString(MaskingSpec)
//             << ">"
//             << " AEnableLds: "
//             << AEnableLds << ", "
//             << "B0EnableLds: "
//             << B0EnableLds << ", "
//             << "B1EnableLds: "
//             << B1EnableLds << ", "
//             << "NumPrefetch: "
//             << NumPrefetch << ", "
//             << "LoopScheduler: "
//             << LoopSchedToString[LoopSched] << ", "
//             << "PipelineVersion: "
//             << PipelineVersionToString[PipelineVer];
//         // clang-format on

//         return str.str();
//     }


    // from grouped fused xdl kernel

    using Block2CTileMap = OffsettedBlockToCTileMap<typename GridwiseGemm::DefaultBlock2CTileMap>;

    struct GroupKernelArg
    {
        // pointers
        const ADataType* p_a_grid_;
        const B0DataType* p_b_grid_;
        const B1DataType* p_b1_grid_;
        CDataType* p_c_grid_;

        // tensor descriptors for block/thread-wise copy
        AGridDesc a_grid_desc_ak0_m_ak1_;
        B0GridDesc b_grid_desc_bk0_n_bk1_;
        B1GridDesc b1_grid_desc_bk0_n_bk1_;
        typename GridwiseGemm::CGridDescriptor_MBlock_MPerBlock_NBlock_NPerBlock
            c_grid_desc_mblock_mperblock_nblock_nperblock_;

        // batch & stride
        index_t num_blocks_per_batch_;
        ComputeBasePtrOfStridedBatch compute_base_ptr_of_batch_;

        // check C0 masking and padding
        C0MatrixMask c0_matrix_mask_;

        // block-to-c-tile map
        Block2CTileMap block_2_ctile_map_;

        index_t block_start_, block_end_;
    };

    struct GroupDeviceArg
    {
        // lengths for the last dimensions of overall problem for sanity check of vector load/store
        std::vector<index_t> raw_lengths_mz_nz_kz_gemm1nz_;

        // strides for the last dimensions of each tensor for sanity check of vector load/store
        std::vector<index_t> a_mz_kz_strides_;
        std::vector<index_t> b_nz_kz_strides_;
        std::vector<index_t> b1_nz_kz_strides_;
        std::vector<index_t> c_mz_gemm1nz_strides_;

        // for gridwise gemm check
        CGridDesc_M_N c_grid_desc_m_n_;
    };

    // Argument
    // FIXME: constness
    struct Argument : public BaseArgument
    {
        Argument(std::vector<const void*> p_a_vec,
                 std::vector<const void*> p_b_vec,
                 std::vector<const void*> p_b1_vec,
                 std::vector<void*> p_c_vec,
                 std::vector<std::vector<const void*>> p_acc0_biases_vec,
                 std::vector<std::vector<const void*>> p_acc1_biases_vec,
                 std::vector<ProblemDesc> problem_desc_vec,
                 AElementwiseOperation a_element_op,
                 B0ElementwiseOperation b_element_op,
                 AccElementwiseOperation acc_element_op,
                 B1ElementwiseOperation b1_element_op,
                 CElementwiseOperation c_element_op)
            : a_element_op_{a_element_op},
              b_element_op_{b_element_op},
              acc_element_op_{acc_element_op},
              b1_element_op_{b1_element_op},
              c_element_op_{c_element_op}
        {
            // TODO ANT: implement bias addition
            group_count_ = problem_desc_vec.size();

            if(!(group_count_ == p_a_vec.size() && group_count_ == p_b_vec.size() &&
                 group_count_ == p_b1_vec.size() && group_count_ == p_c_vec.size()))
            {
                throw std::runtime_error("wrong! group_count_ != a/b/b1/c_vec.size");
            }

            if(!(p_acc0_biases_vec.size() == p_acc1_biases_vec.size()))
            {
                throw std::runtime_error("wrong! acc0_bias_vec.size != acc1_bias_vec.size");
            }

            grid_size_ = 0;

            for(std::size_t i = 0; i < group_count_; i++)
            {
                const auto p_a_grid  = static_cast<const ADataType*>(p_a_vec[i]);
                const auto p_b_grid  = static_cast<const B0DataType*>(p_b_vec[i]);
                const auto p_b1_grid = static_cast<const B1DataType*>(p_b1_vec[i]);
                const auto p_c_grid  = static_cast<CDataType*>(p_c_vec[i]);

                const auto& problem_desc = problem_desc_vec[i];

                // copied code to transform vector to sized array

                std::array<index_t, NumDimG + NumDimM + NumDimN> a_lengths;
                std::array<index_t, NumDimG + NumDimM + NumDimN> a_strides;
                std::array<index_t, NumDimG + NumDimM + NumDimN> b0_lengths;
                std::array<index_t, NumDimG + NumDimM + NumDimN> b0_strides;
                std::array<index_t, NumDimG + NumDimM + NumDimN> b1_lengths;
                std::array<index_t, NumDimG + NumDimM + NumDimN> b1_strides;
                std::array<index_t, NumDimG + NumDimM + NumDimN> c_lengths;
                std::array<index_t, NumDimG + NumDimM + NumDimN> c_strides;
                std::transform(problem_desc.a_gs_ms_ks_lengths.begin(),
                            problem_desc.a_gs_ms_ks_lengths.end(),
                            a_lengths.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.a_gs_ms_ks_strides.begin(),
                            problem_desc.a_gs_ms_ks_strides.end(),
                            a_strides.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.b0_gs_ns_ks_lengths.begin(),
                            problem_desc.b0_gs_ns_ks_lengths.end(),
                            b0_lengths.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.b0_gs_ns_ks_strides.begin(),
                            problem_desc.b0_gs_ns_ks_strides.end(),
                            b0_strides.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.b1_gs_os_ns_lengths.begin(),
                            problem_desc.b1_gs_os_ns_lengths.end(),
                            b1_lengths.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.b1_gs_os_ns_strides.begin(),
                            problem_desc.b1_gs_os_ns_strides.end(),
                            b1_strides.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.c_gs_ms_os_lengths.begin(),
                            problem_desc.c_gs_ms_os_lengths.end(),
                            c_lengths.begin(),
                            [](index_t i) { return i; });
                std::transform(problem_desc.c_gs_ms_os_strides.begin(),
                            problem_desc.c_gs_ms_os_strides.end(),
                            c_strides.begin(),
                            [](index_t i) { return i; });

                // const auto a_grid_desc_ak0_m_ak1 = MakeAGridDescriptor(
                //     problem_desc.a_gs_ms_ks_lengths, problem_desc.a_gs_ms_ks_strides);
                // const auto b_grid_desc_bk0_n_bk1 = MakeB0GridDescriptor(
                //     problem_desc.b0_gs_ns_ks_lengths, problem_desc.b0_gs_ns_ks_strides);
                // const auto b1_grid_desc_bk0_n_bk1 = MakeB1GridDescriptor(
                //     problem_desc.b1_gs_os_ns_lengths, problem_desc.b1_gs_os_ns_strides);
                // const auto c_grid_desc_m_n = Transform::MakeCGridDescriptor_M_N(
                //     problem_desc.c_gs_ms_os_lengths, problem_desc.c_gs_ms_os_strides);

                // const auto a_grid_desc_g_m_k = Transform::MakeAGridDescriptor_G_M_K(
                //     problem_desc.a_gs_ms_ks_lengths, problem_desc.a_gs_ms_ks_strides);
                // const auto b_grid_desc_g_n_k = Transform::MakeB0GridDescriptor_G_N_K(
                //     problem_desc.b0_gs_ns_ks_lengths, problem_desc.b0_gs_ns_ks_strides);
                // const auto b1_grid_desc_g_n_k = Transform::MakeB1GridDescriptor_G_N_K(
                //     problem_desc.b1_gs_os_ns_lengths, problem_desc.b1_gs_os_ns_strides);
                // const auto c_grid_desc_g_m_n = Transform::MakeCGridDescriptor_G_M_N(
                //     problem_desc.c_gs_ms_os_lengths, problem_desc.c_gs_ms_os_strides);

                const auto a_grid_desc_ak0_m_ak1 = MakeAGridDescriptor(
                    a_lengths, a_strides);
                const auto b_grid_desc_bk0_n_bk1 = MakeB0GridDescriptor(
                    b0_lengths, b0_strides);
                const auto b1_grid_desc_bk0_n_bk1 = MakeB1GridDescriptor(
                    b1_lengths, b1_strides);
                const auto c_grid_desc_m_n = Transform::MakeCGridDescriptor_M_N(
                    c_lengths, c_strides);

                const auto a_grid_desc_g_m_k = Transform::MakeAGridDescriptor_G_M_K(
                    a_lengths, a_strides);
                const auto b_grid_desc_g_n_k = Transform::MakeB0GridDescriptor_G_N_K(
                    b0_lengths, b0_strides);
                const auto b1_grid_desc_g_n_k = Transform::MakeB1GridDescriptor_G_N_K(
                    b1_lengths, b1_strides);
                const auto c_grid_desc_g_m_n = Transform::MakeCGridDescriptor_G_M_N(
                    c_lengths, c_strides);

                const auto c_grid_desc_mblock_mperblock_nblock_nperblock =
                    GridwiseGemm::MakeCGridDescriptor_MBlock_MPerBlock_NBlock_NPerBlock(
                        c_grid_desc_m_n);

                const index_t BlockStart     = grid_size_;
                const auto block_2_ctile_map = Block2CTileMap(c_grid_desc_m_n, BlockStart);
                const index_t batch_count    = c_grid_desc_g_m_n.GetLength(I0);
                const index_t grid_size_grp =
                    block_2_ctile_map.CalculateGridSize(c_grid_desc_m_n) * batch_count;
                const index_t BlockEnd = grid_size_ + grid_size_grp;

                // batch stride
                const auto compute_base_ptr_of_batch = ComputeBasePtrOfStridedBatch(
                    a_grid_desc_g_m_k, b_grid_desc_g_n_k, b1_grid_desc_g_n_k, c_grid_desc_g_m_n);

                // C0 mask
                const auto c0_matrix_mask = C0MatrixMask(b_grid_desc_g_n_k.GetLength(I1));

                grid_size_ += grid_size_grp;

                // for each group, make sure acc0_biases_gs_ms_ns_lengths.size() == NumAcc0Bias and
                // so on
                if(!(problem_desc.acc0_biases_gs_ms_ns_lengths.size() == NumAcc0Bias &&
                     problem_desc.acc0_biases_gs_ms_ns_strides.size() == NumAcc0Bias &&
                     problem_desc.acc1_biases_gs_ms_os_lengths.size() == NumAcc1Bias &&
                     problem_desc.acc1_biases_gs_ms_os_strides.size() == NumAcc1Bias))
                {
                    throw std::runtime_error(
                        "wrong! number of biases in function argument does not "
                        "match that in template argument");
                }

                group_kernel_args_.push_back({p_a_grid,
                                              p_b_grid,
                                              p_b1_grid,
                                              p_c_grid,
                                              a_grid_desc_ak0_m_ak1,
                                              b_grid_desc_bk0_n_bk1,
                                              b1_grid_desc_bk0_n_bk1,
                                              c_grid_desc_mblock_mperblock_nblock_nperblock,
                                              block_2_ctile_map.CalculateGridSize(c_grid_desc_m_n),
                                              compute_base_ptr_of_batch,
                                              c0_matrix_mask,
                                              block_2_ctile_map,
                                              BlockStart,
                                              BlockEnd});

                group_device_args_.push_back(
                    {{problem_desc.a_gs_ms_ks_lengths[NumDimG + NumDimM - 1],
                      problem_desc.b0_gs_ns_ks_lengths[NumDimG + NumDimL - 1],
                      problem_desc.b0_gs_ns_ks_lengths[NumDimG + NumDimL + NumDimK - 1],
                      problem_desc.b1_gs_os_ns_lengths[NumDimG + NumDimN - 1]},
                     {problem_desc.a_gs_ms_ks_strides[NumDimG + NumDimM - 1],
                      problem_desc.a_gs_ms_ks_strides[NumDimG + NumDimM + NumDimK - 1]},
                     {problem_desc.b0_gs_ns_ks_strides[NumDimG + NumDimL - 1],
                      problem_desc.b0_gs_ns_ks_strides[NumDimG + NumDimL + NumDimK - 1]},
                     {problem_desc.b1_gs_os_ns_strides[NumDimG + NumDimN - 1],
                      problem_desc.b1_gs_os_ns_strides[NumDimG + NumDimN + NumDimL - 1]},
                     {problem_desc.c_gs_ms_os_strides[NumDimG + NumDimM - 1],
                      problem_desc.c_gs_ms_os_strides[NumDimG + NumDimM + NumDimN - 1]},
                     c_grid_desc_m_n});
            }
        }

        std::vector<GroupKernelArg> group_kernel_args_;
        std::vector<GroupDeviceArg> group_device_args_;

        std::size_t group_count_;
        index_t grid_size_;

        AElementwiseOperation a_element_op_;
        B0ElementwiseOperation b_element_op_;
        AccElementwiseOperation acc_element_op_;
        B1ElementwiseOperation b1_element_op_;
        CElementwiseOperation c_element_op_;
    };

    // Invoker
    struct Invoker : public BaseInvoker
    {
        using Argument = DeviceOp::Argument;

        float Run(const Argument& arg, const StreamConfig& stream_config = StreamConfig{})
        {
            if(!DeviceOp::IsSupportedArgument(arg))
            {
                throw std::runtime_error("wrong! unsupported argument");
            }

            bool all_has_main_k_block_loop  = true;
            bool some_has_main_k_block_loop = false;
            for(std::size_t i = 0; i < arg.group_count_; i++)
            {
                const auto K = arg.group_kernel_args_[i].a_grid_desc_ak0_m_ak1_.GetLength(I0) *
                               arg.group_kernel_args_[i].a_grid_desc_ak0_m_ak1_.GetLength(I2);
                const bool y = GridwiseGemm::CalculateHasMainKBlockLoop(K);
                all_has_main_k_block_loop &= y;
                some_has_main_k_block_loop |= y;
            }

            hipGetErrorString(
                hipMemcpyWithStream(arg.p_workspace_,
                                    arg.group_kernel_args_.data(),
                                    arg.group_kernel_args_.size() * sizeof(GroupKernelArg),
                                    hipMemcpyHostToDevice,
                                    stream_config.stream_id_));

            float ave_time = 0;

            auto launch_kernel = [&](auto has_main_k_block_loop_) {
                const auto kernel =
                    kernel_grouped_gemm_softmax_gemm_wmma_cshuffle<DeviceOp,
                                                                     GridwiseGemm,
                                                                     GroupKernelArg,
                                                                     AElementwiseOperation,
                                                                     B0ElementwiseOperation,
                                                                     AccElementwiseOperation,
                                                                     B1ElementwiseOperation,
                                                                     CElementwiseOperation,
                                                                     has_main_k_block_loop_>;

                return launch_and_time_kernel(
                    stream_config,
                    kernel,
                    dim3(arg.grid_size_),
                    dim3(BlockSize),
                    0,
                    cast_pointer_to_constant_address_space(arg.p_workspace_),
                    arg.group_count_,
                    arg.a_element_op_,
                    arg.b_element_op_,
                    arg.acc_element_op_,
                    arg.b1_element_op_,
                    arg.c_element_op_);
            };

            // Gemm1_K is split into Gemm1_K0/K1 where K1 is known at compile time, so we only need
            // to concern Gemm0's loop
            if(all_has_main_k_block_loop)
            {
                ave_time = launch_kernel(integral_constant<bool, true>{});
            }
            else if(!some_has_main_k_block_loop)
            {
                ave_time = launch_kernel(integral_constant<bool, false>{});
            }
            else
            {
                throw std::runtime_error("wrong! all gemm problems have to simultaneously meet "
                                         "has_main_k_block_loop or no_main_k_block_loop");
            }

            return ave_time;
        }

        // polymorphic
        float Run(const BaseArgument* p_arg,
                  const StreamConfig& stream_config = StreamConfig{}) override
        {
            return Run(*dynamic_cast<const Argument*>(p_arg), stream_config);
        }
    };

    static constexpr bool IsValidCompilationParameter()
    {
        // TODO: properly implement this check
        return true;
    }

    // TODO EVS: port to gfx1100

    static bool IsSupportedArgument(const Argument& arg)
    {
        if(!(ck::get_device_name() == "gfx1100"))
        {
            return false;
        }

        // TODO ANT: Check if tensor specialization & strides mismatch

        // bool all_has_main_k_block_loop  = true;
        // bool some_has_main_k_block_loop = false;

        // for(std::size_t i = 0; i < arg.group_count_; i++)
        // {
        //     const auto& kernel_arg = arg.group_kernel_args_[i];
        //     const auto& device_arg = arg.group_device_args_[i];

        //     // Check if C permute dimension matches GEMM + GEMM shape
        //     const index_t c_m       = device_arg.c_grid_desc_m_n_.GetLength(I0);
        //     const index_t c_gemm1n  = device_arg.c_grid_desc_m_n_.GetLength(I1);
        //     const index_t a_m       = kernel_arg.a_grid_desc_ak0_m_ak1_.GetLength(I1);
        //     const index_t b1_gemm1n = kernel_arg.b1_grid_desc_bk0_n_bk1_.GetLength(I1);
        //     if(!(c_m == a_m && c_gemm1n == b1_gemm1n))
        //     {
        //         return false;
        //     }

        //     // Check if having main loop
        //     const auto K = kernel_arg.a_grid_desc_ak0_m_ak1_.GetLength(I0) *
        //                    kernel_arg.a_grid_desc_ak0_m_ak1_.GetLength(I2);
        //     const bool y = GridwiseGemm::CalculateHasMainKBlockLoop(K);
        //     all_has_main_k_block_loop &= y;
        //     some_has_main_k_block_loop |= y;

        //     // Note: we need raw lengths since threadwise copy can not handle vector load when
        //     // part of vector is out of bounds
        //     const auto MzRaw      = device_arg.raw_lengths_mz_nz_kz_gemm1nz_[0];
        //     const auto NzRaw      = device_arg.raw_lengths_mz_nz_kz_gemm1nz_[1];
        //     const auto KzRaw      = device_arg.raw_lengths_mz_nz_kz_gemm1nz_[2];
        //     const auto Gemm1NzRaw = device_arg.raw_lengths_mz_nz_kz_gemm1nz_[3];

        //     // Check scalar per vector requirement
        //     const auto a_extent_lowest  = ABlockTransferSrcVectorDim == 2 ? KzRaw : MzRaw;
        //     const auto b_extent_lowest  = BBlockTransferSrcVectorDim == 2 ? KzRaw : NzRaw;
        //     const auto b1_extent_lowest = B1BlockTransferSrcVectorDim == 2 ? NzRaw : Gemm1NzRaw;
        //     const auto c_extent_lowest  = Gemm1NzRaw;

        //     if(!(a_extent_lowest % ABlockTransferSrcScalarPerVector == 0 &&
        //          b_extent_lowest % BBlockTransferSrcScalarPerVector == 0 &&
        //          b1_extent_lowest % B1BlockTransferSrcScalarPerVector == 0 &&
        //          c_extent_lowest % CShuffleBlockTransferScalarPerVector_NPerBlock == 0))
        //     {
        //         return false;
        //     }

        //     // Check vector load/store requirement
        //     const auto a_stride_lowest = ABlockTransferSrcVectorDim == 2
        //                                      ? device_arg.a_mz_kz_strides_[1]
        //                                      : device_arg.a_mz_kz_strides_[0];
        //     const auto b_stride_lowest = BBlockTransferSrcVectorDim == 2
        //                                      ? device_arg.b_nz_kz_strides_[1]
        //                                      : device_arg.b_nz_kz_strides_[0];
        //     const auto b1_stride_lowest = B1BlockTransferSrcVectorDim == 2
        //                                       ? device_arg.b1_nz_kz_strides_[1]
        //                                       : device_arg.b1_nz_kz_strides_[0];
        //     const auto c_stride_lowest =
        //         device_arg.c_mz_gemm1nz_strides_[1]; // cshuffle assumes lowest dim in Gemm1Ns to be
        //                                              // contiguous

        //     if(!(a_stride_lowest == 1 || b_stride_lowest == 1 || b1_stride_lowest == 1 ||
        //          c_stride_lowest == 1))
        //     {
        //         return false;
        //     }

        //     if(!GridwiseGemm::CheckValidity(kernel_arg.a_grid_desc_ak0_m_ak1_,
        //                                     kernel_arg.b_grid_desc_bk0_n_bk1_,
        //                                     kernel_arg.b1_grid_desc_bk0_n_bk1_,
        //                                     device_arg.c_grid_desc_m_n_,
        //                                     kernel_arg.block_2_ctile_map_))
        //     {
        //         return false;
        //     }
        // }

        // // all gemm problems have to simultaneously meet has_main_k_block_loop or
        // // no_main_k_block_loop
        // if(!(all_has_main_k_block_loop || !some_has_main_k_block_loop))
        // {
        //     return false;
        // }

        return true;
    }

    // polymorphic
    bool IsSupportedArgument(const BaseArgument* p_arg) override
    {
        return IsSupportedArgument(*dynamic_cast<const Argument*>(p_arg));
    }

    static auto MakeArgument(std::vector<const void*> p_a_vec,
                             std::vector<const void*> p_b_vec,
                             std::vector<const void*> p_b1_vec,
                             std::vector<void*> p_c_vec,
                             std::vector<std::vector<const void*>> p_acc0_biases_vec,
                             std::vector<std::vector<const void*>> p_acc1_biases_vec,
                             std::vector<ProblemDesc> problem_desc_vec,
                             AElementwiseOperation a_element_op,
                             B0ElementwiseOperation b_element_op,
                             AccElementwiseOperation acc_element_op,
                             B1ElementwiseOperation b1_element_op,
                             CElementwiseOperation c_element_op)
    {
        return Argument{p_a_vec,
                        p_b_vec,
                        p_b1_vec,
                        p_c_vec,
                        p_acc0_biases_vec,
                        p_acc1_biases_vec,
                        problem_desc_vec,
                        a_element_op,
                        b_element_op,
                        acc_element_op,
                        b1_element_op,
                        c_element_op};
    }

    static auto MakeInvoker() { return Invoker{}; }

    // polymorphic
    std::unique_ptr<BaseArgument>
    MakeArgumentPointer(std::vector<const void*> p_a_vec,
                        std::vector<const void*> p_b_vec,
                        std::vector<const void*> p_b1_vec,
                        std::vector<void*> p_c_vec,
                        std::vector<std::vector<const void*>> p_acc0_biases_vec,
                        std::vector<std::vector<const void*>> p_acc1_biases_vec,
                        std::vector<ProblemDesc> problem_desc_vec,
                        AElementwiseOperation a_element_op,
                        B0ElementwiseOperation b_element_op,
                        AccElementwiseOperation acc_element_op,
                        B1ElementwiseOperation b1_element_op,
                        CElementwiseOperation c_element_op) override
    {
        return std::make_unique<Argument>(p_a_vec,
                                          p_b_vec,
                                          p_b1_vec,
                                          p_c_vec,
                                          p_acc0_biases_vec,
                                          p_acc1_biases_vec,
                                          problem_desc_vec,
                                          a_element_op,
                                          b_element_op,
                                          acc_element_op,
                                          b1_element_op,
                                          c_element_op);
    }

    // polymorphic
    std::unique_ptr<BaseInvoker> MakeInvokerPointer() override
    {
        return std::make_unique<Invoker>(Invoker{});
    }

    // polymorphic
    std::string GetTypeString() const override
    {
        auto str = std::stringstream();

        // clang-format off
        str << "DeviceGroupedGemmSoftmaxGemmPermute_Wmma_CShuffle"
            << "<"
            // TODO EVS: update accordingly
            // << BlockSize << ", "
            // << MPerBlock << ", "
            // << NPerBlock << ", "
            // << KPerBlock << ", "
            // << AK1 << ", "
            // << BK1 << ", "
            // << MPerBlock << ", "
            // << Gemm1NPerBlock << ", "
            // << Gemm1KPerBlock << ", "
            // << B1K1 << ", "
            // << getGemmSpecializationString(GemmSpec) << ", "
            // << "ASpec" << getTensorSpecializationString(ASpec) << ", "
            // << "B0Spec" << getTensorSpecializationString(BSpec) << ", "
            // << "B1Spec" << getTensorSpecializationString(B1Spec) << ", "
            // << "CSpec" << getTensorSpecializationString(CSpec) << ", "
            // << getMaskingSpecializationString(MaskingSpec) << ", "
            // << MPerXDL << ", "
            // << NPerXDL << ", "
            // << MXdlPerWave << ", "
            // << NXdlPerWave << ", "
            // << ABlockTransferSrcScalarPerVector << ", "
            // << BBlockTransferSrcScalarPerVector << ", "
            // << CShuffleMXdlPerWavePerShuffle << ", "
            // << CShuffleNXdlPerWavePerShuffle
            << ">";
        // clang-format on

        return str.str();
    }

    size_t GetWorkSpaceSize(const BaseArgument* p_arg) const override
    {
        return dynamic_cast<const Argument*>(p_arg)->group_count_ * sizeof(GroupKernelArg);
    }
};

} // namespace device
} // namespace tensor_operation
} // namespace ck
