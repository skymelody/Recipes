//
// Created by skymelody on 2019/12/13.
//

#ifndef MP4TOH264_SPSPPS_H
#define MP4TOH264_SPSPPS_H

#include <vector>
#include <cstdint>
#include "BitStream.h"
using Byte = unsigned char;

const int SPS_MAX_SCALING_LIST_COUNT = 12;
const int SPS_MAX_NUM_REF_FRAMES_IN_PIC_ORDER_CNT_CYCLE = 256;
const int SPS_MAX_ID = 255;
const int PPS_MAX_ID = 255;
const int PPS_MAX_SLICE_GROUPS = 256;
const int MAX_PIC_SIZE_IN_MAP_UNITS = 65536;

struct ScalingList4x4 {
    int scale[16];
};

struct ScalingList8x8 {
    int scale[64];
};



struct MuxSps {
    MuxSps();
    uint32_t profile_idc;
    uint32_t constraint_set0_flag;
    uint32_t constraint_set1_flag;
    uint32_t constraint_set2_flag;
    uint32_t constraint_set3_flag;
    uint32_t level_idc;
    uint32_t seq_parameter_set_id;
    uint32_t chroma_format_idc;
    uint32_t separate_colour_plane_flag;
    uint32_t bit_depth_luma_minus8;
    uint32_t bit_depth_chroma_minus8;
    uint32_t qpprime_y_zero_transform_bypass_flag;
    uint32_t seq_scaling_matrix_present_flag;
    ScalingList4x4 scaling_list_4x4[6];
    bool                  use_default_scaling_matrix_4x4[SPS_MAX_SCALING_LIST_COUNT];
    ScalingList8x8 scaling_list_8x8[6];
    Byte use_default_scaling_matrix_8x8[SPS_MAX_SCALING_LIST_COUNT];
    uint32_t log2_max_frame_num_minus4;
    uint32_t pic_order_cnt_type;
    uint32_t log2_max_pic_order_cnt_lsb_minus4;
    uint32_t delta_pic_order_always_zero_flags;
    int          offset_for_non_ref_pic;
    int          offset_for_top_to_bottom_field;
    uint32_t num_ref_frames_in_pic_order_cnt_cycle;
    uint32_t offset_for_ref_frame[SPS_MAX_NUM_REF_FRAMES_IN_PIC_ORDER_CNT_CYCLE];
    uint32_t num_ref_frames;
    uint32_t gaps_in_frame_num_value_allowed_flag;
    uint32_t pic_width_in_mbs_minus1;
    uint32_t pic_height_in_map_units_minus1;
    uint32_t frame_mbs_only_flag;
    uint32_t mb_adaptive_frame_field_flag;
    uint32_t direct_8x8_inference_flag;
    uint32_t frame_cropping_flag;
    uint32_t frame_crop_left_offset;
    uint32_t frame_crop_right_offset;
    uint32_t frame_crop_top_offset;
    uint32_t frame_crop_bottom_offset;

    std::vector<Byte> data;

    void GetWidthAndHeight(uint32_t& width, uint32_t& height) const;
};

struct MuxPps {
    MuxPps();

    unsigned int pic_parameter_set_id;
    unsigned int seq_parameter_set_id;
    unsigned int entropy_coding_mode_flag;
    unsigned int pic_order_present_flag;
    unsigned int num_slice_groups_minus1;
    unsigned int slice_group_map_type;
    unsigned int run_length_minus1[PPS_MAX_SLICE_GROUPS];
    unsigned int top_left[PPS_MAX_SLICE_GROUPS];
    unsigned int bottom_right[PPS_MAX_SLICE_GROUPS];
    unsigned int slice_group_change_direction_flag;
    unsigned int slice_group_change_rate_minus1;
    unsigned int pic_size_in_map_units_minus1;
    unsigned int num_ref_idx_10_active_minus1;
    unsigned int num_ref_idx_11_active_minus1;
    unsigned int weighted_pred_flag;
    unsigned int weighted_bipred_idc;
    int          pic_init_qp_minus26;
    int          pic_init_qs_minus26;
    int          chroma_qp_index_offset;
    unsigned int deblocking_filter_control_present_flag;
    unsigned int constrained_intra_pred_flag;
    unsigned int redundant_pic_cnt_present_flag;
    std::vector<Byte> data;
};

bool ParseSps(const Byte* spsData, int dataSize ,MuxSps& sps);
bool ParsePps(const Byte* ppsData, int dataSize, MuxPps& pps);
void Unescape(std::vector<Byte>& data);
uint32_t ReadGolomb(BitStream& bits);
int SignedGolomb(uint32_t codeNum);


#endif //MP4TOH264_SPSPPS_H
