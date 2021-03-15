//
// Created by skymelody on 2019/12/13.
//

#include "SpsPps.h"
#include "BitStream.h"
#include <cstring>
#include <iterator>
#include <iostream>
using namespace std;

void Unescape(vector<Byte>& data) {
    uint32_t zero_cnt = 0;
    uint32_t byte_removed = 0;
    Byte* out = data.data();
    const Byte* in = data.data();
    int size = data.size();

    for (uint32_t i = 0; i < size; ++i) {
        if (zero_cnt == 2 && in[i] == 3 && i+1 < size && in[i+1] <= 3) {
            ++byte_removed;
            zero_cnt = 0;
        }
        else {
            out[i-byte_removed] = in[i];
            if (in[i] == 0) {
                ++zero_cnt;
            }
            else {
                zero_cnt = 0;
            }
        }
    }
    data.resize(size - byte_removed);
}


uint32_t ReadGolomb(BitStream& bits) {
    uint32_t leadingZero = 0;
    while (bits.ReadBit() == 0) {
        ++leadingZero;
        if (leadingZero > 32) {
            return 0;
        }
        int n = bits.GetBitsRead();
        n = 1;
    }
    if (leadingZero) {
        uint32_t tmp = 0;
        int nbytes = leadingZero / 8;
        for (int i = 0; i < nbytes; ++i) {
            tmp = (tmp << 8) | bits.ReadBits(8);
        }
        int left = leadingZero % 8;
        tmp = (tmp << left) | bits.ReadBits(left);
        int res = (1 << leadingZero) - 1 + tmp;
        int n = bits.GetBitsRead();
        return res;
    }
    else {
        int n = bits.GetBitsRead();
        return 0;
    }
}


int SignedGolomb(uint32_t codeNum) {
    if (codeNum % 2) {
        return (codeNum+1)/2;
    }
    return -((int)codeNum/2);
}

bool ParseSps(const Byte* spsData, const int dataSize ,MuxSps& sps) {
    // insert data to sps struct
    sps.data.insert(sps.data.end(), spsData, spsData+dataSize);

    auto data = sps.data;
    Unescape(data);
    BitStream bits(data);

    bits.SkipBits(8);
    sps.profile_idc = bits.ReadBits(8);
    sps.constraint_set0_flag = bits.ReadBit();
    sps.constraint_set1_flag = bits.ReadBit();
    sps.constraint_set2_flag = bits.ReadBit();
    sps.constraint_set3_flag = bits.ReadBit();
    bits.SkipBits(4);
    sps.level_idc  = bits.ReadBits(8);
    sps.seq_parameter_set_id = ReadGolomb(bits);
    if (sps.seq_parameter_set_id > SPS_MAX_ID) {
        return false;
    }
    if (sps.profile_idc == 100 ||
        sps.profile_idc == 110 ||
        sps.profile_idc == 122 ||
        sps.profile_idc == 244 ||
        sps.profile_idc == 44 ||
        sps.profile_idc == 84 ||
        sps.profile_idc == 86) {
        sps.chroma_format_idc = ReadGolomb(bits);
        sps.separate_colour_plane_flag = 0;
        if (sps.chroma_format_idc == 3) {
            sps.separate_colour_plane_flag = bits.ReadBit();
        }
        sps.bit_depth_luma_minus8 = ReadGolomb(bits);
        sps.bit_depth_chroma_minus8 = ReadGolomb(bits);
        sps.qpprime_y_zero_transform_bypass_flag = bits.ReadBit();
        sps.seq_scaling_matrix_present_flag = bits.ReadBit();
        if (sps.seq_scaling_matrix_present_flag) {
            for (int i = 0; i < (sps.chroma_format_idc != 3 ? 8 : 12); ++i) {
                uint32_t seqScalingListPresentFlag = bits.ReadBit();
                if (seqScalingListPresentFlag) {
                    if (i < 6) {
                        int lastScale = 8;
                        int nextScale = 8;
                        for (uint32_t j = 0; j < 16; ++j) {
                            if (nextScale) {
                                int deltaScale = SignedGolomb(ReadGolomb(bits));
                                nextScale = (lastScale + deltaScale + 256) % 256;
                                sps.use_default_scaling_matrix_4x4[i] = (j == 0 && nextScale == 0);
                            }
                            sps.scaling_list_4x4[i].scale[j] = (nextScale == 0 ? lastScale : nextScale);
                            lastScale = sps.scaling_list_4x4[i].scale[j];
                        }
                    }
                    else {
                        int lastScale = 8;
                        int nextScale = 8;
                        for (uint32_t j = 0; j < 64; ++j) {
                            if (nextScale) {
                                int deltaScale = SignedGolomb(ReadGolomb(bits));
                                nextScale = (lastScale+deltaScale+256)%256;
                                sps.use_default_scaling_matrix_8x8[i-6] = (j == 0 && nextScale == 0);
                            }
                            sps.scaling_list_8x8[i-6].scale[j] = (nextScale == 0 ? lastScale : nextScale);
                            lastScale = sps.scaling_list_8x8[i-6].scale[j];
                        }

                    }
                }
            }
        }
    }
    sps.log2_max_frame_num_minus4 = ReadGolomb(bits);
    sps.pic_order_cnt_type = ReadGolomb(bits);
    if (sps.pic_order_cnt_type > 2) {
        return false;
    }
    if (sps.pic_order_cnt_type == 0) {
        sps.log2_max_pic_order_cnt_lsb_minus4 = ReadGolomb(bits);
    }
    else if (sps.pic_order_cnt_type == 1) {
        sps.delta_pic_order_always_zero_flags = bits.ReadBit();
        sps.offset_for_non_ref_pic = SignedGolomb(ReadGolomb(bits));
        sps.offset_for_top_to_bottom_field = SignedGolomb(ReadGolomb(bits));
        sps.num_ref_frames_in_pic_order_cnt_cycle = ReadGolomb(bits);
        if (sps.num_ref_frames_in_pic_order_cnt_cycle > SPS_MAX_NUM_REF_FRAMES_IN_PIC_ORDER_CNT_CYCLE) {
            return false;
        }
        for (uint32_t i = 0; i < sps.num_ref_frames_in_pic_order_cnt_cycle; ++i) {
            sps.offset_for_ref_frame[i] = SignedGolomb(ReadGolomb(bits));
        }
    }
    sps.num_ref_frames = ReadGolomb(bits);
    sps.gaps_in_frame_num_value_allowed_flag = bits.ReadBit();
    sps.pic_width_in_mbs_minus1 = ReadGolomb(bits);
    sps.pic_height_in_map_units_minus1 = ReadGolomb(bits);
    sps.frame_mbs_only_flag = bits.ReadBit();
    if (!sps.frame_mbs_only_flag) {
        sps.mb_adaptive_frame_field_flag = bits.ReadBit();
    }
    sps.direct_8x8_inference_flag = bits.ReadBit();
    sps.frame_cropping_flag = bits.ReadBit();
    if (sps.frame_cropping_flag) {
        sps.frame_crop_left_offset = ReadGolomb(bits);
        sps.frame_crop_right_offset = ReadGolomb(bits);
        sps.frame_crop_top_offset = ReadGolomb(bits);
        sps.frame_crop_bottom_offset = ReadGolomb(bits);
    }


    return true;
}


