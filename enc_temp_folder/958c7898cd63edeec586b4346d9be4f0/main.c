#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


TaskHandle_t blinkfunc, printFunc, deleteFunc, tempReadingFunc, spamQueue;

QueueHandle_t learningQueue;

int globalCountToImplementMutex = 0;

void tempReading(void* pvParameters) {

	int reading;

	for (;;) {

		printf("Enter sensor value: ");

		if (scanf_s("%d", &reading) != 1) {

			printf("wrong datatype entered\n");

			return -1;

		}

		if (xQueueSend(learningQueue, &reading, 500) == pdPASS) {

			printf("Reading sent to queue\n");

			vTaskDelay(50 / portTICK_PERIOD_MS);

		}

		else {

			printf("Add to queue operation failed\n");
			vTaskDelay(100 / portTICK_PERIOD_MS);

		}
	}

}
void blinkLed(void* pvParameters) {

	while (1) {

		if (uxQueueSpacesAvailable(learningQueue) == 0) {

			printf("on\n");
			vTaskDelay(500 / portTICK_PERIOD_MS);
			printf("off\n");
			vTaskDelay(500 / portTICK_PERIOD_MS);

		}
		
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}

void printString(void* pvParameters) {

	int recieveQueue;

	for (;;) {

		if (xQueueReceive(learningQueue, &recieveQueue, 500) == pdPASS) {

			printf("Received Temperature reading is: %d\n", recieveQueue);
			vTaskDelay(10000 / portTICK_PERIOD_MS);

		}
		
	}
}

void queueSpammer(void* pvParameters) {

	int spammer = 650;
	int i = 1;

	for (;;) {
	
		while (i > 0) {

			spammer += 100;
			
			printf("Queue space available is: %d\n", uxQueueSpacesAvailable(learningQueue));

			if (xQueueSend(learningQueue, &spammer, 500) == pdPASS) {

				printf("Sent %d to queue\n", spammer);
				vTaskDelay(pdMS_TO_TICKS(1));

			}

			else {

				printf("Spam is now sucessful\n");
				vTaskDelay(1 / portTICK_PERIOD_MS);

			}
		}
	}

}

void deleteTaskOne(void* pvParameters) {

	int i = 0;

	for (;;) {

		while (i < 5) {

			printf("I is currently number %d\n", i);
			i++;
			vTaskDelay(1000 / portTICK_PERIOD_MS);

		}

		printf("Deleting task 1\n");
		vTaskDelete(printFunc);
		printf("Task one deleted\n");
		vTaskDelay(2000 / portTICK_PERIOD_MS);

		while (i >= 0) {

			printf("I is currently number %d\n", i);
			i--;
			vTaskDelay(1000 / portTICK_PERIOD_MS);

		}

		printf("Deleting the deleting function\n");
		vTaskDelete(deleteFunc);

	}

}

void main() {

	learningQueue = xQueueCreate(5, sizeof(int));

	xTaskCreate(tempReading, "funcToSimulateSensorValue", 1, NULL, 3, &tempReadingFunc);
	xTaskCreate(blinkLed, "funcToBlinkLed", 1, NULL, 3, &blinkfunc);
	xTaskCreate(printString, "funcToPrintString", 1, NULL, 3, &printFunc);
	xTaskCreate(queueSpammer, "funcToSpamQueue", 1, NULL, 3, &spamQueue);
	//xTaskCreate(deleteTaskOne, "funcToDeletTask", 500, NULL, 0, &deleteFunc);
	vTaskStartScheduler();

	for (;;) {}

}