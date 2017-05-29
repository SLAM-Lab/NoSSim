#ifndef _POST_REQUEST_H
#define _POST_REQUEST_H

void init_libcurl();
void cleanup_libcurl();
int post_req(char * data);

#endif
