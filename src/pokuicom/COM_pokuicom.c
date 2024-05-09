#include "COM_pokuicom.h"

#include "stm32469i_discovery.h"

#include "poktocol.h"
#include "./../pokraphic/PKC_segments.h"


UART_HandleTypeDef huart6;

extern PKC_item segmentsMatchScore;
struct poktocol obj;
uint8_t recvd_data;

// Latched data, for request response
enum pokprotocol_team latchedColor = POKTOCOL_TEAM_BLUE;
int latchedMatchStarted = 0;

// Statistics
int statReceivedRequest = 0;

// Global variable status
int COM_pokuicomAlive = 0;

static void PCOM_receive(struct poktocol_msg *msg, void *user_data) {
    switch (msg->cmd) {
        case POKTOCOL_CMD_TYPE_WRITE:
            switch (msg->type) {
                case POKTOCOL_DATA_TYPE_SCORE:
                    PKC_segmentsSetTarget(&segmentsMatchScore, (int)msg->data.score);
                    break;
                case POKTOCOL_DATA_TYPE_TEAM:
                    break;
                case POKTOCOL_DATA_TYPE_MATCH_STARTED:
                    break;
            }
            break;
        case POKTOCOL_CMD_TYPE_REQUEST:
            statReceivedRequest++;
            switch (msg->type) {
                case POKTOCOL_DATA_TYPE_SCORE:
                    break;
                case POKTOCOL_DATA_TYPE_TEAM:
                    PCOM_send_team_color(latchedColor);
                    break;
                case POKTOCOL_DATA_TYPE_MATCH_STARTED:
                    if (latchedMatchStarted) {
                        PCOM_notify_start_of_match();
                    }
                    break;
            }
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

    latchedMatchStarted = 1;

    struct poktocol_msg msg = {
        .cmd = POKTOCOL_CMD_TYPE_WRITE,
        .type = POKTOCOL_DATA_TYPE_MATCH_STARTED
    };

    pokprotocol_send(&obj, &msg);
}

void PCOM_send_team_color(enum pokprotocol_team color) {
    // Latch the color, for request responses
    latchedColor = color;

    struct poktocol_msg msg = {
        .cmd = POKTOCOL_CMD_TYPE_WRITE,
        .type = POKTOCOL_DATA_TYPE_TEAM,
        .data.team = color
    };

    pokprotocol_send(&obj, &msg);
}


int PCOM_init(void) {
    struct poktocol_config cfg = {
        .receive = PCOM_receive,
        .send = PCOM_send_buffer,
        .user_data = NULL
    };
    pokprotocol_init(&obj, &cfg);

    // Init first UART IT
    HAL_UART_Receive_IT(&huart6, &recvd_data, 1);

    return 0;
}


void PCOM_tick() {
    if (statReceivedRequest > 0) {
        COM_pokuicomAlive = 1;
    } else {
        COM_pokuicomAlive = 0;
    }
    statReceivedRequest = 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // TODO: place this interrupt elsewhere, to handle other UART interrupts
    if (huart == &huart6) {
        HAL_UART_Receive_IT(&huart6, &recvd_data, 1);
        pokprotocol_feed_byte(&obj, recvd_data);
    }
}