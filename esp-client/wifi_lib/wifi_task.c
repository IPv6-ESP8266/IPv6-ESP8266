
#include "lwip/sockets.h"
#include "lwip/lwip/ip_addr.h"
#include "lwip/lwip/netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "espressif/c_types.h"

#define zero(S) memset(&S, 0, sizeof(S))

LOCAL xTaskHandle CLIENT_TASK_HANDLE;
const char *TARGET_IPV6 = "2001:410:0:1:2c2a:d77c:35f0:bdd9";


LOCAL void client_task(void *arg) {

    vTaskDelay(2000);
    (void)arg; // unused

    int result;
    struct ip6_addr target_ipv6_addr;

    // should intialize `target_ipv6_addr`
    result = ip6addr_aton(TARGET_IPV6, &target_ipv6_addr);
    if (result == 0) {
        os_printf("Invalid ipv6 address\n");
        goto failure;
    }

    int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        os_printf("socket creation failed\n");
        goto failure;
    }

    os_printf("socket created\n");

    // prepare an `struct sockaddr_in6` connection to port 8080
    struct sockaddr_in6 sock_addr;
    zero(sock_addr);
    sock_addr.sin6_family = AF_INET6;

    struct in6_addr *ref_s6_addr = &sock_addr.sin6_addr;
    struct ip6_addr *ref_target_ipv6_addr = &target_ipv6_addr;
    inet6_addr_from_ip6addr(ref_s6_addr, ref_target_ipv6_addr);

    sock_addr.sin6_port = htons(8080);


    int retry = 0;

    result = connect(socket_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    while (result != 0) {
        if (result != 0) {
            os_printf("connected failed with %d\n", result);
            vTaskDelay(100);

            if (retry > 5) {
                goto failure;
            }
        }
        retry += 1;
    }
    os_printf("connected \n");

    char msg[] = "abc\n";

    result = send(socket_fd, msg, strlen(msg), 0);
    if (result == -1) {
        os_printf("Send failed\n");
    }


failure:
    vTaskDelete(NULL);
    return;
}

void user_task() {
    int ret;
    const signed char *const task_name = (signed char *)"client task";

    ret = xTaskCreate(client_task, task_name, 1024, NULL, 6, &CLIENT_TASK_HANDLE);
    if (ret != pdPASS) {
        os_printf("unable to start task client task");
        return;
    }
}

