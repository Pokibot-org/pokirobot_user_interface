#include "COM_pokuicom.h"

#include "stm32469i_discovery.h"

#include "poktocol.h"
#include "./../pokraphic/PKC_segments.h"


UART_HandleTypeDef huart6;

extern PKC_item segmentsMatchScore;

struct poktocol obj;
bool has_color_info = false;
enum pokprotocol_team received_color;

uint8_t recvd_data;

static void PCOM_receive(struct poktocol_msg *msg, void *user_data) {
    switch (msg->type) {
        case POKTOCOL_DATA_TYPE_SCORE:
            PKC_segmentsSetTarget(&segmentsMatchScore, (int)msg->data.score);
            break;
        case POKTOCOL_DATA_TYPE_TEAM:
            break;
        case POKTOCOL_DATA_TYPE_MATCH_STARTED:
            break;
    }
}

static void PCOM_send_buffer(char *buffer, size_t len, void *user_data) {
    HAL_UART_Transmit_DMA(&huart6, (uint8_t*)buffer, len);
    // HAL_UART_Transmit(&huart6, "TRUC\r\n", 6, 100);
    printf("TX: ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\r\n");
}

void PCOM_send_score(uint8_t score) {
    struct poktocol_msg msg = {
        .cmd = POKTOCOL_CMD_TYPE_WRITE,
        .type = POKTOCOL_DATA_TYPE_SCORE,
        .data.score = score
    };

    pokprotocol_send(&obj, &msg);
}

void PCOM_notify_start_of_match() {
    struct poktocol_msg msg = {
        .cmd = POKTOCOL_CMD_TYPE_WRITE,
        .type = POKTOCOL_DATA_TYPE_MATCH_STARTED
    };

    pokprotocol_send(&obj, &msg);
}

void PCOM_send_team_color(enum pokprotocol_team color) {
    struct poktocol_msg msg = {
        .cmd = POKTOCOL_CMD_TYPE_WRITE,
        .type = POKTOCOL_DATA_TYPE_TEAM,
        .data.team = color
    };

    pokprotocol_send(&obj, &msg);
}

int charR = 0;

int PCOM_init(void) {
    struct poktocol_config cfg = {
        .receive = PCOM_receive,
        .send = PCOM_send_buffer,
        .user_data = NULL
    };
    pokprotocol_init(&obj, &cfg);

    printf("Start IT\r\n");
    HAL_UART_Receive_IT(&huart6, &recvd_data, 1);

    //HAL_UART_Transmit_DMA(&huart6, "TRUC\r\n", 6);

    // HAL_Delay(500);
    // printf("NBR: %d\r\n", charR);


    return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart6) {
        HAL_UART_Receive_IT(&huart6, &recvd_data, 1);
        pokprotocol_feed_byte(&obj, recvd_data);
        // printf("R%02X\r\n", recvd_data);
        // printf("%c", recvd_data);
        charR++;
    }
}