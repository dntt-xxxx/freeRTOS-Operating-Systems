/**
 * ESP32 Dining Philosophers Solution: Arbitrator/Waiter
 * 
 * One possible solution to the Dining Philosophers problem.
 * 
 * Based on http://www.cs.virginia.edu/luther/COA2/S2019/pa05-dp.html
 * 
 * Date: February 8, 2021
 * Author: Shawn Hymel
 * License: 0BSD
 */

// You'll likely need this on vanilla FreeRTOS
//#include semphr.h

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
#define NUM_TASKS 50 // Number of tasks (philosophers)
#define TASK_STACK_SIZE  2048   // Bytes in ESP32, words in vanilla FreeRTOS

// Globals
static SemaphoreHandle_t bin_sem;   // Wait for parameters to be read
static SemaphoreHandle_t done_sem;  // Notifies main task when done
static SemaphoreHandle_t chopstick[NUM_TASKS];

static SemaphoreHandle_t arbitrator;  // Controls who eats when (e.g. waiter)
arbitrator = xSemaphoreCreateMutex();

static volatile uint32_t availHeap;
static volatile uint32_t allocHeap;

//*****************************************************************************
// Tasks
 void getInfoMemHeap(volatile uint32_t *availHeap,volatile uint32_t *allocHeap)
 {
  *allocHeap+=*availHeap-esp_get_free_heap_size();
  *availHeap=esp_get_free_heap_size();
  Serial.println(*availHeap);Serial.print("-");Serial.print(*allocHeap);Serial.println();
  
 }
// The only task: eating
void eat(void *parameters) {
  int num;
  char buf[50];
  // Copy parameter and increment semaphore count
  num = *(int *)parameters;
  //getInfoMemHeap(&allocHeap,&availHeap);
  xSemaphoreGive(bin_sem);
  
  // We have to wait our turn to eat
  xSemaphoreTake(arbitrator, portMAX_DELAY);
  // Take left chopstick
  xSemaphoreTake(chopstick[num], portMAX_DELAY);
  sprintf(buf, "Philosopher %i took chopstick %i", num, num);
  Serial.println(buf);
  // Add some delay to force deadlock
  vTaskDelay(10 / portTICK_PERIOD_MS);
  // Take right chopstick
  xSemaphoreTake(chopstick[(num+1)%NUM_TASKS], portMAX_DELAY);
  sprintf(buf, "Philosopher %i took chopstick %i", num, (num+1)%NUM_TASKS);
  Serial.println(buf);
  // Do some eating
  sprintf(buf, "Philosopher %i is eating", num);
  Serial.println(buf);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  // Put down right chopstick
  xSemaphoreGive(chopstick[(num+1)%NUM_TASKS]);
  sprintf(buf, "Philosopher %i returned chopstick %i", num, (num+1)%NUM_TASKS);
  Serial.println(buf);
  // Put down left chopstick
  xSemaphoreGive(chopstick[num]);
  sprintf(buf, "Philosopher %i returned chopstick %i", num, num);
  Serial.println(buf);
  // Tell the arbitrator (waiter) that we're done
  xSemaphoreGive(arbitrator);
  // Notify main task and delete self
  xSemaphoreGive(done_sem);
  vTaskDelete(NULL);
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {
  char task_name[20];
  // Configure Serial
  
  Serial.begin(115200);
  
  
  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Dining Philosophers Arbitrator Solution---");
  //Serial.println(xTaskGetTickCount());

  availHeap=esp_get_free_heap_size();
 // allocHeap=(uint32_t)0;
 // getInfoMemHeap(&allocHeap,&availHeap);
  // Create kernel objects before starting tasks
  bin_sem = xSemaphoreCreateBinary();
 // getInfoMemHeap(&allocHeap,&availHeap);
  done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);
  //getInfoMemHeap(&allocHeap,&availHeap);
  for (int i = 0; i < NUM_TASKS; i++) {
    chopstick[i] = xSemaphoreCreateMutex();
  }
  getInfoMemHeap(&allocHeap,&availHeap);
  
  arbitrator = xSemaphoreCreateMutex();
  
  getInfoMemHeap(&allocHeap,&availHeap);
  // Have the philosphers start eating
  for (int i = 0; i < NUM_TASKS; i++) {
    sprintf(task_name, "Philosopher %i", i);
    xTaskCreatePinnedToCore(eat,task_name,TASK_STACK_SIZE,(void *)&i,1,NULL,app_cpu);
    xSemaphoreTake(bin_sem, portMAX_DELAY);
  }


  // Wait until all the philosophers are done
  for (int i = 0; i < NUM_TASKS; i++) {
    xSemaphoreTake(done_sem, portMAX_DELAY);
  }
  //Serial.println("end");
  //Serial.println(availHeap);
  //Serial.println(allocHeap);
  //Serial.println(xTaskGetTickCount());
  // Say that we made it through without deadlock
  Serial.println("Done! No deadlock occurred!");
}

void loop() {
  // Do nothing in this task
}