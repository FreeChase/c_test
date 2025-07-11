#include <stdio.h>
#include <stdint.h>
#include <string.h>
// #include <arpa/inet.h> // 用于字节序转换函数
#include <winsock2.h>


#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN  4

// 以太网帧结构
struct ethernet_frame {
    uint8_t dest_mac[MAC_ADDR_LEN];
    uint8_t src_mac[MAC_ADDR_LEN];
    uint16_t type;
    uint8_t *payload;
    uint16_t payload_size;
};

// IP头部结构
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
};

struct tcp_header {
    uint16_t src_port;           // 源端口
    uint16_t dest_port;          // 目标端口
    uint32_t sequence;           // 序列号
    uint32_t acknowledgment;     // 确认号
    uint8_t  data_offset;        // 数据偏移
    uint8_t  flags;              // 标志
    uint16_t window;             // 窗口大小
    uint16_t checksum;           // 校验和
    uint16_t urgent_pointer;     // 紧急指针
};

struct pseudo_header {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint8_t reserved;
    uint8_t protocol;
    uint16_t tcp_length;
};



uint8_t packet[] = {
        0x18, 0x1d, 0xea, 0x81, 0x30, 0x12, 0x22, 0xa0, 0xf0, 0xe8, 0x40, 0xd5, 0x08, 0x00, 0x45, 0x00,
        0x00, 0x34, 0xec, 0x6f, 0x00, 0x00, 0x35, 0x06, 0x50, 0x69, 0xad, 0xc2, 0xae, 0xbc, 0xc0, 0xa8,
        0x2b, 0xc4, 0x14, 0x6c, 0xc0, 0xdd, 0x7e, 0x99, 0xe3, 0x7a, 0xd6, 0x8a, 0xa6, 0x04, 0x80, 0x10,
        0x01, 0x20, 0x82, 0xa6, 0x00, 0x00, 0x01, 0x01, 0x05, 0x0a, 0xd6, 0x8a, 0xa6, 0x03, 0xd6, 0x8a,
        0xa6, 0x04,
};

uint8_t packet_iphead[] = {
     0x45, 0x00, 0x00, 0x34, 
     0xec, 0x6f, 0x00, 0x00, 
     0x35, 0x06, 0x00, 0x00, 
     0xad, 0xc2, 0xae, 0xbc, 
     0xc0, 0xa8, 0x2b, 0xc4,
};

void parse_tcp_header(const uint8_t *buffer, struct tcp_header *header) {
    header->src_port = ntohs(*(uint16_t *)&buffer[0]);
    header->dest_port = ntohs(*(uint16_t *)&buffer[2]);
    header->sequence = ntohl(*(uint32_t *)&buffer[4]);
    header->acknowledgment = ntohl(*(uint32_t *)&buffer[8]);
    header->data_offset = (buffer[12] >> 4) & 0x0F;
    header->flags = buffer[13];
    header->window = ntohs(*(uint16_t *)&buffer[14]);
    header->checksum = ntohs(*(uint16_t *)&buffer[16]);
    header->urgent_pointer = ntohs(*(uint16_t *)&buffer[18]);
}

// 解析以太网帧
void parse_ethernet_frame(struct ethernet_frame *frame, uint8_t *buffer, uint16_t buffer_size) {
    memcpy(frame->dest_mac, buffer, MAC_ADDR_LEN);
    memcpy(frame->src_mac, buffer + MAC_ADDR_LEN, MAC_ADDR_LEN);
    frame->type = ntohs(*(uint16_t *)(buffer + 12));
    frame->payload = buffer + 14;
    frame->payload_size = buffer_size - 14;
}

// 解析IP头部
void parse_ip_header(const uint8_t *buffer, struct ip_header *header) {
    header->version_and_header_length = buffer[0];
    header->type_of_service = buffer[1];
    header->total_length = ntohs(*(uint16_t *)&buffer[2]);
    header->identification = ntohs(*(uint16_t *)&buffer[4]);
    header->flags_and_fragment_offset = ntohs(*(uint16_t *)&buffer[6]);
    header->ttl = buffer[8];
    header->protocol = buffer[9];
    header->header_checksum = ntohs(*(uint16_t *)&buffer[10]);
    header->src_ip = *(uint32_t *)&buffer[12];
    header->dest_ip = *(uint32_t *)&buffer[16];
}

uint16_t compute_ip_checksum(uint8_t *header, size_t len) {
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *)header;

    for (sum = 0; len > 1; len -= 2) {
        sum += *ptr++;
    }

    if (len > 0) {
        sum += *(uint8_t *)ptr;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)~sum;
}

