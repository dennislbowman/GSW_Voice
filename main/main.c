#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "esp_dsp.h"
#include "bsp_board_extra.h"

#include "esp_wn_iface.h"
#include "esp_wn_models.h"

#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "model_path.h"
#include "esp_process_sdkconfig.h"
#include "axp2101.h"

#include "esp_afe_sr_iface.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
extern lv_obj_t *label_3;
char tmp[100];
#include "esp_mn_speech_commands.h"
#include <sys/socket.h>
#include <netdb.h>
#include "esp_timer.h"
extern int sock;
extern struct sockaddr_in dest_addr;
extern void display_setup(void);
extern void wifi_init_sta();
extern bool connectf;
extern bool wififail;
extern bool connected;
extern void udpinit();

extern lv_obj_t *label_2;
extern lv_obj_t *label_1;
extern lv_obj_t *label_4;
extern lv_obj_t *label_5;
extern lv_obj_t *label_6;
extern lv_obj_t *label_7;
extern lv_obj_t *label_8;
extern lv_obj_t *imgdir;
#include "c_library_v2/common/mavlink.h"

const unsigned int localPort = 14555; // Local listening port
const uint8_t GSCID = 253;
mavlink_message_t msg;
mavlink_message_t msgctl;
mavlink_status_t status;
mavlink_sys_status_t sysstatus;
mavlink_set_position_target_global_int_t target_int;
mavlink_position_target_global_int_t target_pos;
mavlink_request_data_stream_t stream;
mavlink_global_position_int_t gpspos;
mavlink_local_position_ned_t gpslocalned;
mavlink_gps_raw_int_t gpsraw;
mavlink_battery_status_t batstatus;
mavlink_set_mode_t fmode;
mavlink_heartbeat_t hb;
mavlink_vibration_t vibe;
mavlink_set_position_target_local_ned_t postarned;
mavlink_vfr_hud_t vfr_hud;
mavlink_timesync_t timesync;
mavlink_distance_sensor_t distance_sensor;
uint8_t txbuf[MAVLINK_MAX_PACKET_LEN];

void rxmavlink(void *p);
bool run = false;
int wakeup_flag = 0;
static const esp_afe_sr_iface_t *afe_handle = NULL;
static volatile int task_flag = 0;
// srmodel_list_t *models = NULL;
char *cmds[] = {"ARM", "TAKEOFF", "GO", "UP", "DOWN", "RIGHT", "LEFT", "FORWARD", "BACK", "YAW LEFT", "YAW RIGHT", "RETURN TO HOME", "LAND", "LOITER", "HOLD"};
#define TAG "audio_fft"
char *Fmode[] = {"STABILIZE", "ACRO", "ALT_HOLD", "AUTO", "GUIDED", "LOITER", "RTL", "CIRCLE", "NA", "LAND"};

#define N_SAMPLES 1024
#define SAMPLE_RATE 16000
#define CHANNELS 1
#define DISPLAY_REFRESH_MS 200
#define STRIPE_COUNT 64

#define CANVAS_WIDTH 410
#define CANVAS_HEIGHT 200

__attribute__((aligned(16))) int16_t raw_data[N_SAMPLES * CHANNELS];
__attribute__((aligned(16))) float audio_buffer[N_SAMPLES];
__attribute__((aligned(16))) float wind[N_SAMPLES];
__attribute__((aligned(16))) float fft_buffer[N_SAMPLES * 2];
__attribute__((aligned(16))) float spectrum[N_SAMPLES / 2];

float display_spectrum[STRIPE_COUNT];
float peak[STRIPE_COUNT];
size_t bytes_read;
int16_t *i2s_buff;
esp_err_t ret;
srmodel_list_t *models = NULL;
int prevcmd = 0;