bool ParsePps(const Byte *ppsData, int dataSize, MuxPps &pps) {
    pps.data.insert(pps.data.end(), ppsData, ppsData+dataSize);
    auto data = pps.data;
    Unescape(data);
    BitStream bits(data);

    bits.SkipBits(8);

    pps.pic_parameter_set_id = ReadGolomb(bits);
    if (pps.pic_parameter_set_id> PPS_MAX_ID) {
        return false;
    }
    pps.seq_parameter_set_id = ReadGolomb(bits);
    if (pps.seq_parameter_set_id > PPS_MAX_ID) {
        return false;
    }
    pps.entropy_coding_mode_flag = bits.ReadBit();
    pps.pic_order_present_flag = bits.ReadBit();
    pps.num_slice_groups_minus1 = ReadGolomb(bits);
    if (pps.num_slice_groups_minus1 >= PPS_MAX_SLICE_GROUPS) {
        return false;
    }
    if (pps.num_slice_groups_minus1 > 0) {
        pps.slice_group_map_type = ReadGolomb(bits);
        if (pps.slice_group_map_type == 0) {
            for (uint32_t i = 0; i < pps.num_slice_groups_minus1; ++i) {
                pps.run_length_minus1[i] = ReadGolomb(bits);
            }
        }
        else if (pps.slice_group_map_type == 2) {
            for (uint32_t i = 0; i < pps.num_slice_groups_minus1; ++i) {
                pps.top_left[i] = ReadGolomb(bits);
                pps.bottom_right[i] = ReadGolomb(bits);
            }
        }
        else if (pps.slice_group_map_type == 3 ||
                 pps.slice_group_map_type == 4 ||
                 pps.slice_group_map_type == 5) {
            pps.slice_group_change_direction_flag = bits.ReadBit();
            pps.slice_group_change_rate_minus1 = ReadGolomb(bits);
        }
        else if (pps.slice_group_map_type == 6) {
            pps.pic_size_in_map_units_minus1 = ReadGolomb(bits);
            if (pps.pic_size_in_map_units_minus1 >= MAX_PIC_SIZE_IN_MAP_UNITS) {
                return false;
            }
            uint32_t numBitsPerSliceGroupId;
            if (pps.num_slice_groups_minus1+1 > 4) {
                numBitsPerSliceGroupId = 3;
            }
            else if (pps.num_slice_groups_minus1+1 > 2) {
                numBitsPerSliceGroupId = 2;
            }
            else {
                numBitsPerSliceGroupId = 1;
            }
            for (uint32_t i = 0; i <= pps.pic_size_in_map_units_minus1; ++i) {
                bits.ReadBits(numBitsPerSliceGroupId);
            }
        }
    }
    pps.num_ref_idx_10_active_minus1 = ReadGolomb(bits);
    pps.num_ref_idx_11_active_minus1 = ReadGolomb(bits);
    pps.weighted_pred_flag           = bits.ReadBit();
    pps.weighted_bipred_idc          = bits.ReadBits(2);
    pps.pic_init_qp_minus26          = SignedGolomb(ReadGolomb(bits));
    pps.pic_init_qs_minus26          = SignedGolomb(ReadGolomb(bits));
    pps.chroma_qp_index_offset       = SignedGolomb(ReadGolomb(bits));
    pps.deblocking_filter_control_present_flag = bits.ReadBit();
    pps.constrained_intra_pred_flag            = bits.ReadBit();
    pps.redundant_pic_cnt_present_flag         = bits.ReadBit();
    return true;
}

