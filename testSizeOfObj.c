#include<C:\Users\dongo\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\tools\sdk\include\freertos\freertos\FreeRTOSConfig.h>
#define NUM_TASKS 10

  static SemaphoreHandle_t bin_sem;
  static SemaphoreHandle_t done_sem;
  static SemaphoreHandle_t chopstick[NUM_TASKS];
void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println(); 
  
  Serial.println("---FreeRTOS Dining Philosophers Hierarchy Solution---");
  
  Serial.print("Ban dau:");
  Serial.println(esp_get_free_heap_size());
  
  bin_sem = xSemaphoreCreateBinary();
  Serial.println("sau khi tao binsemaphore:");
  Serial.println(esp_get_free_heap_size());
  

  done_sem = xSemaphoreCreateCounting(NUM_TASKS,0);
  Serial.print("sau khi tao counting semaphore:");
  Serial.println(esp_get_free_heap_size());
  for (int i = 0; i < NUM_TASKS; i++) {
    chopstick[i] = xSemaphoreCreateMutex();
  }
  Serial.print("sau khi tao mutex semaphore:");
  Serial.println(esp_get_free_heap_size());
  xTaskCreatePinnedToCore(Task1code,"Task1",2048,NULL,1,NULL,0);
  
  xTaskCreatePinnedToCore(Task1code,"Task2",2048,NULL,1,NULL,0);        
}

void Task1code( void * parameter ){
  Serial.println("trong task task:");
  Serial.print(esp_get_free_heap_size());
  vTaskDelete(NULL);
}
void loop() {
  
}