#include <stdint.h>
#include "ezsift.h"
unsigned char* get_kp_raw_data(unsigned char* kp_data);
uint32_t get_kp_raw_size(unsigned char* kp_data);
unsigned char* keypoints_extraction(const char* filename);
uint32_t keypoints_matching(unsigned char* kp_data1, unsigned char* kp_data2, const char* file1, const char* file2);
uint32_t keypoints_matching(unsigned char* kp_data1, unsigned char* kp_data2, int node1, int node2, int frame);
ezsift::Image<unsigned char> read_jpg(const char *filename);
unsigned char* kp_list_serialization(std::list<ezsift::SiftKeypoint> kpt_list);
std::list<ezsift::SiftKeypoint> kp_list_deserialization(unsigned char* data);

