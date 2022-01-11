

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
enum { NUM_TASKS = 5 };           // Number of tasks (philosophers)
enum { TASK_STACK_SIZE = 2048 };  // Bytes in ESP32, words in vanilla FreeRTOS

// Globals
static SemaphoreHandle_t bin_sem;   // Wait for parameters to be read
static SemaphoreHandle_t done_sem;  // Notifies main task when done
static SemaphoreHandle_t chopstick[NUM_TASKS];
volatile int check=1;
//*****************************************************************************
// Tasks

// The only task: eating
void eat(void *parameters) {
   char buf[50];
 // sprintf(buf, "Time BEGIN: %i",xTaskGetTickCount());
  //Serial.println(buf);
  int num;
  int idx_1;
  int idx_2;
  // Copy parameter and increment semaphore count
  num = *(int *)parameters;
  sprintf(buf, "Heap available remain: %i", esp_get_free_heap_size());
  Serial.println(buf);
  xSemaphoreGive(bin_sem);
 
    /*
  if(check){
    check=0;
    sprintf(buf, "Task size %i", esp_get_free_heap_size());
    Serial.println(buf);
    //uint32_t size=(ESP.getHeapSize()-ESP.getFreeHeap());
    //uint32_t size=ESP.getHeap();
    //Serial.println(size);
  }
  */
  // Assign priority: pick up lower-numbered chopstick first
  if (num < (num + 1) % NUM_TASKS) {
    idx_1 = num;
    idx_2 = (num + 1) % NUM_TASKS;
  } else {
    idx_1 = (num + 1) % NUM_TASKS;
    idx_2 = num;
  }
  // Take lower-numbered chopstick
  xSemaphoreTake(chopstick[idx_1], portMAX_DELAY);
  sprintf(buf, "Philosopher %i took chopstick %i", num, num);
  Serial.println(buf);
  // Add some delay to force deadlock
  vTaskDelay(10 / portTICK_PERIOD_MS);
  // Take higher-numbered chopstick
  xSemaphoreTake(chopstick[idx_2], portMAX_DELAY);
  sprintf(buf, "Philosopher %i took chopstick %i", num, (num+1)%NUM_TASKS);
  Serial.println(buf);
  // Do some eating
  sprintf(buf, "Philosopher %i is eating", num);
  Serial.println(buf);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  // Put down higher-numbered chopstick
  xSemaphoreGive(chopstick[idx_2]);
  sprintf(buf, "Philosopher %i returned chopstick %i", num, (num+1)%NUM_TASKS);
  Serial.println(buf);
  // Put down lower-numbered chopstick
  xSemaphoreGive(chopstick[idx_1]);
  sprintf(buf, "Philosopher %i returned chopstick %i", num, num);
  Serial.println(buf);
  // Notify main task and delete self
  xSemaphoreGive(done_sem);
  vTaskDelete(NULL);
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

  char task_name[20];
  char buf[30];
  // Configure Serial
  Serial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  //vTaskDelay(1000 / portTICK_PERIOD_MS);
  //Serial.println();
  Serial.println("---FreeRTOS Dining Philosophers Hierarchy Solution---");

  // Create kernel objects before starting tasks
  bin_sem = xSemaphoreCreateBinary();
  done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);
  for (int i = 0; i < NUM_TASKS; i++) {
    chopstick[i] = xSemaphoreCreateMutex();
  }

  // Have the philosphers start eating
  sprintf(buf, "Time start: %i(tick)",xTaskGetTickCount());
  Serial.println(buf);
  for (int i = 0; i < NUM_TASKS; i++) {
    sprintf(task_name, "Philosopher %i", i);
    xTaskCreatePinnedToCore(eat,
                            task_name,
                            TASK_STACK_SIZE,
                            (void *)&i,
                            1,
                            NULL,
                            app_cpu);
    xSemaphoreTake(bin_sem, portMAX_DELAY);
  }


  // Wait until all the philosophers are done
  for (int i = 0; i < NUM_TASKS; i++) {
    xSemaphoreTake(done_sem, portMAX_DELAY);
  }
  sprintf(buf, "Time end: %i(tick)",xTaskGetTickCount());
  Serial.println(buf);
  // Say that we made it through without deadlock
  Serial.println("Done! No deadlock occurred!");
}

void loop() {
  // Do nothing in this task
}