#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// timer struct
typedef struct {
    int count;
    int max_count;
    struct timespec interval;
    struct timespec next_time;
} software_timer_t;

// Initialize the timer
void timer_init(software_timer_t *timer, int max_count, long interval_ns) {
    timer->count = 0;
    timer->max_count = max_count;
    timer->interval.tv_sec = interval_ns / 1000000000; // Convert nanoseconds to seconds
    timer->interval.tv_nsec = interval_ns % 1000000000; // Remaining nanoseconds

    //设置首次触发时间，
    clock_gettime(CLOCK_MONOTONIC, &timer->next_time);
    timer->next_time.tv_nsec += interval_ns;
    if (timer->next_time.tv_nsec >= 1000000000) {
        timer->next_time.tv_sec += 1;
        timer->next_time.tv_nsec -= 1000000000;
    }
}

// 检查定时器触发
int timer_check(software_timer_t *timer) {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // 比较当前时间和下次触发时间
    if (current_time.tv_sec > timer->next_time.tv_sec ||
        (current_time.tv_sec == timer->next_time.tv_sec && 
        current_time.tv_nsec >= timer->next_time.tv_nsec)) {

        // 更新下次触发时间
        timer->next_time.tv_sec += timer->interval.tv_sec;
        timer->next_time.tv_nsec += timer->interval.tv_nsec;
        if (timer->next_time.tv_nsec >= 1000000000) {
            timer->next_time.tv_sec += 1;
            timer->next_time.tv_nsec -= 1000000000;
        }
        return 1;   // 需要触发
    }
    return 0;   // 不需要触发
}

// 定时器回调函数
void timer_callback(software_timer_t *timer) {
    printf("Hello World! Timer count: %d/%d\n", timer->count + 1, timer->max_count);
    timer->count++;
}

int main(){
    software_timer_t timer;

    // 初始化定时器，每秒触发1次， 共10次
    timer_init(&timer, 10, 1000000000); // 1 second in nanoseconds

    printf("Timer started. Waiting for events...\n");

    while(1) {
        if (timer_check(&timer)){
            timer_callback(&timer);

            // 检查是否完成
            if (timer.count >= timer.max_count) {
                printf("Timer completed. Exiting...\n");
                break; // 退出循环
            }
        }
        usleep(10000); // Sleep for 100milliseconds to avoid busy waiting
    }

    return 0;
}