MuxSps::MuxSps() :
        profile_idc(0),
        constraint_set0_flag(0),
        constraint_set1_flag(0),
        constraint_set2_flag(0),
        constraint_set3_flag(0),
        level_idc(0),
        seq_parameter_set_id(0),
        chroma_format_idc(1),
        separate_colour_plane_flag(0),
        bit_depth_luma_minus8(0),
        bit_depth_chroma_minus8(0),
        qpprime_y_zero_transform_bypass_flag(0),
        seq_scaling_matrix_present_flag(0),
        log2_max_frame_num_minus4(0),
        pic_order_cnt_type(0),
        log2_max_pic_order_cnt_lsb_minus4(0),
        delta_pic_order_always_zero_flags(0),
        offset_for_non_ref_pic(0),
        offset_for_top_to_bottom_field(0),
        num_ref_frames_in_pic_order_cnt_cycle(0),
        num_ref_frames(0),
        gaps_in_frame_num_value_allowed_flag(0),
        pic_width_in_mbs_minus1(0),
        pic_height_in_map_units_minus1(0),
        frame_mbs_only_flag(0),
        mb_adaptive_frame_field_flag(0),
        direct_8x8_inference_flag(0),
        frame_cropping_flag(0),
        frame_crop_left_offset(0),
        frame_crop_right_offset(0),
        frame_crop_top_offset(0),
        frame_crop_bottom_offset(0)
{
    memset(scaling_list_4x4, 0, sizeof(scaling_list_4x4));
    memset(use_default_scaling_matrix_4x4, 0, sizeof(use_default_scaling_matrix_4x4));
    memset(scaling_list_8x8, 0, sizeof(scaling_list_8x8));
    memset(use_default_scaling_matrix_8x8, 0, sizeof(use_default_scaling_matrix_8x8));
    memset(offset_for_ref_frame, 0, sizeof(offset_for_ref_frame));
}

void MuxSps::GetWidthAndHeight(uint32_t &width, uint32_t &height) const {
    width = (pic_width_in_mbs_minus1+1)*16;
    height = (2-frame_mbs_only_flag)*(pic_height_in_map_units_minus1+1)*16;

    if (frame_cropping_flag) {
        uint32_t cropH = 2*(frame_crop_left_offset+frame_crop_right_offset);
        uint32_t cropV = 2*(frame_crop_top_offset+frame_crop_bottom_offset)*(2-frame_mbs_only_flag);
        if (cropH < width) {
            width -= cropH;
        }
        if (cropV < height) {
            height -= cropV;
        }
    }
}


MuxPps::MuxPps() :
        pic_parameter_set_id(0),
        seq_parameter_set_id(0),
        entropy_coding_mode_flag(0),
        pic_order_present_flag(0),
        num_slice_groups_minus1(0),
        slice_group_map_type(0),
        slice_group_change_direction_flag(0),
        slice_group_change_rate_minus1(0),
        pic_size_in_map_units_minus1(0),
        num_ref_idx_10_active_minus1(0),
        num_ref_idx_11_active_minus1(0),
        weighted_pred_flag(0),
        weighted_bipred_idc(0),
        pic_init_qp_minus26(0),
        pic_init_qs_minus26(0),
        chroma_qp_index_offset(0),
        deblocking_filter_control_present_flag(0),
        constrained_intra_pred_flag(0),
        redundant_pic_cnt_present_flag(0)
{
    memset(run_length_minus1, 0, sizeof(run_length_minus1));
    memset(top_left, 0, sizeof(top_left));
    memset(bottom_right, 0, sizeof(bottom_right));
}

