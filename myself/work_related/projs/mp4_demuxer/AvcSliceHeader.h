//
// Created by skymelody on 2019/12/14.
//

#ifndef MP4TOH264_AVCSLICEHEADER_H
#define MP4TOH264_AVCSLICEHEADER_H


struct AvcSliceHeader {

    AvcSliceHeader();


    unsigned int size; // not from the bitstream, this is computed after parsing
    unsigned int first_mb_in_slice;
    unsigned int slice_type;
    unsigned int pic_parameter_set_id;
    unsigned int colour_plane_id;
    unsigned int frame_num;
    unsigned int field_pic_flag;
    unsigned int bottom_field_flag;
    unsigned int idr_pic_id;
    unsigned int pic_order_cnt_lsb;
    int          delta_pic_order_cnt[2];
    unsigned int redundant_pic_cnt;
    unsigned int direct_spatial_mv_pred_flag;
    unsigned int num_ref_idx_active_override_flag;
    unsigned int num_ref_idx_l0_active_minus1;
    unsigned int num_ref_idx_l1_active_minus1;
    unsigned int ref_pic_list_reordering_flag_l0;
    unsigned int reordering_of_pic_nums_idc;
    unsigned int abs_diff_pic_num_minus1;
    unsigned int long_term_pic_num;
    unsigned int ref_pic_list_reordering_flag_l1;
    unsigned int luma_log2_weight_denom;
    unsigned int chroma_log2_weight_denom;
    unsigned int cabac_init_idc;
    unsigned int slice_qp_delta;
    unsigned int sp_for_switch_flag;
    int          slice_qs_delta;
    unsigned int disable_deblocking_filter_idc;
    int          slice_alpha_c0_offset_div2;
    int          slice_beta_offset_div2;
    unsigned int slice_group_change_cycle;
    unsigned int no_output_of_prior_pics_flag;
    unsigned int long_term_reference_flag;
    unsigned int difference_of_pic_nums_minus1;
    unsigned int long_term_frame_idx;
    unsigned int max_long_term_frame_idx_plus1;
};


#endif //MP4TOH264_AVCSLICEHEADER_H
