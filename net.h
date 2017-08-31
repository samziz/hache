#ifndef NET_H
#define NET_H

void conn_handler(void *arg);
void listen_and_write(int port);
void write_sock_data_to_db(void *ptr);

#endif