uint16_t compute_tcp_checksum(uint16_t *addr, int len) {
    uint32_t sum = 0;
    while (len > 1) {
        sum += *addr++;
        len -= sizeof(uint16_t);
    }
    if (len > 0) {
        sum += *(uint8_t *)addr;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return (uint16_t)~sum;
}


uint16_t tcp_checksum(struct ip_header *ip_hdr, struct tcp_header *tcp_hdr, uint8_t *data, int data_len) {
    // 创建伪头部
    struct pseudo_header psh;
    psh.src_addr = ip_hdr->src_ip;
    psh.dst_addr = ip_hdr->dest_ip;
    psh.reserved = 0;
    psh.protocol = ip_hdr->protocol;
    psh.tcp_length = htons(sizeof(struct tcp_header) + data_len);

    int psh_size = sizeof(struct pseudo_header);
    int tcp_header_size = sizeof(struct tcp_header);

    // 估算所需的缓冲区大小
    int total_len = psh_size + tcp_header_size + data_len;
    uint8_t *buffer = (uint8_t *)malloc(total_len);
    printf("tcp psh_size %d\r\n",psh_size);
    printf("tcp tcp_header_size %d\r\n",tcp_header_size);
    printf("tcp total_len %d\r\n",total_len);
    printf("psh.src_addr   0x%x\r\n",psh.src_addr  ); 
    printf("psh.dst_addr   0x%x\r\n",psh.dst_addr  ); 
    printf("psh.reserved   0x%x\r\n",psh.reserved  ); 
    printf("psh.protocol   0x%x\r\n",psh.protocol  ); 
    printf("psh.tcp_length 0x%x\r\n",psh.tcp_length);
    // 组装伪头部、TCP头部和数据到缓冲区
    memcpy(buffer, &psh, psh_size);
    memcpy(buffer + psh_size, tcp_hdr, tcp_header_size);
    memcpy(buffer + psh_size + tcp_header_size, data, data_len);

    // 计算校验和
    uint16_t checksum = compute_tcp_checksum((uint16_t *)buffer, total_len);
    free(buffer);
    return checksum;
}


// 打印IP地址
void print_ip_address(uint32_t ip) {
    printf("%d.%d.%d.%d",
           (ip >> 24) & 0xFF,
           (ip >> 16) & 0xFF,
           (ip >> 8) & 0xFF,
           ip & 0xFF);
}

// 主函数
int main() {
    uint16_t tcpChecksum;
    // 假设这是一个接收到的以太网帧（需要替换为实际数据）
    uint8_t ethernet_frame_buffer[] = {
        // 以太网帧的字节数据
    };
    uint16_t frame_size = sizeof(packet);

    struct ethernet_frame eth_frame;

    printf("tcpip\r\n");
    parse_ethernet_frame(&eth_frame, packet, frame_size);

    printf("eth payload_size %d\r\n",eth_frame.payload_size);
    // 检查类型是否为IP数据包（IPv4）
    if (eth_frame.type == 0x0800) {
        struct ip_header ip_hdr;
        parse_ip_header(eth_frame.payload, &ip_hdr);

        // 打印IP头部信息
        printf("Source IP: ");
        print_ip_address(ntohl(ip_hdr.src_ip));
        printf("\nDestination IP: ");
        print_ip_address(ntohl(ip_hdr.dest_ip));
        printf("\n");
        // 如果协议是TCP（协议号6）
        if (ip_hdr.protocol == 6) {
            struct tcp_header tcp_hdr;
            printf("ver and length: 0x%x\n", ip_hdr.version_and_header_length);
            parse_tcp_header(eth_frame.payload + (ip_hdr.version_and_header_length & 0x0F) * 4, &tcp_hdr);

            // 打印TCP头部信息
            printf("Source Port: %u\n", tcp_hdr.src_port);
            printf("Destination Port: %u\n", tcp_hdr.dest_port);
            printf("sequence: %u\n", tcp_hdr.sequence);
            printf("acknowledgment: %d\n", tcp_hdr.acknowledgment);
            printf("data_offset: %d\n", tcp_hdr.data_offset);
            printf("window: %d\n", tcp_hdr.window);

            // 计算IP头部长度（单位：字节）
            int ip_header_length = (ip_hdr.version_and_header_length & 0x0F) * 4;

            // 计算TCP头部长度（单位：字节）
            int tcp_header_length = (tcp_hdr.data_offset ) * 4;

            // 获取IP总长度（单位：字节）
            // int total_length = ntohs(ip_hdr.total_length);
            int total_length = (ip_hdr.total_length);

            // 计算TCP数据长度（单位：字节）
            int tcp_data_length = total_length - ip_header_length - tcp_header_length;

            printf("ip_header_length: %d\n", ip_header_length);
            printf("tcp_header_length: %d\n", tcp_header_length);
            printf("total_length: %d\n", total_length);
            printf("tcp_data_length: %d\n", tcp_data_length);

            printf("head length 0x%x\r\n",ip_hdr.version_and_header_length);
            // 其他字段...
            printf("check sum1 0x%x\r\n",ip_hdr.header_checksum);
            // 填充ip_hdr的其他字段
            tcpChecksum = tcp_checksum( &ip_hdr, &tcp_hdr, (uint8_t *)eth_frame.payload+40 , eth_frame.payload_size - 40 );
            printf("tcpChecksum 0x%x\r\n", tcpChecksum);

            printf("sizeof(ip_hdr) %d\r\n",sizeof(ip_hdr));
            // 根据packet_iphead，注意 校验位置要填0 计算IP部分校验和
            ip_hdr.header_checksum = compute_ip_checksum((uint8_t *)&packet_iphead, sizeof(packet_iphead));
            printf("check sum2 0x%x\r\n",ip_hdr.header_checksum);
        }
    }

    return 0;
}
