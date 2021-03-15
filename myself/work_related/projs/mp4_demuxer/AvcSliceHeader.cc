//
// Created by skymelody on 2019/12/14.
//

#include "AvcSliceHeader.h"

AvcSliceHeader::AvcSliceHeader() :
        size(0),
        first_mb_in_slice(0),
        slice_type(0),
        pic_parameter_set_id(0),
        colour_plane_id(0),
        frame_num(0),
        field_pic_flag(0),
        bottom_field_flag(0),
        idr_pic_id(0),
        pic_order_cnt_lsb(0),
        redundant_pic_cnt(0),
        direct_spatial_mv_pred_flag(0),
        num_ref_idx_active_override_flag(0),
        num_ref_idx_l0_active_minus1(0),
        num_ref_idx_l1_active_minus1(0),
        ref_pic_list_reordering_flag_l0(0),
        reordering_of_pic_nums_idc(0),
        abs_diff_pic_num_minus1(0),
        long_term_pic_num(0),
        ref_pic_list_reordering_flag_l1(0),
        luma_log2_weight_denom(0),
        chroma_log2_weight_denom(0),
        cabac_init_idc(0),
        slice_qp_delta(0),
        sp_for_switch_flag(0),
        slice_qs_delta(0),
        disable_deblocking_filter_idc(0),
        slice_alpha_c0_offset_div2(0),
        slice_beta_offset_div2(0),
        slice_group_change_cycle(0),
        no_output_of_prior_pics_flag(0),
        long_term_reference_flag(0),
        difference_of_pic_nums_minus1(0),
        long_term_frame_idx(0),
        max_long_term_frame_idx_plus1(0)
{
    delta_pic_order_cnt[0] = delta_pic_order_cnt[1] = 0;
}
