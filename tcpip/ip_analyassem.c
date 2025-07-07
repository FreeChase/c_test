#include <stdio.h>
#include <stdint.h>
#include <string.h>
// #include <arpa/inet.h> // 在Windows中使用<winsock2.h>
#include <winsock2.h>

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN  4
#define ETHERNET_TYPE_IPv4 0x0800
#define MAX_PAYLOAD_SIZE 1500

struct ethernet_frame {
    uint8_t dest_mac[MAC_ADDR_LEN];
    uint8_t src_mac[MAC_ADDR_LEN];
    uint16_t type;
    uint8_t payload[MAX_PAYLOAD_SIZE];
};

struct ip_header {
    uint8_t version_and_header_length;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_and_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t src_ip;
    uint32_t dest_ip;
    // 注意：实际的IP头部可能包含选项字段，这里未包括
};

struct tcp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t sequence;
    uint32_t acknowledgment;
    uint8_t  data_offset;
    uint8_t  flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;
    // 注意：实际的TCP头部可能包含选项字段，这里未包括
};
void assemble_ip_header(struct ip_header *ip_hdr, uint32_t src_ip, uint32_t dest_ip, uint16_t total_length, uint8_t protocol) {
    ip_hdr->version_and_header_length = 0x45; // IPv4 和头部长度5（20字节）
    ip_hdr->type_of_service = 0;
    ip_hdr->total_length = htons(total_length);
    ip_hdr->identification = 0;
    ip_hdr->flags_and_fragment_offset = 0;
    ip_hdr->ttl = 64; // 默认TTL
    ip_hdr->protocol = protocol;
    ip_hdr->header_checksum = 0; // 初始设置为0，稍后计算
    ip_hdr->src_ip = src_ip;
    ip_hdr->dest_ip = dest_ip;
    // 计算IP头部校验和（留给读者实现）
}

void assemble_tcp_header(struct tcp_header *tcp_hdr, uint16_t src_port, uint16_t dest_port, uint32_t sequence, uint32_t acknowledgment, uint16_t window) {
    tcp_hdr->src_port = htons(src_port);
    tcp_hdr->dest_port = htons(dest_port);
    tcp_hdr->sequence = htonl(sequence);
    tcp_hdr->acknowledgment = htonl(acknowledgment);
    tcp_hdr->data_offset = 0x50; // TCP头部长度5（20字节）
    tcp_hdr->flags = 0x18; // PSH 和 ACK 标志
    tcp_hdr->window = htons(window);
    tcp_hdr->checksum = 0; // 初始设置为0，稍后计算
    tcp_hdr->urgent_pointer = 0;
    // 计算TCP头部校验和（留给读者实现）
}
void assemble_ethernet_frame(struct ethernet_frame *frame, const uint8_t *dest_mac, const uint8_t *src_mac, uint16_t type, const uint8_t *payload, size_t payload_size) {
    memcpy(frame->dest_mac, dest_mac, MAC_ADDR_LEN);
    memcpy(frame->src_mac, src_mac, MAC_ADDR_LEN);
    frame->type = htons(type);
    memcpy(frame->payload, payload, payload_size);
}




int main() {
    int i;
    struct ethernet_frame eth_frame;
    uint8_t dest_mac[MAC_ADDR_LEN] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t src_mac[MAC_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

    // 构建IP头部和TCP头部
    struct ip_header ip_hdr;
    struct tcp_header tcp_hdr;
    uint8_t tcp_data[] = {0x12, 0x34, 0x56, 0x78}; // TCP数据

    size_t ip_total_length = sizeof(struct ip_header) + sizeof(struct tcp_header) + sizeof(tcp_data);
    assemble_ip_header(&ip_hdr, inet_addr("192.168.1.2"), inet_addr("192.168.1.3"), ip_total_length, IPPROTO_TCP);
    assemble_tcp_header(&tcp_hdr, 12345, 80, 0, 0, 8192);

    // 将IP头部、TCP头部和TCP数据放入以太网帧的有效载荷中
    memcpy(eth_frame.payload, &ip_hdr, sizeof(ip_hdr));
    memcpy(eth_frame.payload + sizeof(ip_hdr), &tcp_hdr, sizeof(tcp_hdr));
    memcpy(eth_frame.payload + sizeof(ip_hdr) + sizeof(tcp_hdr), tcp_data, sizeof(tcp_data));


    // 组装以太网帧
    assemble_ethernet_frame(&eth_frame, dest_mac, src_mac, ETHERNET_TYPE_IPv4, eth_frame.payload, ip_total_length);

    for (i = 0; i < 14 + ip_total_length; i++)
    {
        if(i%16 == 0)
            printf("\r\n");
        printf("0x%02x ", *((char*)&eth_frame + i) & 0xff);
    }
    
    // ... 在此处可以发送eth_frame或进一步处理 ...

    return 0;
}

