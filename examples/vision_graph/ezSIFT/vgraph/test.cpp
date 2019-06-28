/*  Copyright (c) 2013, Robert Wang, email: robertwgh (at) gmail.com
    All rights reserved. https://github.com/robertwgh/ezSIFT

    Description:Detect keypoints and extract descriptors from two input images.
                Then, match features from two images using brute-force method.

    Revision history:
        September 15th, 2013: initial version.
        July 2nd, 2018: code refactor.
*/
#include "vgraph_test.h"
#include "profile.h"
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h> 

static inline double now_sec(){
   struct timeval time;
   if (gettimeofday(&time,NULL)) return 0;
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

unsigned char* keypoints_extraction_profile(int frame, int node){

    double t = now_sec();
    start_timer("read_jpg", frame, node, 0);
    char filename[40];
    sprintf(filename, "data/input/Node%d/%d.jpg", node, frame);
    ezsift::Image<unsigned char> img;
    img = read_jpg(filename);
    stop_timer("read_jpg", frame, node, 0);
    std::cout << "read_jpg time is: " << (now_sec() - t)*1000000 << std::endl;

    //ezsift::double_original_image(true);
    t = now_sec();
    start_timer("keypoints_extraction", frame, node, 0);
    std::list<ezsift::SiftKeypoint> kpt_list;
    ezsift::sift_cpu(img, kpt_list, true);
    unsigned char* ret = kp_list_serialization(kpt_list);
    stop_timer("keypoints_extraction", frame, node, 0);
    std::cout << "keypoints_extraction time is: " << (now_sec() - t)*1000000 << std::endl;

    return ret;
}

uint32_t keypoints_matching_profile(unsigned char* kp_data1, unsigned char* kp_data2, int node1, int node2, int frame){
    double t = now_sec();
    start_timer("keypoints_matching", frame, node1, node2);
    std::list<ezsift::SiftKeypoint> kpt_list1, kpt_list2;
    kpt_list1 = kp_list_deserialization(kp_data1);
    kpt_list2 = kp_list_deserialization(kp_data2);
    std::list<ezsift::MatchPair> match_list;
    ezsift::match_keypoints(kpt_list1, kpt_list2, match_list);
    stop_timer("keypoints_matching", frame, node1, node2);
    std::cout << "keypoints_matching time is: " << (now_sec() - t)*1000000 << std::endl;
    //copy profile data between node1->node2 to node2->node1
    copy_to("keypoints_matching", frame, node1, node2, frame, node2, node1); 

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
    std::string output_file =  std::to_string(frame) + "_frame_"  + std::to_string(node1) + "_" + std::to_string(node2) +  ".ppm";
    
    ezsift::draw_match_lines_to_ppm_file(output_file.c_str(), image1,
                                         image2, match_list);
    std::cout << "# of matched keypoints: "
              << static_cast<unsigned int>(match_list.size()) << std::endl;
    return (uint32_t)(match_list.size());
}

int main(int argc, char *argv[])
{    
/*
    double t0 = now_sec();
    unsigned char* kp1 = keypoints_extraction("0.jpg");
    std::cout << "keypoints_extraction 1 duration: " << (now_sec() - t0) << std::endl;
    uint32_t size = get_kp_raw_size(kp1);
    unsigned char* data = get_kp_raw_data(kp1);
    std::cout << "Keypoint size is: "  << size << std::endl;
    t0 = now_sec();
    unsigned char* kp2 = keypoints_extraction("1.jpg");
    std::cout << "keypoints_extraction 2 duration: " << (now_sec() - t0) << std::endl;
    size = get_kp_raw_size(kp2);
    data = get_kp_raw_data(kp2);
    std::cout << "Keypoint size is: "  << size << std::endl;
    t0 = now_sec();
    keypoints_matching(kp1, kp2, "0.jpg", "1.jpg");
    std::cout << "keypoints_matching 1 2 duration: " << (now_sec() - t0) << std::endl;
    free(kp1);
    free(kp2);
    
    t0 = now_sec();
    kp1 = keypoints_extraction("data/input/Node0/0.jpg");
    std::cout << "keypoints_extraction 1 duration: " << (now_sec() - t0) << std::endl;
    t0 = now_sec();
    kp2 = keypoints_extraction("data/input/Node1/0.jpg");
    std::cout << "keypoints_extraction 2 duration: " << (now_sec() - t0) << std::endl;
    size = get_kp_raw_size(kp1);
    std::cout << "Keypoint size is: "  << size << std::endl;
    size = get_kp_raw_size(kp2);
    std::cout << "Keypoint size is: "  << size << std::endl;
    t0 = now_sec();
    keypoints_matching(kp1, kp2, 0, 1, 0);
    std::cout << "keypoints_matching 1 2 duration: " << (now_sec() - t0) << std::endl;
    free(kp1);
    free(kp2);
*/
    //Profile keypoints_extraction
    profile_start();
    for(int frame_num = 0; frame_num < FRAME_NUM; frame_num++){
       for(int device_0 = 0; device_0 < MAX_EDGE_NUM; device_0++) {
          for(int device_1 = device_0 + 1; device_1 < MAX_EDGE_NUM; device_1++) {
             char img_0[40];
             char img_1[40];
             sprintf(img_0, "data/input/Node%d/%d.jpg", device_0, frame_num);
             sprintf(img_1, "data/input/Node%d/%d.jpg", device_1, frame_num);  
             unsigned char* kp1 = keypoints_extraction_profile(frame_num, device_0);      
             unsigned char* kp2 = keypoints_extraction_profile(frame_num, device_1);                
             keypoints_matching_profile(kp1, kp2, device_0, device_1, frame_num);
          }
       }   
    }
    profile_end(0, 1);
    return 0;
}