double degToR(double degrees)
{
    const double pi = 3.14159265358979323846;
    return degrees * (pi / 180.0);
}
void sendMAVLink(mavlink_message_t msg)
{
    int len = mavlink_msg_to_send_buffer(txbuf, &msg);

    int errr = sendto(sock, txbuf, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (errr < 0)
    {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
}
void udpstartup()
{
    stream.target_component = 1;
    stream.target_system = 1;
    stream.start_stop = 1;
    timesync.target_component = 1;
    timesync.target_system = 1;

    mavlink_msg_heartbeat_pack(GSCID, 25, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, 0);
    sendMAVLink(msg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    mavlink_msg_timesync_encode(GSCID, 25, &msg, &timesync);
    sendMAVLink(msg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    stream.req_message_rate = 2;
    stream.req_stream_id = 2;
    mavlink_msg_request_data_stream_encode(GSCID, 25, &msg, &stream);
    sendMAVLink(msg);

    vTaskDelay(100 / portTICK_PERIOD_MS);
    stream.req_message_rate = 1;
    stream.req_stream_id = 6;
    mavlink_msg_request_data_stream_encode(GSCID, 25, &msg, &stream);
    sendMAVLink(msg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    stream.req_message_rate = 1;
    stream.req_stream_id = 10;
    mavlink_msg_request_data_stream_encode(GSCID, 25, &msg, &stream);
    sendMAVLink(msg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    stream.req_message_rate = 1;
    stream.req_stream_id = 11;
    mavlink_msg_request_data_stream_encode(GSCID, 25, &msg, &stream);
    sendMAVLink(msg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    stream.req_message_rate = 1;
    stream.req_stream_id = 12;
    mavlink_msg_request_data_stream_encode(GSCID, 25, &msg, &stream);
    sendMAVLink(msg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    stream.req_message_rate = 4;
    stream.req_stream_id = 1;
    mavlink_msg_request_data_stream_encode(GSCID, 25, &msg, &stream);
    sendMAVLink(msg);
}

// Callback for handling notifications
uint8_t rx_buffer[2048];
void rxmavlink(void *p)
{
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t socklen = sizeof(source_addr);
    fd_set read_fds;
    struct timeval tv;

    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);

    tv.tv_sec = 0; // No timeout
    tv.tv_usec = 0;
    int len;

    while (1)
    {
        len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
        if (len < 0)
        {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
        }

        if (len > 0)
        {
            //      rxpkt = true;
            mavlink_message_t msg;
            mavlink_status_t status;

            for (int i = 0; i < len; i++)
            {
                if (mavlink_parse_char(MAVLINK_COMM_0, rx_buffer[i], &msg, &status))
                {
                    switch (msg.msgid)
                    {

                    case MAVLINK_MSG_ID_HEARTBEAT:
                        if (msg.compid == 1)
                        {
                            mavlink_msg_heartbeat_decode(&msg, &hb);
                            ESP_LOGI(TAG, "Received HEARTBEAT from system %d, component %d", msg.sysid, msg.compid);
                        }

                        break;
                    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                        mavlink_msg_global_position_int_decode(&msg, &gpspos);
                        break;
                    case MAVLINK_MSG_ID_SYS_STATUS:
                        mavlink_msg_sys_status_decode(&msg, &sysstatus);
                        break;
                    case MAVLINK_MSG_ID_BATTERY_STATUS:
                        mavlink_msg_battery_status_decode(&msg, &batstatus);
                        break;
                    case MAVLINK_MSG_ID_GPS_RAW_INT:
                        mavlink_msg_gps_raw_int_decode(&msg, &gpsraw);

                        break;
                    case MAVLINK_MSG_ID_LOCAL_POSITION_NED:
                        mavlink_msg_local_position_ned_decode(&msg, &gpslocalned);
                        break;
                    case MAVLINK_MSG_ID_VIBRATION:
                        mavlink_msg_vibration_decode(&msg, &vibe);
                        break;
                    case MAVLINK_MSG_ID_VFR_HUD:
                        mavlink_msg_vfr_hud_decode(&msg, &vfr_hud);
                        break;
                    case MAVLINK_MSG_ID_DISTANCE_SENSOR:
                        mavlink_msg_distance_sensor_decode(&msg, &distance_sensor);
                        break;

                    default:
                        break;
                    }
                }
            }
        }

        //        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
float feedtime = 0;
void feed_Task(void *arg)
{
    int64_t start_time;
    int64_t end_time;

    esp_afe_sr_data_t *afe_data = arg;
    if (bsp_extra_codec_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Audio codec init failed");
        vTaskDelete(NULL);
    }

    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int nch = afe_handle->get_feed_channel_num(afe_data);
    int feed_channel = CHANNELS;
    assert(nch == feed_channel);
    i2s_buff = malloc(audio_chunksize * sizeof(int16_t) * feed_channel);
    assert(i2s_buff);

    while (task_flag)
    {
        start_time = esp_timer_get_time();
        //        ret =  esp_get_feed_data(true, i2s_buff, audio_chunksize * sizeof(int16_t) * feed_channel);
        ret = bsp_extra_i2s_read(i2s_buff, audio_chunksize * sizeof(int16_t) * feed_channel, &bytes_read, portMAX_DELAY);
        if (ret != ESP_OK || bytes_read != audio_chunksize * sizeof(int16_t) * feed_channel)
        {
            ESP_LOGW(TAG, "I2S read error: %d, bytes: %d", ret, bytes_read);
            continue;
        }
        afe_handle->feed(afe_data, i2s_buff);
        end_time = esp_timer_get_time();
        feedtime = (end_time - start_time) / 1000;
    }
    if (i2s_buff)
    {
        free(i2s_buff);
        i2s_buff = NULL;
    }
    vTaskDelete(NULL);
}
void send_cmd(int cmd)
{
    if (run)
    {
        ESP_LOGI(TAG, "cmd: %s sent", cmds[cmd]);
        switch (cmd)
        {
        case 0: // Arm
        {
            mavlink_command_long_t arm;
            arm.param1 = 1; // Arm
            arm.param2 = 0;
            arm.param3 = 0;
            arm.param4 = 0;
            arm.param5 = 0;
            arm.param6 = 0;
            arm.param7 = 0;
            arm.confirmation = 0;
            arm.target_component = 1;
            arm.target_system = 1;
            arm.command = MAV_CMD_COMPONENT_ARM_DISARM;
            mavlink_msg_command_long_encode(GSCID, 25, &msg, &arm);
            sendMAVLink(msg);
            ESP_LOGI(TAG, "Sent ARM command");
            break;
        }
        case 1: // Takeoff
        {
            mavlink_command_long_t takeoff;
            takeoff.param1 = 0;
            takeoff.param2 = 0;
            takeoff.param3 = 0;
            takeoff.param4 = 0; // Use NaN for yaw to indicate no change
            takeoff.param5 = 0;
            takeoff.param6 = 0;
            takeoff.param7 = 1; // Altitude in meters
            takeoff.confirmation = 0;
            takeoff.target_system = 1;
            takeoff.target_component = 1;
            takeoff.command = MAV_CMD_NAV_TAKEOFF;
            mavlink_msg_command_long_encode(GSCID, 25, &msg, &takeoff);
            sendMAVLink(msg);
            ESP_LOGI(TAG, "Sent TAKEOFF command");
            break;
        }
        case 11: // RTL
        {
            mavlink_command_long_t rtl;
            rtl.param1 = 0;
            rtl.param2 = 0;
            rtl.param3 = 0;
            rtl.param4 = 0; // Use NaN for yaw to indicate no change
            rtl.param5 = 0;
            rtl.param6 = 0;
            rtl.param7 = 0; // Altitude in meters
            rtl.confirmation = 0;
            rtl.target_system = 1;
            rtl.target_component = 1;
            rtl.command = MAV_CMD_NAV_RETURN_TO_LAUNCH;
            mavlink_msg_command_long_encode(GSCID, 25, &msg, &rtl);
            sendMAVLink(msg);
            ESP_LOGI(TAG, "Sent RTL command");
            break;
        }

        case 12: // land
        {
            mavlink_command_long_t land;
            land.param1 = 0;
            land.param2 = 0;
            land.param3 = 0;
            land.param4 = 0; // Use NaN for yaw to indicate no change
            land.param5 = 0;
            land.param6 = 0;
            land.param7 = 0; // Altitude in meters
            land.confirmation = 0;
            land.target_system = 1;
            land.target_component = 1;
            land.command = MAV_CMD_NAV_LAND;
            mavlink_msg_command_long_encode(GSCID, 25, &msg, &land);
            sendMAVLink(msg);
            ESP_LOGI(TAG, "Sent LAND command");
            break;
        }
        case 3: // Up
        {
            mavlink_set_position_target_local_ned_t up;
            up.time_boot_ms = 0;
            up.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            up.type_mask = 0b100111111000;
            up.x = 0;
            up.y = 0;
            up.z = -1; // Move up by 1 meter
            up.vx = 0;
            up.vy = 0;
            up.vz = 0;
            up.afx = 0;
            up.afy = 0;
            up.afz = 0;
            up.yaw = 0; // Use NaN for yaw to indicate no change
            up.yaw_rate = 0;
            up.target_system = 1;
            up.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &up);
            sendMAVLink(msg);
            break;
        }
        case 4: // Down
        {
            mavlink_set_position_target_local_ned_t dn;
            dn.time_boot_ms = 0;
            dn.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            dn.type_mask = 0b100111111000;
            dn.x = 0;
            dn.y = 0;
            dn.z = 1; // Move down by 1 meter
            dn.vx = 0;
            dn.vy = 0;
            dn.vz = 0;
            dn.afx = 0;
            dn.afy = 0;
            dn.afz = 0;
            dn.yaw = 0; // Use NaN for yaw to indicate no change
            dn.yaw_rate = 0;
            dn.target_system = 1;
            dn.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &dn);
            sendMAVLink(msg);
            break;
        }
        case 5: // Right
        {
            mavlink_set_position_target_local_ned_t rt;
            rt.time_boot_ms = 0;
            rt.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            rt.type_mask = 0b100111111000;
            rt.x = 0;
            rt.y = 1; // Move right by 1 meter
            rt.z = 0;
            rt.vx = 0;
            rt.vy = 0;
            rt.vz = 0;
            rt.afx = 0;
            rt.afy = 0;
            rt.afz = 0;
            rt.yaw = 0; // Use NaN for yaw to indicate no change
            rt.yaw_rate = 0;
            rt.target_system = 1;
            rt.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &rt);
            sendMAVLink(msg);
            break;
        }
        case 6: // Left
        {
            mavlink_set_position_target_local_ned_t lt;
            lt.time_boot_ms = 0;
            lt.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            lt.type_mask = 0b100111111000;
            lt.x = 0;
            lt.y = -1; // Move left by 1 meter
            lt.z = 0;
            lt.vx = 0;
            lt.vy = 0;
            lt.vz = 0;
            lt.afx = 0;
            lt.afy = 0;
            lt.afz = 0;
            lt.yaw = 0; // Use NaN for yaw to indicate no change
            lt.yaw_rate = 0;
            lt.target_system = 1;
            lt.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &lt);
            sendMAVLink(msg);
            break;
        }
        case 7: // Forward
        {
            mavlink_set_position_target_local_ned_t fw;
            fw.time_boot_ms = 0;
            fw.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            fw.type_mask = 0b100111111000;
            fw.x = 1; // Move forward by 1 meter
            fw.y = 0;
            fw.z = 0;
            fw.vx = 0;
            fw.vy = 0;
            fw.vz = 0;
            fw.afx = 0;
            fw.afy = 0;
            fw.afz = 0;
            fw.yaw = 0; // Use NaN for yaw to indicate no change
            fw.yaw_rate = 0;
            fw.target_system = 1;
            fw.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &fw);
            sendMAVLink(msg);
            break;
        }
        case 8: // Back
        {
            mavlink_set_position_target_local_ned_t bk;
            bk.time_boot_ms = 0;
            bk.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            bk.type_mask = 0b100111111000;
            bk.x = -1; // Move backward by 1 meter
            bk.y = 0;
            bk.z = 0;
            bk.vx = 0;
            bk.vy = 0;
            bk.vz = 0;
            bk.afx = 0;
            bk.afy = 0;
            bk.afz = 0;
            bk.yaw = 0; // Use NaN for yaw to indicate no change
            bk.yaw_rate = 0;
            bk.target_system = 1;
            bk.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &bk);
            sendMAVLink(msg);
            break;
        }
        case 9: // Yaw Left
        {
            mavlink_set_position_target_local_ned_t yl;
            yl.time_boot_ms = 0;
            yl.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            yl.type_mask = 0b100111111000;
            yl.x = 0;
            yl.y = 0;
            yl.z = 0;
            yl.vx = 0;
            yl.vy = 0;
            yl.vz = 0;
            yl.afx = 0;
            yl.afy = 0;
            yl.afz = 0;
            yl.yaw = -0.1745329252; // -10 deg.
            yl.yaw_rate = 0;
            yl.target_system = 1;
            yl.target_component = 1;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &yl);
            sendMAVLink(msg);
            break;
        }
        case 10: // Yaw Right
        {
            mavlink_set_position_target_local_ned_t yr;
            yr.time_boot_ms = 0;
            yr.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
            yr.type_mask = 0b100111111000;
            yr.x = 0;
            yr.y = 0;
            yr.z = 0;
            yr.vx = 0;
            yr.vy = 0;
            yr.vz = 0;
            yr.afx = 0;
            yr.afy = 0;
            yr.afz = 0;
            yr.yaw = 0.1745329252; // 10 deg.
            yr.yaw_rate = 0;
            yr.target_system = 1;
            yr.target_component = 0;
            mavlink_msg_set_position_target_local_ned_encode(GSCID, 25, &msg, &yr);
            sendMAVLink(msg);
            break;
        }
        }
    }
}
float detecttime = 0;
void detect_Task(void *arg)
{
    int64_t start_time;
    int64_t end_time;

    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    char *mn_name = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_ENGLISH);
    printf("multinet:%s\n", mn_name);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);
    model_iface_data_t *model_data = multinet->create(mn_name, 10000);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data); // Add speech commands from sdkconfig
    esp_mn_commands_add(32, "ARM");
    esp_mn_commands_add(33, "TAKEOFF");
    esp_mn_commands_add(34, "GO");
    esp_mn_commands_add(35, "UP");
    esp_mn_commands_add(36, "DOWN");
    esp_mn_commands_add(37, "RIGHT");
    esp_mn_commands_add(38, "LEFT");
    esp_mn_commands_add(39, "FORWARD");
    esp_mn_commands_add(40, "BACK");
    esp_mn_commands_add(41, "YAW LEFT");
    esp_mn_commands_add(42, "YAW RIGHT");
    esp_mn_commands_add(43, "RETURN TO HOME");
    esp_mn_commands_add(44, "LAND");
    esp_mn_commands_update();
    assert(mu_chunksize == afe_chunksize);
    // print active speech commands

    multinet->print_active_speech_commands(model_data);

    printf("------------detect start------------\n");
    while (task_flag)
    {
        wakeup_flag = 1;
        start_time = esp_timer_get_time();
        afe_fetch_result_t *res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL)
        {
            printf("fetch error!\n");
            break;
        }

        if (wakeup_flag == 1)
        {
            esp_mn_state_t mn_state = multinet->detect(model_data, res->data);

            if (mn_state == ESP_MN_STATE_DETECTING)
            {
                //                printf("DETECTing\n");
                continue;
            }

            if (mn_state == ESP_MN_STATE_DETECTED)
            {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                if (mn_result->num > 1)
                    continue;
                printf("TOP %d, command_id: %d, phrase_id: %d, string: %s, prob: %f\n",
                       0, mn_result->command_id[0], mn_result->phrase_id[0], mn_result->string, mn_result->prob[0]);
                ESP_LOGI(TAG, "cmd: %d", cmds[mn_result->command_id[0]]);
                send_cmd(mn_result->command_id[0]);
                bsp_display_lock(pdMS_TO_TICKS(200));
                lv_label_set_text(label_3, cmds[mn_result->command_id[0]]);
                bsp_display_unlock();

                //                prevcmd = mn_result->command_id[0] - 32;
                printf("-----------listening-----------\n");
            }
        }
        end_time = esp_timer_get_time();
        detecttime = (end_time - start_time) / 1000;
    }
    if (model_data)
    {
        multinet->destroy(model_data);
        model_data = NULL;
    }
    printf("detect exit\n");
    vTaskDelete(NULL);
}
int64_t time1;
uint8_t percent;
float avgbatv[10];
float avgbatc[10];
float avgB = 0;
float avgC = 0;
uint8_t avgind = 0;
uint8_t avgpt = 0;
void app_main(void)
{
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t socklen = sizeof(source_addr);
    i2c_master_bus_handle_t i2c_bus = bsp_i2c_get_handle();
    axp2101_init(i2c_bus);
    if (axp2101_check_chip_id() != ESP_OK)
    {
        printf("AXP2101 not found!\n");
        return;
    }
    // Print battery state
    axp2101_batt_status_t status;
    axp2101_get_battery_status(&status);

    //    printf("Battery status: %d%%\n", percent);

    ESP_LOGI(TAG, "Starting Audio Spectrum Analyzer");

    //   wifi_init_sta();

    lv_display_t *disp = bsp_display_start();
    if (disp)
    {
        bsp_display_backlight_on();
    }

    bsp_display_lock(pdMS_TO_TICKS(200));
    display_setup();
    bsp_display_unlock();
    models = esp_srmodel_init("model");
    if (models)
    {
        for (int i = 0; i < models->num; i++)
        {
            if (strstr(models->model_name[i], ESP_WN_PREFIX) != NULL)
            {
                printf("wakenet model in flash: %s\n", models->model_name[i]);
            }
        }
    }

    afe_config_t *afe_config = afe_config_init("M", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
    afe_handle = esp_afe_handle_from_config(afe_config);
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);
    afe_config_free(afe_config);
    task_flag = 1;

    connectf = false;
    wififail = false;
    hb.custom_mode = 0;
    while (1)
    {

        if (connectf == true)
        {
            connectf = false;
            bsp_display_lock(pdMS_TO_TICKS(200));
            lv_label_set_text(label_2, "Connecting");
            bsp_display_unlock();
            wifi_init_sta();
        }
        if (connected == true)
        {
            bsp_display_lock(pdMS_TO_TICKS(200));
            lv_label_set_text(label_2, "Connected");
            bsp_display_unlock();
            connected = false;
            udpinit();
            udpstartup();
            xTaskCreate(rxmavlink, "rxmavlink", 6 * 1024, NULL, 5, NULL);
            xTaskCreatePinnedToCore(&feed_Task, "feed", 8 * 1024, (void *)afe_data, 5, NULL, 0);
            xTaskCreatePinnedToCore(&detect_Task, "detect", 4 * 1024, (void *)afe_data, 5, NULL, 1);
            time1 = esp_timer_get_time();
            run = true;
        }
        if (wififail == true)
        {
            bsp_display_lock(pdMS_TO_TICKS(200));
            lv_label_set_text(label_2, "Connect fail");
            bsp_display_unlock();
            wififail = false;
            connected = false;
            run = false;
        }

        if (run == true)
        {
            mavlink_msg_heartbeat_pack(GSCID, 25, &msg, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, 0, 0, 0);
            sendMAVLink(msg);
            //            printf("detecttime:%f feedtime:%f\n", detecttime, feedtime);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            avgbatv[avgpt] = sysstatus.voltage_battery / 1000.0;
            avgbatc[avgpt++] = sysstatus.current_battery / 100.0;
            if (avgind < 9)
                avgind++;
            if (avgpt > 9)
                avgpt = 0;
            avgB = 0;
            avgC = 0;
            for (int i = 0; i < avgind; i++)
            {
                avgB += avgbatv[i];
                avgC += avgbatc[i];
            }
            avgB = avgB / avgind;
            avgC = avgC / avgind;

            bsp_display_lock(pdMS_TO_TICKS(200));
            lv_label_set_text_fmt(label_4, "FM:%s", Fmode[hb.custom_mode]);
            lv_label_set_text_fmt(label_1, "%.1fV, %.1fA", avgB, avgC);
            lv_label_set_text_fmt(label_5, "GPS: %d sat:%d hdop: %.1f", gpsraw.fix_type, gpsraw.satellites_visible, gpsraw.eph / 100.0);
            lv_label_set_text_fmt(label_6, "AGL: %.1f ft", (distance_sensor.current_distance / 100.0) * 3.2808399);
            lv_image_set_rotation(imgdir, vfr_hud.heading * 10);
            lv_label_set_text_fmt(label_8, "%d\u00B0", vfr_hud.heading);
            bsp_display_unlock();
        }
        if (esp_timer_get_time() - (time1 / 1000) > 1000)
        {
            time1 = esp_timer_get_time();
            axp2101_get_battery_percentage(&percent);
            bsp_display_lock(pdMS_TO_TICKS(200));
            lv_label_set_text_fmt(label_7, "%d%%", percent);
            bsp_display_unlock();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
