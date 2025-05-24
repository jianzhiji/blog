#include <string.h> // 包含字符串处理函数库
#include "esp_event.h" // 包含ESP32事件处理功能库
#include "esp_log.h" // 包含ESP32日志功能库

#include "esp_netif.h" // 包含ESP32网络接口功能库

#include "nvs_flash.h" // 包含ESP32非易失性存储功能库
#include "esp_wifi.h" // 包含ESP32 Wi-Fi功能库
#include "esp_http_server.h" // 包含ESP32 HTTP服务器功能库

static const char *TAG = "WEB_IMG";

// 嵌入资源（命名由 objcopy 自动生成）
extern const uint8_t _binary_index_html_start[];
extern const uint8_t _binary_index_html_end[];
extern const uint8_t _binary_pic_png_start[];
extern const uint8_t _binary_pic_png_end[];

// 处理index.html文件请求
static esp_err_t index_handler(httpd_req_t *req)
{
    // 设置响应类型为text/html
    httpd_resp_set_type(req, "text/html");
    // 发送index.html文件内容
    return httpd_resp_send(req, (const char *)_binary_index_html_start, _binary_index_html_end - _binary_index_html_start);
}

// 处理http 图片请求
static esp_err_t pic_handler(httpd_req_t *req)
{
    // 设置响应类型为image/png
    httpd_resp_set_type(req, "image/png");
    // 发送响应，返回图片数据
    return httpd_resp_send(req, (const char *)_binary_pic_png_start, _binary_pic_png_end - _binary_pic_png_start);
}

// 定义一个函数，用于启动web服务器
httpd_handle_t start_webserver(void)
{
    // 定义一个httpd_config_t类型的变量，用于存储httpd的配置信息
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // 定义一个httpd_handle_t类型的变量，用于存储httpd的句柄
    httpd_handle_t server = NULL;

    // 如果httpd_start函数返回值为ESP_OK，则表示启动成功
    if (httpd_start(&server, &config) == ESP_OK) {
        // 定义一个httpd_uri_t类型的变量，用于存储uri的配置信息
        httpd_uri_t index_uri = {
            // 设置uri的路径
            .uri       = "/",
            // 设置uri的方法为GET
            .method    = HTTP_GET,
            // 设置uri的处理函数为index_handler
            .handler   = index_handler,
            // 设置uri的用户上下文为NULL
            .user_ctx  = NULL
        };
        // 注册uri的处理函数
        httpd_register_uri_handler(server, &index_uri);

        // 定义一个httpd_uri_t类型的变量，用于存储uri的配置信息
        httpd_uri_t pic_uri = {
            // 设置uri的路径
            .uri       = "/pic.png",
            // 设置uri的方法为GET
            .method    = HTTP_GET,
            // 设置uri的处理函数为pic_handler
            .handler   = pic_handler,
            // 设置uri的用户上下文为NULL
            .user_ctx  = NULL
        };
        // 注册uri的处理函数
        httpd_register_uri_handler(server, &pic_uri);
    }

    // 返回httpd的句柄
    return server;
}

void wifi_init_softap()//配置 Wi-Fi 为 AP 模式
{
    // 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    // 创建默认的事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // 创建默认的WiFi AP网络接口
    esp_netif_create_default_wifi_ap();

    // 初始化WiFi配置
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // 初始化WiFi
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 设置WiFi配置
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32_WEB", // 设置AP的SSID
            .ssid_len = strlen("ESP32_WEB"), // 设置SSID的长度
            .password = "12345678", // 设置AP的密码
            .max_connection = 2, // 设置最大连接数
            .authmode = WIFI_AUTH_WPA_WPA2_PSK // 设置认证模式
        },
    };

    // 如果密码为空，则设置为开放模式
    if (strlen((char *)wifi_config.ap.password) == 0)
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;

    // 设置WiFi模式为AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    // 设置WiFi配置
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    // 启动WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    // 打印AP已启动的信息
    ESP_LOGI(TAG, "Wi-Fi AP started. SSID:%s password:%s", "ESP32_WEB", "12345678");
}

// 主函数
void app_main()
{
    // 初始化非易失性存储器
    ESP_ERROR_CHECK(nvs_flash_init());
    // 初始化软AP
    wifi_init_softap();
    // 启动web服务器
    start_webserver();
}
