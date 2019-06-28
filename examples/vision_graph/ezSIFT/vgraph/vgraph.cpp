#include "vgraph.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ezsift.h"
#include "stb_image.h"
#include <iostream>

ezsift::Image<unsigned char> read_jpg(const char *filename){
    int w, h, c;
    unsigned char * rgb_image = stbi_load(filename, &w, &h, &c, 0);
    if (!rgb_image) {
        fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename, stbi_failure_reason());
        exit(0);
    }
    ezsift::Image<float> img(w, h);
    int i,j,k;
    for(k = 0; k < c; ++k){
        for(j = 0; j < h; ++j){
            for(i = 0; i < w; ++i){
                int dst_index = i + w*j;
                int src_index = k + c*i + c*w*j;
		if(k==0) img.data[dst_index] = 0.2126 * (float)rgb_image[src_index]/255.;
  		if(k==1) img.data[dst_index] += 0.7152 * (float)rgb_image[src_index]/255.;
  		if(k==2) {
                   img.data[dst_index] += 0.0722 * (float)rgb_image[src_index]/255.;
                   img.data[dst_index] = img.data[dst_index]*255;
		}
            }
        }
    }
    free(rgb_image);
    return img.to_uchar(); 
}

unsigned char* kp_list_serialization(std::list<ezsift::SiftKeypoint> kpt_list){
   unsigned char* data = (unsigned char*)malloc(sizeof(ezsift::SiftKeypoint)*kpt_list.size()+4);
   unsigned char* data_index = data;
   uint32_t size = (uint32_t)kpt_list.size();
   memcpy(data_index, &size, sizeof(uint32_t));
   data_index = data_index + 4;
   for(ezsift::SiftKeypoint it : kpt_list){
      memcpy(data_index, (unsigned char*)&it, sizeof(ezsift::SiftKeypoint));
      data_index = data_index + sizeof(ezsift::SiftKeypoint);
   }
   return data;
}

unsigned char* get_kp_raw_data(unsigned char* kp_data){
   unsigned char* data_index = kp_data;
   return data_index;
}

uint32_t get_kp_raw_size(unsigned char* kp_data){
   uint32_t size;
   unsigned char* data_index = kp_data;
   memcpy(&(size), data_index, sizeof(uint32_t));
   return size*sizeof(ezsift::SiftKeypoint)+sizeof(uint32_t);
}

uint32_t get_kp_size(unsigned char* kp_data){
   uint32_t size;
   unsigned char* data_index = kp_data;
   memcpy(&(size), data_index, sizeof(uint32_t));
   return size;
}

uint32_t get_kp_byte_size(unsigned char* kp_data){
   uint32_t size;
   unsigned char* data_index = kp_data;
   memcpy(&(size), data_index, sizeof(uint32_t));
   return size*sizeof(ezsift::SiftKeypoint);
}

unsigned char* get_kp_data(unsigned char* kp_data){
   unsigned char* data_index = kp_data;
   data_index = data_index + 4;
   return data_index;
}

std::list<ezsift::SiftKeypoint> kp_list_deserialization(unsigned char* data){
   uint32_t size = get_kp_byte_size(data)/sizeof(ezsift::SiftKeypoint);
   unsigned char* data_index = get_kp_data(data);
   std::list<ezsift::SiftKeypoint> kp_list;
   for(uint32_t i = 0; i < size; i++){
      ezsift::SiftKeypoint kp; 
      memcpy((unsigned char*)&kp, data_index, sizeof(ezsift::SiftKeypoint));
      data_index = data_index + sizeof(ezsift::SiftKeypoint);
      kp_list.push_back(kp);
   }
   return kp_list;
}

unsigned char* keypoints_extraction(const char* filename){
    ezsift::Image<unsigned char> img;
    img = read_jpg(filename);
    std::list<ezsift::SiftKeypoint> kpt_list;
    //ezsift::double_original_image(true);
    ezsift::sift_cpu(img, kpt_list, true);
    return kp_list_serialization(kpt_list);
}

uint32_t keypoints_matching(unsigned char* kp_data1, unsigned char* kp_data2, const char* file1, const char* file2){
    std::list<ezsift::SiftKeypoint> kpt_list1, kpt_list2;
    kpt_list1 = kp_list_deserialization(kp_data1);
    kpt_list2 = kp_list_deserialization(kp_data2);
    std::list<ezsift::MatchPair> match_list;
    ezsift::match_keypoints(kpt_list1, kpt_list2, match_list);

    // Read two input images
    ezsift::Image<unsigned char> image1, image2;
    image1 = read_jpg(file1);
    image2 = read_jpg(file2);
    
    ezsift::draw_match_lines_to_ppm_file("sift_matching_a_b.ppm", image1,
                                         image2, match_list);
    std::cout << "# of matched keypoints: "
              << static_cast<unsigned int>(match_list.size()) << std::endl;

    return (uint32_t)(match_list.size());
}

uint32_t keypoints_matching(unsigned char* kp_data1, unsigned char* kp_data2, int node1, int node2, int frame){
    std::list<ezsift::SiftKeypoint> kpt_list1, kpt_list2;
    kpt_list1 = kp_list_deserialization(kp_data1);
    kpt_list2 = kp_list_deserialization(kp_data2);
    std::list<ezsift::MatchPair> match_list;
    ezsift::match_keypoints(kpt_list1, kpt_list2, match_list);

    // Read two input images
    ezsift::Image<unsigned char> image1, image2;
    
    std::string img1 = "data/input/Node" + std::to_string(node1) + "/" + std::to_string(frame)+".jpg";
    std::string img2 = "data/input/Node" + std::to_string(node2) + "/" + std::to_string(frame)+".jpg";
   
    image1 = read_jpg(img1.c_str());
    image2 = read_jpg(img2.c_str());

    ezsift::draw_keypoints_to_ppm_file("sift_keypoints_a.ppm", image1,
                                       kpt_list1);
    ezsift::draw_keypoints_to_ppm_file("sift_keypoints_b.ppm", image2,
                                       kpt_list2);
    
    std::string output_file = "sift_matching_" + std::to_string(node1) + "_" + std::to_string(node2) + "_frame_" + std::to_string(frame) + ".ppm";
    
    ezsift::draw_match_lines_to_ppm_file(output_file.c_str(), image1,
                                         image2, match_list);
    std::cout << "# of matched keypoints: "
              << static_cast<unsigned int>(match_list.size()) << std::endl;

    return (uint32_t)(match_list.size());
}

